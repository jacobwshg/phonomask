import csv

from seg_fm_maps import SFMMaps
from feat_ofs_maps import OfsMaps
import utils

# Manager
class FeatureProfile:
    def __init__(self, table_path, ofsmaps=OfsMaps(), sfmmaps=SFMMaps()):
        self.table_path = table_path
        self.ofsmaps = ofsmaps
        self.sfmmaps = sfmmaps
        self.loaded = False

    def set_table_path(self, new_path):
        self.table_path = new_path
        self.loaded = False

    def load_table(self):
        if not self.loaded:
            self.ofsmaps = OfsMaps()
            self.sfmmaps = SFMMaps()
            with open(self.table_path, mode='r', encoding='utf-8') as table:
                reader = csv.reader(table)
                for rowno, row in enumerate(reader):
                    if rowno == 0:
                        # Header row; register feature labels
                        # Skip cell (0, 0), which is "IPA"
                        self.ofsmaps.register_feature_list(row[1:])
                    else:
                        # Content row with an IPA segment and its feature values;
                        # Compute and register the segment's feature matrix
                        segment = row[0]
                        feat_mtx = 0b0
                        for offset, feat_val in enumerate(row[1:]):
                            if feat_val == '+':
                                feat_mtx |= (0b1 << offset)
                            elif feat_val != '-':
                                print(f'warning: {segment} nonbinary \
feature value currently degraded to `-`')
                        self.sfmmaps.register(segment, feat_mtx)
            self.loaded = True

    def num_feats(self):
        return self.ofsmaps.num_feats

    def all_feats_mask(self):
        return (0b1 << self.num_feats()) - 1

    # TODO
    def __str__(self):
        header = '[[Feature Profile]]'
        fmlayout = self.ofsmaps.get_feat_mtx_layout()
        seg_fm_list = self.sfmmaps.to_list()
        seg_fm_str = '\n'.join([f'{seg}\t{fm:0>{self.num_feats()}b}'\
                                for seg, fm in seg_fm_list])
        return '\n'.join([header, fmlayout, seg_fm_str, ])

    # Serialize
    '''
    TODO: not ready yet;
    now only segment - feat mtx mappings
    need to keep feature labels and layout, or else mappings meaningless
    '''
    def to_file(self, path):
        seg_fm_list = self.sfmmaps.to_list()
        with open(path, mode='w', encoding='utf-8') as f:
            writer = csv.writer(f)
            writer.writerow([self.table_path])
            for seg, fm in seg_fm_list:
                writer.writerow([seg, fm])

    # Deserialize
    def from_file(self, path):
        pass

    """
    Return a human-readable string for the 'effective' features 
    of a segment and their values, as selected by EF_MASK

    EF_MASK may be the return value of one of the bitmap comparison utilities
    with feature matrices as arguments

    The default mask selects for all features
    """
    def seg_effective_features(self, segment,\
                               ef_mask=~0b0,\
                               pos_only=False):
        '''
        Strictly speaking, when EF_MASK uses the default value we need to 
        discard the 1s above the feature bits. However, since 
        the callee method never uses any of these higher bits
        it makes no difference.
        '''
        #ef_mask &= self.all_feats_mask()
        # Obtain feature matrix of segment
        feat_mtx = self.sfmmaps.feat_mtx_of(segment)
        if feat_mtx is None:
            print(f'Segment [{segment}] not found in feature profile')
            return ''
        else:
            # Obtain string form of feature matrix
            return self.ofsmaps.feat_mtx_string(feat_mtx, ef_mask, pos_only)

    # Return a human-readable string for the complete feature matrix of a segment
    def segment_features(self, segment):
        return self.seg_effective_features(segment)

    # Return a human-readable string for a segment's positive features
    def seg_positive_features(self, segment):
        return self.seg_effective_features(segment, pos_only=True)

    """
    Accepts a sequence of segments separated by space and compare their 
    feature matrices
    If SHOW_SHARED is True, return their shared feature bundle;
        else return the changed feature bundle (between a pair of segments)
    """
    def compare_seg_features(self, segments_str, show_shared):
        segments = segments_str.split(' ')
        if not segments:
            print('No segment provided!')
            return None
        elif (not show_shared) and len(segments) > 2:
            print('Unable to compare changed features for more than 2 segments')
            return None

        valid_segments = []
        feat_mtxs = []
        for seg in segments:
            feat_mtx = self.sfmmaps.feat_mtx_of(seg)
            if feat_mtx == None:
                print(f'Segment [{seg}] not found in feature profile')
            else:
                valid_segments.append(seg)
                feat_mtxs.append(feat_mtx)

        if len(valid_segments) < 2:
            print('Unable to compare features for less than 2 known segments')
            return None

        ef_mask = utils.bitmaps_get_same(feat_mtxs, length=self.num_feats())\
                  if show_shared\
                  else utils.bitmaps_get_diff(feat_mtxs[0], feat_mtxs[1],\
                                              length=self.num_feats())
        ef_feats = self.seg_effective_features(valid_segments[1],\
                                               ef_mask=ef_mask)
        if show_shared:
            print('Shared features computed for ' +\
                  ', '.join([f'[{seg}]' for seg in valid_segments]))
        else:
            print(f'Changed features computed for [{valid_segments[0]}] -> \
[{valid_segments[1]}]')
        return ef_feats

    """
    Accept a sequence of segments separated by space
    Return their shared feature bundle
    """
    def shared_features(self, segments_str):
        return self.compare_seg_features(segments_str, True)

    """
    Accepts a pair of segments (exactly 2) separated by space
    Return the feature bundle corresponding to the change from
    the former to the latter
    """
    def changed_features(self, segment_pair_str):
        return self.compare_seg_features(segment_pair_str, False)


prof_lx301 = FeatureProfile('./lx301-base.csv')

if __name__ == '__main__':
    table_path = './lx301-base.csv'
    profile = FeatureProfile(table_path)
    profile.load_table()
    print(profile)

__all__ = [\
    'FeatureProfile',\
    'prof_lx301',\
]

