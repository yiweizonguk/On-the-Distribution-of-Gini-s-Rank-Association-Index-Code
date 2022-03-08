#include <iostream>
#include <bits/stdc++.h>
#include <unordered_map>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <unistd.h>
#include <fstream>
#include <thread>
using namespace std;


int main() 
{
    string timeFile = "timeRecord.txt";          // file that records the running time
    ofstream fout(timeFile.c_str());
    time_t startTime, endTime;
    int n;
    for (n=5; n<10; n++)
    {
        startTime = time(NULL);
        PerMain(n, 9);
        endTime = time(NULL);
        cout << "n = " << n << ",   Total time: " << endTime - startTime << endl;		//s为单位
        fout << "n = " << n << ",   Total time: " << endTime - startTime << "\r\n";		//s为单位
    }
    fout.close();

    return 0;
}


// compilation and run
// g++ resGiniGamma.cpp -o resGiniGamma -std=c++11 -pthread -Wl,--no-as-needed
// ./resGiniGamma


int Per(int n)
{
    // compute permanent of matrix A (n x n)
    // the degree of the ij-th element in matrix A is |n+1-i-j| - |j-i|

    // tbe degree of matrix A
    int L = 30;
    int A[L][L];
    int i, j;
    for (i=0; i<n; i++)
    {
        for (j=0; j<n; j++)
        {
            A[i][j] = abs(n + 1 - (i + 1) - (j + 1)) - abs((j + 1) - (i + 1));
        }
    }


    // compute permanent according to BBFG formula
    __int128 delta[L], deltaProd, div;
    int k;
    unordered_map<int,__int128> polyRes, polySumProd, polyForProd;
    unordered_map<int,__int128>::iterator it, cur;
    polyRes.clear();

    for(int m=0; m < (1<<(n-1)); m++)   // from 0 to 2^(n-1)
    {
        // delta is the the sign
        i = n-1;
        for (int k=m; k; k=k/2)
		{
			delta[i] = k % 2 ? 1: -1;
			i -= 1;
		}
		while (i > 0)
		{
		    delta[i] = -1;
		    i -= 1;
		}
        delta[0] = 1;


        polySumProd.clear();
        polySumProd[0] = 1;
        for (j=0; j<n; j++)
        {
            deltaProd = 1;   // record the sign

            // define the polynomial corresponding to the j-th column of A
            // note that the degree may be duplicated in one column
            polyForProd.clear();
            for(i=0; i<n; i++)
            {
                cur = polyForProd.find(A[i][j]);
                if (cur != polyForProd.end())
                {
                    polyForProd[A[i][j]] = (cur->second) + delta[i];
                }
                else
                {
                    polyForProd[A[i][j]] = delta[i];
                }
                deltaProd *= delta[i];
            }

            // cumproduct for the column polynomials
            polySumProd =PolyProd(polySumProd, polyForProd);
        }

        // sum the column-cumproduct polynomials
        if (deltaProd>0)
        {
            polyRes = PolyAdd(polyRes, polySumProd);
        }
        else
        {
            polyRes = PolyMinus(polyRes, polySumProd);
        }
    }

    // divide the constant coef 2^(n-1)
    div = 1<<(n-1);
    for (it=polyRes.begin(); it!=polyRes.end(); ++it)
    {
        polyRes[it->first] = (it->second) / div;
    }

    // print the results
    for (it=polyRes.begin(); it!=polyRes.end(); ++it)
    {
        cout << it->first << " => ";
        print(it->second);
        cout << '\n';
    }
    cout << '\n';
    return 0;
}


int PerMain(int n, int nCpu)
{
    // this function compute permanent parallelly
    // divide the whole loops (1,2,...,2^(n-1)) into several parts, and compute the results of each part with PerMulti on different cpus

    // divide the whole loops (1,2,...,2^(n-1)) for each cpu evenly
    mkdir(to_string(n).c_str(), S_IRWXU);
    int startEnd[100][2];
    int nTask = 1<<(n-1);
    int a = nTask / nCpu;
    int b = nTask - nCpu * a;
    int i;

    for (i=0; i<b; i++)
    {
        startEnd[i][0] = (a + 1) * i;
    }
    for (i=b; i<nCpu+1; i++)
    {
        startEnd[i][0] = (a + 1) * b + a * (i-b);
    }
    for (i=0; i<nCpu; i++)
    {
        startEnd[i][1] = startEnd[i+1][0] - 1;
    }

    // distribute the tasks for each cpu
    thread threads[20];
    for (i=0; i<nCpu; i++)
    {
        threads[i] = thread(PerMulti, n, startEnd[i][0], startEnd[i][1], i);
    }

    for (i=0; i<nCpu; i++)
    {
        threads[i].join();
    }

    return 0;
}


