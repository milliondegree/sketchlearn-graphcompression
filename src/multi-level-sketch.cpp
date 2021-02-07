#include "multi-level-sketch.h"


vector<int> intToVector(int n) {
  vector<int> res (32, 0);
  for (int i=31; i>=0; i--) {
    if (n!=0) {
      if (n%2) {
        res[i] = 1;
      }
      n /= 2;
    }
  }
  return res;
}


int vectorToInt(vector<int>& v) {
  int res = 0;
  for (int i=31; i>=0; i--) {
    res += pow(2, 31-i) * v[i];
  }
  return res;
}


double gaussian_prob(double d) {
  return 1 - erf(d/sqrt(2));
}


int median(vector<double>& v) {
  int n = v.size();
  if (n%2) {
    return v[n/2];
  }
  else {
    return (v[n/2-1]+v[n/2])/2;
  }
}


double mean(vector<double>& v) {
  double res = 0.0;
  for (double it : v) {
    res += it;
  }
  return res/v.size();
}


double variance(vector<double>& v) {
  double m = mean(v);
  double res = 0.0;
  for (double it : v) {
    res += pow(abs(it-m), 2);
  }
  return res/v.size();
}


MultiLevelSketch::MultiLevelSketch() {};


MultiLevelSketch::MultiLevelSketch(int r, int c, int k) {
  this->R = r;
  this->C = c;
  this->K = k;
  this->khfs = KHashFunctions(r, c);
  initSketch();
};


void MultiLevelSketch::initSketch() {
  vector<vector<vector<int>>> sketchTable;
  for (int i=0; i<this->R; i++) {
    vector<vector<int>> tmp;
    for (int j=0; j<this->C; j++) {
      vector<int> ttmp (this->K+1, 0);
      tmp.push_back(ttmp);
    }
    sketchTable.push_back(tmp);
  }
  this->sketch = sketchTable;
}


void MultiLevelSketch::resetSketch() {
  assert(this->R>0&&this->C>0&&this->K>0);
  for (int i=0; i<this->R; i++) {
    for (int j=0; j<this->C; j++) {
      for (int k=0; j<this->K+1; k++) {
        this->sketch[i][j][k] = 0;
      }
    }
  }
}


void MultiLevelSketch::feedFlowKey(int flowkey) {
  vector<int> hashed = this->khfs.getHashedValues(flowkey);
  vector<int> booleans = intToVector(flowkey);
  for (int r=0; r<this->R; r++) {
    int c = hashed[r];
    this->sketch[r][c][0]++;
    for (int k=0; k<this->K; k++) {
      if (booleans[k]) {
        this->sketch[r][c][k+1]++;
      }
    }
  }
}


vector<vector<vector<int>>> MultiLevelSketch::getSketch() {
  return this->sketch;
}


void MultiLevelSketch::modelInference(double theta, unordered_map<int, int>& lfm, unordered_map<int, vector<double>>& blp) {
  vector<vector<double>> distributions = computeDistribution_v2();
  while (true) {
    cout << "new round" << endl;
    terminate(R*C*K, distributions);
    unordered_map<int, int> lfmt;
    unordered_map<int, vector<double>> blpt;
    for (int r=0; r<this->R; r++) {
      for (int c=0; c<this->C; c++) {
        extractLargeFlows(theta, r, c, distributions, lfmt, blpt);
      }
    }
    for (auto it=lfmt.begin(); it!=lfmt.end(); it++) {
      cout << it->first << ' ' << it->second << endl;
    }
    for (auto it=lfmt.begin(); it!=lfmt.end(); it++) {
      lfm[it->first] = it->second;
    }
    for (auto it=blpt.begin(); it!=blpt.end(); it++) {
      blp[it->first] = it->second;
    }
    removeFlows(lfmt);
    for (int r=0; r<this->R; r++) {
      for (int c=0; c<this->C; c++) {
        cout << this->sketch[r][c][0] << ' ';
      }
      cout << endl;
    }
    cout << endl;
    distributions = computeDistribution_v2();
    if (terminate(R*C*K, distributions)) {
      break;
    }
    if (lfmt.size()==0) {
      theta /= 2;
    }
  }
}


