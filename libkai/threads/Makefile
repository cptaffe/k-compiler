SRC=thread.c threads.s
OBJ=$(patsubst %.s,%.o,$(patsubst %.c,%.o,$(SRC)))
LIB=libthread.o

.PHONY: all clean

all: $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.s
	$(CC) $(CFLAGS) -c $<

$(LIBS):
	$(MAKE) $(MAKEFLAGS) -C $(dir $@)

$(LIB): $(OBJ)
	# archive all prerequisites (regardless of age) to target
	$(LD) -o $@ -r $^

clean:
	$(RM) $(LIB) $(OBJ)
