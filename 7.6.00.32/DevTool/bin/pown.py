#!/usr/bin/env python
# pown.py
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
import string
import os

def getFilePattern (options):
    own = os.environ ['OWN']
    if options.nodoc:
        result = ' ' + own + '/sys/desc/... ' + own + '/sys/src/... ' + own + '/sys/config/... '
    else:
        result = ' ' + own + '/sys/...'
    return result

def main (options, args):
    """executes p4 restricted to $OWN

    e.g. 'pown sync' expands to 'p4 sync $OWN/sys/...'

    """
    argLine = string.join (args) + getFilePattern (options)
    rc = os.system ('p4 ' + argLine)
    sys.exit (rc)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('nodoc', None, '', None, 'exclude sys/doc/... from operation'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
