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

        # 'Push' feature-value tuples into mask for feature bundle
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
        self.nfeats = len(ofsmap)
    
    def __repr__(self):
        return f'Tester(andmask = {self.andmask:0>{self.nfeats}b}, \
xormask = {self.xormask:0>{self.nfeats}b})'

    # True iff a segment's feature matrix matches the given feature bundle
    def test_feat_mtx(self, feat_mtx):
      return (feat_mtx & self.andmask) ^ self.xormask == 0

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

        # Total number of features in the active feature profile
        num_total_feats = len(ofsmap)
        feat_mtx_mask =  (0b1 << num_total_feats) - 1
        '''
        Make andmask into its 'negative film' in order to clear 
        rather than test for positive feature bits.
        Also discard higher 1's above features
        '''
        andmask = (~andmask) & feat_mtx_mask
        self.andmask = andmask
        self.ormask = ormask
        self.nfeats = num_total_feats

    def __repr__(self):
        return f'Setter(andmask = {self.andmask:0>{self.nfeats}b}, \
ormask = {self.ormask:0>{self.nfeats}b})'

    # Apply a specified change to a segment's feature matrix
    def set_feat_mtx(self, feat_mtx):
        return feat_mtx & self.andmask | self.ormask

__all__ = [ 'Tester', 'Setter', ]

