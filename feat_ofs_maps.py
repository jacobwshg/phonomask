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
            self.feats_by_ofs = sorted(self.ofs_feat_map.items(), key=lambda kv:-kv[0])
            self.is_sorted = True

    def get_feat_mtx_layout(self):
        self.sort()
        return ' | '.join([feature for offset, feature in self.feats_by_ofs])

__all__ = [ 'OfsMaps', ]

