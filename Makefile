# compiler and linker
CC:=gcc
LD:=gcc

#doxygen command
DOXY:=doxygen

#FLAGS
CFLAGS:=-Wall -DDEBUG -g

#directories
BINDIR:=bin
OBJDIR:=$(BINDIR)/obj
SRCDIR:=src
HDRDIR:=headers
DOCDIR:=doc

#files
MAIN:=main.c
SRC:=$(addprefix $(SRCDIR)/, ending_utils.c shm_lib.c \
	msg_utils.c)
HDR:=$(addprefix $(HDRDIR)/, ending_utils.h shm_lib.h \
	msg_utils.h std_lib.h)
DOXYFILE:=Doxyfile 

#object files
OBJECTS=$(addprefix $(OBJDIR)/, main.o ending_utils.o shm_lib.o msg_utils.o)

#target
TARGET:=$(BINDIR)/elaborato_IPC

#-----------------------rules----------------------------

all : doc build

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

doc: $(HDR)
	$(DOXY) $(DOXYFILE)

clean:
	rm -rf $(OBJDIR)

clean-doc:
	rm -rf $(DOCDIR)

clean-all:
	rm -rf $(BINDIR)
	rm -rf $(DOCDIR)
