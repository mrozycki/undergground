#include <iostream>
using namespace std;

int main()
{
	int a, k, i; cin >> a >> k;

	for (i = 1; k > 0; k--)
	{
		i *= a;
	}

	cout << i << endl;

	return 0;
}
