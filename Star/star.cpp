#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "../tools.h"
#include <algorithm>
/*
g++ -o star star.cpp -pthread -std=c++11
*/

///DEFINES///
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
/*
      2= left GAP
      3= top GAP
      4= diag MATCH/MISMATCH
      5= left+top
      6= left+diag
      7= top+diag
      9= left+top+diag
                                  */

///GLOBAL///
vector<NW> Matrix;
vector <tuple<string,string,int> > Alignments;
int i,j;
string first,second;
vector <pair <vector<NW>,int>> Results;
std::mutex mtx,mtx_max;

///STRUCTS///
struct thread_pos
{
  int position;
  bool orientation;
  int iteration;
};
struct alignment
{
  int position;
  pair <string,string> aligned;
};

///FUNCTIONS///
NW fill(int pos, int match_value)
{
  int top,left,diag,maxim,trace=0;
  top=Matrix[pos-(j+1)].first+GAP;
  left=Matrix[pos-1].first+GAP;
  diag=Matrix[pos-(j+2)].first+match_value;
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

void PrintMatrix()
{
    for(int x=0;x<=i;x++)
    {
        for (int y=0;y<=j;y++)
        {
            cout<<Matrix[x*(j+1)+y].first<<"\t";
        }
        cout<<endl;
    }
}

void fillMatrix(thread_pos data)
{
  int pos=data.position,match_value;
  if(data.orientation)
  {
    pos++;
    while(pos%(j+1)!=0) //horizonatal
    {
      if(first[pos/(j+1)-1]==second[pos%(j+1)-1])
      {
        match_value=MATCH;
      }
      else
      {
        match_value=MISMATCH;
      }
      Matrix[pos]=fill(pos,match_value);
      pos++;
    }
  }
  else
  {
    pos+=j+1;
    while(pos<=Matrix.size())
    {
      if(first[pos/(j+1)-1]==second[pos%(j+1)-1])
      {
        match_value=MATCH;
      }
      else
      {
        match_value=MISMATCH;
      }
      Matrix[pos]=fill(pos,match_value);
      pos+=j+1;
    }
  }
}
//int contadorr=0;
void Align(alignment data)
{
  /*//mtx_max.lock();
  cout<<thread_max<<endl;
  //mtx_max.unlock();*/
  struct alignment data2,data3;
  //thread firstt,secondt,thirdt;
  //cout<<data.aligned.first<<endl<<data.aligned.second<<endl;
  //cout<<"{"<<data.position<<"}"<<endl;
  int a=Matrix[data.position].second;
  //cout<<data.position<<endl;
  /*switch (Matrix[data.position].second)
  {
    case DONE:*/
    if(a==DONE)
    {
      //cout<<data.aligned.first<<endl<<data.aligned.second<<endl;
      mtx.lock();
      //cout<<"{"<<contador++<<"}"<<endl;
      //cout<<"ok"<<endl;
      Alignments.push_back(make_tuple(data.aligned.first,data.aligned.second,0));
      mtx.unlock();
      return;
      //break;
    }
    //case LEFTGAP:
      //cout<<(data->position+1)/(i+1)-2<<endl;
      //cout<<second[(data->position+1)/(i+1)-2]<<endl;
    if(a==LEFTGAP)
      {
      data.aligned.first=second[((data.position)%(j+1))-1]+data.aligned.first; //arriba
      data.aligned.second="-"+data.aligned.second;                             //abajo
      data.position=data.position-1;
      Align(data);
      //break;
    }
    //case TOPGAP:
      //data.aligned.second=data.aligned.second+
      if(a==TOPGAP)
      {data.aligned.first="-"+data.aligned.first; //arriba
      data.aligned.second=first[(data.position/(j+1))-1]+data.aligned.second;                             //abajo
      data.position=data.position-(j+1);
      Align(data);
      return;
      //break;
    }
    //case DIAG:
      if(a==DIAG)
      /*cout<<data.position<<endl;
      cout<<(data.position+1)/(i+1)-2<<endl;
      cout<<data.position%(j+1)<<endl;*/
      {
      data.aligned.first=second[(data.position)%(j+1)-1]+data.aligned.first;
      //cout<<"->"<<second[(data.position)%(i+1)-1]<<endl;
      data.aligned.second=first[(data.position/(j+1))-1]+data.aligned.second;
      data.position=data.position-(j+2);
      Align(data);
      //cout<<data.position<<endl;

      //data.aligned.first=data.aligned.first+
      //break;
    }
    //case LEFTTOP:
    if(a==LEFTTOP)
      {data2=data;
      ///LEFT
      //cout<<"LEFTTOP"<<endl;
      data.aligned.first=second[((data.position)%(j+1))-1]+data.aligned.first; //arriba
      data.aligned.second="-"+data.aligned.second;                             //abajo
      data.position=data.position-1;
      //thread firstt(Align,data);
      Align(data);
      ///TOP
      data2.aligned.first="-"+data2.aligned.first; //arriba
      data2.aligned.second=first[(data2.position/(j+1))-1]+data2.aligned.second;                             //abajo
      data2.position=data2.position-(j+1);
      //mtx_max.lock();
      /*thread firstt(Align,data2);
      firstt.join();*/
      Align(data2);
    }
    if(a==LEFTDIAG)
    {
      //cout<<"LEFTDIAG"<<endl;
      data2=data;
      ///LEFT
      data.aligned.first=second[((data.position)%(j+1))-1]+data.aligned.first; //arriba
      data.aligned.second="-"+data.aligned.second;
      //cout<<data.aligned.first<<"..."<<data.aligned.second<<endl;                        //abajo
      data.position=data.position-1;
      //thread firstt(Align,data);
      Align(data);
      ///DIAG
      data2.aligned.first=second[(data2.position%(j+1))-1]+data2.aligned.first;
      data2.aligned.second=first[(data2.position/(j+1))-1]+data2.aligned.second;
      //  cout<<second[(data2.position+1)/(i+1)-2]<<"-"<<first[(data2.position%(j+1))-1]<<endl;
      //cout<<data2.aligned.first<<"..."<<data2.aligned.second<<endl;                        //abajo
      //cout<<first[(data2.position%(j+1))-2]<<endl;
      data2.position=data2.position-(j+2);
      //mtx_max.lock();
      /*thread firstt(Align,data2);
      firstt.join();*/
      Align(data2);
    }
    /*case LEFTDIAG:
      break;*/
    if(a==TOPDIAG)
    {
      //cout<<"TOPDIAG"<<"{"<<data.position<<"}"<<endl;
      data2=data;
      ///TOP
      data.aligned.first="-"+data.aligned.first; //arriba
      data.aligned.second=first[(data.position/(j+1))-1]+data.aligned.second;                             //abajo
      data.position=data.position-(j+1);
      //cout<<data.position;
      //thread firstt(Align,data);
      Align(data);
      ///DIAG
      data2.aligned.first=second[(data2.position%(j+1))-1]+data2.aligned.first;
      data2.aligned.second=first[(data2.position/(j+1))-1]+data2.aligned.second;
      data2.position=data2.position-(j+2);
      //cout<<"->"<< data2.position<<endl;
      //mtx_max.lock();
      /*thread firstt(Align,data2);
      firstt.join();*/
      Align(data2);
    }
    /*case TOPDIAG:
      break;
    case ALL:
      break;
  }*/
    if(a==ALL)
    {
      data2=data;
      data3=data;
      //cout<<"ALL -> "<<data.position-(j+1)<<" -> "<<data2.position-(j+2)<<" -> "<<data3.position-1<<endl;
      ///TOP
      data.aligned.first="-"+data.aligned.first; //arriba
      data.aligned.second=first[(data.position/(i+1))-1]+data.aligned.second;                             //abajo
      data.position=data.position-(j+1);
      //thread firstt(Align,data);
      Align(data);
      ///DIAG
      data2.aligned.first=second[(data2.position+1%(j+1))-1]+data2.aligned.first;
      data2.aligned.second=first[(data2.position/(i+1))-1]+data2.aligned.second;
      data2.position=data2.position-(j+2);
      //thread secondt (Align,data2);
      Align(data2);
      ///LEFT
      data3.aligned.first=second[((data3.position)%(j+1))-1]+data3.aligned.first; //arriba
      data3.aligned.second="-"+data3.aligned.second;                             //abajo
      data3.position=data3.position-1;
      //mtx_max.lock();
      /*thread firstt(Align,data3);
      firstt.join();*/
      Align(data3);
    }
}

void PrintTrace()
{
    for(int x=0;x<=i;x++)
    {
        for (int y=0;y<=j;y++)
        {
            cout<<Matrix[x*(j+1)+y].first<<"\t";
        }
        cout<<endl;
    }
}

void NeedWuns(string first, string second)
{
  //cout<<first<<"-"<<second<<endl;
  Matrix.resize(0);
  struct thread_pos t_p[2];
  i=first.size();
  j=second.size();
  Matrix.resize((i+1)*(j+1));
  Matrix[0]=make_pair(0,-1);
  for (int x=1;x<=j;x++)
  {
      Matrix[x]=make_pair(Matrix[(x-1)].first+GAP,LEFTGAP);
  }
  for(int y=1;y<=i;y++)
  {
    Matrix[y*(j+1)]=make_pair(Matrix[(y-1)*(j+1)].first+GAP,TOPGAP);
  }
  for(int z=1;z<=min(i,j);z++)
  {
    if(first[z-1]==second[z-1])
    {
      Matrix[(j+1)*z+z]=fill((j+1)*z+z,MATCH);
    }
    else
    {
      Matrix[(j+1)*z+z]=fill((j+1)*z+z,MISMATCH);
    }

    for(int a=0;a<2;a++)
    {
      t_p[a].position=((j+1)*z+z);
      t_p[a].orientation=a;
      t_p[a].iteration=z;
    }
    thread firstt(fillMatrix,t_p[0]);
    thread secondt(fillMatrix,t_p[1]);
    firstt.join();
    secondt.join();
  }
}
int get_center(int cant)
{
  vector<int> sumas;
  int sum=0;
  for(int x=0;x<=cant-1;x++)
  {
      for (int y=0;y<=cant-1;y++)
      {
          sum+=Results[x*(cant)+y].second;
      }
      sumas.push_back(sum);
      sum=0;
  }
  return max_element(sumas.begin(),sumas.end()) - sumas.begin();
}
void GapFill(string *sequence, int size)
{
  while(sequence->size()<size)
  {
    *sequence+='-';
  }
}

int sps(std::vector<string> rptaS)
{
  int score = 0;

  for (size_t s = 0; s < rptaS[0].size(); s++) {
    //por cada fila
    for (size_t i = 0; i < rptaS.size(); i++) {
      //std::cout << "/* "<< rptaS[i][s]<<" */" << '\n';
      for (size_t j = i+1; j < rptaS.size(); j++) {
        if (rptaS[i][s]=='-' && rptaS[j][s]=='-') {

        }else if (rptaS[i][s]=='-' || rptaS[j][s]=='-') {
          score-=2;
        }else if (rptaS[i][s]==rptaS[j][s]) {
          score+=1;
        }else if (rptaS[i][s]!=rptaS[j][s]) {
          score-=1;
        }
      }
    }
    //std::cout << "score: "<<score << '\n';
  }
  return score;
}

using namespace std;

int main()
{
  vector <string> secuences;
  int cant,center;
  cin>>cant;
  secuences.resize(cant);
  Results.resize(cant*cant);
  for (int i=0;i<cant;i++)
  {
    cin>>secuences[i];
  }
  auto start = std::chrono::system_clock::now();
  for (int i=0;i<cant-1;i++)
  {
    for(int j=i+1;j<cant;j++)
    {
      first=secuences[i];
      second=secuences[j];
      NeedWuns(first,second);
      Results[i*(cant)+j]=make_pair(Matrix,Matrix[Matrix.size()-1].first);
      Results[j*(cant)+i]=make_pair(vector<NW>(),Matrix[Matrix.size()-1].first);
    }
  }
  /*for(int x=0;x<cant;x++)
  {
      for (int y=0;y<cant;y++)
      {
          cout<<Results[x*(cant)+y].second<<"\t";
      }
      cout<<endl;
  }*/
  Matrix.clear();
  Matrix.shrink_to_fit();
  Matrix.resize(0);
  int max_sequence=0;
  vector <string> finalSequences;
  center=get_center(cant);
  cout<<"Centro: "<<center<<endl;
  first=secuences[center];
  j=first.size();


  for (int y=0; y<center;y++)
  {
    Matrix.clear();
    Matrix.shrink_to_fit();
    Matrix.resize(0);
    second=secuences[y];
    i=second.size();
    Matrix=Results[center+y*cant].first;
    //cout<<center+y*cant<<": "<<Matrix[Matrix.size()-1].first<<endl;
    struct alignment al[1];
    al[0].position=Matrix.size()-1;
    al[0].aligned=make_pair("","");
    //cout<<"Struct creado"<<endl;
    //PrintTrace();
    Align(al[0]);
    //cout<<"Aligned"<<endl;
    int aux=get<0>(Alignments[0]).size();
    max_sequence=max(max_sequence,aux);
    finalSequences.push_back(get<0>(Alignments[0]));
    Alignments.clear();
    Alignments.shrink_to_fit();
    Alignments.resize(0);
  }
  cout<<"first: "<<first<<endl;
  finalSequences.push_back(first);
  i=first.size();
  for (int y=center+1;y<cant;y++)
  {
    second=secuences[y];
    j=second.size();
    Matrix=Results[center*cant+y].first;
    cout<<Matrix[Matrix.size()-1].first<<endl;
    struct alignment al[1];
    al[0].position=Matrix.size()-1;
    al[0].aligned=make_pair("","");
    //cout<<"Struct creado"<<endl;
    //PrintTrace();
    Align(al[0]);
    //cout<<"Aligned"<<endl;
    int aux=get<0>(Alignments[0]).size();
    max_sequence=max(max_sequence,aux);
    finalSequences.push_back(get<0>(Alignments[0]));
    Alignments.clear();
    Alignments.shrink_to_fit();
    Alignments.resize(0);
  }
    //cout<<Results[center*cant+y].second<<"\t";
  for(int x=0;x<finalSequences.size();x++)
  {
    GapFill(&finalSequences[x],max_sequence);
    cout<<finalSequences[x]<<endl;
  }
cout<<"Score: "<<sps(finalSequences)<<endl;
auto end = std::chrono::system_clock::now();
  double elapsed = std::chrono::duration_cast<std::chrono::duration<double> >(end - start).count();
  std::cout << "tiempo " <<to_string(elapsed)<< '\n';
  /*Results.clear();
  Results.resize()*/
  return 0;
}
