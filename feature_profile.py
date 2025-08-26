import csv

from seg_fm_maps import SFMaps

# Manager
class FeatureProfile:
    def __init__(self, table_path, ofsmap={}, sfmaps=SFMaps()):
        self.table_path = table_path
        self.ofsmap = ofsmap
        self.sfmaps = sfmaps
        self.num_feats = 0

    def set_table_path(self, new_path):
        self.table_path = new_path

    def load_table(self):
        num_feats = 0
        with open(self.table_path, mode='r', encoding='utf-8') as table:
            reader = csv.reader(table)
            for rowno, row in enumerate(reader):
                if rowno == 0:
                    # Header row with feature labels
                    for offset, feature in enumerate(row[1:]):
                        # Skip cell (0, 0), which is "IPA"
                        self.ofsmap.setdefault(feature, offset)
                        self.num_feats += 1
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
    def __repr__(self):
        header = '[[Feature Profile]]'
        return header

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
    ofsmap = {}
    sfmaps = SFMaps()
    profile = FeatureProfile(table_path)
    profile.load_table()
    for item in profile.ofsmap.items():
        print(item)
    profile.to_file('./lx301-ser.csv')

__all__ = [ 'FeatureProfile', ]

