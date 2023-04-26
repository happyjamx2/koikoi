CMD	   = ../bin/koikoi.exe
SRCDIR = ../src
SRCS   = koikoi.c const.c function.c
OBJDIR = ../obj
OBJS   = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

CC     = gcc
CFLAGS = -g -Wall
LFLAGS = -lm -mlong-double-64

build: $(CMD)

$(CMD): $(OBJS)
	$(CC) $(OBJS) -o $(CMD)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f "$(CMD)"
	rm -f *.o
	rm -f *.ilk
	rm -f *.pdb