#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include "../tools.h"
/*
g++ -o smitwat Smith\ Waterman.cpp -pthread -std=c++11
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
#define DONE 0
using namespace std;

typedef pair<int,int> SW; //score, trace
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
vector<SW> Matrix;
vector <tuple<string,string,int> > Alignments;
vector <int> start_positions;
int i,j,max_alignment=0;
string first,second;

///STRUCTS///
std::mutex mtx,mtx_max;
struct thread_pos
{
  int position;
  bool orientation;
  int iteration;
};
struct alignment
{
  int position;
  pair<string,string> aligned;
};


///FUNCTIONS///
SW fill(int pos, int match_value)
{
//cout<<match_value;
  int top,left,diag,maxim,trace=0;
  //cout<<"{"<<pos<<"}"<<endl;
  top=Matrix[pos-(j+1)].first+GAP;
  left=Matrix[pos-1].first+GAP;
  diag=Matrix[pos-(j+2)].first+match_value;
  maxim=max(0,max(top,max(left,diag)));
  //cout<<"["<<pos<<"]"<<"{"<<top<<"}"<<"{"<<left<<"}"<<"{"<<diag<<"}";
  //cout<<"<<"<<maxim<<">>"<<endl
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
  int aux=max(maxim,max_alignment);
  mtx.lock();
  if(aux==maxim)
  {
    if(aux==max_alignment)
    {
      start_positions.push_back(pos);
    }
    else
    {
      start_positions.clear();
      start_positions.push_back(pos);
    }
    max_alignment=aux;
  }
  mtx.unlock();
  /*if(maxim==0)
  {
    return make_pair(0,0);
  }*/
  //cout<<"["<<trace<<"]"<<endl;
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

