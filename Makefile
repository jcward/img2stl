CC = gcc
CFLAGS = -Wall -Wextra -g -I/usr/include
SRC = cvar.c  main.c  math.c  patch.c  stl.c  tga.c  png.c
OBJ = $(SRC:.c=.o)
TARGET = img2stl
LDFLAGS = -lm -L/usr/lib -lpng


all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
