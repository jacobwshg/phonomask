"""
A pair of maps (dicts) between feature labels and 
their indices within a feature matrix,
as derived from a specific feature table
"""
class FeatIdxMaps:
    def __init__(self):
        self.feat_idx_map = {}
        self.idx_feat_map = {}
        self.num_feats = 0

        '''
        Cached mapping list ordered by descending index -
        used for printing bitwise layout of feature matrices
        '''
        self.feats_by_idx = []
        self.is_sorted = False

    """
    Register a single feature-index mapping
    """
    def register(self, feature, index):
        self.feat_idx_map.setdefault(feature, index)
        self.idx_feat_map.setdefault(index, feature)
        self.is_sorted = False
        self.num_feats += 1

    """
    Register the complete feature list of a table
    by assigning indices to features in the order encountered
    Return the number of features registered
    """
    def register_feature_list(self, features):
        self.feat_idx_map = {}
        self.idx_feat_map = {}
        # Automatically assign indices
        for index, feature in enumerate(features):
            self.register(feature, index)

    # TODO: handle missing feature
    def feature_at(self, index):
        return self.idx_feat_map.get(index)

    # TODO: handle missing feature
    def index_of(self, feature):
        return self.feat_idx_map.get(index)

    """
    Cache a sorted representation as a member
    """
    def sort(self):
        if not self.is_sorted:
            self.feats_by_idx = sorted(self.feat_idx_map.items(),\
                                       key=lambda fi_pair:-fi_pair[1])
            self.is_sorted = True

    """
    Return a string that shows the bitwise layout of features
    as described by their indices
    """
    def get_feat_mtx_layout(self):
        self.sort()
        return ' | '.join([feature for feature, _ in self.feats_by_idx])

    """
    Restore the human-readable string form of a feature matrix
        Example
        Feature matrix layout = son | syl | cons
        Feature matrix = 0b110
        String = '[-cons, +syl, +son]'
    EFFECTIVE_MASK allows for selective emission of features, resulting in a 
        feature bundle rather than a full feature matrix
    If POSITIVE_ONLY is True, only positive effective features are emitted 
    """
    def feat_mtx_string(self, feat_mtx, effective_mask, positive_only):
        # Accumulator list of '+/-feature' strings
        feat_val_list = []
        for index in range(self.num_feats):
            feature = self.feature_at(index)
            if (effective_mask >> index) & 0b1 == 0b1:
                # Feature is effective; candidate for emission
                value = feat_mtx >> index & 0b1
                '''Old implementation without positivity condition
                feat_val_list.append(f'+{feature}' if value == 0b1 else f'-{feature}'
                '''
                if value == 0b1:
                    feat_val_list.append(f'+{feature}')
                elif not positive_only:
                    feat_val_list.append(f'-{feature}')
        feat_val_str = ', '.join(feat_val_list)
        return f'[{feat_val_str}]'

__all__ = [\
    'FeatIdxMaps',\
]

