.SUFFIXES:
.SUFFIXES: .o .c

.c.o:
	$(CC) -c $<

OBJS=instdbmverifypw.o
TARGET=instdbmverifypw

${TARGET}: ${OBJS}
	$(CC) -o ${TARGET} ${OBJS}

install:
	cp ${TARGET} ${TOOL}/bin/${TARGET}
	chown root:bin  ${TOOL}/bin/${TARGET}
	chmod 04555 ${TOOL}/bin/${TARGET}

clean: 
	-rm ${OBJS} ${TARGET}
