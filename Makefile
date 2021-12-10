SRCSHELL = myshell.c
OBJSHELL = myshell.o
OBJLS = myls.o
SRCLS = myls.c
PROGRAM = myshell
MYLS = myls
CC = gcc
CFLAGS = -g

all:$(PROGRAM) $(MYLS) 

$(PROGEAM):$(OBJSHELL)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(OBJSHELL)
$(MYLS):$(OBJLS)
	$(CC) $(CFLAGS) -o $(MYLS) $(OBJLS)
%.o:%.c
	@$(CC) $(CFLAGS) -c $<
.PHONY:clean
clean:
	-rm -rf *.o $(PROGRAM) $(MYLS)
