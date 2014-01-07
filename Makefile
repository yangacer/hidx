CC=clang
CFLAGS=-g -Wall -DCANONICAL_HASH
LDFLAGS=
SOURCES=hidx.c bucket.c test_hidx.c test_bucket.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test_hidx test_bucket

all: $(SOURCES) $(EXECUTABLE) 
	
test_hidx: test_hidx.o hidx.o
	$(CC) $(LDFLAGS) test_hidx.o hidx.o -o $@

test_bucket: test_bucket.o bucket.o
	$(CC) $(LDFLAGS) test_bucket.o bucket.o -o $@

.c.o:
	$(CC) ${CFLAGS} -c $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)
