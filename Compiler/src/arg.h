/*
 * arg.h
 *
 *  Created on: 13.04.2013
 *      Author: user
 */

#pragma once

#include <fstream>
#include <string>

typedef struct {
//	std::ifstream iFile;
	const char * iFileName;

//	std::ofstream asmOutFile;
	const char * asmOutFileName;

//	std::ofstream reportOutFile;
	const char * reportOutFileName;

	const char * out;

	const char * compiler;
} Arg;

int parse_args(int argc, char ** argv, Arg* arg);
