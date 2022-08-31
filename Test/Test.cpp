#include <iostream>
#include <chrono>
#include <random>

using namespace std;


int main()
{
    int64_t time64 = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    cout <<"xxbit==   "<< time64 <<endl;
    cout <<"xx32bit==   "<< (uint32_t)time64 <<endl;
    unsigned seed1 = chrono::system_clock::now().time_since_epoch().count();
     
    std::mt19937 g1 (seed1);

    uint32_t u32Random = g1();
    
    cout <<"32bit==   "<< u32Random <<endl;
    
    std::mt19937_64 g2 (seed1);

    uint64_t u64Random = g2();
    
    cout <<"64bit==   "<< u64Random <<endl;
    
    cout<<endl;
}