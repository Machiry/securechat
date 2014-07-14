CC=gcc
OUTFOLDER=out
HEADERS_FOLDER=$(OUTFOLDER)/headers
LIB_FOLDER=$(OUTFOLDER)/lib
OBJ_FOLDER=$(OUTFOLDER)/obj
UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
	LIBEXT=.dylib
	LIBFLAGS=-dynamiclib
endif
ifeq ($(UNAME),Linux)
	LIBEXT=.so
	LIBFLAGS=-shared,Wl,-soname,lib$(LIBNAME).so.1
endif
LIBNAME=libdslib$(LIBEXT)
DEBUG_FLAG=
all: lib copyobj copyheaders copylibrary cleancurr
debug: setdebug all
clean: cleancurr cleanout
setdebug:
	$(eval DEBUG_FLAG=-g)
cleancurr:
	rm -rf *.o
	rm -rf $(LIBNAME)
cleanout:
	rm -rf $(OUTFOLDER)
stack:
	$(CC) $(DEBUG_FLAG) -fPIC -c stack.c
llist:
	$(CC) $(DEBUG_FLAG) -fPIC -c linkedlist.c
btree:
	$(CC) $(DEBUG_FLAG) -fPIC -c binarytree.c
queue:
	$(CC) $(DEBUG_FLAG) -fPIC -c queue.c 
lib: stack llist btree queue
	$(CC) $(DEBUG_FLAG) $(LIBFLAGS) stack.o linkedlist.o binarytree.o queue.o -o $(LIBNAME)
copyheaders:
	mkdir -p $(HEADERS_FOLDER)
	cp *.h $(HEADERS_FOLDER)
copylibrary:
	mkdir -p $(LIB_FOLDER)
	cp $(LIBNAME) $(LIB_FOLDER)
copyobj:
	mkdir -p $(OBJ_FOLDER)
	cp *.o $(OBJ_FOLDER)
