CC = gcc
CFLAGS = -I../include -Wall
OBJDIR = ../obj
SRCDIR = ../src
BINDIR = .
TARGET = $(BINDIR)/setup

# Source and object files
SRC = $(SRCDIR)/main.c $(SRCDIR)/language.c $(SRCDIR)/python.c $(SRCDIR)/git.c
OBJ = $(OBJDIR)/main.o $(OBJDIR)/language.o $(OBJDIR)/python.o $(OBJDIR)/git.o

# Default target
all: $(TARGET)

# Linking the binary
$(TARGET): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compiling source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJDIR) $(BINDIR)/setup

# Rebuild from scratch
rebuild: clean all
