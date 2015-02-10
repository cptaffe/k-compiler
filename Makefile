
SRC=kai.c
OBJ=$(SRC:.c=.o)
BIN=$(SRC:.c=)

# comp library
LIBS=libkai/libkai.o
CFLAGS+=-I$(dir $(LIBS))

MAKEFLAGS+= -j5

.PHONY: all clean

all: $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(LIBS):
	$(MAKE) $(MAKEFLAGS) -C $(dir $@)

$(BIN): $(OBJ) $(LIBS)
	# compile all prerequisites (regardless of age, preserving order) to target
	$(CC) $(CFLAGS) -o $@ $+


clean:
	$(RM) $(BIN) $(OBJ)
	$(foreach var,$(LIBS), $(MAKE) $(MAKEFLAGS) -C $(dir $(var)) clean;)
