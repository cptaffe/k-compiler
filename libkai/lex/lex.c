
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"

int kai_lex_pos_init(kai_lex_pos *pos) {
	if (!memset(pos, sizeof(kai_lex_pos), 0)) {
		return 1;
	} else { return 0; }
}

int kai_lex_buf_init(kai_lex_buf *buf) {
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
	} else { return 0; }
}

void kai_lex_free(kai_lex *lex) {
	kai_lex_buf_free(&lex->buf);
}

int kai_lex_throw(kai_lex *lex, char *str) {
	printf("got: %s\n", str);
	return 0;
}
