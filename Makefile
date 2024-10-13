src=src
bin=bin
obj=$(bin)/obj

exec=$(bin)/lsr

sources=$(shell find $(src) -name *.c)
objects=$(patsubst $(src)/%.c, $(obj)/%.o, $(sources))

INSTALL_DIR=/usr/local/bin

cflags=-g
lflags=-g -ggdb -fsanitize=address -lm


$(exec): $(objects)
	gcc $(lflags) -o $@ $^

$(obj)/%.o: $(src)/%.c mkdirs
	gcc -c $(cflags) -o $@ $<

mkdirs:
	-mkdir -p $(build)
	-mkdir -p $(obj)


install: $(exec)
	install -m 755 $(exec) $(INSTALL_DIR)

uninstall:
	rm -rf $(INSTALL_DIR)/$(notdir $(exec))

clean:
	rm -rf $(build)
