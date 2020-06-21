
# wdiff3.py
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


def main (options, args):
    """compares three files side by side

    Allows to merge changes into one file.

    Arguments: base mine other
        base    the common ancestor of mine and other
        mine    the local file
        other   the matching file in a different branch

    Colors:
        yellow  lines changed

    Keys:
        n       next difference
        p       previous difference
        Ctrl-s  save file

    Cursors left panel:
        right arrow     merge changes into middle panel

    Cursors middle panel:
        exchange        revert change

    Cursors right panel:
        left arrow      merge changes into middle panel

    Options:
    """
    import diffTools.wdiffLib
    base = args [0]
    mine = args [1]
    other = args [2]
    wdiff = diffTools.wdiffLib.WDiff3 (options, base, mine, other)
    wdiff.mainLoop ('wdiff3', line = options.line)

def _options ():
    from diffTools.wdiffLib import _version, readOptions
    defaults = readOptions (nonum = None, nobase = None)
    return [
        # (optstring, varname, typechar, default, help)
        ('version', None, '', _version, 'version information'),
        ('line', None, ':', -1, 'move to this line'),
        ('nobase', None, '', defaults ['nobase'], 'don\'t display base source'),
        ('nonum', None, '', defaults ['nonum'], 'don\'t display line numbers'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

