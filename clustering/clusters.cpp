#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "math.h"
#include <stdlib.h>
#include <fstream>
#include "../tools.h"
#include <algorithm>


using namespace std;
vector <string> data_names;
vector <vector<float>> data;
vector <vector<float>> matrix;
vector <vector<float>> dis_matrix;
pair <int,int> min_position;
float min_val=INFINITY;
float max_cal=-INFINITY;


///Data functions begin
void getData()
{
  std::fstream datafile;
  datafile.open("diauxic.txt");
  char c;
  string s="";
  float aux;
  data.resize(1);
  while(datafile.get(c))
  {
    if((c==' '||c=='\t')&&s!="")
    {
      if(data_names.size()<data.size())
      {
        data_names.push_back(s);
      }
      else
      {
        data[data.size()-1].push_back(stof(s));
      }
      //cout<<s<<'\t';
      s="";
    }
    else if(c=='\n')
    {
      data[data.size()-1].push_back(stof(s));
      s="";
      data.resize(data.size()+1);
    }
    else
    {
      s+=c;
    }
  }
  if(s!="")
  {
    data[data.size()-1].push_back(stof(s));
  }
  data.resize(data.size()-1);
}
void gen_Matrix()
{
  matrix.resize(data.size());
  for(int i=0;i<matrix.size();i++)
  {
    matrix[i].resize(data.size());
  }
  //cout<<data.size()<<endl;
  for (int i=0;i<data.size();i++)
  {
    for(int j=i+1;j<data.size();j++)
    {
      //cout<<"{"<<i<<", "<<j<<"}"<<endl;
      matrix[i][j]=euclidian_distance(data[i],data[j]);
      min_val=min(matrix[i][j],min_val);
      if(min_val==matrix[i][j])
      {
        min_position.first=i;
        min_position.second=j;
      }
    }
  }
}
///data functions end

///general functions begin
void get_new_min()
{
  min_val=INFINITY;
  for(int i=0;i<matrix.size();i++)
  {
    for(int j=0;j<matrix.size();j++)
    {
      if(matrix[i][j]!=0 && min(min_val,matrix[i][j])==matrix[i][j])
      {
        min_position.first=i;
        min_position.second=j;
        min_val=matrix[i][j];
      }
    }
  }
}

float get_min_for(int index)
{
  float min_local=INFINITY;
  for(int i=0;i<index;i++)
  {
    min_local=min(min_local,matrix[i][index]);
  }
  for(int i=index+1;i<matrix.size();i++)
  {
    min_local=min(min_local,matrix[min(i,index)][max(i,index)]);
  }
  return min_local;
}
///general functions end

///metodos aglomerativos begin
void agl_minimo(int cant_clust) ///single linkeage
{
  while(data_names.size()>cant_clust)
  {
    //cout<<min_val<<" en: ("<<min_position.first<<", "<<min_position.second<<")"<<endl;
    if (min_position.first>min_position.second)
    {
      float aux=min_position.first;
      min_position.first=min_position.second;
      min_position.second=aux;
    }
    for(int i=0;i<min_position.first;i++)
    {
      matrix[i][min_position.first]=min(matrix[i][min_position.first],matrix[i][min_position.second]);
    }
    for(int i=min_position.first+1;i<matrix.size();i++)
    {
      matrix[min_position.first][i]=min(matrix[min_position.first][i],matrix[min(i,min_position.second)][max(i,min_position.second)]);
    }
    /*
    cout<<'\t'<<"     "<<'\t';
    for(int i=0;i<matrix.size();i++)
    {
      cout<<data_names[i]<<"\t";
    }
    cout<<endl;
    for(int i=0;i<matrix.size();i++)
    {
      cout<<data_names[i]<<'\t'<<" -> "<<'\t';
      for(int j=0;j<matrix.size();j++)
      {
        cout<<matrix[i][j]<<'\t';
      }
      cout<<endl;
    }
    cout<<"_______________________________"<<endl;*/
    data_names[min_position.first]+=" "+data_names[min_position.second];
    data_names.erase(data_names.begin()+min_position.second);
    matrix.erase(matrix.begin()+min_position.second);
    for(int i=0;i<matrix.size();i++)
    {
      matrix[i].erase(matrix[i].begin()+min_position.second);
    }

/*
    cout<<'\t'<<"     "<<'\t';
    for(int i=0;i<matrix.size();i++)
    {
      cout<<data_names[i]<<"\t";
    }
    cout<<endl;
    for(int i=0;i<matrix.size();i++)
    {
      cout<<data_names[i]<<'\t'<<" -> "<<'\t';
      for(int j=0;j<matrix.size();j++)
      {
        cout<<matrix[i][j]<<'\t';
      }
      cout<<endl;
    }
    cout<<"_______________________________"<<endl;
*/
    get_new_min();
  }
}

