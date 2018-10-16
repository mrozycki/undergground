#include <iostream>
using namespace std;

int main()
{
	long long n;
	cin >> n;

	int c[80];
	c[0] = 1; c[1] = 1;
	for (int i = 2; i < 60; i++)
	{
		c[i] = (c[i-1] + c[i-2])%10;
	}

	cout << c[n%60-1];

	return 0;
}
