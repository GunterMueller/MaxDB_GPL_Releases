
# isystree.py
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


import os
from vmake.isystreeLib import SysTree, VmakeModules, createTreeRoot, createTGZ, createZIP

def copyVmakeModules (options, args):
    modules = VmakeModules (args, options.vmake, options.list).getModules ()
    tree = SysTree (options)
    own = os.environ ['OWN'] + '/sys/'
    for module in modules:
        tree.copy (own + module)
    if tree.filesNotFound:
        print 'The following files are missing:'
        for fname in tree.filesNotFound:
            print '    ' + fname

def copySourceDirs (options, args):
    args = map (os.path.expandvars, filter (None, args))
    if len (args) == 0:
        args = [os.environ ['DESC'], os.environ ['SRC']]
    tree = SysTree (options)
    for arg in args:
        tree.copy (arg)

_sourceOptions = [('list', ['sources.lst']),
               ('readme', 'sources.readme'),
               ('verbose', 1)]

_testOptions = [('list', ['alltest.lst']),
               ('exclude', ['sources.lst']),
               ('tag', 'test'),
               ('readme', 'tests.readme'),
               ('verbose', 1)]

_tgzOptions = [('tgz', 1), ('nl', 'unix')]
_zipOptions = [('zip', 1), ('nl', 'dos')]

configs = {
    'source': _sourceOptions + _tgzOptions,
    'test'  : _testOptions + _tgzOptions,
    'sourcez': _sourceOptions + _zipOptions,
    'testz'  : _testOptions + _zipOptions,
    'dummy': [('list', ['pyredist.lst']),
              ('verbose', 1)] + _zipOptions,
}

def checkOptions (options):
    if options.config:
        for name, value in configs [options.config]:
            setattr (options, name, value)


def main (options, args):
    checkOptions (options)
    archiveDir = os.path.abspath (options.outdir)
    options.outdir = os.path.join (options.outdir, options.nl)
    if options.tag:
        options.outdir = os.path.join (options.outdir, options.tag)
    options.outdir = createTreeRoot (options.outdir, options.nl, options.readme)
    if options.nocopy:
        options.verbose = 1
    if options.vmake or options.list:
        copyVmakeModules (options, args)
    else:
        copySourceDirs (options, args)
    if options.tgz and not options.nocopy:
        createTGZ (options.outdir, archiveDir, options.verbose, options.tag)
    if options.zip and not options.nocopy:
        createZIP (options.outdir, archiveDir, options.verbose, options.tag)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('n', 'nocopy', '', None, 'show, don\'t copy'),
        ('nl', None, ':', 'unix', 'text file format [unix, dos, native]'),
        ('o', 'outdir', ':', os.environ ['TMP'], 'output directory'),
        ('vmake', None, '::', None, 'specify platforms for vmake'),
        ('list', None, '::', None, 'additional vmake prots'),
        ('unconditional', None, '', None, 'copy unconditional'),
        ('v', 'verbose', '', None, 'verbose output'),
        ('x', 'exclude', '::', None, 'files to exclude'),
        ('tgz', None, '', None, 'create .tgz archive'),
        ('zip', None, '', None, 'create .zip archive'),
        ('tag', None, ':', None, 'tag added to archive name'),
        ('readme', None, ':', None, 'add readme file'),
        ('p4tree', None, ':', None, 'p4 directory containing additional files'),
        ('config', None, ':', None, 'config with options ' + str (configs.keys ())),
        ]

if __name__ == '__main__':
    import sys
    import optlib
    args = sys.argv [1:]
    optlib.optMain2 (main, _options (), optlib.expandFileArgs (args))

