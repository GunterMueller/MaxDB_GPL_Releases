
# iwdiff3.py
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

def fatalError (msg):
    sys.stderr.write ('iwdiff3: ' + msg + '\n')
    sys.exit (1)


def checkOptions (options):
    if not options.rel:
        fatalError ('requires option -rel <own>')
    import os
    if not os.path.isdir (options.rel):
        fatalError ('OWN ' + options.rel + ' does not exist')
    if options.rel == os.environ ['OWN']:
        fatalError ('-rel ' + options.rel + ' must be different from $OWN')

def main (options, args):
    """compares current source with source in branch

    Takes one argument, the name of a source (in iview notation)
    in the current tree.

    iwdiff3 compares this file with
        - the corresponding source in the tree given by the -rel option
        - their common ancestor

    panels
        right   the common ancestor of $OWN and branch
        middle  the current file (e.g. rel 8.0)
        left    the matching file in a different branch (e.g. rel 7.2)
                When necessary, a global rename is done on this file

    Colors:
        yellow  lines changed

    Keys:
        n       next difference
        p       previous difference
        Ctrl-s  save file

    Cursors left panel:
        right arrow     merge changes from base into $OWN

    Cursors middle panel:
        exchange        revert change

    Cursors right panel:
        left arrow      merge changes from branch into $OWN

    Options:
    """
    checkOptions (options)
    from diffTools.iwdiffLib import IWDiff3
    fname = args [0]
    wdiff = IWDiff3 (options, options.rel, fname)
    wdiff.mainLoop ('iwdiff3 - ' + fname, options.line)

def _options ():
    from diffTools.wdiffLib import _version, readOptions
    defaults = readOptions (rel = None, nonum = None, nobase = None)
    return [
        # (optstring, varname, typechar, default, help)
        ('version', None, '', _version, 'version information'),
        ('line', None, ':', -1, 'move to this line'),
        ('rel', None, ':', defaults ['rel'], '%OWN% of alternate branch'),
        ('nobase', None, '', defaults ['nobase'], 'don\'t display base source'),
        ('nonum', None, '', defaults ['nonum'], 'don\'t display line numbers'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