vector<vector<double>> MultiLevelSketch::computeDistribution() {
  vector<vector<double>> res;
  int n = 0;
  for (int j=0; j<this->C; j++) {
    n += this->sketch[0][j][0];
  }
  for (int k=0; k<this->K; k++) {
    double freqs = 0.0;
    for (int i=0; i<this->R; i++) {
      for (int j=0; j<this->C; j++) {
        freqs += 1.0*this->sketch[i][j][k+1]/this->sketch[i][j][0];
      }
    }
    double p = freqs/(R*C);
    double delta = p*(1-p)*this->C/n;
    res.push_back(vector<double>({p, delta}));
  }
  return res;
}


vector<vector<double>> MultiLevelSketch::computeDistribution_v2() {
  vector<vector<double>> res;
  int n = 0;
  for (int j=0; j<this->C; j++) {
    n += this->sketch[0][j][0];
  }
  for (int k=0; k<this->K; k++) {
    vector<double> ratios;
    for (int i=0; i<this->R; i++) {
      for (int j=0; j<this->C; j++) {
        ratios.push_back(1.0*this->sketch[i][j][k+1]/this->sketch[i][j][0]);
      }
    }
    double p = mean(ratios);
    double delta = variance(ratios);
    res.push_back(vector<double>({p, delta}));
  }
  return res;
}


void MultiLevelSketch::extractLargeFlows(double theta, int r, int c, vector<vector<double>>& distributions,
                                        unordered_map<int, int>& lfm, unordered_map<int, vector<double>>& blp) {
  /*
  distributions: estimated gaussian distribution parameters of each bits in the whole sketch
  lfm: large flow map, which maps the flow key to its frequence
  blp: bit level probabilities, which maps flow key to bit level probabilities
  */
  // cout << "row " << r  << ", col " << c << endl;
  assert(r>=0&&r<this->R&&c>=0&&c<this->C);
  // initialize a map to store the large flows
  unordered_map<int, int> lfmt;
  unordered_map<int, vector<double>> blpt;
  // estinate bit-level probabilities.
  vector<double> phat;
  for (int k=1; k<=this->K; k++) {
    double ratio = 1.0*this->sketch[r][c][k]/this->sketch[r][c][0];
    // cout << r << ' ' << c << ' ' << k << ' ' << ratio << endl;
    if (ratio<theta) {
      phat.push_back(0.0);
    }
    else if (ratio>1-theta) {
      phat.push_back(1.0);
    }
    else {
      double mean = distributions[k-1][0];
      double variance = distributions[k-1][1];
      double std = sqrt(variance);
      double r1 = (mean-theta) / (1-theta);
      double r2 = mean / (1-theta);
      double d1 = fabs(r1-mean) / std;
      double d2 = fabs(r2-mean) / std;
      double p1 = gaussian_prob(d1) * mean;
      double p2 = gaussian_prob(d2) * (1-mean);
      phat.push_back(p1/(p1+p2));
    }
  }
  // build template bool representation
  vector<int> temp;
  for (int k=0; k<this->K; k++) {
    if (phat[k]>0.99) {
      temp.push_back(1);
    }
    else if (phat[k]<0.01) {
      temp.push_back(0);
    }
    else {
      temp.push_back(-1);
    }
  }
  // for (int ele : temp) {
  //   cout << ele << ' ';
  // }
  // cout << endl;
  // enumerate all flow keys which match the template
  unordered_set<int> candidates  = enumerateCandidates(temp);
  // find the flow keys whose hashed value equals j
  for (auto it=candidates.begin(); it!=candidates.end(); it++) {
    if (this->khfs.getHashedValue(*it, r)==c) {
      if (lfmt.find(*it)==lfmt.end()) {
        vector<int> f = intToVector(*it);
        // cout << "detect " << *it << endl;
        // estimate its frequence
        lfmt[*it] = estimateFrequence(r, c, f, distributions);
        // cout << "frequence of " << *it << ' ' << lfmt[*it] << endl;
        // associate bit-level probabilities
        vector<double> tv;
        for (int k=0; k<this->K; k++) {
          if (f[k]) {
            tv.push_back(phat[k]);
          }
          else {
            tv.push_back(1-phat[k]);
          }
        }
        blpt[*it] = tv;
        // verify candidate flowkeys
        for (int i=0; i<this->R; i++) {
          if (i!=r) {
            int ic = this->khfs.getHashedValue(*it, i);
            for (int kk=0; kk<this->K; kk++) {
              if (f[kk]==0 && this->sketch[i][ic][0]-this->sketch[i][ic][kk+1]<lfmt[*it]) {
                lfmt[*it] = this->sketch[i][ic][0]-this->sketch[i][ic][kk+1];
              }
              else if (f[kk]==1 && this->sketch[i][ic][kk+1]<lfmt[*it]) {
                lfmt[*it] = this->sketch[i][ic][kk+1];
              }
              // cout << kk << ' ' << lfmt[*it] << endl;
              // cout << this->sketch[i][c][0] << ' ' << this->sketch[i][c][kk] << endl;
            }
          }
        }
        // cout << "after verification " << endl;
        // cout << *it << ' ' << lfmt[*it] << endl;
        // cout << endl;
        if (lfmt[*it] >= theta*this->sketch[r][c][0]) {
          lfm[*it] = lfmt[*it];
          blp[*it] = blpt[*it];
        }
      }
    }
  }
}


