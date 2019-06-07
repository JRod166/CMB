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
vector <pair <string,int>> data_names;
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

float get_min_distance(vector <pair <string,int>> clusters, int index)
{
  float min_local=INFINITY;
  for(int i=0;i<clusters.size();i++)
  {
    min_local=min(min_local,matrix[min(index,clusters[i].second)][max(index,clusters[i].second)]);
  }
  return min_local;
}

float get_max_distance(vector <pair <string,int>> clusters, int index)
{
  float max_local=-INFINITY;
  for(int i=0;i<clusters.size();i++)
  {
    max_local=max(max_local,matrix[min(index,clusters[i].second)][max(index,clusters[i].second)]);
  }
  return max_local;
}

float get_avg_distance(vector <pair <string,int>> clusters, int index)
{
  float acumulado=0;
  for(int i=0;i<clusters.size();i++)
  {
    acumulado+=matrix[min(index,clusters[i].second)][max(index,clusters[i].second)];
  }
  return acumulado/clusters.size();
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
    data_names[min_position.first].first+=" "+data_names[min_position.second].first;
    data_names[min_position.first].second+=data_names[min_position.second].second;
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
///metodos aglomerativos end

///metodos disociativos begin
vector<pair <string,int>> dis_minimo(int cant_clust)
{
  vector <pair <string,int>> auxiliar;  //matriz pequeña Nombre de elemento-indice en matriz de distancias
  vector <vector <pair <string,int>>> clusters;  //clusters (cada vector es un cluster)
  vector <vector <float> > util_matrix; ///matriz pequeña
  vector <pair <string,int> > aux;
  int max_temp_index;
  float max_item=-INFINITY;
  util_matrix.resize(matrix.size());
  for(int i=0;i<matrix.size();i++)
  {
    auxiliar.push_back(make_pair(data_names[i].first,i));
    util_matrix[i].resize(3);
    util_matrix[i][0]=get_min_for(i);
  }
  while (clusters.size()<cant_clust && auxiliar.size()!=2)
  {
    max_item=-INFINITY;
    for(int i=0;i<util_matrix.size();i++)
    {
      max_item=max(max_item,util_matrix[i][0]);
      if(max_item==util_matrix[i][0])
      {
        max_temp_index=i;
      }
    }
    //cout<<"selected: "<<auxiliar[max_temp_index].first<<endl;
    aux.resize(1);
    aux[0]=auxiliar[max_temp_index];
    clusters.push_back(aux);
    auxiliar.erase(auxiliar.begin()+max_temp_index);
    util_matrix.erase(util_matrix.begin()+max_temp_index);
    for(int i=0;i<util_matrix.size();i++)
    {
      if (auxiliar[i].second<max_temp_index)
      {
        util_matrix[i][1]=matrix[auxiliar[i].second][max_temp_index];
      }
      else
      {
        util_matrix[i][1]=matrix[max_temp_index][auxiliar[i].second];
      }
      util_matrix[i][2]=util_matrix[i][0]-util_matrix[i][1];
      if(util_matrix[i][2]>=0)
      {
        //cout<<"inserting: "<<auxiliar[i].first<<endl;
        clusters[clusters.size()-1].push_back(auxiliar[i]);
        util_matrix.erase(util_matrix.begin()+i);
        auxiliar.erase(auxiliar.begin()+i);
        i--;
      }
    }
    bool zero=1;
    if(clusters[clusters.size()-1].size()>1)
    {
      while(zero)
      {
        zero=0;
        for(int i=0;i<util_matrix.size();i++)
        {
          util_matrix[i][1]=get_min_distance(clusters[clusters.size()-1],auxiliar[i].second);
          util_matrix[i][2]=util_matrix[i][0]-util_matrix[i][1];
          if(util_matrix[i][2]>=0)
          {
            //cout<<"inserting: "<<auxiliar[i].first<<endl;
            clusters[clusters.size()-1].push_back(auxiliar[i]);
            util_matrix.erase(util_matrix.begin()+i);
            auxiliar.erase(auxiliar.begin()+i);
            i--;
            zero=1;
          }
        }
      }
    }
    /*cout<<"     ";
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
    }*/
  }
  vector <pair<string,int>> return_vec;
  string aux_S;
  int contador;
  for (int i=0;i<clusters.size();i++)
  {
    aux_S="";
    contador=0;
    for (int j=0;j<clusters[i].size();j++)
    {
      aux_S+=clusters[i][j].first;
      aux_S+=" , ";
      contador++;
    }
    aux_S=aux_S.substr(0,aux_S.size()-3);
    return_vec.push_back(make_pair(aux_S,contador));
  }
  contador=0;
  aux_S="";
  for (int i=0;i<auxiliar.size();i++)
  {
    aux_S+=auxiliar[i].first;
    aux_S+=" , ";
    contador++;
  }
  aux_S=aux_S.substr(0,aux_S.size()-3);
  return_vec.push_back(make_pair(aux_S,contador));
  return return_vec;
}

vector<pair <string,int>> dis_maximo(int cant_clust)
{
  vector <pair <string,int>> auxiliar;  //matriz pequeña Nombre de elemento-indice en matriz de distancias
  vector <vector <pair <string,int>>> clusters;  //clusters (cada vector es un cluster)
  vector <vector <float> > util_matrix; ///matriz pequeña
  vector <pair <string,int> > aux;
  int max_temp_index;
  float max_item=-INFINITY;
  util_matrix.resize(matrix.size());
  for(int i=0;i<matrix.size();i++)
  {
    auxiliar.push_back(make_pair(data_names[i].first,i));
    util_matrix[i].resize(3);
    util_matrix[i][0]=get_min_for(i);
  }
  while (clusters.size()<cant_clust && auxiliar.size()!=2)
  {
    max_item=-INFINITY;
    for(int i=0;i<util_matrix.size();i++)
    {
      max_item=max(max_item,util_matrix[i][0]);
      if(max_item==util_matrix[i][0])
      {
        max_temp_index=i;
      }
    }
    aux.resize(1);
    aux[0]=auxiliar[max_temp_index];
    clusters.push_back(aux);
    auxiliar.erase(auxiliar.begin()+max_temp_index);
    util_matrix.erase(util_matrix.begin()+max_temp_index);
    for(int i=0;i<util_matrix.size();i++)
    {
      if (auxiliar[i].second<max_temp_index)
      {
        util_matrix[i][1]=matrix[auxiliar[i].second][max_temp_index];
      }
      else
      {
        util_matrix[i][1]=matrix[max_temp_index][auxiliar[i].second];
      }
      util_matrix[i][2]=util_matrix[i][0]-util_matrix[i][1];
      if(util_matrix[i][2]>=0)
      {
        clusters[clusters.size()-1].push_back(auxiliar[i]);
        util_matrix.erase(util_matrix.begin()+i);
        auxiliar.erase(auxiliar.begin()+i);
        i--;
      }
    }
    bool zero=1;
    if(clusters[clusters.size()-1].size()>1)
    {
      while(zero)
      {
        zero=0;
        for(int i=0;i<util_matrix.size();i++)
        {
          util_matrix[i][1]=get_max_distance(clusters[clusters.size()-1],auxiliar[i].second);
          util_matrix[i][2]=util_matrix[i][0]-util_matrix[i][1];
          if(util_matrix[i][2]>=0)
          {
            //cout<<"inserting: "<<auxiliar[i].first<<endl;
            clusters[clusters.size()-1].push_back(auxiliar[i]);
            util_matrix.erase(util_matrix.begin()+i);
            auxiliar.erase(auxiliar.begin()+i);
            i--;
            zero=1;
          }
        }
      }
    }
  }
  vector <pair<string,int>> return_vec;
  string aux_S;
  int contador;
  for (int i=0;i<clusters.size();i++)
  {
    aux_S="";
    contador=0;
    for (int j=0;j<clusters[i].size();j++)
    {
      aux_S+=clusters[i][j].first;
      aux_S+=" , ";
      contador++;
    }
    aux_S=aux_S.substr(0,aux_S.size()-3);
    return_vec.push_back(make_pair(aux_S,contador));
  }
  contador=0;
  aux_S="";
  for (int i=0;i<auxiliar.size();i++)
  {
    aux_S+=auxiliar[i].first;
    aux_S+=" , ";
    contador++;
  }
  aux_S=aux_S.substr(0,aux_S.size()-3);
  return_vec.push_back(make_pair(aux_S,contador));
  return return_vec;
}

vector<pair <string,int>> dis_average(int cant_clust)
{
  vector <pair <string,int>> auxiliar;  //matriz pequeña Nombre de elemento-indice en matriz de distancias
  vector <vector <pair <string,int>>> clusters;  //clusters (cada vector es un cluster)
  vector <vector <float> > util_matrix; ///matriz pequeña
  vector <pair <string,int> > aux;
  int max_temp_index;
  float max_item=-INFINITY;
  util_matrix.resize(matrix.size());
  for(int i=0;i<matrix.size();i++)
  {
    auxiliar.push_back(make_pair(data_names[i].first,i));
    util_matrix[i].resize(3);
    util_matrix[i][0]=get_min_for(i);
  }
  while (clusters.size()!=cant_clust && auxiliar.size()!=2)
  {
    max_item=-INFINITY;
    for(int i=0;i<util_matrix.size();i++)
    {
      max_item=max(max_item,util_matrix[i][0]);
      if(max_item==util_matrix[i][0])
      {
        max_temp_index=i;
      }
    }
    aux.resize(1);
    aux[0]=auxiliar[max_temp_index];
    clusters.push_back(aux);
    auxiliar.erase(auxiliar.begin()+max_temp_index);
    util_matrix.erase(util_matrix.begin()+max_temp_index);
    for(int i=0;i<util_matrix.size();i++)
    {
      if (auxiliar[i].second<max_temp_index)
      {
        util_matrix[i][1]=matrix[auxiliar[i].second][max_temp_index];
      }
      else
      {
        util_matrix[i][1]=matrix[max_temp_index][auxiliar[i].second];
      }
      util_matrix[i][2]=util_matrix[i][0]-util_matrix[i][1];
      if(util_matrix[i][2]>=0)
      {
        clusters[clusters.size()-1].push_back(auxiliar[i]);
        util_matrix.erase(util_matrix.begin()+i);
        auxiliar.erase(auxiliar.begin()+i);
        i--;
      }
    }
    bool zero=1;
    if(clusters[clusters.size()-1].size()>1)
    {
      while(zero)
      {
        zero=0;
        for(int i=0;i<util_matrix.size();i++)
        {
          util_matrix[i][1]=get_avg_distance(clusters[clusters.size()-1],auxiliar[i].second);
          util_matrix[i][2]=util_matrix[i][0]-util_matrix[i][1];
          if(util_matrix[i][2]>=0)
          {
            //cout<<"inserting: "<<auxiliar[i].first<<endl;
            clusters[clusters.size()-1].push_back(auxiliar[i]);
            util_matrix.erase(util_matrix.begin()+i);
            auxiliar.erase(auxiliar.begin()+i);
            i--;
            zero=1;
          }
        }
      }
    }
  }
  vector <pair<string,int>> return_vec;
  string aux_S;
  int contador;
  for (int i=0;i<clusters.size();i++)
  {
    aux_S="";
    contador=0;
    for (int j=0;j<clusters[i].size();j++)
    {
      aux_S+=clusters[i][j].first;
      aux_S+=" , ";
      contador++;
    }
    aux_S=aux_S.substr(0,aux_S.size()-3);
    return_vec.push_back(make_pair(aux_S,contador));
  }
  contador=0;
  aux_S="";
  for (int i=0;i<auxiliar.size();i++)
  {
    aux_S+=auxiliar[i].first;
    aux_S+=" , ";
    contador++;
  }
  aux_S=aux_S.substr(0,aux_S.size()-3);
  return_vec.push_back(make_pair(aux_S,contador));
  return return_vec;
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

/*
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
*/

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
    data_names=dis_minimo(cant);
    break;
  case 5:
    data_names=dis_maximo(cant);
    break;
  case 6:
    data_names=dis_average(cant);
    break;
  }
  auto end = std::chrono::system_clock::now();
  double elapsed1 = std::chrono::duration_cast<std::chrono::duration<double> >(end - start).count();
  for(int i=0;i<data_names.size();i++)
  {
    cout<<data_names[i].second<<"->"<<'\t'<<"<<<"<< data_names[i].first<<">>>"<<'\t'<<endl;
  }
  cout<<"Time: "<<elapsed1<<"s"<<endl;



  return 0;
}
