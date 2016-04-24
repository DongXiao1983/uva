PROGRAM = readlog
CC = gcc
CLOCALOPTS = -Wall -g3 -I ./

SRC =  $(wildcard *.c)
OBJDIR = obj
OBJS = $(patsubst %.c, $(OBJDIR)/%.o, $(SRC))

all: default $(OBJS)
	$(CC) -o $(PROGRAM)  $(OBJS) 


$(filter %.o,$(OBJS)):$(OBJDIR)/%.o:%.c
	$(CC) -c $(CLOCALOPTS) $< -o $@

default:
	mkdir -p $(OBJDIR)

#all: $(OBJS)
#	$(CC) -g -o $(PROGRAM)  $(OBJS) $(CLOCALOPTS) -I ./

clean:
	rm -rf $(PROGRAM)  $(OBJS) $(OBJDIR) 
