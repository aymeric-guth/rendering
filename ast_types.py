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


class AstNode:
    indent = 0

    def __init__(self):
        self.left = None

    def __repr__(self):
        return f"AST(value={self.left})"


class BinaryOp(AstNode):
    def __init__(self, value, *, left=None, right=None):
        self.value = value
        self.left = left
        self.right = right
        self.indent = AstNode.indent + 1
        AstNode.indent += 1

    def __repr__(self):
        indent = "\t" * (AstNode.indent - self.indent)
        _indent = "\t" * (AstNode.indent - self.indent - 1)
        return f"BinaryOp(\n{indent}value={self.value}, \n{indent}left={self.left}, \n{indent}right={self.right}\n{_indent})"


class UnaryOp(AstNode):
    def __init__(self, value, *, left=None):
        self.value = value
        self.left = left

    def __repr__(self):
        return f"UnaryOp(value={self.value}, left={self.left})"


def parser(tokens):
    node = None
    root = None
    stack = []

    for tok in tokens:

        if isinstance(tok, StartExpr):
            ...

        elif isinstance(tok, EndExpr):
            return root

        elif isinstance(tok, Op):
            if node is None:
                if stack:
                    node = BinaryOp(tok)
                    node.left = stack.pop()
                    root = node
                else:
                    raise Exception(f"Invalid Syntax for node: {node} {tok}")
            elif node.left is None:
                node.left = tok
            elif node.right is None:
                node.right = tok
            else:
                buf = node
                node = BinaryOp(tok)
                node.left = buf
                root = node

        elif isinstance(tok, LiteralInt):
            if node is None:
                stack.append(tok)
            elif node.left is None:
                node.left = tok
            elif node.right is None:
                node.right = tok
            else:
                raise Exception(f"Invalid Syntax for node: {node} {tok}")


def parserv2(tokens):
    node = None
    root = None
    stack = []

    for tok in tokens:

        if isinstance(tok, StartExpr):
            root = AstNode()
            stack.append(root)

        elif isinstance(tok, EndExpr):
            return root

        elif isinstance(tok, Op):
            if not stack:
                raise NotImplementedError(
                    f"Unary Operation not implemented: {node} {tok}"
                )
            if isinstance(stack[-1], LiteralInt):
                node = BinaryOp(tok)
                node.left = stack.pop()
            elif isinstance(stack[-1], BinaryOp):
                node = stack.pop()
                if node.left is None:
                    node.left = BinaryOp(tok)
                    stack.append(node)
                elif node.right is None:
                    node.right = BinaryOp(tok)
                    stack.append(node.right)
                else:
                    node = BinaryOp(tok)
                    node.left = stack.pop()
                    stack.append(node)
            elif isinstance(stack[-1], AstNode):
                node = stack.pop()
                node.left = BinaryOp(tok)
                stack.append(node.left)
            else:
                raise RuntimeError(f"Invalid State: {tok=} {node=}")

        elif isinstance(tok, LiteralInt):
            if not stack:
                stack.append(tok)
            elif isinstance(stack[-1], BinaryOp):
                node = stack.pop()
                if node.left is None:
                    node.left = tok
                    stack.append(node)
                elif node.right is None:
                    node.right = tok
                    stack.append(node)
                else:
                    raise RuntimeError(f"Invalid State: {tok=} {node=}")
            elif isinstance(stack[-1], LiteralInt):
                raise SyntaxError(f"Invalid expression: {tok=} {node=}")
