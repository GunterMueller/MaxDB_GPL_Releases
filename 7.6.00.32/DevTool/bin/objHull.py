#!/usr/bin/env python
# objHull.py
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

import os

from vmake import orefLib

def objHull (options, args):
    hull = orefLib.Hull (options.input)
    files = hull.getHull (args)
    for file in files:
        print file

def main (options, args):
    objHull (options, args)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('i', 'input', ':', None, 'name of xref file'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())