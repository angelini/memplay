CFLAGS=-Wall -Wextra -g -std=c11

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TARGET=memplay

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -rf `find . -name "*.dSYM" -print`
	rm -rf $(OBJECTS) $(TARGET)
