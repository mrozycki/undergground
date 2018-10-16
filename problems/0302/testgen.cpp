#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>

int main()
{
	char filename[20];

	for (int i = 2; i <= 7; i++)
	{
		sprintf (filename, "%d.in", i);
		FILE* in = fopen (filename, "w");

		sprintf (filename, "%d.out", i);
		FILE* out = fopen (filename, "w");
		
		int n = pow (10, i-1);

		for (int j = 0; j < n; j++)
		{
			int a = rand()%n+1;
			fprintf (in, "%d ", a);
			fprintf (out, "%d ", 2*a);
		}
		fprintf(in, "0");

		fclose(in);
		fclose(out);
	}

	return 0;
}
