#########################
# customise these
CFILES := foo.c bar.c
PROG := prog
CFLAGS := -Wall -Wextra -g
LDFLAGS :=
########################

# -MMD generates dependencies while compiling
CFLAGS += -MMD
CXX := g++

OBJFILES := $(CFILES:.c=.o)
DEPFILES := $(CFILES:.c=.d)

$(PROG) : $(OBJFILES)
	$(LINK.o) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(PROG) $(OBJFILES) $(DEPFILES)

-include $(DEPFILES)


