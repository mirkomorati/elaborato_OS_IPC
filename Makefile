# compiler and linker
CC:=gcc
LD:=gcc

#FLAGS
CFLAGS:=-Wall -DDEBUG 

#directories
BINDIR:=bin
OBJDIR:=$(BINDIR)/obj
SRCDIR:=src

#files
MAIN:=main.c
SRC:=$(addprefix $(SRCDIR)/, ending_utils.c shmatrix_lib.c)
HDR:=headers/ending_utils.h headers/shmatrix_lib.h

#object files
OBJECTS=$(addprefix $(OBJDIR)/, main.o ending_utils.o shmatrix_lib.o)

#target
TARGET:=$(BINDIR)/elaborato_IPC

#-----------------------rules----------------------------

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ 

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/main.o : $(MAIN) 
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJECTS) : | $(OBJDIR)

$(OBJDIR) :
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)
