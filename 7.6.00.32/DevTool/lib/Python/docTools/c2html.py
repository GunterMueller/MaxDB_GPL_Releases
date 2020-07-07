
# python2html.py
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#


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
