#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <future>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include "system/process.h"
#include "grader.h"

using namespace std;

namespace ugg {
int grade(std::string_view problemid, boost::filesystem::path const& executable_path)
{
	char testpath[32];
	sprintf(testpath, "problems/%s/tests", problemid.data());
	FILE *testfile = fopen(testpath, "r");
	if (!testfile)
	{
		return -1;
	}

	int testnum;
	fscanf(testfile, "%d", &testnum);

	bool passed = true;
	for (int i = 1; i <= testnum; i++)
	{
		spdlog::info("Running test {}/{}", i, testnum);
		char inpath[32], outpath[32], infile[32], outfile[32];
		fscanf(testfile, "%s %s", infile, outfile);
		sprintf(inpath, "problems/%s/%s", problemid.data(), infile);
		sprintf(outpath, "problems/%s/%s", problemid.data(), outfile);
		int maxmem, maxtime;
		fscanf(testfile, "%d %d", &maxmem, &maxtime);
		spdlog::debug("Test details: inpath={}, outpath={}, maxmem={}, maxtime={}", inpath, outpath, maxmem, maxtime);

		int event_pipe[2];
		pipe(event_pipe);

		auto tester = std::async(std::launch::async, [executable_path, inpath, outpath, event_pipe, maxmem, maxtime] {
			auto solution_process = ugg::system::start_process(executable_path);

			auto sender = std::async(std::launch::async, [&solution_process, inpath] {
				spdlog::info("Starting input sender");
				FILE *indata = fopen(inpath, "r");
				char instr[32];
				while (fscanf(indata, "%s", instr) != -1)
				{
					spdlog::info("writing: {}", instr);
					solution_process.in().printf("%s\n", instr);
				}
				fclose(indata);
				spdlog::info("Input sender finished");
			});

			auto receiver = std::async(std::launch::async, [&solution_process, outpath, event_pipe] {
				spdlog::info("Starting output receiver");
				char outstr[32], ansstr[32];
				FILE *outdata = fopen(outpath, "r");
				auto answer = solution_process.out();

				while (answer.scanf("%s", ansstr) != -1 && fscanf(outdata, "%s", outstr) != -1) {
					spdlog::info("received: {} (expected: {})", ansstr, outstr);
					if (strcmp(ansstr, outstr)) {
						write(event_pipe[1], "6", 1);
						break;
					}
				}

				if (answer.scanf("%s", ansstr) != -1 || fscanf(outdata, "%s", outstr) != -1) {
					write(event_pipe[1], "6", 1);
				} else {
					write(event_pipe[1], "7", 1);
				}

				fclose(outdata);
				spdlog::info("Output receiver finished");
			});

			spdlog::info("Starting timer");
			auto solution_future = solution_process.exit_future();
			if (solution_future.wait_for(std::chrono::seconds(10)) != std::future_status::ready) {
				write(event_pipe[1], "0", 1);
			} else if (solution_future.get() != system::exit_status::SUCCESS) {
				write(event_pipe[1], "5", 1);
			}
			sender.wait();
			receiver.wait();

			// check the resource usage
			rusage usage;
			getrusage(RUSAGE_CHILDREN, &usage);
			long tusage = (usage.ru_utime.tv_sec+usage.ru_stime.tv_sec)*1000 + (usage.ru_utime.tv_usec+usage.ru_stime.tv_usec)/1000;
			spdlog::info("Memory usage: {}/{}kb", usage.ru_maxrss, maxmem);
			spdlog::info("Time taken: {}/{}ms", tusage, maxtime);

			if (usage.ru_maxrss > maxmem) {
				write(event_pipe[1], "4", 1);
			} else if (usage.ru_maxrss < 900) {
				write(event_pipe[1], "5", 1);
			}

			if (tusage > maxtime) {
				write (event_pipe[1], "3", 1);
			}
			close(event_pipe[1]);
		});

		int grade = 0;
		char event;
		spdlog::info("Waiting for events");
		while (read(event_pipe[0], &event, 1)) {
			if (event == '0') {
				spdlog::info("Timeout");
				grade = 3;
				break;
			} else if (event == '5') {
				spdlog::info("Runtime error");
				grade = 5;
				break;
			} else if (event == '6') {
				spdlog::info("Incorrect output");
				grade = 6;
				break;
			} else if (event == '3') {
				spdlog::info("Time limit exceeded");
				grade = 3;
				break;
			} else if (event == '4') {
				spdlog::info("Memory limit exceeded");
				grade = 3;
				break;
			} else if (event == '7') {
				spdlog::info("Correct output");
				if (grade == 0)
					grade = 7;
			} else {
				spdlog::warn("Unkown event: {}", event);
			}
		}

		spdlog::info("Waiting for tester");
		tester.wait();

		if (grade != 7) {
			passed = false;
			spdlog::info("Test #{} failed, grade: {}", i, grade);
			return grade + i*8;
		}
	}

	spdlog::info("All tests suceeded, grade: 7");
	return 7;
}
}