#include <vector>
#include <thread>

using namespace std;

#define Q 3
#define P 5-Q ///P=5 (siempre restarle Q)

vector <tuple <string,string,int>> arr;

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    /* create temp arrays */
    vector <tuple<string,string,int>> L, R;
    L.resize(n1);
    R.resize(n2);

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (get<2>(L[i]) <= get<2>(R[j]))
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        mergeSort(l, m);
        mergeSort(m+1, r);
        merge(l, m, r);
    }
}

void Sort(vector<tuple <string,string,int>> * arreglo)
{
  arr=*arreglo;
  mergeSort(0,arr.size()-1);
  *arreglo=arr;
}

int stringPenalization(string data)
{
  int localPenalization=0;
  for(int i=0;i<data.size();i++)
  {
    if(data[i]=='-')
    {
      localPenalization+=Q;
      if(data[i-1]!='-')
      {
        localPenalization+=P;
      }
    }
  }
  return localPenalization;
}

void getPenalization(tuple <string,string,int> *data)
{
  get<2>(*data)=stringPenalization(get<0>(*data))+stringPenalization(get<1>(*data));
  //cout<<"{"<<get<2>(data)<<"}"<<endl;
}

float euclidian_distance(vector<float> a, vector <float> b)
{
  float acumulado=0;
  for(int i=0;i<a.size();i++)
  {
    acumulado+=pow(abs(a[i]-b[i]),2);
  }
  return sqrt(acumulado);
}
