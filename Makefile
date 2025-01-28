program = fractal-cli

ifeq ($(PREFIX),)
	ifeq ($(USER),root)
		PREFIX = /usr/local
	else
		PREFIX = ~/.local
	endif
endif

$(program): $(program).c
	gcc -Wall -lcurses -o $(program) $(program).c

install: $(program)
	install -Dm755 $(program) $(PREFIX)/bin/

.PHONY: uninstall
uninstall:
	rm -f $(PREFIX)/bin/$(program)

.PHONY: clean
clean:
	rm -f $(program)
