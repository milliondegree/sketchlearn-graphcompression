
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <math.h>
#include <random>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
#include "multi-level-sketch.h"

using namespace std;


int readFromFile(string& filepath, unordered_map<string, int>& freqs) {
  cout << "reading from file " << filepath << endl;
  ifstream fp(filepath);
  string line;
  int count = 0;

  while (getline(fp, line)) {
    if (!line.empty()) {
      vector<string> splits;
      boost::split(splits, line, boost::is_any_of(" "));
      if (freqs.find(splits[0])==freqs.end()) {
        freqs[splits[0]] = stoi(splits[1]);
        count++;
      }
      else {
        cout << "duplicate edges!" << endl;
        cout << splits[0] << ' ' << splits[1] <<endl;
      }
    }
  }
  return count;
}


void printVector(vector<int> v) {
  cout << '[';
  for (auto it=v.begin(); it!=v.end(); it++) {
    cout << *it << ' ';
  }
  cout << ']' << endl;
}


void print2DVector(vector<vector<int>> v) {
  cout << '[';
  for (int i=0; i<v.size(); i++) {
    printVector(v[i]);
  }
  cout << ']' << endl;
}


string generateRandomHexString(int length) {
  string s;
  for (int i=0; i<length; i++) {
    int t = rand()%16;
    s += hexTable[t];
  }
  return s;
}


// int main(int argc, char* argv[]) {

//   unordered_map<string, string> args;
//   vector<string> argvs;
//   for (int i=0; i<argc; i++) {
//     argvs.push_back(string(argv[i]));
//   }
//   for (int i=1; i<argc; i+=2) {
//     assert(i+1<argc);
//     if (argvs[i]=="-k") {
//       args["k"] = argvs[i+1];
//     }
//     else if (argvs[i]=="-m") {
//       args["m"] = argvs[i+1];
//     }
//     else if (argvs[i]=="-f") {
//       args["f"] = argvs[i+1];
//     }
//   }

//   // pre-process of the data
//   unordered_map<string, int> freqs;
//   unordered_map<string, int> largeflows;
//   unordered_map<string, int> n1s;
//   int flownumber = readFromFile(args["f"], freqs);
//   int totalnumber = 0;
//   for (auto it : freqs) {
//     totalnumber += it.second;
//   }
//   int thre = totalnumber/stoi(args["m"]);
   

//   // build hierarchy mls
//   int R = stoi(args["k"]);
//   // int C = stoi(args["m"]);
//   // int C = log(totalnumber);
//   int C = stoi(args["m"]);
//   cout << C << endl;
//   int K = 336;
//   MultiLevelSketch mls (R, C, K/2, 0);
//   KHashFunctions kfs = mls.getKHashFunctions();
//   vector<MultiLevelSketch> mlses;
//   for (int i=0; i<R*C; i++) {
//     MultiLevelSketch mlst (R, C, K/2, 0);
//     mlses.push_back(mlst);
//   }
//   for (auto it : freqs) {
//     string f = it.first;
//     int freq = it.second;
//     string n1 = f.substr(0, 42);
//     string n2 = f.substr(42, 84);
//     mls.feedFlowKey(n1, freq);
//     for (int r=0; r<R; r++) {
//       int c = kfs.getHashedValue(n1, r);
//       mlses[r*C+c].feedFlowKey(n2, freq);
//     }
//     // if it is large flow
//     if (freq>thre) {
//       largeflows.insert(it);
//     }
//     n1s[n1] += freq;
//   }

//   cout << "number of distinct flows: " << flownumber << endl;
//   cout << "total frequency: " << totalnumber << endl;
//   cout << "number of n1s: " << n1s.size() << endl;

//   unordered_map<string, int> lfm;
//   unordered_map<string, vector<double>> blp;
//   mls.modelInference(0.5, lfm, blp);
//   int match = 0;
//   for (auto it : n1s) {
//     if (lfm.find(it.first)!=lfm.end()) {
//       match++;
//     }
//   }
//   cout << "recall of n1s: " << 1.0*match/n1s.size() << endl;
//   cout << "precision of n1s: " << 1.0*match/(lfm.size()+1e-10) << endl;

