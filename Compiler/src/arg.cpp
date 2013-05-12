/*
 * arg.cpp
 *
 *  Created on: 13.04.2013
 *      Author: user
 */
#include <iostream>
#include "arg.h"
#include <getopt.h>
#include <cstdlib>

static void initArg(Arg* arg) {
	arg->asmOutFileName = 0;
	arg->out = 0;
	arg->iFileName = 0;
	arg->reportOutFileName = 0;
}

int parse_args(int argc, char ** argv, Arg* arg) {
	initArg(arg);

	int c;
	const char usage[] = "\ncompiler <filename> [OPTIONS ...]\n\
	--out		<filename>\n\
	--asm		<filename>\n\
	--compiler	<filename>\n\
	--report	<filename>\n\n";

	if (argc < 2) {
		std::cout << usage;
		exit(1);
	}

	static struct option long_options[] = {
		{"out", required_argument, 0, 1},
		{"report", required_argument, 0, 2},
		{"asm", required_argument, 0, 3},
		{"compiler", required_argument, 0, 4},
		{0, 0, 0, 0}
	};

	while ((c = getopt_long(argc, argv, "1:2:3", long_options, NULL)) != -1) {
		switch(c) {
		case 1:
			arg->out = optarg;
			break;
		case 2:
			arg->reportOutFileName = optarg;
			break;
		case 3:
			arg->asmOutFileName = optarg;
			break;
		case 4:
			arg->compiler = optarg;
			break;
		case '?':
			std::cout << "unknown option:" << optarg;
			exit(1);
			break;
		default:
			std::cout << usage;
			exit(1);
		}
	}

    if (argc - optind != 1) {
    	std::cout << usage;
    	exit(1);
    }

	arg->iFileName = argv[optind++];

	return 0;
}


