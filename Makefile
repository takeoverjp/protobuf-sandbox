.PHONY: all clean
BIN:=person merge merge_qiita compare load_message
all: $(BIN)
clean:
	rm -f $(BIN) *.pb.h *.pb.cc compare.old.data* *~

PROTOBUF_PATH:=/usr/
# PROTOBUF_PATH:=/usr/local/protobuf-3.14.0/
PROTOBUF_CFLAGS:=`PKG_CONFIG_PATH=$(PROTOBUF_PATH)/lib/pkgconfig/ pkg-config --cflags protobuf`
PROTOBUF_LIBS:=`PKG_CONFIG_PATH=$(PROTOBUF_PATH)/lib/pkgconfig/ pkg-config --libs protobuf`
PROTOBUF_FLAGS:=$(PROTOBUF_CFLAGS) $(PROTOBUF_LIBS) -Wl,-rpath=$(PROTOBUF_PATH)/lib/

.SUFFIXES: .pb.cc .proto
.proto.pb.cc:
	$(PROTOBUF_PATH)/bin/protoc $^ --cpp_out=.

CFLAGS:=-g -Og $(PROTOBUF_FLAGS)

person: person.pb.cc main.c
	g++ -o $@ $^ $(CFLAGS)

merge: merge.pb.cc merge.cc
	g++ -o $@ $^ $(CFLAGS)

merge_qiita: merge_qiita.pb.cc merge_qiita.cc
	g++ -o $@ $^ $(CFLAGS)

store_old: compare.old.pb.cc store_old.cc
	g++ -o $@ $^ $(CFLAGS)

compare.old.data.1: store_old
	./store_old

compare.old.data.2: store_old
	./store_old

compare: compare.new.pb.cc compare.cc
	g++ -o $@ $^ $(CFLAGS)

load_message: sample.pb.cc load_message.cc
	g++ -o $@ $^ $(CFLAGS)

.PHONY: run-compare
run-compare: compare compare.old.data.1 compare.old.data.2
	./compare
