##MAKE = make
CC = gcc
SRCDIR = src
OBJDIR = obj
SRC = $(wildcard $(shell find $(SRCDIR) -name '*.c')) ## "wildcard" faz com que o makefile procure todos arquivos .c em src (separados por espaço)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC)) ## "patsubst" faz com que o makefile substitua o caminho de cada arquivo .c por .o
TARGET = programa-principal
TARGET_TEST = programa-testes
FLAGS = -g -Wall -Wextra -ffunction-sections -fdata-sections
LIBS = -lglib-2.0 -lncurses ## >> sudo apt-get install libglib2.0-dev libncurses5-dev libncursesw5-dev
INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -Iinclude
LINKER_FLAGS = -Wl,--gc-sections,--print-gc-sections

## make = make programa-principal (default)
##all: $(TARGET)
all: release ## using the release target as default, so automatic tests can be run

## make debug (clean e compila com flags de debug)
debug: FLAGS += -g3 -DDEBUG
debug: clean $(TARGET)

## make release (clean e compila com flags de release - otimização)
release: FLAGS += -O3 -DNDEBUG
release: clean $(TARGET)

## make programa-principal (compila programa principal)
$(TARGET): $(OBJ)
	$(CC) $(FLAGS) $(INCLUDE) $(OBJ) -o $(TARGET) $(LIBS) $(LINKER_FLAGS)
	$(MAKE) $(TARGET_TEST)

## make programa-testes (compila programa de testes)
$(TARGET_TEST): $(OBJ)
	$(CC) $(FLAGS) $(INCLUDE) $(OBJ) -o $(TARGET_TEST) $(LIBS) $(LINKER_FLAGS)

## make docs (gera documentação)
## >> sudo apt-get install doxygen
docs:
	doxygen ../Doxyfile

## make clean-docs (limpa documentação)
clean-docs:
	rm -rf ../docs/html

## make open-docs (abre documentação)
open-docs:
	xdg-open ../docs/html/index.html

## (re)compila todos arquivos .c em .o
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $(INCLUDE) $< -o $@ $(LIBS)

## make clean (limpa todos arquivos gerados)
clean:
	rm -rf $(OBJDIR)
	rm -rf $(TARGET)
	rm -rf $(TARGET_TEST)