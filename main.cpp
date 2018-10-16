#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>  
#include <mysql/mysql.h>
using namespace std;

int main(int argc, char **argv)
{
	MYSQL db;
	MYSQL_RES *result;
	MYSQL_ROW r;
	FILE *compilation, *judgement;
	char query[1024];
	
	setvbuf(stdout, NULL, _IONBF, 0);

	printf ("MySQL init...\t\t\t");
	if (mysql_init(&db))
	{
		printf ("[ OK ]\n");
	}
	else
	{
		printf ("[FAIL]\nexiting...\n");
		return 0;
	}

	printf ("MySQL connect... \t\t");
	if (argc > 1)
	{
		if (mysql_real_connect(&db, "mysql-966399.vipserv.org", "calki_judge", "h6cMCQJALcHLjNuU", "calki_judge", 0, NULL, 0))
		{
			printf ("[ OK ]\n");
		}
		else
		{
			printf ("[FAIL]\nexiting...\n");
			return 0;
		}
	}
	else
	{
		if (mysql_real_connect(&db, "localhost", "judge", "h6cMCQJALcHLjNuU", "judge", 0, NULL, 0))
		{
			printf ("[ OK ]\n");
		}
		else
		{
			printf ("[FAIL]\nexiting...\n");
			return 0;
		}
	}

	while (true)
	{
		sprintf(query, "SELECT submissions.id, category, storage FROM submissions, problems WHERE problemid = problems.id AND grade = '0'");
		if (mysql_real_query(&db, query, strlen(query)) != 0)
		{
			printf("Error: MySQL query failed\n");
			continue;
		}

		result = mysql_store_result(&db);
		if (!result)
		{
			printf("Error: MySQL query storage failed\n");
			continue;
		}

		while ((r = mysql_fetch_row(result)))
		{
			if (argc > 1)
			{
				sprintf (query, "wget http://undergground.pl/uploads/%s -O solutions/%s -q", r[2], r[2]);
				system (query);
			}
			sprintf (query, "g++ -O2 -Wall solutions/%s -o solution 2>&1", r[2]);
			printf ("%s\t%s:\tCompiling", r[0], r[2]);
			compilation = popen (query, "r");
			if (fgetc(compilation) != -1)	
			{
				pclose(compilation);

				printf ("[FAIL]\n");
				sprintf (query, "UPDATE submissions SET grade='2',checktime=CURRENT_TIMESTAMP WHERE id = '%s';", r[0]);	
				mysql_real_query(&db, query, strlen(query));
			}
			else
			{
				pclose(compilation);

				printf ("[ OK ] Running ");
				sprintf (query, "./grader %s", r[1]);

				judgement = popen(query, "r");

				if (!judgement) 
				{
					printf ("popen fail");
					continue;
				}

				char chunk[64] = "";
				while (strcmp(chunk, "GRADE") && strcmp(chunk, "ERROR"))
				{
					fscanf(judgement, "%s", chunk);
					if (!strcmp(chunk, "TEST"))
					{
						fscanf(judgement, "%s", chunk);
						printf("%s,", chunk);
					}
				}
				fscanf(judgement, "%s", chunk);
				pclose(judgement);

				if (!strcmp(chunk, "7"))
					printf ("\t\t[ OK ]\n");
				else
					printf ("\t\t[FAIL] (%s)\n", chunk);

				sprintf(query, "UPDATE submissions SET grade='%s', checktime=CURRENT_TIMESTAMP WHERE id = '%s';", chunk, r[0]);	
				mysql_real_query(&db, query, strlen(query));
			}

		}

		sleep (1);
	}

	mysql_close(&db);
	printf ("MySQL disconnect...\t\t[ OK ]\n");
	return 0;
}
