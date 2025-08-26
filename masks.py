from fvtuple import FVTuple

# Used in the `A`, `X`, `Y` parts of a rule
class Tester:
    """
    andmask: 
        Select relevant features from a segment.
    xormask: 
        Test relevant features from the segment.

    Examples
        'syl' offset 0
        'cons' offset 1 
        'son' offset 2

      [-syl] 
          andmask = 0b001 
          xormask = 0b000 (logically 0b__0)
      [+cons]
          andmask = 0b010
          xormask = 0b010 (logically 0b_1_)
      [-cons, +son]
          andmask = 0b110
          xormask = 0b100 (logically 0b10_)

    """

    def __init__(self, fvtups, ofsmap):
        andmask = 0b0
        xormask = 0b0

        for t in fvtups:
            feature = t.feature
            offset = ofsmap[feature]
            value = t.value

            feature_flag = 0b1 << offset
            andmask |= feature_flag
            if value:
                xormask |= feature_flag
        
        self.andmask = andmask
        self.xormask = xormask
    
    def __repr__(self):
        return f'Tester(andmask = {bin(self.andmask)}, \
xormask = {bin(self.xormask)})'

    # True iff a segment matches the feature bundle
    def test_seg_bits(self, seg_bstring):
      return (seg_bstring & self.andmask) ^ self.xormask == 0

# Used in the `B` part of a rule
class Setter:
    """
    andmask: 
        Clear relevant features from a segment.
    ormask: 
        Set relevant features in the segment to new values.

    Examples
        'syl' offset 0
        'cons' offset 1 
        'son' offset 2

      [-syl] 
          andmask = 0b110
          ormask  = 0b000 (logically 0b__0)
      [+cons]
          andmask = 0b101
          ormask  = 0b010 (logically 0b_1_)
      [-cons, +son]
          andmask = 0b001
          ormask  = 0b100 (logically 0b10_)

    """

    def __init__(self, fvtups, ofsmap):
        andmask = 0b0
        ormask = 0b0

        for t in fvtups:
            feature = t.feature
            offset = ofsmap[feature]
            value = t.value

            feature_flag = 0b1 << offset 
            andmask |= feature_flag
            if value:
                ormask |= feature_flag

        andmask = ~andmask
        self.andmask = andmask
        self.ormask = ormask

    def __repr__(self):
        return f'Setter(andmask = {bin(self.andmask)}, \
ormask = {bin(self.ormask)})'

    # Apply a specified change to a segment's bit-string
    def set_seg_bits(self, seg_bstring):
        return seg_bstring & self.andmask | self.ormask