//   match = 0;
//   vector<unordered_map<string, int>> lfms;
//   vector<unordered_map<string, vector<double>>> blps;
//   for (int r=0; r<R; r++) {
//     for (int c=0; c<C; c++) {
//       lfms.push_back(unordered_map<string, int> ());
//       blps.push_back(unordered_map<string, vector<double>> ());
//       mlses[r*C+c].modelInference(0.5, lfms[r*C+c], blps[r*C+c]);
//     }
//   }
//   for (auto it : n1s) {
//     string n1 = it.first;
//     int freq = it.second;
//     unordered_map<string, int> lfmt;
//     int c0 = kfs.getHashedValue(n1, 0);
//     for (auto itt : lfms[c0]) {
//       string n2 = itt.first;
//       bool flag = true;
//       for (int r=1; r<R; r++) {
//         int c = kfs.getHashedValue(n1, r);
//         if (lfms[r*C+c].find(n2)==lfms[r*C+c].end()) {
//           flag = false;
//           break;
//         }
//       }
//       if (flag) {
//         lfmt[n1+itt.first] = itt.second;
//       }
//     }
//     for (auto itt : lfmt) {
//       if (freqs.find(itt.first)!=freqs.end()) {
//         match++;
//       }
//     }
//   }
//   cout << "total recall: " << match*1.0/freqs.size();
// }







// int main(int argc, char* argv[]) {

//   unordered_map<string, string> args;
//   vector<string> argvs;
//   for (int i=0; i<argc; i++) {
//     argvs.push_back(string(argv[i]));
//   }
//   for (int i=1; i<argc; i+=2) {
//     assert(i+1<argc);
//     if (argvs[i]=="-k") {
//       args["k"] = argvs[i+1];
//     }
//     else if (argvs[i]=="-m") {
//       args["m"] = argvs[i+1];
//     }
//     else if (argvs[i]=="-f") {
//       args["f"] = argvs[i+1];
//     }
//   }

//   // pre-process of the data
//   unordered_map<string, int> freqs;
//   int flownumber = readFromFile(args["f"], freqs);
//   int totalnumber = 0;
//   for (auto it : freqs) {
//     totalnumber += it.second;
//   }
//   int thre = totalnumber/stoi(args["m"]);
   

//   // build distributed mls
//   unordered_map<string, MultiLevelSketch> mlses;
//   unordered_map<string, unordered_map<string, int>> freqOfNode;
//   for (auto it : freqs) {
//     string f = it.first;
//     int freq = it.second;
//     string node = f.substr(0, 2);
//     if (mlses.find(node)==mlses.end()) {
//       mlses[node] = MultiLevelSketch(stoi(args["k"]), stoi(args["m"]), 336, 0);
//       freqOfNode[node] = unordered_map<string, int> ();
//     }
//     mlses[node].feedFlowKey(f, freq);
//     freqOfNode[node][f] = freq;
//   }

//   cout << "number of distinct flows: " << flownumber << endl;
//   cout << "total frequency: " << totalnumber << endl;

//   for (auto it : mlses) {
//     int match = 0;
//     int largematch = 0;
//     string node = it.first;
//     cout << "node " << node << endl;
//     cout << "total flow number: " << freqOfNode[node].size() << endl;
//     int totalfreq = 0;
//     for (auto it : freqOfNode[node]) {
//       totalfreq += it.second;
//     }
//     int thre = totalfreq*1.0/stoi(args["m"]);
//     unordered_map<string, int> largeflows;
//     int largeflownumber = 0;
//     for (auto it : freqOfNode[node]) {
//       if (it.second>thre) {
//         largeflownumber++;
//         largeflows.insert(it);
//       }
//     }
//     cout << "threshold: " << thre << ", number of largeflows: " << largeflownumber << endl;

