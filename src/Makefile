CC           = gcc
<<<<<<< HEAD
CFLAGS       = -O2 -Wall -Werror -pedantic -std=gnu99
=======
CFLAGS       = -O2 -Wall -Werror -pedantic -std=gnu99 -pthread
>>>>>>> upstream/master
#CFLAGS       = -O0 -g

#set the following variables for custom libraries and/or other objects
EXTOBJS      =
<<<<<<< HEAD
LIBS         = -lm -lpthread
LIBPATHS     = 
INCLUDEPATHS = 
=======
LIBS         = -lm -lpthread -no-pie
LIBPATHS     = 
INCLUDEPATHS =
ARCHIVE      = sock.a
>>>>>>> upstream/master

OBJ         = ./.build

PROGRAM     = netster
SRC         = $(wildcard ./*.c)
OBJS        = $(patsubst ./%.c,$(OBJ)/%.o,$(SRC))

$(PROGRAM): $(OBJS)
<<<<<<< HEAD
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJ)/%.o: %.c netster.h
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) -c -o $@ $<
=======
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(ARCHIVE) $(LIBS) || $(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJ)/%.o: %.c netster.h
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c -o $@ $<
>>>>>>> upstream/master

.PHONY: part1
part1:
	cp -n .templates/chat.* .
	cp .templates/part1.h netster.h

.PHONY: part2
part2:
	cp -n .templates/file.* .
	cp .templates/part2.h netster.h

.PHONY: part3
part3:
	cp -n .templates/stopandwait.* .
	cp .templates/part3.h netster.h

.PHONY: part4
part4:
	cp -n .templates/gobackn.* .
	cp .templates/part4.h netster.h

<<<<<<< HEAD
=======
.PHONY: archive
archive: sock.c
	$(CC) -c $(CFLAGS) $(LIBS) sock.c -o sock.a 

>>>>>>> upstream/master
.PHONY: clean
clean:
	-rm -f $(OBJS) $(PROGRAM)
	rm -rf $(OBJ)
