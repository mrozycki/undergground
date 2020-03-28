#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <mysql.h>
#include <spdlog/spdlog.h>

#include "compiler.h"
#include "grader.h"

int process_submission(char const* id, char const* problem, char const* submission_file) {
	spdlog::info("Processing submission request #{}; problem {}", id, problem);
	auto executable_path = ugg::compile(submission_file);
	if (!executable_path) {
		return 2;
	}

	return ugg::grade(problem, *executable_path);
}

int main()
{
	MYSQL db;
	MYSQL_RES *result;
	MYSQL_ROW r;
	char query[1024];

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
			auto grade = process_submission(r[0], r[1], r[2]);
			spdlog::info("Storing grade in database");
			sprintf(query, "UPDATE submissions SET grade='%d', checktime=CURRENT_TIMESTAMP WHERE id = '%s';", grade, r[0]);
			mysql_real_query(&db, query, strlen(query));
		}

		sleep(1);
	}

	mysql_close(&db);
	spdlog::info("Disconnecting from database");
	return 0;
}