//     unordered_map<string, int> lfm;
//     unordered_map<string, vector<double>> blp;
//     mlses[node].modelInference(0.5, lfm, blp);
//     for (auto it : largeflows) {
//       string f = it.first;
//       int freq = it.second;
//       int diff = abs(freq-freqOfNode[node][f]);
//       if (lfm.find(f)!=lfm.end()&&
//           diff*1.0/freqOfNode[node][f]<1.0
//           ) {
//         largematch++;
//       }
//     }
//     cout << "large flow recall: " << 1.0*largematch/(largeflows.size()+1e-10) << endl;
//     for (auto it : freqOfNode[node]) {
//       string f = it.first;
//       int freq = it.second;
//       if (lfm.find(f)!=lfm.end()
//           // &&abs(lfm[f]-freq)>=1
//           ) {
//         match++;
//       }
//     }
//     cout << "flowkey types: " << freqOfNode[node].size() << ' ' << "extracted: " << match << endl;
//     double recall = match*1.0/freqOfNode[node].size();
//     // if (recall<1) {
//     //   cout << "recall: " << recall << endl;
//     //   for (auto it : freqOfNode[node]) {
//     //     if (abs(it.second-lfm[it.first])>=1) {
//     //       cout << "FN: " << it.first << ' ' << it.second << ' ' << lfm[it.first] << endl;
//     //     }
//     //   }
//     //   mlses[node].printSketchTable();
//     // }
//     for (auto it : lfm) {
//       if (freqOfNode[node].find(it.first)==freqOfNode[node].end()) {
//         cout << "FP: " << it.first << ' ' << it.second << endl;
//       }
//     }
//     cout << endl;
//   }
// }













