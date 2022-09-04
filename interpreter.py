from lexer_types import (
    StartExpr,
    EndExpr,
    LexerState,
    Token,
    # LiteralFloat,
    LiteralInt,
    Op,
    # OpAdd,
    # OpSub,
    # OpMul,
    # OpDiv,
)

from ast_types import BinaryOp, AstNode


def print_node(node, indent=0):
    idt = " " * indent
    if isinstance(node, BinaryOp):
        print(f"{idt}BinaryOp({node.value}):")
        if node.left is not None:
            print(f"{idt}lhs:")
            print_node(node.left, indent + 2)
        if node.right is not None:
            print(f"{idt}rhs:")
            print_node(node.right, indent + 2)
    elif isinstance(node, LiteralInt):
        print(f"{idt}LiteralInt({node.value})")
    elif isinstance(node, AstNode):
        print(f"Ast({print_node(node.left, indent+2)})")


def compile(ast):
    print_node(ast)
