baz: foo.o bar.o
	g++ foo.o bar.o -o baz

foo.o: foo.cpp
	g++ -c foo.cpp -o foo.o

bar.o: bar.cpp
	g++ -c bar.cpp -o bar.o

.PHONY: clean

clean:
	rm -rf foo.o bar.o baz

