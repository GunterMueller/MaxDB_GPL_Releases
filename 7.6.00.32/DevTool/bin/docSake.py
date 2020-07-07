
# docSake.py
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
sys.stderr = sys.stdout
import os

from fileutil import TeeStream

from docTools import _version
from docTools.FileSystem import FileSystem, VmakeFS
from docTools.docSakeLib import MainDoc

def getOutdir (options):
    if options.deploy:
        relver = os.environ ['RELVER']
        release = relver [1] + '.' + relver [2]
        options.outdir = r'\\p26326\InetPub\wwwroot\Doku\rel-' + release
        if options.verbose:
            print 'Deploying to', options.outdir

def getWeaver (options):
    if 1:
        from docTools.SapDBWeaver import SapDBWeaver
        result = SapDBWeaver (None, options.outdir, None, options.deploy)
    else:
        from docTools.NoWeaver import NoWeaver
        result = NoWeaver ()
    return result

def getFlags (options):
    options.unconditional = options.build
    options.recursive = not options.norecurse
    return options

def main (options, args):
    logstream = TeeStream (open (options.log, 'wt'))
    sys.stdout = sys.stderr = logstream
    getOutdir (options)
    sourceFS = VmakeFS ().subdir ('sys/src')
    docsrcFS = VmakeFS ().subdir ('sys/doc')
    targetFS = FileSystem (options.outdir)
    flags = getFlags (options)
    allOK = 1
    weaver = getWeaver (options)
    try:
        for arg in args:
            doc = MainDoc (sourceFS, docsrcFS, arg)
            if options.list:
                doc.list ()
            else:
                doc.weaver = weaver
                ok, result = doc.make (targetFS, flags)
                allOK = ok and allOK
    finally:
        weaver.close ()
        del weaver
    if not allOK:
        sys.exit (1)

def _options ():
    defOut = os.environ ['WRK'] + '/doc'
    return [
        # (optstring, varname, typechar, default, help)
        ('v', 'verbose', '', None, 'verbose output'),
        ('list', None, '', None, 'list collection'),
        ('n', 'noAction', '', None, 'no action'),
        ('b', 'build', '', None, 'build unconditional'),
        ('o', 'outdir', ':', defOut, 'output directory'),
        ('log', None, ':', 'docSake.log', 'name of log file'),
        #('indexfile', None, ':', 'index.html', 'name of main index file'),
        ('index', None, '', None, 'create main index'),
        ('deploy', None, '', None, 'write files to deployment site'),
        ('version', None, '', _version, 'version information'),
        ('norecurse', None, '', None, 'don\'t recurse on directories'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
