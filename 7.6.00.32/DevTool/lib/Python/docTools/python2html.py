
# python2html.py
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#


import string

from source2html import Source2html

class Python2html (Source2html):
    def __init__ (self):
        Source2html.__init__ (self, 
                string.split ("""and break class continue def del elif else
                except finally for from global if import in is lambda not or
                pass print raise return try while"""), '#')

_defaultConverter = Python2html ()

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
