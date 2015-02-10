
// Copyright (c) 2015 Connor Taffe <cpaynetaffe@gmail.com>
/*
This file fulfils some of the api for the comp library.
*/

#include <stdio.h>
#include "lex.h"

int kai_throw(char *str) {
	kai_lex lex[1];

	const int len = (sizeof(lex) / sizeof(kai_lex));

	for (int i = 0; i < len; i++) {
		if (kai_lex_init(&lex[i])) {
			return 1;
		}
	}

	for (int i = 0; i < len; i++) {
		if (kai_lex_throw(&lex[i], str)) {
			return 2;
		}
	}
	return 0;
}
