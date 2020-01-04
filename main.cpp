#include <iostream>
#include <stdlib.h>
#include <time.h>
//#include <random>


using namespace std;

int main()
{
    /*
        std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 9);

    for(int seq=0; seq<50;seq++)
        cout << dist(rd)<<endl;
    exit(-1);
    */

    int dif, width=30, height=24, num_mines=200;
    cout<<"DIFFICULTY: \n 1 = Beginner (9x9 - 10 Mines)  \n 2 = Medium (16x16 - 40 Mines) \n 3 = Difficult (30x16 - 99 Mines) \n 4 = Custom (max: 30x24 - 200 Mines) \n";
    cin>>dif;
    switch(dif) {
  case 1:
    width=9;
    height=9;
    num_mines=10;
    break;
  case 2:
    width=16;
    height=16;
    num_mines=40;
    break;
  case 3:
    width=30;
    height=16;
    num_mines=99;
    break;
  case 4:
    cout<<"WIDTH = ";
    cin>>width;
    cout<<"HEIGHT = ";
    cin>>height;
    cout<<"MINES = ";
    cin>>num_mines;
    break;
}
    int v[26][32], i, j, a, k=0, m, n, jm, im;
    srand (time(NULL));

    for (i=1; i<=height; i++)
    {
        for (j=1; j<=width; j++)
        {
            a=rand();
            cout<<a;
           if (a<(RAND_MAX*(num_mines-k)/(height*width-((i-1)*width+j))))
           {
               v[i][j]=1;
               k=k+1;
           }
           else v[i][j]=0;
        }
        cout<<endl;

    }

    for (jm=0; jm<32; jm++) //marginile iau valoarea 0
    {
        v[0][jm]=0;
        for (int nmn=height+1; nmn<26; nmn++)
        v[nmn][jm]=0;
    }
    for (im=1; im<=height; im++)
    {
        v[im][0]=0;
        for (int nnn=width+1; nnn<32; nnn++)
        v[im][nnn]=0;
    }

    for (m=1; m<=height; m++) //afisarea propriu-zisa
    {
        for (n=1; n<=width; n++)
        {
            if (v[m][n]==1) cout<<"* ";
            else cout<<v[m-1][n-1]+v[m-1][n]+v[m-1][n+1]+v[m+1][n-1]+v[m+1][n]+v[m+1][n+1]+v[m][n-1]+v[m][n+1]<<" ";
        }
        cout<<endl;
    }

    cout<<endl<<k;



    return 0;
}
