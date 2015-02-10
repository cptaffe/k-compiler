
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"

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
	char str[lex->buf.at - lex->buf.base];
	memcpy(&str[0], &lex->buf.buf[lex->buf.base], sizeof(str));
	str[sizeof(str)] = 0; // null terminate

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

// function declaration
int lex_char_is_id(char c) {
	if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A') || c == '_') {
		return 1;
	} else { return 0; }
}

int lex_char_is_num(char c) {
	if (c >= '0' && c <= '9') {
		return 1;
	} else { return 0; }
}

// function declaration
int lex_char_is_ws(char c) {
	if (c == '\t' || c == '\n' || c == ' ') {
		return 1;
	} else { return 0; }
}

int lex_util_id(kai_lex *lex) {
	int pos = lex->pos.pos;
	char c;
	while (lex_char_is_id(c = lex_next(lex))) {}
	lex_back(lex);
	if (lex->pos.pos > pos) {
		lex_emit(lex);
		return 1;
	} else { return 0; }
}

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

int lex_util_paren(kai_lex *lex, int i) {
	char parens[2] = {'(', ')'};
	char c = lex_next(lex);
	if (c == parens[i]) {
		lex_emit(lex);
		return 1;
	} else {
		// syntax error
		lex_dump(lex);
		printf("%d:%d err: unexpected character '%c'\n", lex->pos.line_pos + 1, lex->pos.char_pos, c);
		return 0; // cannot continue
	}
}

void *lex_state_all(kai_lex *lex);

void *lex_state_sexp_term(kai_lex *lex) {
	// expects paren
	if (lex_util_paren(lex, 1)) {
		return &lex_state_all;
	} else { return NULL; }
}

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
		return NULL;
	} else if (lex_char_is_id(c)) {
		// lex identifier
		return NULL;
	} else if (c == ')') {
		// end of list
		return &lex_state_sexp_term;
	} else {
		// does not conform to any literal, err
		printf("%d:%d err: unexpected character for list '%c'\n", lex->pos.line_pos + 1, lex->pos.char_pos, c);
		return NULL;
	}
}

// id for sexp function or operation.
void *lex_state_sexp_name(kai_lex *lex) {
	if (lex_util_id(lex)) {
		// non-optional whitespace
		if (lex_peek(lex) != ')') {
			if (lex_util_ws(lex)) {
				return &lex_state_lit;
			}
		} else {
			return &lex_state_lit;
		}
	}
	printf("%d:%d err: unexpected non-id\n", lex->pos.line_pos + 1, lex->pos.char_pos);
	return NULL; // does not conform, cannot continue
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
	printf("got: %s\n", str);
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
