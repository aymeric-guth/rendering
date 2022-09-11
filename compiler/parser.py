from enum import Enum, auto
from _types import StartExpr, EndExpr, Op, BinaryOp, LiteralInt, AstNode


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
            elif isinstance(stack[-1], AstNode):
                ...
            elif isinstance(stack[-1], LiteralInt):
                raise SyntaxError(f"Invalid expression: {tok=} {node=}")


class ParserState(Enum):
    INIT = auto()
    DONE = auto()
    NEXT = auto()
    OP = auto()
    NUM = auto()


def parserv3(tokens):
    root = None
    node = None
    state = ParserState.INIT
    stack = []

    while 1:
        match state:

            case ParserState.INIT:
                tok = next(tokens)
                print(f"New Token: {tok}")
                assert isinstance(tok, StartExpr)
                root = AstNode()
                state = ParserState.NEXT

            case ParserState.DONE:
                return root

            case ParserState.NEXT:
                tok = next(tokens)
                if isinstance(tok, EndExpr):
                    state = ParserState.DONE
                    continue
                elif isinstance(tok, BinaryOp):
                    stack.append(tok)
                    state = ParserState.OP
                elif isinstance(tok, LiteralInt):
                    stack.append(tok)
                    state = ParserState.NUM
                else:
                    state = ParserState.NEXT


def parserv4(scanner):
    if isinstance(scanner.peek(), StartExpr):
        ...
    elif isinstance(scanner.peek(), EndExpr):
        ...
