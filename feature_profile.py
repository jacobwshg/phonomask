import csv

from seg_fm_maps import SFMaps
from feat_ofs_maps import OfsMaps

# Manager
class FeatureProfile:
    def __init__(self, table_path, ofsmaps=OfsMaps(), sfmaps=SFMaps()):
        self.table_path = table_path
        self.ofsmaps = ofsmaps
        self.sfmaps = sfmaps
        self.num_feats = 0

    def set_table_path(self, new_path):
        self.table_path = new_path

    def load_table(self):
        self.ofsmaps = OfsMaps()
        self.sfmaps = SFMaps()
        self.num_feats = 0
        with open(self.table_path, mode='r', encoding='utf-8') as table:
            reader = csv.reader(table)
            for rowno, row in enumerate(reader):
                if rowno == 0:
                    # Header row with feature labels
                    # Skip cell (0, 0), which is "IPA"
                    self.num_feats = self.ofsmaps.register_feature_list(row[1:])
                else:
                    # Content row with an IPA segment and its 
                    # feature values
                    segment = row[0]
                    feat_mtx = 0b0
                    for offset, feat_val in enumerate(row[1:]):
                        if feat_val == '+':
                            feat_mtx |= (0b1 << offset)
                        elif feat_val != '-':
                            print(f'warning: {segment} has nonbinary feature value')
                    self.sfmaps.register(segment, feat_mtx)

    # TODO
    def __str__(self):
        header = '[[Feature Profile]]'
        fmlayout = self.ofsmaps.get_feat_mtx_layout()
        seg_fm_list = self.sfmaps.to_list()
        seg_fm_str = '\n'.join([f'{seg}\t{fm:0>{self.num_feats}b}'\
                                for seg, fm in seg_fm_list])
        return '\n'.join([header, fmlayout, seg_fm_str, ])

    # Serialize
    def to_file(self, path):
        contents = self.sfmaps.to_list()
        with open(path, mode='w', encoding='utf-8') as f:
            writer = csv.writer(f)
            writer.writerow([self.table_path])
            for seg, fm in contents:
                writer.writerow([seg, fm])

    # Deserialize
    def from_file(self, path):
        pass

if __name__ == '__main__':
    table_path = './lx301-base.csv'
    profile = FeatureProfile(table_path)
    profile.load_table()
    print(profile)

__all__ = [ 'FeatureProfile', ]

