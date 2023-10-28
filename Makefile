
TARGET = mydiff
CFLAGS = -std=c99 -pedantic -Wall -g -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
SOURCE = mydiff.c
OBJECT = $(SOURCE:.c=.o)


.PHONY: all clean


all: $(TARGET)

$(TARGET): $(OBJECT)
	gcc $(CFLAGS) -o $(TARGET) $(OBJECT)

$(OBJECT): $(SOURCE)
	gcc $(CFLAGS) -o $(OBJECT) -c $(SOURCE)

clean:
	rm -f $(OBJECTS) $(TARGET)
