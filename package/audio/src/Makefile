TARGET = audio

.PHONY: clean
.PHONY: $(TARGET)

$(TARGET): $(wildcard *.c)
	$(CC) -ltinyalsa -o '$@' $^

clean:
	-rm -rf $(TARGET) *.o
