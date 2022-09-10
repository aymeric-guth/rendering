from _types import (
    LiteralInt,
    OpAdd,
    OpSub,
    OpMul,
    OpDiv,
    BinaryOp,
    AstNode,
)


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


def evaluate(node, precedence=-1):
    if isinstance(node, LiteralInt):
        return node.value
    elif isinstance(node, BinaryOp):
        if node.value.precedence > precedence:
            if isinstance(node.value, OpAdd):
                return evaluate(node.left, node.value.precedence) + evaluate(
                    node.right, node.value.precedence
                )
            elif isinstance(node.value, OpMul):
                return evaluate(node.left) * evaluate(node.right)
            elif isinstance(node.value, OpSub):
                return evaluate(node.left) - evaluate(node.right)
            elif isinstance(node.value, OpDiv):
                return evaluate(node.left) / evaluate(node.right)
            ...
        else:
            if isinstance(node.value, OpAdd):
                return evaluate(node.left) + evaluate(node.right)
            elif isinstance(node.value, OpMul):
                return evaluate(node.left) * evaluate(node.right)
            elif isinstance(node.value, OpSub):
                return evaluate(node.left) - evaluate(node.right)
            elif isinstance(node.value, OpDiv):
                return evaluate(node.left) / evaluate(node.right)
    elif node is None:
        ...
    else:
        raise RuntimeError(f"Invalid NodeType: {node}")
