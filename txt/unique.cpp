#include<iostream>
#include <stdio.h>
#include <set>
using namespace std;

set<string> st;
int main()
{
    freopen("words-snl.txt","r",stdin);
    freopen("sorted-snl.txt","w",stdout);
    string tmp;
    while(cin>>tmp)
    {

        st.insert(tmp);
    }
    for(set<string>::const_iterator it=st.begin();it!=st.end();++it)
        cout<<(*it)<<endl;


    return 0;
}
