GPP = g++
CFLAGS = -g -Wall

comp: main.o scanner.o parser.o parseTree.o idStack.o statsem.o
	$(GPP) $(CFLAGS) -o comp main.o scanner.o parser.o parseTree.o idStack.o statsem.o

main.o: main.cpp scanner.h parser.h parseTree.h
	$(GPP) $(CFLAGS) -c main.cpp
	
codegen.o: codegen.h

statsem.o: statsem.h

idStack.o: idStack.h

parseTree.o: parseTree.h idStack.h statsem.h codegen.h

parser.o: parser.h parseTree.h

scanner.o: scanner.h parseTree.h statsem.h

clean:
	rm *.o P3
