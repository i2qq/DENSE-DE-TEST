CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O2
SRC = src/dense_main.c src/dense_eval.c src/dense_dict.c src/dense_proj.c src/dense_de.c src/dense_cb.c src/dense_py.c
TARGET = dense

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

install: $(TARGET)
	install -d /usr/local/bin
	install -m 755 $(TARGET) /usr/local/bin/

clean:
	rm -f $(TARGET) *.o

.PHONY: all install clean
