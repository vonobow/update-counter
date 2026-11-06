// Copyright (c) 2026 akamoz.jp
// SPDX-License-Identifier: BSD-2-Clause

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>

#define XERR(msg)	((std::cerr << __FILE__ << "(" << __LINE__ << ") [E] " << msg << std::endl), exit(1))
#define WARN(msg)	(std::cerr << __FILE__ << "(" << __LINE__ << ") [W] " << msg << std::endl)
#define CHECK(b, msg)	((b)?(void)(0):(void)(XERR(msg)))

const char *getarg(int argc, char *argv[], int &argi, const char *msg = nullptr)
{
	if (argi >= argc) {
		if (msg != nullptr)
			XERR(msg);
		XERR("no value for argument " << argi);
	}
	return argv[argi++];
}

#define ARGSCANF(fmt, v, msg)	CHECK(sscanf(getarg(argc, argv, argi), (fmt), v) == 1, msg)

int main(int argc, char *argv[]) {
	int argi = 1;
	std::string s, t;
	uintmax_t initval = 0;
	bool reinitialize = false;
	while (argi < argc) {
		s = argv[argi];
		if (s == "-")
			break;
		if (s.substr(0, 1) != "-")
			break;
		argi++;
		if (s == "--")
			break;
		if (s == "-initval")
			ARGSCANF("%ju", &initval, "incorrect initial value");
		else if (s == "-reinitialize")
			reinitialize = true;
		else
			XERR("unknown option at argument " << argi);
	}

	const char *counterFile = getarg(argc, argv, argi, "no counter filename");
	int fd = open(counterFile, O_RDWR | O_CREAT, 0666);
	CHECK(fd != -1, "failed to open the counter file");
	FILE *fp = fdopen(fd, "r+");
	CHECK(fp != nullptr, "failed to bind the counter file with a stream");

	uintmax_t counter = 0;
	CHECK(lockf(fd, F_LOCK, 0) == 0, "failed to lock the counter file");
	if (reinitialize)
		counter = initval;
	else {
		if (fscanf(fp, "%ju", &counter) != 1) {
			WARN("failed to read counter value, clear the count");
			counter = initval;
		}
		else
			counter++;
	}
	CHECK(ftruncate(fd, 0) == 0, "failed to clear the counter file");
	rewind(fp);
	CHECK(fprintf(fp, "%jd\n", counter) > 0, "failed to write value to the file");
	CHECK(fclose(fp) == 0, "failed to close the counter file"); // and unlock

	printf("%ju\n", counter);

	return 0;
}
