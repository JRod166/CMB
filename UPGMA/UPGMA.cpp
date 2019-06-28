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
vector <vector<float>> data;
vector <pair <string,int>> data_names;
vector <vector<float>> matrix;
float min_val=INFINITY;
pair <int,int> min_position;
vector < tuple < string,string,float > > branches;
ofstream file;
/*
vector <vector<float>> dis_matrix;
float max_cal=-INFINITY;
*/
///Data functions begin
void getData()
{
  std::fstream datafile;
  datafile.open("upgma.txt");
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
        data_names.push_back(make_pair(s,1));
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

float round_(float var)
{
    // 37.66666 * 100 =3766.66
    // 3766.66 + .5 =37.6716    for rounding off value
    // then type cast to int so value is 3766
    // then divided by 100 so the value converted into 37.66
    float value = (int)(var * 100 + .5);
    return (float)value / 100;
}

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
///general functions end

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
    file<<'"'<<data_names[min_position.first].first+" "+data_names[min_position.second].first<<'"'<<" -- {"<<'"'<<data_names[min_position.first].first<<'"';
    if(data_names[min_position.first].first.size()<=8)
    {
      file<<" [shape=plaintext]";
    }
    file<<"}"<<endl;//<<"[headlabel="<<'"'<<matrix[min_position.first][min_position.second]/2<<'"'<<"]"<<endl;
    file<<'"'<<data_names[min_position.first].first+" "+data_names[min_position.second].first<<'"'<<" -- {"<<'"'<<data_names[min_position.second].first<<'"';
    if(data_names[min_position.second].first.size()<=8)
    {
      file<<" [shape=plaintext]";
    }
    file<<"}"<<endl;//<<"[headlabel="<<'"'<<round_(matrix[min_position.first][min_position.second]/2)<<'"'<<"]"<<endl;
    //branches.push_back(make_tuple(data_names[min_position.first].first,data_names[min_position.second].first,matrix[min_position.first][min_position.second]));
    for(int i=0;i<min_position.first;i++)
    {
      matrix[i][min_position.first]=(matrix[i][min_position.first]+matrix[i][min_position.second])/2;
    }
    for(int i=min_position.first+1;i<matrix.size();i++)
    {
      matrix[min_position.first][i]=(matrix[min_position.first][i]+matrix[min(i,min_position.second)][max(i,min_position.second)])/2;
    }
    data_names[min_position.first].first+=" "+data_names[min_position.second].first;
    data_names[min_position.first].second+=data_names[min_position.second].second;
    data_names.erase(data_names.begin()+min_position.second);
    matrix.erase(matrix.begin()+min_position.second);
    for(int i=0;i<matrix.size();i++)
    {
      matrix[i].erase(matrix[i].begin()+min_position.second);
    }

    get_new_min();
  }
}


int main()
{
  file.open("upgma.dot");
  file<<"graph G { node [shape=point]\n";
  getData();
  auto start = std::chrono::system_clock::now();
  gen_Matrix();
  auto end = std::chrono::system_clock::now();
  double elapsedgen = std::chrono::duration_cast<std::chrono::duration<double> >(end - start).count();
/*  vector <string> names;
  for (int i =0;i<data_names.size();i++)
  {
    names.push_back(data_names[i].first);
  }
  */
  start = std::chrono::system_clock::now();
  agl_promedio(1);
  end = std::chrono::system_clock::now();
  double elapsedalg = std::chrono::duration_cast<std::chrono::duration<double> >(end - start).count();
  file<<"}";
  file.close();
  start = std::chrono::system_clock::now();
  system("dot -Tpdf upgma.dot -o upgma.pdf");
  system("xdg-open upgma.pdf");
  end = std::chrono::system_clock::now();
  double elapsedplot = std::chrono::duration_cast<std::chrono::duration<double> >(end - start).count();
  cout<< "Distancias: "<<elapsedgen<<endl;
  cout<< "Algoritmo: "<<elapsedalg<<endl;
  cout<< "Ploteo: "<<elapsedplot<<endl;
  return 0;
}