void PrintTrace()
{
    for(int x=0;x<=i;x++)
    {
        for (int y=0;y<=j;y++)
        {
            cout<<Matrix[x*(j+1)+y].second<<"\t";
        }
        cout<<endl;
    }
}
//void *fillMatrix(void *thread_info)
void fillMatrix(thread_pos data)
{
  int pos=data.position,match_value;
  //cout<<"{"<<pos<<"}"<<endl;
  if(data.orientation)
  {
    pos++;
    while(pos%(j+1)!=0) //horizonatal
    {
      //cout<<"{"<<pos<<"}"<<"<"<<pos-(data->iteration*(j+1))-1<<">"<<first[data->iteration-1]<<"-"<<second[pos-(data->iteration*(j+1))-1]<<endl;
      //cout<<"{"<<pos<<"}"<<endl;]
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
    //cout<<0<<endl;
  }
  else //vertical
  {
    pos+=j+1;
    while(pos<=Matrix.size())
    {
      //cout<<"{"<<pos<<"}"<<first[pos/(i+1)-1]<<"-"<<second[data->iteration-1]<<endl;
      if(first[pos/(j+1)-1]==second[pos%(j+1)-1])
      {
        match_value=MATCH;
      }
      else
      {
        match_value=MISMATCH;
      }
      //cout<<"{"<<pos<<"}"<<endl;
      Matrix[pos]=fill(pos,match_value);
      pos+=j+1;
    }
    //cout<<1<<endl;
  }
}

//int contador=1;

void Align(alignment data)
{
  struct alignment data2,data3;
  int a=Matrix[data.position].second;
  int b=Matrix[data.position].first;
  if(b==DONE)
  {
    //cout<<data.aligned.first<<endl<<data.aligned.second<<endl;
    mtx.lock();
    //cout<<"{"<<contador++<<"}"<<endl;
    //cout<<"ok"<<endl;
    Alignments.push_back(make_tuple(data.aligned.first,data.aligned.second,0));
    mtx.unlock();
    //break;
    return;
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
  else {
    //cout<<"error: "<<a<<"->"<<b<<"{"<<data.position<<"}"<<endl;
    //exit(0);
  }

}

int main()
{
    /*vector<pthread_t> threads;
    threads.resize(2);*/
    struct thread_pos t_p[2];
    void* status;
    int show;
    getline(cin,first);
    getline(cin,second);
    cin>>show;
    /*if(first.size()>second.size())
    {
      string aux;
      aux=first;
      first=second;
      second=aux;
    }*/
    //cout<<first<<endl<<second<<endl;
    //cout<<first<<endl<<second<<endl;
    i=first.size();
    j=second.size();
    //Init Score Matrix
    Matrix.resize((i+1)*(j+1));
    //cout<<Matrix.size()<<endl;
    auto start = std::chrono::system_clock::now();
    Matrix[0]=make_pair(0,0);
    for (int x=1;x<=j;x++)
    {
        //cout<<"{"<<x<<"}"<<endl;
        Matrix[x]=make_pair(0,LEFTGAP);
        //cout<<"{"<<Matrix[x].first<<"}"<<endl;
    }
    for(int y=1;y<=i;y++)
    {
      //cout<<"{"<<y*(j+1)<<"}";
      Matrix[y*(j+1)]=make_pair(0,TOPGAP);
      //cout<<"{"<<Matrix[(y-1)*(j+1)].first<<"}";
      //cout<<"{"<<Matrix[(y)*(j+1)].first<<"}"<<endl;
    }
    for(int z=1;z<=min(i,j);z++)
    {
      //cout<<z<<endl;
      //cout<<first[z-1]<<"-"<<second[z-1]<<endl;
      //cout<<j*z+z<<endl;
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
        //thread (&threads[a],NULL,fillMatrix,(void *)&t_p[a]);
      }
      thread firstt(fillMatrix,t_p[0]);
      thread secondt(fillMatrix,t_p[1]);
      /*for(int a=0;a<2;a++)
      {
        pthread_join(threads[a],&status);
      }*/
      firstt.join();
      secondt.join();
    }
    //pthread_exit(NULL);
    auto end = std::chrono::system_clock::now();
    double elapsed1 = std::chrono::duration_cast<std::chrono::duration<double> >(end - start).count();
    //cout<<Matrix.size()<<endl;
    //threads.resize(1);
    struct alignment al[start_positions.size()];

    int max_threads=thread::hardware_concurrency();
    vector<thread> threads;

    //al[0].aligned=make_pair("","");
    /*pthread_create(&threads[0],NULL,Align,(void *)&al[0]);*/
    /*
    PrintMatrix();
    cout<<endl;
    PrintTrace();
    cout<<endl;*/
    start = std::chrono::system_clock::now();
    //Align(al[0]);
    for (int start=0;start<start_positions.size();start++)
    {
      al[start].position=start_positions[start];
      al[start].aligned.first="";
      al[start].aligned.second="";
      /*if(threads.size()>=max_threads)
      {
        for(int threads_count=0;threads_count<threads.size();threads_count++)
        {
          threads[threads_count].join();
        }
        threads.clear();
      }
      threads.push_back(std::thread(Align,al[start]));*/
      //cout<<al[start].position<<endl;
      Align(al[start]);
    }
    /*for(int threads_count=0;threads_count<threads.size();threads_count++)
    {
      threads[threads_count].join();
    }
    threads.clear();*/
    //pthread_join(threads[0],&status);
    //first.join();
    for (int start=0;start<Alignments.size();start++)
    {
      if(threads.size()>=max_threads)
      {
        for(int threads_count=0;threads_count<threads.size();threads_count++)
        {
          threads[threads_count].join();
        }
        threads.clear();
      }
      threads.push_back(std::thread(getPenalization,&Alignments[start]));
    }
    for(int threads_count=0;threads_count<threads.size();threads_count++)
    {
      threads[threads_count].join();
    }
    threads.clear();
    Sort(&Alignments);
    end = std::chrono::system_clock::now();
    show=min((int)Alignments.size(),show);
    for(int alins=0;alins<show;alins++)
    {
      cout<<"________________"<<endl;
      cout<<get<0>(Alignments[alins])<<endl<<get<1>(Alignments[alins])<<endl;
      cout<<"Penalidad:"<<get<2>(Alignments[alins])<<endl;
    }
      double elapsed = std::chrono::duration_cast<std::chrono::duration<double> >(end - start).count();
    elapsed = std::chrono::duration_cast<std::chrono::duration<double> >(end - start).count();
    cout<<"STARTS: "<<start_positions.size()<<endl;
    cout<<"Numero de alineamientos: "<<Alignments.size()<<endl;
    cout<<"El score es: "<<max_alignment<<endl;
    printf("El tiempo del Algoritmo Needleman-Wunsch: %.9f segundos\n",elapsed1);
    printf("El tiempo del Algoritmo de Trazado: %.9f segundos\n",elapsed);

    return 0;
}
