
// Copyright (c) 2015 Connor Taffe <cpaynetaffe@gmail.com>
/*
This file fulfils some of the api for the comp library.
*/

#include "lex.h"

int kai_throw(char *str) {
	kai_lex lex;
	if (kai_lex_init(&lex)) {
		return 1;
	}
	if (kai_lex_throw(&lex, str)) {
		return 2;
	}
	return 0;
}
