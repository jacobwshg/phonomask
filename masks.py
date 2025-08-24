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

if __name__ == '__main__':
    from collections import defaultdict as DDict
    ofsmap = DDict(lambda: -1)
    features = ['syl', 'cons', 'son', 'nas', 'lat']
    for ofs, feature in enumerate(features):
        ofsmap[feature] = ofs
    print('Feature offset map')
    for item in ofsmap.items():
        print(f'    {item}')
    print('... | lat | nas | son | cons | syl')
    print('')

    print('Dummy feature bundle: [+syl, -cons, +son, +nas, -lat]')
    fvtups = [\
        FVTuple(feature = 'syl',  value = True),\
        FVTuple(feature = 'cons', value = False),\
        FVTuple(feature = 'son',  value = True),\
        FVTuple(feature = 'nas',  value = True),\
        FVTuple(feature = 'lat',  value = False),\
    ]
    tester = Tester(fvtups, ofsmap)
    setter = Setter(fvtups, ofsmap)
    print(tester)
    print(setter)
    print('')

    # Dummy rule 
    '''
       ... lat | nas | son | cons | syl
             _     1     1      _     0
    -> ...  *0     1     1     *1    *1
    '''
    print('Dummy rule: [-syl, +son, +nas] -> [+syl, +cons, -lat]')
    tester_fvtups = [\
        FVTuple(feature = 'syl', value = False),\
        FVTuple(feature = 'son', value = True),\
        FVTuple(feature = 'nas', value = True),\
    ]
    setter_fvtups = [\
        FVTuple(feature = 'syl',  value = True),\
        FVTuple(feature = 'cons', value = True),\
        FVTuple(feature = 'lat',  value = False),\
    ]
    tester = Tester(tester_fvtups, ofsmap)
    setter = Setter(setter_fvtups, ofsmap)
    seg_bstrings = [
        # should pass test
        0b1101100, 0b1001100, 0b1011110,\
        # should fail test
        0b1010110, 0b1100101, 0b1110110,\
    ]
    for sb in seg_bstrings:
        sb_new = sb
        if tester.test_seg_bits(sb):
            sb_new = setter.set_seg_bits(sb)
        print(f'{bin(sb)} -> {bin(sb_new)}') 

    exit(0)

