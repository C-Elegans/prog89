LIBMPSSE_DIR=libmpsse
LIBMPSSE_LIB=libmpsse/src/libmpsse.a
CFLAGS=-I$(LIBMPSSE_DIR)/src -std=c11 -Wall -Werror -Wextra -Wno-unused-parameter -Wno-unused-variable
CFLAGS+=-fsanitize=address -Wimplicit-fallthrough -g
8051_prog: main.o programmer.o writeopt.o run.o ihex.o $(LIBMPSSE_LIB)
	$(CC) $^ -o $@ -lftdi -fsanitize=address -g


$(LIBMPSSE_LIB):
	cd $(LIBMPSSE_DIR)/src && ./configure
	make -C $(LIBMPSSE_DIR)/src
clean:
	rm -f 8051_prog
	rm *.o
