
SRC=lex.c
OBJ=$(SRC:.c=.o)
LIB=liblex.o

CFLAGS+= --std=gnu99

.PHONY: all clean

all: $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(LIB): $(OBJ)
	# archive all prerequisites (regardless of age) to target
	$(LD) -o $@ -r $^

clean:
	$(RM) $(LIB) $(OBJ)
