CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = cvar.c  main.c  math.c  patch.c  stl.c  tga.c
OBJ = $(SRC:.c=.o)
TARGET = tga2stl
LDFLAGS = -lm  # Add this line for math library

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
