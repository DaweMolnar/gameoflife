.PHONY: all
all: gameoflife

LDFLAGS += -lstdc++ -lSDL
CPPFLAGS += -Wall -Wextra -pedantic
CPPFLAGS += -O0
CPPFLAGS += -g
CPPFLAGS += -pg
OBJECTS = gameoflife.o
SOURCE = gameoflife.cc
gameoflife: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o gameoflife $(LDFLAGS)

.PHONY: clean
clean:
	rm -f gameoflife
