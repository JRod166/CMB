#ifndef UPGRADE_H
#define UPGRADE_H

#include <iostream>
#include <vector>
#include <string>

#define GAP -2
#define MATCH +1
#define MISMATCH -1
#define LEFTGAP 2
#define TOPGAP 3
#define DIAG 4
#define LEFTTOP 5
#define LEFTDIAG 6
#define TOPDIAG 7
#define ALL 9
#define DONE -1
using namespace std;

typedef pair<int,int> NW; //score, trace

class NeedWuns_2{
public:
  vector <int> Matrix;
  vector <int> ScoreMatrix;
  int i;
  int j;
  string first;
  string second;
  NeedWuns_2(string,string);
  vector <int> gen_matrix();
  NW fill(int,int);

};

#endif
