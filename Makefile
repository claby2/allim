SRC := src
OBJ := obj

LIBS := -lxcb

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

all: allim allimd

$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p obj/
	$(CC) -I$(SRC) -c $< -o $@

allim: $(OBJECTS) src/bin/allim.c
	$(CC) $^ -o $@ $(LIBS)

allimd: $(OBJECTS) src/bin/allimd.c
	$(CC) $^ -o $@

clean:
	rm -f allim allimd
	rm -rf obj/

.PHONY: all clean
