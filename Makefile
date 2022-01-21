SRC := src
OBJ := obj

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

all: allim allimd

$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p obj/
	$(CC) -I$(SRC) -c $< -o $@

allim: $(OBJECTS)
	$(CC) $^ src/bin/$@.c -o $@

allimd: $(OBJECTS)
	$(CC) $^ src/bin/$@.c -o $@

clean:
	rm -f allim allimd
	rm -rf obj/
