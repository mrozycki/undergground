#include <iostream>
using namespace std;

int main()
{
	int n, k = 0;
	cin >> n;

	for (int i = 1; i*i <= n; i++)
	{
		if (n%i == 0)
		{
			if (i*i == n)
				k++;
			else
				k += 2;
		}
	}

	cout << k;

	return 0;
}
