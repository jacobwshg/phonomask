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
ofsmap_sorted_rev = sorted(test_ofsmap.items(), key = lambda kv:-kv[1])
layout = ' | '.join([f'{kv[0]}' for kv in ofsmap_sorted_rev])
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
       ... lat | nas | son | cons | syl
             _     1     1      _     0
    -> ...  *0     1     1     *1    *1
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

    # Dummy feature matrices
    # (may not describe meaningful segments)
    feat_mtxs = [
        # should pass test
        0b1101100, 0b1001100, 0b1011110,\
        # should fail test
        0b1010110, 0b1100101, 0b1110110,\
    ]
    for fm in feat_mtxs:
        fm_new = fm
        if tester.test_feat_mtx(fm):
            sb_new = setter.set_feat_mtx(fm)
        print(f'{bin(fm)} -> {bin(fm_new)}')


if __name__ == '__main__':
    feature_bundle_test()
    rule_test()
    exit(0)

