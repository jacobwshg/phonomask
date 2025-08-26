# A pair of global maps (dicts) 
# between IPA segments and feature matrices
class SFMaps:
    def __init__(self):
        self.seg_fm_map = {}
        self.fm_seg_map = {}

    def to_list(self):
        return self.seg_fm_map.items()

    def register(self, segment, feat_mat):
        self.seg_fm_map.setdefault(segment, feat_mat)
        self.fm_seg_map.setdefault(feat_mat, segment)

__all__ = [ 'SFMaps', ]

