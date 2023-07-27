PROG = manga_reader
SRC  = ${PROG}.c
LIBS = -lxcb -lxcb-image 
CC   = gcc 
FLAGS = -lm 

${PROG}: ${SRC}
	${CC} -o $@ ${SRC} ${LIBS} ${FLAGS}

test: test.c 
	${CC} -o test test.c -lm


