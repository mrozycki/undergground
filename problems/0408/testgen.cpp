#include <iostream>
using namespace std;

int main()
{
	int n;
	cin >> n;

	cout << "1 ";

	for (int a = 1, b = 1; b <= n;)
	{
		cout << b << " ";
		int c = b;
		b = a+b;
		a = c;
	}

	return 0;
}
