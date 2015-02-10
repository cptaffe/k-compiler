
// Copyright (c) 2015 Connor Taffe <cpaynetaffe@gmail.com>
/*
This file fulfils some of the api for the comp library.
*/

#ifndef KAI_LEX_H_
#define KAI_LEX_H_

// position data
typedef struct {
	int pos;		// total characters
	int char_pos;	// characters on current line
	int line_pos;	// current line number
} kai_lex_pos;

// position functionality
int kai_lex_pos_init(kai_lex_pos *pos);

typedef struct {
	char *str;	// string we are lexing
	char *buf;	// character buffer
	int max;	// maximum allocated location
	int at;		// current location
	int strat;		// current location
	int base;	// base location
} kai_lex_buf;

// position functionality
int kai_lex_buf_init(kai_lex_buf *buf);
void kai_lex_buf_free(kai_lex_buf *buf);

typedef struct {
	kai_lex_pos pos;	// source position
	kai_lex_buf buf;	// character buffer
	void *func;
} kai_lex;

// lexer functionality
int kai_lex_init(kai_lex *lex);
void kai_lex_free(kai_lex *lex);
int kai_lex_throw(kai_lex *lex, char *str);

#endif // KAI_LEX_H_
