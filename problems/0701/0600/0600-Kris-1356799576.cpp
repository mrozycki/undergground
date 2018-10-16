#include <iostream>

using namespace std;

int main()
{
    int a, b;
    cin >> a >> b;
    
    
    for(int i=0;; i++)
    {
            if(a>b)
                   a=a-b;
            else
                   b=b-a;
            
            if(a==b)
            break;
    }
    cout << a;
    
    
    //system("pause");
    return 0;
}
