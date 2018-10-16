#include <cstring>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>

int main()
{
	srand(time(NULL));
	char filename[20];

	for (int i = 2; i <= 8; i++)
	{
		sprintf (filename, "%d.in", i);
		FILE* in = fopen (filename, "w");

		sprintf (filename, "%d.out", i);
		FILE* out = fopen (filename, "w");
		
		int n = pow (10, i-1);
		int S = 0, j;

		for (j = 0; S < n; j++)
		{
			int a = rand()%(i<4 ? 10 : 100)+1;
			fprintf (in, "%d ", a);
			S += a;
		}
		fprintf(in, "0");
		fprintf(out, "%d", S/j);

		fclose(in);
		fclose(out);
	}

	return 0;
}
