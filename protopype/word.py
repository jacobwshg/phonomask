from sys import argv

IPA_FULL_CHAR_MAX = 0x2af
BETA = 0x3b2
CHI = 0x3c7
TIE = 0x361

def is_full_char(ch):
    codept = ord(ch)
    return codept <= IPA_FULL_CHAR_MAX or codept == BETA or codept == CHI

def word_to_seglist(word):
    seglist = []
    wordlen = len(word)
    segbuf = ''
    # whether to wait for the second full char after a tie
    wait = False 

    idx = 0
    while idx < wordlen:
        ch = word[idx]
        if is_full_char(ch):
            # Encountered full char
            if wait:
                # Full char after tie found
                wait = False
                segbuf += ch
                idx += 1
            elif len(segbuf) == 0:
                # Initial full char in current segment
                segbuf += ch
                idx += 1
            else:
                # Full char after another full char without tie;
                # belongs to the next segment
                # Flush segbuf
                seglist.append(segbuf)
                segbuf = ''
        else:
            # Encountered diacritic/super-/subscript
            segbuf += ch
            if ord(ch) == TIE:
                wait = True
            idx += 1
    # Collect final segment
    seglist.append(segbuf)

    return seglist

def word_from_seglist(seglist):
    return ''.join(seglist)

def print_word_and_seglist(word):
    seglist = word_to_seglist(word)
    print(f'{word}\t{seglist}')

if __name__ == '__main__':
    '''
    A stress test; we're not sure how much superscription 
    to actually support
    '''
    test = 't͡ʃʲʷiɖ͡ʐˠˤuðʰ'
    test_seglist = word_to_seglist(test)
    print(f'Test word: {test}')
    print(f'Segments list: {test_seglist}')
    print(f'Reconstructed word: {word_from_seglist(test_seglist)}')

    for word in argv[1:]:
        print_word_and_seglist(word)
    exit(0)

