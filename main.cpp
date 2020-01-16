#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

int main()
{
    int dif, width, height, num_mines;
    cout<<"DIFFICULTY: \n 1 = Beginner (9x9 - 10 Mines)  \n 2 = Medium (16x16 - 40 Mines) \n 3 = Difficult (16x30 - 99 Mines) \n 4 = Custom (max: 30x30 - 900 Mines) \n \n";
    cin>>dif;
    cout<<endl;
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
    width=16;
    height=30;
    num_mines=99;
    break;
  case 4:
    cout<<"WIDTH = ";
    cin>>width;
    cout<<"HEIGHT = ";
    cin>>height;
    cout<<"MINES = ";
    cin>>num_mines;
    cout<<endl;
    break;
}
    int v[32][32]={0}, i, j, k=0;
    srand (time(NULL));

    for (i=1; i<=height; i++)
        for (j=0; j<=width; j++)
               if (num_mines-k!=0 && rand()<(RAND_MAX*(num_mines-k)/(height*width-((i-1)*width+j))))
               {
                   v[i][j]=-1;
                   k++;
               }

    for (i=1; i<=height; i++)
    {
        for (j=1; j<=width; j++)
            if (v[i][j]==-1) cout<<"*  ";
            else
            {
                for (short m=i-1; m<=i+1; m++)
                    for (short n=j-1; n<=j+1; n++)
                        if (v[m][n]==-1)
                    v[i][j]=v[i][j]+1;
                cout<<v[i][j]<<"  ";
            }
        cout<<endl;
    }

    return 0;
}


