CC = gcc
CFLAGS = -c -Wall
LDFLAGS =
SOURCES = main.c iso2raw.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = iso2raw

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm $(OBJECTS) $(EXECUTABLE)