
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "lex.h"

void lex_err(kai_lex *lex, char *msg, ...) {
	char *str;
	va_list ap;
	va_start(ap, msg);
	vasprintf(&str, msg, ap);
	va_end(ap);

	printf("%d:%d %s\n", lex->pos.line_pos + 1, lex->pos.char_pos + 1, str);
	free(str);
}

int lex_next(kai_lex *lex) {

	// get current indice in string.
	lex->buf.buf[lex->buf.at] = lex->buf.str[lex->buf.strat++];

	// update position
	lex->pos.pos++;
	if (lex->buf.buf[lex->buf.at] == '\n') {
		lex->pos.char_pos = 0;
		lex->pos.line_pos++;
	} else {
		lex->pos.char_pos++;
	}

	// update at.
	return lex->buf.buf[lex->buf.at++];
}

void lex_back(kai_lex *lex) {
	lex->buf.strat--;
	lex->buf.at--;

	// update position
	lex->pos.pos--;
	if (!lex->pos.char_pos) {
		lex->pos.line_pos--;
	} else {
		lex->pos.char_pos--;
	}
}

int lex_peek(kai_lex *lex) {
	int c = lex_next(lex);
	lex_back(lex);
	return c;
}

int lex_emit(kai_lex *lex) {
	// copy
	char str[(lex->buf.at - lex->buf.base) + 1];
	memcpy(&str[0], &lex->buf.buf[lex->buf.base], sizeof(str) - 1);
	str[sizeof(str) - 1] = 0; // null terminate

	// rebase
	lex->buf.at = 0;
	lex->buf.base = 0;

	printf("string: `%s`\n", str);
	return 0;
}

int lex_dump(kai_lex *lex) {
	// rebase
	lex->buf.at = 0;
	lex->buf.base = 0;
	return 0;
}

/*
lex_char_is_{} is used to report if a character is valid in a set.
*/

// char is in identifiers
int lex_char_is_id(char c) {
	if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A') || c == '_') {
		return 1;
	} else { return 0; }
}

// char is in numbers
int lex_char_is_num(char c) {
	if (c >= '0' && c <= '9') {
		return 1;
	} else { return 0; }
}

// char is in whitespace
int lex_char_is_ws(char c) {
	if (c == '\t' || c == '\n' || c == ' ') {
		return 1;
	} else { return 0; }
}

/*
lex_util_{} denotes a reusable lexer function,
it is not part of the state machine directly.
*/

// lexes identifiers
int lex_util_id(kai_lex *lex) {
	int pos = lex->pos.pos;
	char c;

	// first character is not a number
	if (lex_char_is_id(lex_next(lex))) {
		// all other characters are identifiers or numbers
		while (lex_char_is_id(c = lex_next(lex)) || lex_char_is_num(c)) {}
	}
	lex_back(lex);
	if (lex->pos.pos > pos) {
		lex_emit(lex);
		return 1;
	} else { return 0; }
}

// lexes identifiers
int lex_util_num(kai_lex *lex) {
	int pos = lex->pos.pos;
	char c;

	while (lex_char_is_num(c = lex_next(lex))) {}
	lex_back(lex);
	if (lex->pos.pos > pos) {
		lex_emit(lex);
		return 1;
	} else { return 0; }
}

// lexes whitespace
int lex_util_ws(kai_lex *lex) {
	int pos = lex->pos.pos;
	char c;
	while (lex_char_is_ws(c = lex_next(lex))) {}
	lex_back(lex);
	if (lex->pos.pos > pos) {
		lex_emit(lex);
		return 1;
	} else { return 0; }
}

// lexes paren
// 0 for opening paren, 1 for closing paren
int lex_util_paren(kai_lex *lex, int i) {
	char parens[2] = {'(', ')'};
	char c = lex_next(lex);
	if (c == parens[i]) {
		lex_emit(lex);
		return 1;
	} else {
		// syntax error
		lex_dump(lex);
		lex_err(lex, "unexpected character '%c'", c);
		return 0; // cannot continue
	}
}

/*
lex_state_{} functions denote monadic state machine functions.
*/

void *lex_state_all(kai_lex *lex);

// lexes terminal of a s-expression
void *lex_state_sexp_term(kai_lex *lex) {
	// expects paren
	if (lex_util_paren(lex, 1)) {
		lex_util_ws(lex); // optional whitespace
		return &lex_state_all;
	} else { return NULL; }
}

// lexes literals
void *lex_state_lit(kai_lex *lex) {
	char c = lex_peek(lex); // check character
	if (c == '"') {
		// lex string literal
		return NULL;
	} else if (c == '\'') {
		// lex character literal
		return NULL;
	} else if (lex_char_is_num(c)) {
		// lex number literal
		lex_util_num(lex);
	} else if (lex_char_is_id(c)) {
		// lex identifier
		lex_util_id(lex);
	} else if (c == ')') {
		// end of list
		return &lex_state_sexp_term;
	} else {
		// does not conform to any literal, err
		lex_err(lex, "unexpected character for list '%c'", c);
		return NULL;
	}
	// optional whitespace
	lex_util_ws(lex);
	return &lex_state_lit;
}

// id for sexp function or operation.
void *lex_state_sexp_name(kai_lex *lex) {
	if (lex_util_id(lex)) {
		// non-optional whitespace
		if (lex_peek(lex) != ')') {
			if (lex_util_ws(lex)) {
				return &lex_state_lit;
			} else {
				lex_err(lex, "expected whitespace");
				return NULL;
			}
		} else {
			return &lex_state_lit;
		}
	} else {
		lex_err(lex, "expected id");
		return NULL;
	}
}

// lex sexp
void *lex_state_sexp(kai_lex *lex) {
	// expects paren
	if (lex_util_paren(lex, 0)) {
		return &lex_state_sexp_name;
	} else { return NULL; }
}

void *lex_state_all(kai_lex *lex) {
	if (lex_peek(lex) == 0) {
		// eof
		return NULL;
	} else {
		// start symbol
		// optional whitespace
		lex_util_ws(lex);
		return &lex_state_sexp;
	}
}

// lexer state machine
int lex_state(kai_lex *lex) {
	while (lex->func) {
		lex->func = ((void *(*)(kai_lex *)) lex->func)(lex);
	}
	return 0;
}

int kai_lex_throw(kai_lex *lex, char *str) {
	lex->buf.str = str; // set string
	lex_state(lex); // run state machine
	return 0;
}

int kai_lex_pos_init(kai_lex_pos *pos) {
	if (!memset(pos, 0, sizeof(kai_lex_pos))) {
		return 1;
	} else { return 0; }
}

int kai_lex_buf_init(kai_lex_buf *buf) {
	if (!memset(buf, 0, sizeof(kai_lex_buf))) {
		return 1;
	}
	buf->buf = malloc(4096); // approximately one page.
	if (!buf->buf) {
		return 1;
	} else {
		return 0;
	}
}

void kai_lex_buf_free(kai_lex_buf *buf) {
	free(buf->buf);
}

int kai_lex_init(kai_lex *lex) {
	if (kai_lex_buf_init(&lex->buf)) {
		return 1;
	} else if (kai_lex_pos_init(&lex->pos)) {
		return 2;
	} else {
		lex->func = &lex_state_all;
		return 0;
	}
}

void kai_lex_free(kai_lex *lex) {
	kai_lex_buf_free(&lex->buf);
}