int MultiLevelSketch::estimateFrequence(int r, int c, vector<int>& f, vector<vector<double>>& distributions) {
  vector<double> freqs;
  for (int k=0; k<this->K; k++) {
    double ratio = 1.0*this->sketch[r][c][k+1]/this->sketch[r][c][0];
    double sf;
    if (f[k]==1) {
      sf = ((ratio-(distributions[k][0])) / (1-distributions[k][0])) * this->sketch[r][c][0];
    }
    else {
      sf = (1-((ratio+1e-10)/(distributions[k][0]+1e-10))) * this->sketch[r][c][0];
    }
    // if (sf<0) {
    //   cout << "detection abnormal " << sf << ' ' << f[k] << ' ' << ratio << ' ' << distributions[k][0] << endl; 
    // }
    freqs.push_back(sf);
  }
  sort(freqs.begin(), freqs.end());
  return median(freqs);
}


unordered_set<int> MultiLevelSketch::enumerateCandidates(vector<int>& temp) {
  unordered_set<int> res;
  vector<int> tmp (temp.size(), 0);
  recursivelyEnumerateCandidates(temp, 0, tmp, res);
  return res;
}


void MultiLevelSketch::recursivelyEnumerateCandidates(vector<int>& temp, int pos, vector<int>& tmp, unordered_set<int>& res) {
  if (pos==temp.size()) {
    res.insert(vectorToInt(tmp));
    return;
  }
  if (temp[pos]!=-1) {
    tmp[pos] = temp[pos];
    recursivelyEnumerateCandidates(temp, pos+1, tmp, res);
  }
  else {
    tmp[pos] = 0;
    recursivelyEnumerateCandidates(temp, pos+1, tmp, res);
    tmp[pos] = 1;
    recursivelyEnumerateCandidates(temp, pos+1, tmp, res);
  }
  return;
}


void MultiLevelSketch::removeFlows(unordered_map<int, int>& lfm) {
  for (auto it=lfm.begin(); it!=lfm.end(); it++) {
    vector<int> v = intToVector(it->first);
    for (int r=0; r<this->R; r++) {
      int c = this->khfs.getHashedValue(it->first, r);
      this->sketch[r][c][0] -= it->second;
      for (int k=0; k<this->K; k++) {
        if (v[k]==1) {
          this->sketch[r][c][k+1] -= it->second;
        }
      }
    }
  }
}


bool MultiLevelSketch::terminate(int denominator, vector<vector<double>>& distributions) {
  cout << "check termination " << endl;
  int lessOne = 0;
  int lessTwo = 0;
  int lessThree = 0;
  for (int k=1; k<=this->K; k++) {
    double mean = distributions[k-1][0];
    double variance = distributions[k-1][1];
    double std = sqrt(variance);
    for (int r=0; r<this->R; r++) {
      for (int c=0; c<this->C; c++) {
        double ratio = 1.0*this->sketch[r][c][k] / this->sketch[r][c][0];
        double diff = abs(ratio-mean);
        // cout << diff << ' ' << std << endl;
        if (diff<=3*std) {
          lessThree++;
        }
        if (diff<=2*std) {
          lessTwo++;
        }
        if (diff<=std) {
          lessOne++;
        }
      }
    }
  }
  // cout << lessOne << ' ' << lessTwo << ' ' << lessThree << endl;
  double r1 = 1.0*lessOne/denominator;
  double r2 = 1.0*lessTwo/denominator;
  double r3 = 1.0*lessThree/denominator;
  cout << r1 << ' ' << r2 << ' ' << r3 << endl;
  if (r1>0.6826 && r2>0.9544 && r3>0.9973) {
    return true;
  }
  return false;
}


MultiLevelSketch::~MultiLevelSketch() {}