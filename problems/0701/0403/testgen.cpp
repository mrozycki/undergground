#include <iostream>
using namespace std;

int main()
{
	int a, k; cin >> a >> k;

	for (int i = a; i < k; i += a)
		cout << i << endl;

	return 0;
}
