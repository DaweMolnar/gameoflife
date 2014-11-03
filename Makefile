.PHONY: all
all: gameoflife

LDFLAGS += -lstdc++ -lSDL2
CPPFLAGS += -Wall -Wextra -pedantic
CPPFLAGS += -O3
#CPPFLAGS += -g
##CPPFLAGS += -pg
OBJECTS = gameoflife.o
SOURCE = gameoflife.cc
gameoflife: $(OBJECTS)
	$(CC) $(CPPFLAGS) $(OBJECTS) -o gameoflife $(LDFLAGS)

.PHONY: clean
clean:
	rm -f gameoflife $(OBJECTS)
