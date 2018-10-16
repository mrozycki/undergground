#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main()
{
	int n; cin >> n;
	int m = 2000000000;

	cout << n << endl;

	for (int i = 0; i < n; i++)
	{
		int a = rand()%2000000002-1000000001;
		if (a < m) m = a;
		cout << a << " ";
	}

	cout << endl << m;

	return 0;
}
