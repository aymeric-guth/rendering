from enum import Enum, auto


class Token:
    def __init__(self, value=None):
        self._value = value

    @property
    def value(self):
        return self._value

    def __repr__(self):
        return (
            f"{self.__class__.__name__}(value={self.value})"
            if self.value
            else f"{self.__class__.__name__}()"
        )


class LiteralInt(Token):
    def __init__(self, value=None):
        if value is not None:
            self._value = int(value)


class LiteralFloat(Token):
    def __init__(self, value=None):
        if value is not None:
            self._value = float(value)


class Op(Token):
    ...


class OpAdd(Op):
    precedence = 10


class OpMul(Op):
    precedence = 50


class OpDiv(Op):
    precedence = 50


class OpSub(Op):
    precedence = 10


class EndExpr(Token):
    ...


class StartExpr(Token):
    ...


class LexerState(Enum):
    PARSE_NUM = auto()
    PARSE_FLOAT = auto()
    NEXT_TOKEN = auto()
    INIT = auto()
    PARSE_OP = auto()
    UNDEFINED = auto()
    DONE = auto()


class AstNode:
    def __init__(self):
        self.left = None


class BinaryOp(AstNode):
    def __init__(self, value, *, left=None, right=None):
        self.value = value
        self.left = left
        self.right = right


class UnaryOp(AstNode):
    def __init__(self, value, *, left=None):
        self.value = value
        self.left = left
