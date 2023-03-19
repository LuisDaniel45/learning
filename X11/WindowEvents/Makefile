# program name 
PROG = window

# source file 
SRC = ${PROG}.c 

# compiled files (i.e translation to assembly)
OBJ = ${SRC:.c=.o} 

# compiler 
CC = cc

# where include files are stored 
INCS = -l/usr/include/X11

# libraries
LIBS = -lX11

# libraries flags 
LDFLAGS = ${LIBS}
CFLAGS = -Wall -Wextra -Os ${INCS}

all: ${PROG}

# links everything 
${PROG}: ${OBJ}
	${CC} -o $@ ${LDFLAGS} ${OBJ}

# compiles the code and puts it .o file 
%.o: %.c 
	${CC} -c $< ${CFLAGS}

# eliminates compiled file and executable
clean: 
	-rm ${OBJ} ${PROG}

.PHONY: all clean


