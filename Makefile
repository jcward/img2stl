# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I./include

# Directories
SRCDIR = src
INCDIR = include
BINDIR = bin

# Source files and object files
SRC = cvar.c main.c math.c patch.c stl.c tga.c png.c
OBJ = $(addprefix $(SRCDIR)/,$(SRC:.c=.o))
TARGET = $(BINDIR)/img2stl

# Libraries and link flags
LDFLAGS = -lm -L/usr/lib -lpng

# Phony targets
.PHONY: all clean

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile C files to object files
$(SRCDIR)/%.o: $(SRCDIR)/%.c $(wildcard $(INCDIR)/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up rule
clean:
	rm -f $(OBJ) $(TARGET)

# Install rule (optional)
install: all
	install -d $(DESTDIR)/usr/local/bin
	install $(TARGET) $(DESTDIR)/usr/local/bin
