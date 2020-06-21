#!/bin/env python
# ptocProto.py
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


import sys
import os

from ptoc import ptocProtoLib

def main (options, args):
    """Generates C++ header files from Pascal procedure declarations
    """
    if options.iput:
        iget = 1
        iput = 1
    elif options.noget:
        iget = None
        iput = None
    else:
        iget = 1
        iput = None
    proto = ptocProtoLib.PtocProto (options.refType, iget, iput)
    givenOut = options.outdir
    error = None
    for arg in args:
        if givenOut == None:
            outdir = os.environ ['SRC'] + '/' + arg [1:3]
        else:
            outdir = givenOut
        try:
            proto.convert (arg, outdir, not options.quiet, options.unconditional)
        except ptocProtoLib.NeedImfException, exc:
            sys.stderr.write (exc.show (arg) + '\n')
            error = 1
    proto.saveCache ()
    if error:
        sys.exit (1)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('q', 'quiet', '', None, 'no output'),
        ('R', 'refType', ':', 'Auto', "translation of VAR [C++, Both, Pointer, Auto]"),
        ('o', 'outdir', ':', None, 'output directory'),
        ('iput', None, '', None, 'iget and iput header files'),
        ('noget', None, '', None, 'don\'t iget header files'),
        ('version', None, '', ptocProtoLib.__version, 'version'),
        ('unconditional', None, '', None, 'create new header anyway'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
