from pycparser import c_ast, c_parser, c_generator, parse_file
import argparse
import sys
import re
from jinja2 import Environment, FileSystemLoader

functions = []
enums = []

class Func:
    def __init__(self):
        self.name = ""
        self.returntype = "" 
        self.args = []

class Arg:
    def __init__(self):
        self.name = ""
        self.typename = ""
        self.isconst = False
        self.isptr = False

class Enum:
    def __init__(self):
        self.name = ""
        self.values = []

class EnumValue:
    def __init__(self):
        self.name = ""
        self.value = ""

def parse_function(node):
    func = Func()
    func.name = node.type.declname.replace('sys_', '')
    func.returntype = node.type.type.names[0]
    for param in node.args.params[1:]:
        arg = Arg()
        arg.name = param.name
        if type(param.type) is c_ast.PtrDecl:
            arg.isptr = True
            # print(param)
            if param.quals:
                arg.isconst = True
            arg.typename = param.type.type.type.names[0]
        else:
            arg.typename = param.type.type.names[0]
        func.args.append(arg)
    functions.append(func)

def parse_enum(node):
    enum = Enum()
    enum.name = node.name.replace("sys_", "")
    for e in node.type.type.values.enumerators:
        val = EnumValue()
        val.name = e.name.replace("SYS_", "")
        val.value = c_generator.CGenerator().visit(e.value)
        enum.values.append(val)
    enums.append(enum)

class FuncDefVisitor(c_ast.NodeVisitor):
    def visit_FuncDecl(self, node):
        parse_function(node)

class TypeDefVisitor(c_ast.NodeVisitor):
    def visit_Typedef(self, node):
        if type(node.type.type) is c_ast.Enum:
            parse_enum(node)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("syscall")
    args = parser.parse_args()

    with open(args.syscall, 'r') as file:
        src = file.read()

    prelude = """
    typedef char nu_char_t;\n
    typedef int nu_u32_t;\n
    typedef float nu_f32_t;\n
    typedef int nu_status_t;\n
    """

    fixed = "\n".join([line if not re.findall("#include|#ifndef|#endif|#define", line) else "" for line in src.splitlines()])
    ast = c_parser.CParser().parse(prelude + fixed)
    v = FuncDefVisitor()
    v.visit(ast)
    v = TypeDefVisitor()
    v.visit(ast)

    typemap = {
            "void": "void",
            "nu_char_t": "void",
            "nu_u32_t": "u32",
            "nu_f32_t": "f32",
            "nu_status_t": "void"
    }

    env = Environment(loader=FileSystemLoader('.'))
    template = env.get_template("nux.h.jinja")
    print(template.render(functions=functions, enums=enums, typemap=typemap))


