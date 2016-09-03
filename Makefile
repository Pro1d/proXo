CCXX=g++
DEBUG_FLAGS=-g
RELEASE_FLAGS=-03 -march=corei7
TARGET=proXo.a
HEADERS=*/*.h
OBJECTS=

${TARGET}: ${OBJECTS}
	ar cr -o $@ $^

%.o: %.c
		@set -e; rm -f $@; \
		$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

clean:
	rm *OA.o
