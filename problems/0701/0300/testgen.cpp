#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

int main()
{
	int n; cin >> n;
	srand(time(NULL));
	for (int i = 1; i < n; i++)
	{
		cout << rand()%n+1 << " ";
	}
	cout << 0 << endl;

	return 0;
}
