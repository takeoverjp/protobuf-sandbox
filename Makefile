.PHONY: all clean
all: person
clean:
	rm -f person *.pb.h *.pb.cc *~

person.pb.cc: person.proto
	protoc $^ --cpp_out=.

person: person.pb.cc main.c
	g++ -g -Og -o $@ $^ -lprotobuf
