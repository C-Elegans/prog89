LIBMPSSE_DIR=libmpsse
LIBMPSSE_LIB=libmpsse/src/libmpsse.a
LIBYAML_DIR=libyaml
LIBYAML_LIB=$(LIBYAML_DIR)/src/.libs/libyaml.a
CFLAGS=-I$(LIBMPSSE_DIR)/src -I$(LIBYAML_DIR)/include -std=c11 -Wall -Werror -Wextra
CFLAGS+=-Wno-unused-parameter -Wno-unused-variable
CFLAGS+=-fsanitize=address -Wimplicit-fallthrough -g
PREFIX=/usr/local
prog89: main.o programmer.o writeopt.o run.o ihex.o device.o $(LIBMPSSE_LIB) $(LIBYAML_LIB)
	$(CC) $^ -o $@ -lftdi -fsanitize=address -g


$(LIBMPSSE_LIB):
	cd $(LIBMPSSE_DIR)/src && ./configure
	$(MAKE) -C $(LIBMPSSE_DIR)/src
$(LIBYAML_LIB):
	cd $(LIBYAML_DIR) && ./bootstrap && ./configure
	$(MAKE) -C $(LIBYAML_DIR)
clean:
	rm -f prog89
	rm *.o

install: prog89
	cp $< $(PREFIX)/bin/$<
