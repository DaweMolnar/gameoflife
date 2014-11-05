.PHONY: all
all: gameoflife

LDFLAGS += -lstdc++ -lSDL2
CPPFLAGS += -Wall -Wextra -pedantic
CPPFLAGS += -flto -Ofast -fuse-linker-plugin -std=c++11 -pthread
#CPPFLAGS += -g
##CPPFLAGS += -pg
OBJECTS = gameoflife.o
SOURCE = gameoflife.cpp
gameoflife: $(OBJECTS)
	$(CC) $(CPPFLAGS) $(OBJECTS) -o gameoflife $(LDFLAGS)

.PHONY: clean
clean:
	rm -f gameoflife $(OBJECTS)
