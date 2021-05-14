SRCS = Imageomatic0.c Imageomatic.c lodepng.c

TARG = Imageomatic

# specify compiler, compile flags, and needed libs
CC   = gcc
OPTS = -Wall
LIBS = -lm

# this translates .c files in src list to .o’s
OBJS = $(SRCS:.c=.o)

# all is not really needed, but is used to generate the target
all: $(TARG)

# this generates the target executable
$(TARG): $(OBJS)
	$(CC) -o $(TARG) $(OBJS) $(LIBS)

# this is a generic rule for .o files
%.o: %.c
	$(CC) $(OPTS) -c $< -o $@

clean:
	rm -f $(OBJS)

cleanAll:
	rm -rf $(OBJS) $(TARG)
