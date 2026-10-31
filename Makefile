CC ?= cc
CFLAGS ?=
SRC = src/bar.c \
      src/cmus.c \
      src/cover.c \
      src/disk.c \
      src/draw.c \
      src/main.c \
      src/proc.c \
      src/terminal.c

OBJ = ccover

all: $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(OBJ)

clean:
	rm -f $(OBJ)
