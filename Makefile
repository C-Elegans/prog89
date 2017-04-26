LIBMPSSE_DIR=libmpsse
LIBMPSSE_LIB=libmpsse/src/libmpsse.a
CFLAGS=-I$(LIBMPSSE_DIR)/src -Wall -Werror -Wextra -Wno-unused-parameter -Wno-unused-variable
8051_prog: main.o programmer.o $(LIBMPSSE_LIB)
	$(CC) $^ -o $@


$(LIBMPSSE_LIB):
	cd $(LIBMPSSE_DIR)/src && ./configure
	make -C $(LIBMPSSE_DIR)/src
