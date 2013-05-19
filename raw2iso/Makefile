CC = gcc
CFLAGS = -c -Wall
LDFLAGS =
SOURCES = main.c raw2iso.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = raw2iso

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm $(OBJECTS) $(EXECUTABLE)