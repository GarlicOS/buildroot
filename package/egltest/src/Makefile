TARGET = egltest

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(wildcard *.c)
	$(CC) $(CFLAGS) -lEGL -lGLESv2 -DVERSION='"$(VERSION)"' -o '$@' $^

clean:
	-rm -rf $(TARGET) *.o
