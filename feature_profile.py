import csv

from seg_fm_maps import SFMaps
from feat_ofs_maps import OfsMaps
import utils

# Manager
class FeatureProfile:
    def __init__(self, table_path, ofsmaps=OfsMaps(), sfmaps=SFMaps()):
        self.table_path = table_path
        self.ofsmaps = ofsmaps
        self.sfmaps = sfmaps
        self.loaded = False

    def set_table_path(self, new_path):
        self.table_path = new_path
        self.loaded = False

    def load_table(self):
        if not self.loaded:
            self.ofsmaps = OfsMaps()
            self.sfmaps = SFMaps()
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
feature value currently cast to `-`')
                        self.sfmaps.register(segment, feat_mtx)
            self.loaded = True

    def num_feats(self):
        return self.ofsmaps.num_feats

    # TODO
    def __str__(self):
        header = '[[Feature Profile]]'
        fmlayout = self.ofsmaps.get_feat_mtx_layout()
        seg_fm_list = self.sfmaps.to_list()
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
        seg_fm_list = self.sfmaps.to_list()
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
        feat_mtx = self.sfmaps.feat_mtx_of(segment)
        if feat_mtx is None:
            print(f'Segment [{segment}] not found in feature profile')
            return ''
        else:
            # Obtain string form of feature matrix
            return self.ofsmaps.feat_mtx_string(feat_mtx, ef_mask, pos_only)

    # Return a human-readable string for the complete feature matrix of a segment
    def seg_features(self, segment):
        return self.seg_effective_features(segment)

    # Return a human-readable string for a segment's positive features
    def seg_positive_features(self, segment):
        return self.seg_effective_features(segment, pos_only=True)

    def shared_features(self, segments_str):
        segments = segments_str.split(' ')
        if not segments:
            print('No segment provided!')
            return None
        valid_segments = []
        feat_mtxs = []
        for seg in segments:
            feat_mtx = self.sfmaps.feat_mtx_of(seg)
            if feat_mtx == None:
                print(f'Segment [{seg}] not found in feature profile')
            else:
                valid_segments.append(seg)
                feat_mtxs.append(feat_mtx)

        if not feat_mtxs:
            print('No known segments provided')
            return None

        shared_mask = utils.bitmaps_get_same(feat_mtxs,\
                                             length=self.num_feats())
        shared_feats = self.seg_effective_features(valid_segments[0],\
                                                   ef_mask = shared_mask)
        print('Shared features computed for ' +\
              ', '.join([f'[{seg}]' for seg in valid_segments]))
        return shared_feats

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

