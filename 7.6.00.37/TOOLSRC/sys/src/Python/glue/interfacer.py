
# interfacer.py
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


import sys
import getopt
import string
import os
import optlib
import fileutil

def readDefinition (fname):
    fname = fileutil.findExt (fname, ['pyi', 'py'])
    dict = {}
    execfile (fname, dict)
    return dict ['module']

def outfile (fname, options):
    fname = fileutil.basename (fname) + 'module.c'
    return os.path.join (options.outdir, fname)

def main (args):
    options, fileArgs = parseOptions (args)
    for filename in fileArgs:
        if options.verbose:
            print filename
        module = readDefinition (filename)
        module.writeGlue (outfile (filename, options))

# chapter options
def usage ():
    sys.stderr.write ('Usage: interfacer.py [options] fileargs\n')
    sys.stderr.write ('    -o outdir            outdir\n')
    sys.stderr.write ('    -v                   verbose\n')
    sys.stderr.write ('    -h                   this help screen\n')
    try:
        sys.stderr.write ("    " + main.__doc__)
    except:
        pass
    sys.exit (0)

def parseOptions (args):
    outdir = '.'
    verbose = None
    args = optlib.expandFileArgs (args)
    options, fileArgs = getopt.getopt (args, "o:vh")
    for o in options:
        option, optval = o
        if option == '-o':
            outdir = optval
            continue
        if option == '-v':
            verbose = 1         # True
            continue
        if option == '-h':
            usage ()
            continue
    return Options(outdir, verbose), fileArgs

class Options:
    def __init__ (self, outdir, verbose):
        self.outdir = outdir
        self.verbose = verbose
        pass

    def dump (self):
        print "        outdir: ", self.outdir
        print "        verbose: ", self.verbose
        pass

optionStrings = ['-v', 'verbose', '-o:.', 'outdir']

# end chapter options

# entry when called as script
if __name__ == "__main__":
    try:
        main (sys.argv[1:])
    except KeyboardInterrupt:
        sys.stderr.write ("Aborted\n")
        sys.exit (255)