import re

from lexer_types import (
    StartExpr,
    EndExpr,
    LexerState,
    Token,
    # LiteralFloat,
    LiteralInt,
    OpAdd,
    OpSub,
    OpMul,
    OpDiv,
)


is_num = re.compile(r"[0-9]")
is_op_add = re.compile(r"[\+]")
is_op_mul = re.compile(r"[\*]")
is_op_div = re.compile(r"[\/]")
is_op_sub = re.compile(r"[\-]")
is_op = re.compile(r"[\+\*\/\-]")


def lexer(expr):
    i = 0
    buf: list[str] = []
    res: list[Token] = [StartExpr()]
    state = LexerState.NEXT_TOKEN

    while 1:
        match state:

            case LexerState.PARSE_NUM:
                buf.append(expr[i])
                if (i == (len(expr) - 1)) or (not is_num.match(expr[i + 1])):
                    res.append(LiteralInt("".join(buf)))
                    state = LexerState.NEXT_TOKEN
                    buf.clear()
                i += 1

            case LexerState.PARSE_OP:
                if is_op_add.match(expr[i]):
                    res.append(OpAdd())
                    i += 1
                    state = LexerState.NEXT_TOKEN
                elif is_op_mul.match(expr[i]):
                    res.append(OpMul())
                    i += 1
                    state = LexerState.NEXT_TOKEN
                elif is_op_sub.match(expr[i]):
                    res.append(OpSub())
                    i += 1
                    state = LexerState.NEXT_TOKEN
                elif is_op_div.match(expr[i]):
                    res.append(OpDiv())
                    i += 1
                    state = LexerState.NEXT_TOKEN
                else:
                    state = LexerState.UNDEFINED

            case LexerState.NEXT_TOKEN:
                if i >= len(expr):
                    state = LexerState.DONE
                elif is_num.match(expr[i]):
                    state = LexerState.PARSE_NUM
                elif is_op.match(expr[i]):
                    state = LexerState.PARSE_OP
                else:
                    state = LexerState.UNDEFINED

            case LexerState.UNDEFINED:
                raise NotImplementedError(f"Token not implemented: '{expr[i]}'")

            case LexerState.DONE:
                res.append(EndExpr())
                return res


def lexerv2(expr):
    i = 0
    buf: list[str] = []
    state = LexerState.NEXT_TOKEN
    yield StartExpr()

    while 1:
        match state:

            case LexerState.PARSE_NUM:
                buf.append(expr[i])
                if (i == (len(expr) - 1)) or (not is_num.match(expr[i + 1])):
                    tok = LiteralInt("".join(buf))
                    state = LexerState.NEXT_TOKEN
                    buf.clear()
                    yield tok
                else:
                    state = LexerState.PARSE_NUM
                i += 1

            case LexerState.PARSE_OP:
                if is_op_add.match(expr[i]):
                    tok = OpAdd()
                elif is_op_mul.match(expr[i]):
                    tok = OpMul()
                elif is_op_sub.match(expr[i]):
                    tok = OpSub()
                elif is_op_div.match(expr[i]):
                    tok = OpDiv()
                else:
                    state = LexerState.UNDEFINED
                    continue
                i += 1
                state = LexerState.NEXT_TOKEN
                yield tok

            case LexerState.NEXT_TOKEN:
                if i >= len(expr):
                    state = LexerState.DONE
                elif is_num.match(expr[i]):
                    state = LexerState.PARSE_NUM
                elif is_op.match(expr[i]):
                    state = LexerState.PARSE_OP
                else:
                    state = LexerState.UNDEFINED

            case LexerState.UNDEFINED:
                raise NotImplementedError(f"Token not implemented: '{expr[i]}'")

            case LexerState.DONE:
                yield EndExpr()
                return


class Scanner:
    def __init__(self, expr):
        self.parsed = [i for i in lexerv2(expr)]
        self.p = -1

    def get_token(self):
        if self.p < len(self.parsed):
            self.p += 1
        buf = self.parsed[self.p]
        return buf

    def peek(self):
        if (self.p + 1) < len(self.parsed):
            return self.parsed[self.p + 1]
        else:
            return self.parsed[-1]
