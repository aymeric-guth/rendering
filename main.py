from lexer import lexer, lexerv2
from ast_types import parser, parserv2
from interpreter import compile


if __name__ == "__main__":
    expr = "112+246*3/7-21"
    tokens = lexerv2(expr)
    ast = parserv2(tokens)
    print(expr)
    compile(ast)
