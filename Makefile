# compiler and linker
CC:=gcc
LD:=gcc

#doxygen command
DOXY:=doxygen

#FLAGS
CFLAGS?=-Wall -DDEBUG -g -O0

#directories
BINDIR:=bin
OBJDIR:=$(BINDIR)/obj
SRCDIR:=src
HDRDIR:=headers
DOCDIR:=doc
UTILDIR:=utility
UTILBINDIR:=$(BINDIR)/utility

#files
#MAIN:=$.c
SRC:=$(addprefix $(SRCDIR)/, father.c ending_lib.c shm_lib.c \
	msg_lib.c, sem_lib.c io_lib.c child.c)
HDR:=$(addprefix $(HDRDIR)/, father.h ending_lib.h shm_lib.h \
	msg_lib.h std_lib.h sem_lib.h io_lib.h child.h)
DOXYFILE:=Doxyfile 

# utilities
MATRIX_GEN_EXE=$(addprefix $(UTILBINDIR)/, matrix_gen)
MATRIX_GEN_SRC=$(addprefix $(UTILDIR)/, matrix_gen.c)

MATRIX_DIFF_EXE=$(addprefix $(UTILBINDIR)/, matrix_diff)
MATRIX_DIFF_SRC=$(addprefix $(UTILDIR)/, matrix_diff.c)

SH=$(addprefix $(UTILDIR)/, *.sh)
SHCP=$(addprefix $(UTILBINDIR), *.sh)

#object files
OBJECTS=$(addprefix $(OBJDIR)/, father.o ending_lib.o \
	shm_lib.o msg_lib.o sem_lib.o io_lib.o thread_lib.o child.o)

#target
TARGET:=$(BINDIR)/elaborato_IPC

#-----------------------rules----------------------------

all : build utility

build : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ 

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJECTS) : | $(OBJDIR)

$(OBJDIR) :
	mkdir -p $(OBJDIR)

$(BINDIR) :
	mkdir -p $(BINDIR)

$(UTILBINDIR) :
	mkdir -p $(UTILBINDIR)

doc: $(HDR)
	$(DOXY) $(DOXYFILE)

clean:
	rm -rf $(BINDIR)

clean-obj:
	rm -rf $(OBJDIR)

clean-doc:
	rm -rf $(DOCDIR)

clean-all:
	rm -rf $(BINDIR)
	rm -rf $(DOCDIR)
	rm -rf $(UTILBINDIR)

utility: $(UTILBINDIR) $(MATRIX_GEN_EXE) $(MATRIX_DIFF_EXE) $(SHCP)

$(MATRIX_GEN_EXE) : $(MATRIX_GEN_SRC)
	$(CC) $< -o $@

$(MATRIX_DIFF_EXE) : $(MATRIX_DIFF_SRC)
	$(CC) $< -o $@

$(SHCP) : $(SH)
	cp -p $(SH) $(UTILBINDIR)
	
