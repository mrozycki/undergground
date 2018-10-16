#include <iostream>
using namespace std;

int main()
{
	for (int i = 99; i > 2; i--)
	{
		cout << i << " bottles of beer on the wall, " << i << " bottles of beer" << endl;
		cout << "Take one down and pass it around. " << i-1 << " bottles of beer on the wall" << endl << endl;
	}

	cout << "2 bottles of beer on the wall, 2 bottles of beer" << endl;
	cout << "Take one down and pass it around. 1 bottle of beer on the wall" << endl << endl;

	cout << "1 bottle of beer on the wall, 1 bottle of beer" << endl;
	cout << "Take one down and pass it around. No more bottles of beer on the wall" << endl << endl;

	cout << "No more bottles of beer on the wall, no more bottles of beer" << endl;
	cout << "Go to the store and buy some more. 99 bottles of beer on the wall" << endl;


	return 0;
}
