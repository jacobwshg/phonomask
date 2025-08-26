import csv

from seg_fm_maps import SFMaps

def load_profile_table(table_path, ofsmap, sfmaps):
    num_feats = 0
    with open(table_path, mode='r', encoding='utf-8') as table:
        reader = csv.reader(table)
        for rowno, row in enumerate(reader):
            if rowno == 0:
                # Header row with feature labels
                for offset, feature in enumerate(row[1:]):
                    # Skip cell (0, 0), which is "IPA"
                    ofsmap.setdefault(feature, offset)
                    num_feats += 1
            else:
                # Content row with an IPA segment and its 
                # feature values
                segment = row[0]
                feat_mtx = 0b0
                for offset, feat_val in enumerate(row[1:]):
                    if feat_val == '+':
                        feat_mtx |= (0b1 << offset)
                    elif feat_val != '-':
                        print(f'warning: {segment} has nonbinay feature value')
                sfmaps.seg_fm_map.setdefault(segment, feat_mtx)
                sfmaps.fm_seg_map.setdefault(feat_mtx, segment)
    return num_feats

if __name__ == '__main__':
    table_path = './lx301-base.csv'
    ofsmap = {}
    sfmaps = SFMaps()
    num_feats = load_profile_table(table_path, ofsmap, sfmaps)
    for item in ofsmap.items():
        print(item)

    for seg, fm in sfmaps.seg_fm_map.items():
        print(f'{seg}\t0b{fm:0>{num_feats}b}')
    exit(0)

