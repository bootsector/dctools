CC = gcc
CFLAGS = -c -Wall
LDFLAGS =
SOURCES = main.c iso9660.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = extract

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm $(OBJECTS) $(EXECUTABLE) $(EXECUTABLE).exe