void agl_maximo(int cant_clust) ///complete linkeage
{
  while(data_names.size()>cant_clust)
  {
    if (min_position.first>min_position.second)
    {
      float aux=min_position.first;
      min_position.first=min_position.second;
      min_position.second=aux;
    }
    for(int i=0;i<min_position.first;i++)
    {
      matrix[i][min_position.first]=max(matrix[i][min_position.first],matrix[i][min_position.second]);
    }
    for(int i=min_position.first+1;i<matrix.size();i++)
    {
      matrix[min_position.first][i]=max(matrix[min_position.first][i],matrix[min(i,min_position.second)][max(i,min_position.second)]);
    }
    data_names[min_position.first]+=" "+data_names[min_position.second];
    data_names.erase(data_names.begin()+min_position.second);
    matrix.erase(matrix.begin()+min_position.second);
    for(int i=0;i<matrix.size();i++)
    {
      matrix[i].erase(matrix[i].begin()+min_position.second);
    }

    get_new_min();
  }
}

void agl_promedio(int cant_clust) ///average
{
  while(data_names.size()>cant_clust)
  {
    if (min_position.first>min_position.second)
    {
      float aux=min_position.first;
      min_position.first=min_position.second;
      min_position.second=aux;
    }
    for(int i=0;i<min_position.first;i++)
    {
      matrix[i][min_position.first]=(matrix[i][min_position.first]+matrix[i][min_position.second])/2;
    }
    for(int i=min_position.first+1;i<matrix.size();i++)
    {
      matrix[min_position.first][i]=(matrix[min_position.first][i]+matrix[min(i,min_position.second)][max(i,min_position.second)])/2;
    }
    data_names[min_position.first]="("+data_names[min_position.first]+" , "+data_names[min_position.second]+")";
    data_names.erase(data_names.begin()+min_position.second);
    matrix.erase(matrix.begin()+min_position.second);
    for(int i=0;i<matrix.size();i++)
    {
      matrix[i].erase(matrix[i].begin()+min_position.second);
    }

    get_new_min();
  }
}
///metodos aglomerativos end

///metodos disociativos begin
void dis_minimo(int cant_clust)
{
  vector<vector <float>> dis_matrix;
  vector<string> auxiliar=data_names;
  dis_matrix.resize(matrix.size());
  float max_local=-INFINITY;
  int max_index;
  for(int i=0;i<dis_matrix.size();i++)
  {
    dis_matrix[i].push_back(get_min_for(i));
    dis_matrix[i].push_back(INFINITY);
    dis_matrix[i].push_back(INFINITY);
    max_local=max(max_local,dis_matrix[i][0]);
    if(max_local==dis_matrix[i][0])
    {
      max_index=i;
    }
  }
  for(int i=0;i<dis_matrix.size();i++)
  {
    cout<<auxiliar[i]<<'\t'<<dis_matrix[i][0]<<'\t'<<dis_matrix[i][1]<<'\t'<<dis_matrix[i][2]<<'\t'<<endl;
  }
  cout<<auxiliar[max_index]<<endl;

}

void dis_maximo(int cant_clust)
{

}

void dis_average(int cant_clust)
{

}
///metodos disociativos end


int main()
{
  cout<<"Cantidad de clusters: ";
  int cant,opc;
  cin>>cant;
  cout<<"Metodo"<<endl;
  cout<<'\t'<<"Aglomerativo"<<endl;
  cout<<'\t'<<'\t'<<"1. Single linkeage"<<endl;
  cout<<'\t'<<'\t'<<"2. Complete linkeage"<<endl;
  cout<<'\t'<<'\t'<<"3. Average"<<endl;
  cout<<'\t'<<"Disociativo"<<endl;
  cout<<'\t'<<'\t'<<"4. Single linkeage"<<endl;
  cout<<'\t'<<'\t'<<"5. Complete linkeage"<<endl;
  cout<<'\t'<<'\t'<<"6. Average"<<endl;
  cin>>opc;

/*
  for(int i=0;i<data.size();i++)
  {
    cout<<data_names[i]<<'\t';
    for(int j=0;j<data[i].size();j++)
    {
      cout<<data[i][j]<<'\t';
    }
    cout<<endl;
  }
*/

  getData();
  auto start = std::chrono::system_clock::now();
  gen_Matrix();


  cout<<"     ";
  for(int i=0;i<matrix.size();i++)
  {
    cout<<data_names[i]<<"\t";
  }
  cout<<endl;
  for(int i=0;i<matrix.size();i++)
  {
    cout<<data_names[i]<<" -> ";
    for(int j=0;j<matrix.size();j++)
    {
      cout<<matrix[i][j]<<'\t';
    }
    cout<<"<<<>>>>"<<endl;
  }


switch (opc) {
  case 1:
    agl_minimo(cant);
    break;
  case 2:
    agl_maximo(cant);
    break;
  case 3:
    agl_promedio(cant);
    break;
  case 4:
    dis_minimo(cant);
    break;
  case 5:
    dis_minimo(cant);
    break;
  case 6:
    dis_minimo(cant);
    break;
  }
  auto end = std::chrono::system_clock::now();
  double elapsed1 = std::chrono::duration_cast<std::chrono::duration<double> >(end - start).count();
  for(int i=0;i<data_names.size();i++)
  {
    cout<<"<<< "<<data_names[i]<<" >>>"<<'\t'<<endl;
  }
  cout<<"Time: "<<elapsed1<<"s"<<endl;



  return 0;
}