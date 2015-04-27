#include<cstdlib>
#include<iostream>
#include<string>
using namespace std;

int dat[101];
int a;
int b;
int c;
int d;
int e;
struct r1
{
    int b[11];
    int v4;
} r1;
auto swap=[&](int & a,int & b)->void
{
    int c;
    c=a;
    a=b;
    b=c;
};
auto Sort1=[&](int st,int en,int & m,int & i)->void
{
    if((1<(en-st)))
    {
        m=st;
        swap(dat[st],dat[m]);
        i=(st+1);
        while((i<en))
        {
            if((dat[i]<dat[st]))
            {
                m=(m+1);
                if((m<i))
                {
                    swap(dat[m],dat[i]);
                }
                else
                {
                    if((i<m))
                    {
                        swap(dat[m],dat[i]);
                    }
                    else
                    {
                        a=0;
                    }
                }
            }
            else
            {
                a=0;
            }
            i=(i+1);
        }
        swap(dat[st],dat[m]);
        Sort1(st,m,m,i);
        Sort1((m+1),en,m,i);
    }
    else
    {
        a=0;
    }
};
auto Sort=[&]()->void
{
    int m;
    int i;
    Sort1(0,10,m,i);
};
int main()
{
    c=0;
    while((c<10))
    {
        cin>>d;
        dat[c]=d;
    }
    Sort();
    c=0;
    while((c<10))
    {
        cout<<dat[c]<<endl;
    }

    return 0;
}


