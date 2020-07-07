
# python2html.py

import string

from source2html import Source2html

keywords = string.split ("""asm auto bool break
    case catch char class const const_cast continue
    default delete do double dynamic_cast
    else enum explicit extern false float for friend goto
    if inline int long mutable namespace new operator
    private protected public register reinterpret_cast return
    short signed sizeof static static_cast struct switch
    template this throw true try typedef typeid typename
    union unsigned using virtual void volatile wchar_t while""")

class C2html (Source2html):
    def __init__ (self):
        Source2html.__init__ (self, keywords, '//')

_defaultConverter = C2html ()

def convertFile (inname, outname = None):
    _defaultConverter.convertFile (inname, outname)

def main (options, args):
    for arg in args:
        _defaultConverter.convertFile (arg)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
