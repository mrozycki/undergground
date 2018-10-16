#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <fstream>
using namespace std;

int main()
{
	fstream in ("0.in", fstream::out);
	fstream out ("0.out", fstream::out);

	srand (time (NULL));
	
	int n; cin >> n;
	in << n << endl;
	vector<int> Q;
	for (int i = 0; i < n; i++)
	{
		Q.push_back(rand()%n+1);
		in << Q.back() << " ";
	}

	for (int i = n-1; i >= 0; i--)
	{
		out << Q[i] << " ";
	}
	

	return 0;
}
