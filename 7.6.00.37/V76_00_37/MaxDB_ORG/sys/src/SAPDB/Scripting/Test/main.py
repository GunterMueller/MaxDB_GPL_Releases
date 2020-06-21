#!/usr/bin/env python
#
#   @file   main.py
#
#   @author DanielD
#
#   @brief  test Python interfaces to MaxDB
#
#    ========== licence begin  GPL
#    Copyright (c) 2004-2005 SAP AG
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



import sys

from sdbpyTestLib import stdOptions, checkOptions
from sqltest import sqltest
# from dbapitest import dbapitest

def main (options, args):
    checkOptions (options)
    ok = 1
    #for function in [sqltest, dbapitest]:
    for function in [sqltest]:
        if not function (options, args):
            ok = None
    if not ok:
        sys.exit (3)

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, stdOptions ())

