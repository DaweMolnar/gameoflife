.PHONY: all
all: gameoflife

LDFLAGS += -lSDL
CPPFLAGS += -Wall -Wextra -pedantic
CPPFLAGS += -O0
CPPFLAGS += -g
CPPFLAGS += -pg

.PHONY: clean
clean:
	rm -f gameoflife
