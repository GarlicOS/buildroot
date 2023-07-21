TARGET = display

.PHONY: clean
.PHONY: $(TARGET)

$(TARGET): $(wildcard *.c)
	$(CC) -o '$@' $^

clean:
	-rm -rf $(TARGET) *.o
