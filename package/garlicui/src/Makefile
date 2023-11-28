TARGET = garlicui
PO_FILES = $(wildcard locale/*/LC_MESSAGES/*.po)
MO_FILES = $(patsubst %.po,%.mo,$(PO_FILES))

.PHONY: all clean

all: $(TARGET) translations

$(TARGET): $(wildcard *.c)
	$(CC) $(CFLAGS) $(LDFLAGS) -DVERSION='"$(VERSION)"' -o '$@' $^

translations: $(MO_FILES)

%.mo: %.po
	$(HOST_DIR)/bin/msgfmt -o $@ $<

clean:
	-rm -rf $(TARGET) $(MO_FILES) *.o
