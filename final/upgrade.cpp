#include "upgrade.h"


NW NeedWuns_2::fill(int pos, int match_value)
{
  int top,left,diag,maxim,trace=0;
  top=ScoreMatrix[pos-(j+1)]+GAP;
  left=ScoreMatrix[pos-1]+GAP;
  diag=ScoreMatrix[pos-(j+2)]+match_value;
  maxim=max(top,max(left,diag));
  if(maxim==top)
  {
    trace+=TOPGAP;
  }
  if(maxim==left)
  {
    trace+=LEFTGAP;
  }
  if(maxim==diag)
  {
    trace+=DIAG;
  }
  return make_pair(maxim,trace);
}

NeedWuns_2::NeedWuns_2(string first_s, string second_s)
{
  first=first_s;
  second=second_s;
  i=first.size();
  j=second.size();
  Matrix.resize((i+1)*(j+1));
  ScoreMatrix.resize((j+1)*2);
}

vector<int> NeedWuns_2::gen_matrix()
{
  Matrix[0]=-1;
  ScoreMatrix[0]=0;
  pair<int,int>aux;
  for (int x=1;x<=j;x++)
  {
      ScoreMatrix[x]=ScoreMatrix[x-1]+GAP;  ///first row
      Matrix[x]=LEFTGAP;
  }
  for(int z=1;z<=i;z++)
  {
    Matrix[(j+1)*z]=TOPGAP;
    ScoreMatrix[j+1]=ScoreMatrix[0]+GAP;
    for (int x=2;x<=j+1;x++)//second
    {
      if(first[z-1]==second[x-2])
      {

        aux=fill(x+j,MATCH);
      }
      else
      {
        aux=fill(x+j,MISMATCH);
      }
      ScoreMatrix[x+j]=aux.first;
      Matrix[(j+1)*z+x-1]=aux.second;
    }
    ScoreMatrix.erase(ScoreMatrix.begin(),ScoreMatrix.begin()+j+1);
    ScoreMatrix.resize((j+1)*2);
  }
  return Matrix;
}
