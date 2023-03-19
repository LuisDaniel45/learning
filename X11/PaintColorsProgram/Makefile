PROG = paint
SRC = ${PROG}.c
OBJ = ${SRC:.c=.o}

CC = cc

INCS = -l/usr/include/X11 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-4 -pthread
LIBS = -lX11 -lXft 
LDFLAGS = ${LIBS}
CFLAGS = -Wall -Wextra -Os ${INCS}

all: ${PROG}

${PROG}: ${OBJ}
	${CC} -o $@ ${LDFLAGS} ${OBJ}

%.o: %.c
	${CC} -c $< ${CFLAGS}

clean: 
	-rm ${OBJ} ${PROG}

.PHONY: all clean 

