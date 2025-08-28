"""
A pair of maps (dicts) between feature labels and 
their offsets within a feature matrix,
as derived from a specific feature table
"""
class OfsMaps:
    def __init__(self):
        self.feat_ofs_map = {}
        self.ofs_feat_map = {}
        '''
        Cached mapping list ordered by descending offset -
        used for printing bitwise layout of feature matrices
        '''
        self.feats_by_ofs = []
        self.is_sorted = False

    def register(self, feature, offset):
        self.feat_ofs_map.setdefault(feature, offset)
        self.ofs_feat_map.setdefault(offset, feature)
        self.is_sorted = False

    # Register the complete feature list of a table
    # Return the number of features registered
    def register_feature_list(self, features):
        num_feats = 0
        self.feat_ofs_map = {}
        self.ofs_feat_map = {}
        # Automatically assign offsets
        for offset, feature in enumerate(features):
            self.register(feature, offset)
            num_feats += 1
        return num_feats

    # TODO: handle missing feature
    def feature_at(self, offset):
        return self.ofs_feat_map.get(offset)

    # TODO: handle missing feature
    def offset_of(self, feature):
        return self.feat_ofs_map.get(feature)

    def sort(self):
        if not self.is_sorted:
            self.feats_by_ofs = sorted(self.feat_ofs_map.items(),\
                                       key=lambda feat_ofs:-feat_ofs[1])
            self.is_sorted = True

    def get_feat_mtx_layout(self):
        self.sort()
        return ' | '.join([feature for feature, offset in self.feats_by_ofs])

    """
    Restore string form of a feature matrix
    Example
        Feature matrix layout = son | syl | cons
        Feature matrix = 0b110
        String = '[-cons, +syl, +son]'
    """
    def feat_mtx_string(self, feat_mtx):
        # Accumulator list of '+/-feature' strings
        feat_val_list = []
        num_feats = len(self.feat_ofs_map)
        for offset in range(num_feats):
            feature = self.ofs_feat_map.get(offset)
            value = feat_mtx >> offset & 0b1
            feat_val_list.append(f'+{feature}' if value == 0b1 else f'-{feature}')
        feat_val_str = ', '.join(feat_val_list)
        return f'[{feat_val_str}]'

__all__ = [ 'OfsMaps', ]

