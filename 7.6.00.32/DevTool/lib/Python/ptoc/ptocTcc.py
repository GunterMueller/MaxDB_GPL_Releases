#!/bin/env python
# ptocTcc.py        compares pascal Define with generated header
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

from ptoc import ptocProtoLib

def main (options, args):
    """Checks version of C++ header against Pascal source
    """
    tcc = ptocProtoLib.PtocTcc (options.protDir, options.nostrict)
    for arg in args:
        tcc.checkModule (arg)
    if tcc.hasErrors:
        sys.exit (3)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('P', 'protDir', ':', '.', 'protocol directory'),
        ('nostrict', 'nostrict', '', None, 'ignore header without MD5 line'),
        ('version', None, '', ptocProtoLib.__version, 'version'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
