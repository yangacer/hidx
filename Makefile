CC=clang
CFLAGS=-g -Wall -DCANONICAL_HASH
LDFLAGS=
SOURCES=hidx.c bucket.c mhidx.c test_hidx.c test_mhidx.c test_bucket.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test_hidx test_mhidx test_bucket

all: $(SOURCES) $(EXECUTABLE) 
	
test_hidx: test_hidx.o hidx.o
	$(CC) $(LDFLAGS) test_hidx.o hidx.o -o $@

test_mhidx: test_mhidx.o mhidx.o
	$(CC) $(LDFLAGS) test_mhidx.o mhidx.o bucket.o -o $@

test_bucket: test_bucket.o bucket.o
	$(CC) $(LDFLAGS) test_bucket.o bucket.o -o $@


.c.o:
	$(CC) ${CFLAGS} -c $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)
