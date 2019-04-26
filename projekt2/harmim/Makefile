# IOS - Project 2
# Author: Dominik Harmim <xharmi00@stud.fit.vutbr.cz>

CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LDLIBS = -lpthread

EXECUTABLE = proj2
OBJS = $(shell $(CC) -MM *.c | grep ':' | cut -d ':' -f1 | tr '\n' ' ')
PACK = $(EXECUTABLE).zip


.PHONY: all dependencies run pack clean clean_res test


all: $(EXECUTABLE)


$(EXECUTABLE): $(OBJS)


# generated dependencies by gcc, see target dependencies
argument_processor.o: argument_processor.c argument_processor.h
process_generator.o: process_generator.c process_generator.h resources.h
proj2.o: proj2.c argument_processor.h resources.h process_generator.h
resources.o: resources.c resources.h


dependencies:
	$(CC) -MM *.c


run: $(EXECUTABLE)
	./$< $(ARGS)


pack:
	zip $(PACK) *.h *.c Makefile


clean:
	rm -f $(EXECUTABLE) *.o *.out $(PACK)


clean_res: scripts/clean.sh $(EXECUTABLE)
	chmod +x $<
	./$< $(EXECUTABLE)


test: scripts/check-syntax.sh
	chmod +x $<
	./$< $(EXECUTABLE).out
