
// Copyright (c) 2015 Connor Taffe <cpaynetaffe@gmail.com>
/*
This file describes a command line utility utilizing the comp library.
*/

#include <stdio.h>
#include "kai.h"

// accepts command line arguments
int main(int argc, char *argv[]) {

	/* command line arguments as follows
		none as of now.
	*/

	if (argc > 1) {
		fprintf(stderr, "%s does not accept any arguments.\n", argv[0]);
	}

	kai_throw("hello");

}
