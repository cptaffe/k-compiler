
SRC=kai.c
OBJ=$(SRC:.c=.o)
LIB=libkai.o

# comp library
LIBS=lex/liblex.o
CFLAGS+=-I$(dir $(LIBS))

.PHONY: all clean

all: $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(LIB): $(OBJ) $(LIBS)
	# archive all prerequisites (regardless of age) to target
	$(LD) -o $@ -r $^

$(LIBS):
	$(MAKE) $(MAKEFLAGS) -C $(dir $@)

clean:
	$(RM) $(LIB) $(OBJ)
	$(foreach var,$(LIBS), $(MAKE) $(MAKEFLAGS) -C $(dir $(var)) clean;)
