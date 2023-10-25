TARGET = garlicui
PO_FILES = $(wildcard locale/*/LC_MESSAGES/*.po)
MO_FILES = $(patsubst %.po,%.mo,$(PO_FILES))

.PHONY: all clean

all: $(TARGET) translations

$(TARGET): $(wildcard *.c)
	$(CC) $(CFLAGS) -lSDL -lSDL_image -lSDL_ttf -lSDL_gfx -lrsvg-2 -lcairo -lgobject-2.0 -lglib-2.0 -lxml2 -lcjson -licuuc -lm -DVERSION='"$(VERSION)"' -o '$@' $^

translations: $(MO_FILES)

%.mo: %.po
	msgfmt -o $@ $<

clean:
	-rm -rf $(TARGET) $(MO_FILES) *.o
