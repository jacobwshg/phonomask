# A pair of global maps (dicts) 
# between IPA segments and feature matrices
class SFMMaps:
    def __init__(self):
        self.seg_fm_map = {}
        self.fm_seg_map = {}

    def to_list(self):
        return self.seg_fm_map.items()

    def register(self, segment, feat_mtx):
        self.seg_fm_map.setdefault(segment, feat_mtx)
        self.fm_seg_map.setdefault(feat_mtx, segment)

    # TODO: handle missing segment
    def feat_mtx_of(self, segment):
        return self.seg_fm_map.get(segment)

    # TODO: handle missing segment
    def segment_of(self, feat_mtx):
        return self.fm_seg_map.get(feat_mtx)

__all__ = [ 'SFMMaps', ]

