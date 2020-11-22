.PHONY: all clean
all: person merge merge_qiita
clean:
	rm -f person *.pb.h *.pb.cc *~

.SUFFIXES: .pb.cc .proto
.proto.pb.cc:
	protoc $^ --cpp_out=.

person: person.pb.cc main.c
	g++ -g -Og -o $@ $^ -lprotobuf

merge: merge.pb.cc merge.cc
	g++ -g -Og -o $@ $^ -lprotobuf

merge_qiita: merge_qiita.pb.cc merge_qiita.cc
	g++ -g -Og -o $@ $^ -lprotobuf
