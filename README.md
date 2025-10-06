~~black project~~

# Phonomask: A bitmap-based rule verifier for phonological analysis

---

## Background

In the analysis of a natural language's phonology (the system of 
sounds occurring in this language and their interactions), a 
prevalent approach is to view each sound (or _segment_) in the 
language as a "feature matrix".
This structure defines a series of phonological features that describe 
the state and movement of the vocal tract during the production of a 
segment. Based on whether a segment exhibits the particular state and 
movement identified by a given feature, the segment is said to be
positively or negatively valued for that feature. The transcription
of a segment in a phonetic alphabet such as IPA thus functions as
an abbreviation for the segment's complete feature matrix.

With such an approach, the change from one sound to another 
through the course of history or between a plausible mental 
representation and an articulatory realization can be described by
changing the former's feature values into those of the latter.
Given that the domain of each feature's possible values is binary, 
this operation is not unlike setting and clearing bits in a bitmap 
in computing.
`Phonomask` aims precisely to explore this parallel between disciplines 
by investigating the effectiveness of representing feature matrices
with bitmaps and verifying phonological processes through bitwise
manipulation.

## Data structures



## Data flow



## Design considerations

## Future directions