void PerMulti(int n, int mStart, int mEnd, int iCpu)
{
    // this function is nearly the same as Per, but add arguments mStart and mEnd to control the beginning and end of loops
    // we will divide the whole loops (1,2,...,2^(n-1)) into several parts, and compute different part on different cpus
    // so that we can compute permanent parallelly

    int L = 30;
    int i, j;
    int A[L][L];
    for (i=0; i<n; i++)
    {
        for (j=0; j<n; j++)
        {
            A[i][j] = abs(n + 1 - (i + 1) - (j + 1)) - abs((j + 1) - (i + 1));
        }
    }


    __int128 delta[L], deltaProd, div;
    int k;
    unordered_map<int,__int128> polyRes, polySumProd, polyForProd;
    unordered_map<int,__int128>::iterator it, cur;
    polyRes.clear();

    for(int m=mStart; m < mEnd+1; m++)
    {
        i = n-1;
        for (int k=m; k; k=k/2)
		{
			delta[i] = k % 2 ? 1: -1;
			i -= 1;
		}
		while (i > 0)
		{
		    delta[i] = -1;
		    i -= 1;
		}
        delta[0] = 1;

        polySumProd.clear();
        polySumProd[0] = 1;
        for (j=0; j<n; j++)
        {
            deltaProd = 1;
            polyForProd.clear();
            for(i=0; i<n; i++)
            {
                cur = polyForProd.find(A[i][j]);
                if (cur != polyForProd.end())
                {
                    polyForProd[A[i][j]] = (cur->second) + delta[i];
                }
                else
                {
                    polyForProd[A[i][j]] = delta[i];
                }

                deltaProd *= delta[i];
            }
            polySumProd =PolyProd(polySumProd, polyForProd);
        }

        if (deltaProd>0)
        {
            polyRes = PolyAdd(polyRes, polySumProd);
        }
        else
        {
            polyRes = PolyMinus(polyRes, polySumProd);
        }
    }


    // the results of each cpu are recorded into txt file
    string path = to_string(n) + "/" + to_string(iCpu) + ".txt";
    ofstream fout(path.c_str());
    for (it=polyRes.begin(); it!=polyRes.end(); ++it)
    {
        fout << it->first << " " << int128ToStr(it->second) << "\r\n";
    }
    fout.close();
}


unordered_map<int,__int128> PolyProd(unordered_map<int,__int128> p1, unordered_map<int,__int128> p2)
{
    // polynomial multiplication, polynomials are represented with unordered_map (degree int, coef int128)
    unordered_map<int,__int128> p;
    unordered_map<int,__int128>::iterator it1, it2, cur;
    __int128 newCoef;
    int deg;
    for (it1=p1.begin(); it1!=p1.end(); ++it1)
    {
        for (it2=p2.begin(); it2!=p2.end(); ++it2)
        {
            deg = it1->first + it2->first;
            cur = p.find(deg);
            if (cur != p.end())             // modify the coef of existing degree
            {
                newCoef = cur->second + (it1->second) * (it2->second);
                if (newCoef == 0)           // erase zero terms
                {
                    p.erase(cur->first);
                }
                else
                {
                    p[cur->first] = newCoef;
                }
            }
            else                            // add a new degree
            {
                p[deg] = (it1->second) * (it2->second);
            }
        }
    }
    return p;
}


unordered_map<int,__int128> PolyAdd(unordered_map<int,__int128> p1, unordered_map<int,__int128> p2)
{
    // polynomial addition, polynomials are represented with unordered_map (degree int, coef int128)
    unordered_map<int,__int128>::iterator it, cur;
    __int128 newCoef;
    for (it=p2.begin(); it!=p2.end(); ++it)
    {
        cur = p1.find(it->first);
        if (cur != p1.end())              // modify the coef of existing degree
        {
            newCoef = cur->second + it->second;
            if (newCoef == 0)             // erase zero terms
            {
                p1.erase(it->first);
            }
            else
            {
                p1[it->first] = newCoef;
            }
        }
        else                             // add a new degree
        {
            p1[it->first] = it->second;
        }
    }
    return p1;
}


unordered_map<int,__int128> PolyMinus(unordered_map<int,__int128> p1, unordered_map<int,__int128> p2)
{
    // polynomial subtraction, polynomials are represented with unordered_map (degree int, coef int128)
    unordered_map<int,__int128>::iterator it, cur;
    __int128 newCoef;
    for (it=p2.begin(); it!=p2.end(); ++it)     // modify the coef of existing degree
    {
        cur = p1.find(it->first);
        if (cur != p1.end())
        {
            newCoef = cur->second - it->second;
            if (newCoef == 0)                    // erase zero terms
            {
                p1.erase(it->first);
            }
            else
            {
                p1[it->first] = newCoef;
            }
        }
        else                                      // add a new degree
        {
            p1[it->first] = -(it->second);
        }
    }
    return p1;
}


inline void print(__int128 x)
{
    // print int128
    if(x < 0)
    {
        putchar('-');
        x = -x;
    }
    if(x > 9) print(x/10);
    putchar(x%10+'0');
}


string int128ToStr(__int128 x)
{
    // transform int128 to string
    string s("");

    if(x < 0)
    {
        s += '-';
        x = -x;
    }

    if(x == 0)
    {
        s += "0";
    }

    string t("");
    while (x > 0)
    {
        t += ('0' + x%10);
        x /= 10;
    }
    string rt(t.rbegin(), t.rend());
    s += rt;
    return s;
}