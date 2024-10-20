src=src
bin=bin
obj=$(bin)/obj

exec=$(bin)/lsr

sources=$(shell find $(src) -name *.c)
objects=$(patsubst $(src)/%.c, $(obj)/%.o, $(sources))

INSTALL_DIR=/usr/local/bin

cflags=-g -DLASER_NF_SYMBOLS
lflags=-g -ggdb -fsanitize=address

cflags_release=-DLASER_NF_SYMBOLS
lflags_release=


$(exec): $(objects)
	gcc $(lflags) -o $@ $^

$(obj)/%.o: $(src)/%.c mkdirs
	gcc -c $(cflags) -o $@ $<


release: $(objects)
	gcc $(lflags_release) -o $(exec) $^

$(obj)/%.o: $(src)/%.c 
	make clean
	gcc -c $(cflags) -o $@ $<

mkdirs:
	-mkdir -p $(bin)
	-mkdir -p $(obj)


install: release
	install -m 755 $(exec) $(INSTALL_DIR)

uninstall:
	rm -rf $(INSTALL_DIR)/$(notdir $(exec))

clean:
	-rm -rf $(bin)
