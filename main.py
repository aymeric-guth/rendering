from lexer import lexer, lexerv2, Scanner, EndExpr
from ast_types import parser, parserv2, parserv3
from interpreter import evaluate, print_node


if __name__ == "__main__":
    expr = "112+246*3"
    scanner = Scanner(expr)

    while 1:
        print(f"peek(): {scanner.peek()}")
        token = scanner.get_token()
        print(f"get_token(): {token}")
        print(f"peek(): {scanner.peek()}")
        print()
        if isinstance(token, EndExpr):
            break
