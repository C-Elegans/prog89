LIBMPSSE_DIR=libmpsse
LIBMPSSE_LIB=libmpsse/src/libmpsse.a
CFLAGS=-I$(LIBMPSSE_DIR)/src -std=c11 -Wall -Werror -Wextra -Wno-unused-parameter -Wno-unused-variable
CFLAGS+=-fsanitize=address -Wimplicit-fallthrough -g
PREFIX=/usr/local
prog89: main.o programmer.o writeopt.o run.o ihex.o device.o $(LIBMPSSE_LIB)
	$(CC) $^ -o $@ -lftdi -fsanitize=address -g


$(LIBMPSSE_LIB):
	cd $(LIBMPSSE_DIR)/src && ./configure
	make -C $(LIBMPSSE_DIR)/src
clean:
	rm -f prog89
	rm *.o

install: prog89
	cp $< $(PREFIX)/bin/$<
