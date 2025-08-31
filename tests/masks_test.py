import setup

from masks import Tester, Setter
from fvtuple import FVTuple

# Subset of features for testing
test_features = ['syl', 'cons', 'son', 'nas', 'lat']
# Feature offset map for testing
test_ofsmap = {}
for ofs, feature in enumerate(test_features):
    test_ofsmap.setdefault(feature, ofs)

print('Feature offset map: ')
for item in test_ofsmap.items():
    print(f'  {item}')

# Print features layout
feats_by_ofs = sorted(test_ofsmap.items(), key=lambda feat_ofs:-feat_ofs[1])
layout = ' | '.join([f'{feat}' for feat, _ in feats_by_ofs])
print(layout)

print('')

def feature_bundle_test():
    print('Dummy feature bundle: [+syl, -cons, +son, +nas, -lat]')
    fvtups = [\
        FVTuple(feature = 'syl',  value = True),\
        FVTuple(feature = 'cons', value = False),\
        FVTuple(feature = 'son',  value = True),\
        FVTuple(feature = 'nas',  value = True),\
        FVTuple(feature = 'lat',  value = False),\
    ]
    tester = Tester(fvtups, test_ofsmap)
    setter = Setter(fvtups, test_ofsmap)
    print(tester)
    print(setter)

def rule_test():
    # Dummy rule 
    print('Dummy rule: [-syl, +son, +nas] -> [+syl, +cons, -lat]')
    '''
       lat | nas | son | cons | syl
         _     1     1      _     0
    ->  *0     1     1     *1    *1
    '''
    tester_fvtups = [\
        FVTuple(feature = 'syl', value = False),\
        FVTuple(feature = 'son', value = True),\
        FVTuple(feature = 'nas', value = True),\
    ]
    setter_fvtups = [\
        FVTuple(feature = 'syl',  value = True),\
        FVTuple(feature = 'cons', value = True),\
        FVTuple(feature = 'lat',  value = False),\
    ]
    tester = Tester(tester_fvtups, test_ofsmap)
    setter = Setter(setter_fvtups, test_ofsmap)
    print(tester, setter)

    # Dummy feature matrices
    # (may not describe meaningful segments)
    feat_mtxs = [
        # should pass test
        0b01100, 0b01110, 0b11100,\
        # should fail test
        0b10110, 0b01101, 0b01001,\
    ]
    nfeats = len(test_ofsmap)
    for fm in feat_mtxs:
        fm_new = fm
        if tester.test_feat_mtx(fm):
            fm_new = setter.set_feat_mtx(fm)
        print(f'Segment {fm:0>{nfeats}b} -> {fm_new:0>{nfeats}b}')


if __name__ == '__main__':
    feature_bundle_test()
    rule_test()
    exit(0)

