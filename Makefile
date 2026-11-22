CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude $(shell pkg-config --cflags raylib)
LDLIBS = $(shell pkg-config --libs raylib) -lm
SOURCES = src/main.c src/jogo.c src/onda.c src/mapa.c src/inimigo.c src/torre.c src/visual.c src/persistencia.c
TARGET = ragnarok

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
