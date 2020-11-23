.PHONY: all clean
BIN=person merge merge_qiita compare
all: $(BIN)
clean:
	rm -f $(BIN) *.pb.h *.pb.cc compare.old.data* *~

PROTOBUF_PATH=tmp/protobuf-3.14.0/src/
PROTOBUF_FLAGS=-I$(PROTOBUF_PATH) -L$(PROTOBUF_PATH)/.libs/ -Wl,-rpath=$(PROTOBUF_PATH)/.libs/

.SUFFIXES: .pb.cc .proto
.proto.pb.cc:
	protoc $^ --cpp_out=.

person: person.pb.cc main.c
	g++ -g -Og -o $@ $^ -lprotobuf

merge: merge.pb.cc merge.cc
	g++ -g -Og -o $@ $^ -lprotobuf

merge_qiita: merge_qiita.pb.cc merge_qiita.cc
	g++ -g -Og -o $@ $^ -lprotobuf

store_old: compare.old.pb.cc store_old.cc
	g++ -g -Og -o $@ $^ -lprotobuf

compare.old.data.1: store_old
	./store_old

compare.old.data.2: store_old
	./store_old

compare: compare.new.pb.cc compare.cc
	g++ -g -Og -o $@ $^ -lprotobuf

.PHONY: run-compare
run-compare: compare compare.old.data.1 compare.old.data.2
	./compare
