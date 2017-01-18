TARGET	= server

SRCDIR	= ./src
OBJDIR	= ./obj
BINDIR	= ./bin

CC		= gcc
LINKER	= gcc -o

CFLAGS	= -I. -Wall -DDEBUG -g
LFLAGS	= -I. -lm -DDEBUG -g -lncurses
LDFLAGS = -lncurses

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

rm		= rm -f


$(BINDIR)/$(TARGET): compiling $(OBJECTS)
	@echo "Linkage"
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@echo "\t"$(TARGET)


$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "\t"$(subst src/,,$(subst .c,.o,$<))


.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup"


.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"


compiling: 
	@echo "Compilation"


