# compiler and linker
CC:=gcc
LD:=gcc

#doxygen command
DOXY:=doxygen

#FLAGS
CFLAGS:=-Wall -DDEBUG 

#directories
BINDIR:=bin
OBJDIR:=$(BINDIR)/obj
SRCDIR:=src
DOCDIR:=doc

#files
MAIN:=main.c
SRC:=$(addprefix $(SRCDIR)/, ending_utils.c shmatrix_lib.c msg_utils.c)
HDR:=headers/ending_utils.h headers/shmatrix_lib.h \
	headers/msg_utils.h
DOXYFILE:=Doxyfile 

#object files
OBJECTS=$(addprefix $(OBJDIR)/, main.o ending_utils.o shmatrix_lib.o msg_utils.o)

#target
TARGET:=$(BINDIR)/elaborato_IPC

#-----------------------rules----------------------------

all : build doc

build : $(TARGET)

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

doc:
	$(DOXY) $(DOXYFILE)

clean:
	rm -rf $(OBJDIR)

clean-doc:
	rm -rf $(DOCDIR)

clean-all:
	rm -rf $(BINDIR)
	rm -rf $(DOCDIR)
