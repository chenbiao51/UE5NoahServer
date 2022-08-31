#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;


int main()
{
    cout <<"RAND_MAX:"<<RAND_MAX<<endl;
    srand((unsigned)time(NULL));
    for(int i = 0; i < 5; i++)
    {
        cout<<(rand()%2)<<" ";
    }
    cout<<endl;
    for (int i = 0; i < 5; i++)
    {
        cout<<(rand()%5+3)<<" ";
    }
    cout<<endl;
}