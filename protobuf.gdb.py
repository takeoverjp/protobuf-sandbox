# Howto use:
# (gdb) python execfile("protobuf.gdb.py")
# Ref) https://access.redhat.com/documentation/ja-jp/red_hat_enterprise_linux/6/html/developer_guide/debuggingprettyprinters

class GoogleProtobufMessagePrinter:
    def __init__(self, val):
        self.val = val

    def to_string (self):
        # val1 = self.val["Utf8DebugString"]
        # print("val1 = ")
        # print(val1)
        # print(val1)
        # val2 = val1()
        # print("val2 = ")
        # print(val2)
        # val3 = val2["c_str"]
        # print("val3 = ")
        # print(val3)
        # val4 = val3()
        # print("val4 = ")
        # print(val4)
        # return "@@@ " + val4.string()

        # TODO: Get debug_string from self.val
        debug_string = gdb.parse_and_eval("person.Utf8DebugString().c_str()")
        return "{\n" + debug_string.string() + "}"

def lookup_type (val):
    if str(val.type) == 'google::protobuf::Message':
        return GoogleProtobufMessagePrinter(val)
    return None

gdb.pretty_printers.append (lookup_type)
