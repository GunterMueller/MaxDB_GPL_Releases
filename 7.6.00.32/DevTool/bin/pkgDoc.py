#!/usr/bin/env python
# pkgDoc.py

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
