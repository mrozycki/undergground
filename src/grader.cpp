#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include <spdlog/spdlog.h>

#include "grader.h"

using namespace std;

namespace ugg {
int grade(const char* problemid, std::string const& executable_path)
{
	setvbuf(stdout, NULL, _IONBF, 0);

	bool passed = true;

	char testpath[32];
	sprintf(testpath, "problems/%s/tests", problemid);
	FILE *testfile = fopen(testpath, "r");
	if (!testfile)
	{
		return -1;
	}

	int testnum;
	fscanf(testfile, "%d", &testnum);

	for (int i = 1; i <= testnum; i++)
	{
		spdlog::info("Running test {}/{}", i, testnum);
		char inpath[32], outpath[32], infile[32], outfile[32];
		fscanf(testfile, "%s %s", infile, outfile);
		sprintf(inpath, "problems/%s/%s", problemid, infile);
		sprintf(outpath, "problems/%s/%s", problemid, outfile);
		int maxmem, maxtime;
		fscanf(testfile, "%d %d", &maxmem, &maxtime);
		spdlog::debug("Test details: inpath={}, outpath={}, maxmem={}, maxtime={}", inpath, outpath, maxmem, maxtime);

		pid_t pidSolution, pidTester, pidLimiter, pidSender, pidReceiver;
		int cpipe[2]; pipe(cpipe);

		pidTester = fork();
		if (!pidTester)
		{
			close (cpipe[0]);
			int inpipe[2], outpipe[2];
			pipe(inpipe);
			pipe(outpipe);

			pidSolution = fork();
			if (!pidSolution)
			{
				spdlog::debug("Starting solution process");
				prctl(PR_SET_PDEATHSIG, 9);
				struct rlimit memlimit, proclimit;
				memlimit.rlim_cur = 128*1024*1024;
				memlimit.rlim_max = -1;
				setrlimit (RLIMIT_STACK, &memlimit);

				proclimit.rlim_cur = 1;
				proclimit.rlim_max = 1;
				setrlimit (RLIMIT_NPROC, &proclimit);

				dup2 (inpipe[0], STDIN_FILENO);
				dup2 (outpipe[1], STDOUT_FILENO);
				execl (executable_path.c_str(), executable_path.c_str(), NULL);
				write (cpipe[1], "5", 2);
				close(cpipe[1]);
				spdlog::debug("Solution process finished");
				exit(0);
			}
			close(outpipe[1]);

			pidSender = fork();
			if (!pidSender)
			{
				spdlog::debug("Starting input sender process");
				FILE *indata = fopen(inpath, "r");
				char instr[32];
				int k = 0;
				while (fscanf(indata, "%s", instr) != -1)
				{
					write (inpipe[1], instr, strlen(instr));
					write (inpipe[1], "\n", 1);
				}
				write (inpipe[1], "", 1);
				close(inpipe[1]);
				fclose(indata);
				close(cpipe[1]);
				spdlog::debug("Input sender process finished");
				exit(0);
			}

			pidReceiver = fork();
			if (!pidReceiver)
			{
				spdlog::debug("Starting output receiver process");
				char outstr[32], ansstr[32];
				FILE *outdata = fopen(outpath, "r");
				FILE *answer = fdopen(outpipe[0], "r");

				while (fscanf(answer, "%s", ansstr) != -1
						&& fscanf(outdata, "%s", outstr) != -1)
				{
					if (strcmp(ansstr, outstr))
					{
						write(cpipe[1], "6", 2);
						break;
					}
				}

				if (fscanf(answer, "%s", ansstr) != -1
						|| fscanf(outdata, "%s", outstr) != -1)
				{
					write(cpipe[1], "6", 2);
				}
				else
				{
					write(cpipe[1], "7", 1);
				}

				fclose(answer);
				fclose(outdata);
				close(cpipe[1]);
				spdlog::debug("Output receiver process finished");
				exit(0);
			}

			// wait
			write (cpipe[1], "8", 1);
			waitpid (pidSolution, NULL, 0);
			write (cpipe[1], "9", 1);
			kill (pidSender, 9);

			// check the resource usage
			struct rusage usage;
			getrusage(RUSAGE_CHILDREN, &usage);
			long tusage = (usage.ru_utime.tv_sec+usage.ru_stime.tv_sec)*1000 + (usage.ru_utime.tv_usec+usage.ru_stime.tv_usec)/1000;
			spdlog::info("Memory usage: {}/{}kb", usage.ru_maxrss, maxmem);
			spdlog::info("Time taken: {}/{}ms", tusage, maxtime);

			if (usage.ru_maxrss > maxmem)
			{
				write (cpipe[1], "4", 2);
			}
			else if (usage.ru_maxrss < 900)
			{
				write (cpipe[1], "5", 2);
			}

			if (tusage > maxtime)
			{
				write (cpipe[1], "3", 2);
			}
			close(cpipe[1]);
			spdlog::debug("Tester finished");
			exit(0);
		}
		else
		{
			char chunk[32];
			while (read (cpipe[0], chunk, 32))
			{
				if (chunk[0] == '8')
				{
					spdlog::debug("Starting timer");
					break;
				}
			}

			pidLimiter = fork();
			if (!pidLimiter)
			{
				close(cpipe[0]);
				usleep(1000*10000);
				write(cpipe[1], "0", 10);
				close(cpipe[1]);
				exit(0);
			}

			int grade = 0;

			close(cpipe[1]);
			while (read (cpipe[0], chunk, 1))
			{
				if (chunk[0] == '9')
				{
					spdlog::debug("Timer stopped");
					kill(pidLimiter, 9);
				}
				else if (chunk[0] == '0')
				{
					spdlog::debug("Timeout");
					grade = 3;
					kill(pidTester, 9);
					break;
				}
				else if (chunk[0] == '5')
				{
					spdlog::debug("Runtime error");
					grade = 5;
					kill(pidLimiter, 9);
					kill(pidTester, 9);
					break;
				}
				else if (chunk[0] == '6')
				{
					spdlog::debug("Incorrect output");
					grade = 6;
					kill(pidLimiter, 9);
					kill(pidTester, 9);
					break;
				}
				else if (chunk[0] == '3')
				{
					spdlog::debug("Time limit exceeded");
					grade = 3;
					break;
				}
				else if (chunk[0] == '4')
				{
					spdlog::debug("Memory limit exceeded");
					grade = 3;
					break;
				}
				else if (chunk[0] == '7')
				{
					spdlog::debug("Correct output");
					if (grade == 0)
						grade = 7;
				}
			}

			spdlog::debug("Waiting for tester");
			waitpid (pidTester, NULL, 0);

			if (grade == 0)
				continue;
			if (grade != 7)
			{
				passed = false;
				spdlog::info("Test #{} failed, grade: {}", i, grade);
				return grade + i*8;
			}
		}
	}

	if (passed) {
		spdlog::info("All tests suceeded, grade: 7");
		return 7;
	}
}
}