int main(int argc, char* argv[]) {
  cout << "program start" << endl;
  unordered_map<string, string> args;
  vector<string> argvs;
  for (int i=0; i<argc; i++) {
    argvs.push_back(string(argv[i]));
  }
  for (int i=1; i<argc; i+=2) {
    assert(i+1<argc);
    if (argvs[i]=="-k") {
      args["k"] = argvs[i+1];
    }
    else if (argvs[i]=="-m") {
      args["m"] = argvs[i+1];
    }
    else if (argvs[i]=="-f") {
      args["f"] = argvs[i+1];
    }
    else if (argvs[i]=="-p") {
      args["p"] = argvs[i+1];
    }
  }

  if (args.find("f")!=args.end()) {
    // pre-process of the data
    unordered_map<string, int> freqs;
    unordered_map<string, int> largeflows;
    int flownumber = readFromFile(args["f"], freqs);
    int totalnumber = 0;
    for (auto it : freqs) {
      totalnumber += it.second;
    }
    cout << "number of distinct flows: " << flownumber << endl;
    cout << "total frequency: " << totalnumber << endl; 

    int R = stoi(args["k"]);
    // vector<int> fs;
    // for (auto it : freqs) {
    //   fs.push_back(it.second);
    // }
    // sort(fs.begin(), fs.end());
    // int accu = 0;
    // int thre;
    // for (int i=fs.size()-1; i>=0; i--) {
    //   accu += fs[i];
    //   thre = fs[i]-1;
    //   if (accu*1.0/totalnumber>=stod(args["p"])) {
    //     break;
    //   }
    // }
    // int C = totalnumber/thre;
    int C = stoi(args["m"]);
    int thre = totalnumber/C;
    cout << "target thre: " << thre << " c: " << C << endl;
    MultiLevelSketch mls(R, C, 336, 0);
    KHashFunctions kfs = mls.getKHashFunctions();
    for (auto it : freqs) {
      if (it.second>thre) {
        // cout << "large flow: " << it.first << ' ' << it.second << endl;
        largeflows.insert(it);
      }
      mls.feedFlowKey(it.first, it.second);
    }
    cout << "number of large flows: " << largeflows.size() << endl;

    unordered_map<string, int> lfm;
    unordered_map<string, vector<double>> blp;
    mls.modelInference(0.5, lfm, blp);
    // cout << endl << "extracted large flows: " << endl; 
    // for (auto it : lfm) {
    //   if (it.second>thre) {
    //     cout << it.first << ' ' << it.second << endl;
    //   }
    // }
    // cout << endl;
    cout << "number of extracted flows: " << lfm.size() << endl;
    cout << endl;

    vector<vector<vector<int>>> sketch = mls.getSketch();
    // for (int i=0; i<sketch.size(); i++) {
    //   for (int j=0; j<sketch[0].size(); j++) {
    //     cout << sketch[i][j][0] << ' ';
    //   }
    //   cout << endl;
    // }
    // cout << endl;
    vector<vector<vector<int>>> sketchbackup = mls.getSketchBackUp();
    // for (int i=0; i<sketchbackup.size(); i++) {
    //   for (int j=0; j<sketchbackup[0].size(); j++) {
    //     cout << sketchbackup[i][j][0] << ' ';
    //   }
    //   cout << endl;
    // }
    // cout << endl;

    int largematch = 0;
    for (auto it : largeflows) {
      if (lfm.find(it.first)!=lfm.end()) {
        largematch++;
        int diff = abs(it.second-lfm[it.first]);
        if (1.0*diff/it.second>0.1) {
          cout << "freq estimate error: " << it.first << ' ' << it.second << ' ' << lfm[it.first] << endl;
          // string key = it.first;
          // int c = kfs.getHashedValue(key, 0);
          // cout << c << endl;
          // for (int i=0; i<=320; i++) {
          //   cout << sketch[0][c][i] << ' ';
          // }
          // cout << endl;
        }
      }
      else {
        cout << "failed to extract error: " << it.first << ' ' << it.second << endl; 
      }
    }
    cout << "large flow recall: " << 1.0*largematch/(largeflows.size()+1e-10) << endl;

    int match = 0;
    for (auto it : freqs) {
      if (lfm.find(it.first)!=lfm.end()) {
        match++;
        int diff = abs(it.second-lfm[it.first]);
        if (1.0*diff/it.second>0.1) {
          // cout << "freq estimate error: " << it.first << ' ' << it.second << ' ' << lfm[it.first] << endl;
        }
        cout << it.first << ' ' << it.second << ' ' << lfm[it.first] << endl;
      }
      else {
        cout << "failed to extract error: " << it.first << ' ' << it.second << endl; 
      }
    }
    cout << "total flow recall: " << 1.0*match/(freqs.size()+1e-10) << endl;
    cout << "total flow precision: " << 1.0*match/(lfm.size()+1e-10) << endl;
    cout << endl;
  }
  else {
    MultiLevelSketch mls(stoi(args["k"]), stoi(args["m"]), 160, 0);
    mt19937 rng(current_time_nanoseconds());
    // generate random flowkeys
    srand(current_time_nanoseconds());
    // unordered_map<int, int> smallflows;
    unordered_map<string, int> smallflows;
    for (int i=0; i<200; i++) {
      // int key = rand()%(RAND_MAX);
      string key = generateRandomHexString(40);
      int times = rand()%3+1;
      // int times = 0;
      if (smallflows.find(key)==smallflows.end()) {
        for (int i=0; i<times; i++) {
          mls.feedFlowKey(key);
        }
        smallflows[key] = times;
      }
    }

    // unordered_map<int, int> largeflows;
    unordered_map<string, int> largeflows;
    for (int i=0; i<3; i++) {
      // int largeFlow = rand()%(RAND_MAX);
      string largeFlow = generateRandomHexString(40);
      int times = 500*(i+1);
      if (largeflows.find(largeFlow)==largeflows.end()) {    
        for (int j=0; j<times; j++) {
          mls.feedFlowKey(largeFlow);
        }
        largeflows[largeFlow] = times;
        cout << largeFlow << ' ' << times << endl;
      }
    }

    int totalnumber = 0;
    for (auto it=smallflows.begin(); it!=smallflows.end(); it++) {
      totalnumber += it->second;
    }
    for (auto it=largeflows.begin(); it!=largeflows.end(); it++) {
      totalnumber += it->second;
    }
    cout << "total frequency: " << totalnumber << endl; 
    cout << "minimum frequency to be large flow: " << totalnumber/stoi(args["m"]) << endl;

    cout << "Print sketch of level 0" << endl;
    vector<vector<vector<int>>> sketch = mls.getSketch();
    for (int i=0; i<sketch.size(); i++) {
      for (int j=0; j<sketch[0].size(); j++) {
        cout << sketch[i][j][0] << ' ';
      }
      cout << endl;
    }
    cout << endl;

    unordered_map<string, int> lfm;
    unordered_map<string, vector<double>> blp;
    mls.modelInference(0.5, lfm, blp);
    cout << endl << "extracted large flows: " << endl; 
    for (auto it=lfm.begin(); it!=lfm.end(); it++) {
      cout << it->first << ' ' << it->second << endl;
    }
    cout << endl;
    cout << "number of extracted large flows: " << lfm.size() << endl;
  }
}


