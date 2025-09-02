import functools

"""
Bitmap utilities are aimed at feature matrices in this project
but can be generalized
"""

"""
Return a mask for bits shared across a list of bitmaps (0/1 across all elements)
"""
#TODO: try implementing with functools.reduce()
def bitmaps_get_same(bitmap_list, length=32):
    if not bitmap_list:
        return 0b0

    default_mask = (0b1 << length) - 1
    # Accumulate shared 1s and 0s respectively from elements 
    # and from their complements
    accs = [default_mask for _ in range(2)]
    for bmp in bitmap_list:
        accs[0] &= bmp
        accs[1] &= ~bmp
    return accs[0] | accs[1]

"""
Return a mask for bits that are different between a pair of bitmaps
(it is ambiguous to diff more than 2 bitmaps)
"""
def bitmaps_get_diff(bitmap1, bitmap2, length=32):
    return bitmap1 ^ bitmap2

__all__ = [\
    'bitmaps_get_same',\
    'bitmaps_get_diff',\
]

