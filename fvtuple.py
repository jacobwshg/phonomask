from collections import namedtuple as NTup

# Feature-value tuple
# `(feature, True(+)/False(-))`
# TODO: possibly add field to support alpha
# TODO: 0 value?
FVTuple = NTup('FVTuple', ['feature', 'value'])

__all__ = [ 'FVTuple', ]

