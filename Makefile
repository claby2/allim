SRC := src
OBJ := obj

FLAGS := -Wall -Wpedantic -Wextra -g
LIBS := -lxcb -lxcb-randr

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

all: allim allimr allimd

$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p obj/
	$(CC) $(FLAGS) -I$(SRC) -c $< -o $@

allim: $(OBJECTS) src/bin/allim.c
	$(CC) $(FLAGS) $^ -o $@

allimr: $(OBJECTS) src/bin/allimr.c
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

allimd: $(OBJECTS) src/bin/allimd.c
	$(CC) $(FLAGS) $^ -o $@

clean:
	rm -f allim allimd
	rm -rf obj/

.PHONY: all clean
