#!/bin/env python
# pkgDoc.py
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

import pkgDocLib
from FileSystem import FileSystem, VmakeFS
# from VdnWeaver import VdnWeaver
# import SapDBWeaver
from NoWeaver import NoWeaver

def getSourceFileSystem (options):
    if options.vmake:
        fs = VmakeFS ().subdir ('sys/src')
    else:
        fs = FileSystem (options.indir)
    return fs

def main (options, args):
    sourceFS = getSourceFileSystem (options)
    targetFS = FileSystem (options.outdir)
    # defaultWeaver = SapDBWeaver.SapDBWeaver (SapDBWeaver.connect (), options.outdir)
    defaultWeaver = NoWeaver ()
    for arg in args:
        if sourceFS.exists (arg):
            package = pkgDocLib.packageFromFile (arg,
                weaver = defaultWeaver, root = options.outdir)
            package.make (sourceFS, targetFS, options.build)
        else:
            sys.stderr.write ('package ' + sourceFS.absolute (arg) + ' not found\n')

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('b', 'build', '', None, 'build unconditional'),
        ('o', 'outdir', ':', '.', 'output directory'),
        ('i', 'indir', ':', '.', 'input directory'),
        ('vmake', None, '', None, 'read sources from source hierarchy'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
