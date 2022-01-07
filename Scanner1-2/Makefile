CC=g++
EXECUTABLE=scanner
all: $(EXECUTABLE)

$(EXECUTABLE): scanner.o
	$(CC) scanner.o -o $(EXECUTABLE)

scanner.o: scanner.cpp
	$(CC) -c scanner.cpp -std=c++11
clean:
	rm -rf *.o $(EXECUTABLE)

