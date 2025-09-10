from enum import Enum

# A -> B / X _ Y

ARROWS = ['->', '→', '>']

class RuleState(Enum):
    A = 1
    B = 2
    X = 3
    Y = 4

class Rule:
    def __init__(self, rulestr):
        # Init parts
        self.A = ''
        self.arrow = ''    
        self.B = ''
        self.X = []
        self.Y = []

        # Extract feature bundles
        state = RuleState.A
        buf = ''
        inb = False  ## Inside brackets; ignore spaces
        for c in rulestr:
            if c == '[':
                buf += c
                inb = True
            elif c == ']':
                buf += c
                inb = False
            elif c == ' ' and not inb:
                if buf in ARROWS and state == RuleState.A:
                    self.arrow = buf
                    state = RuleState.B
                    buf = ''
                elif buf == '/' and state == RuleState.B:
                    state = RuleState.X
                    buf = ''
                elif buf == '_' and state == RuleState.X:
                    state = RuleState.Y
                    buf = ''
                else:
                    if not buf:
                        pass
                    elif state == RuleState.A:
                        self.A = buf
                    elif state == RuleState.B:
                        self.B = buf
                    elif state == RuleState.X:
                        self.X.append(buf)
                    elif state == RuleState.Y:
                        self.Y.append(buf)
                    buf = ''
            else:
                buf += c
        self.Y.append(buf)

    def __str__(self):
        return (f'{self.A} {self.arrow} {self.B} / \
{' '.join(self.X)} _ {' '.join(self.Y)}')

__all__ = [\
    'Rule',\
]

