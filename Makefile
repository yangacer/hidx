CC=clang
CFLAGS=-g -Wall -DCANONICAL_HASH
LDFLAGS=
SOURCES=hidx.c test_hidx.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test_hidx

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) ${CFLAGS} -c $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)
