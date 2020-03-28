#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <mysql.h>
#include <spdlog/spdlog.h>

#include "grader.h"

using namespace std;

int main(int argc, char **argv)
{
	MYSQL db;
	MYSQL_RES *result;
	MYSQL_ROW r;
	FILE *compilation, *judgement;
	char query[1024];

	setvbuf(stdout, NULL, _IONBF, 0);

	spdlog::info("Starting the grader daemon");
	spdlog::info("Initializing database");
	if (!mysql_init(&db))
	{
		spdlog::error("Failed to initialize database: {}", mysql_error(&db));
		return 0;
	}

	spdlog::info("Connecting to database at '{}'", "127.0.0.1");
	if (!mysql_real_connect(&db, "127.0.0.1", "grader", "123456", "undergground", 0, NULL, 0))
	{
		spdlog::error("Failed to connect to database: {}", mysql_error(&db));
		return 0;
	}

	spdlog::info("Waiting for submissions");
	while (true)
	{
		sprintf(query, "SELECT submissions.id, category, storage FROM submissions, problems WHERE problemid = problems.id AND grade = '0'");
		if (mysql_real_query(&db, query, strlen(query)) != 0)
		{
			spdlog::error("Database query failed: {}", mysql_error(&db));
			continue;
		}

		result = mysql_store_result(&db);
		if (!result)
		{
			spdlog::error("Database query result storage failed: {}", mysql_error(&db));
			continue;
		}

		while ((r = mysql_fetch_row(result)))
		{
			spdlog::info("Processing submission request #{}; problem {}", r[0], r[1]);
			spdlog::info("Compiling file {}", r[2]);
			sprintf (query, "g++ -O2 -Wall solutions/%s -o solution 2>&1", r[2]);
			compilation = popen (query, "r");
			if (fgetc(compilation) != -1)
			{
				pclose(compilation);
				spdlog::info("Compilation failed");
				spdlog::info("Storing result in database");
				sprintf (query, "UPDATE submissions SET grade='2',checktime=CURRENT_TIMESTAMP WHERE id = '%s';", r[0]);
				mysql_real_query(&db, query, strlen(query));
			}
			else
			{
				pclose(compilation);
				spdlog::info("Compilation suceeded, running tests");
				auto grade = ugg::grade(r[1]);
				spdlog::info("Tests finished; grade: {}", grade);
				spdlog::info("Storing result in database", grade);
				sprintf(query, "UPDATE submissions SET grade='%d', checktime=CURRENT_TIMESTAMP WHERE id = '%s';", grade, r[0]);
				mysql_real_query(&db, query, strlen(query));
			}

		}

		sleep(1);
	}

	mysql_close(&db);
	spdlog::info("Disconnecting from database");
	return 0;
}
