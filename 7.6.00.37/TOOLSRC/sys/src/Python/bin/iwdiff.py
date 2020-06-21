
# iwdiff.py
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


import vmake
import diffTools.iwdiffLib

def main (options, args):
    """compares local source against source in $VMAKE_PATH

    Allows to merge changes into one file.

    Colors:
        red     lines deleted
        green   lines added
        yellow  lines changed

    Keys:
        n       next difference
        p       previous difference
        <space> change current diff
        s       save file
        x       exit

    Cursors left panel:
        right arrow     merge changes into right panel

    Cursors right panel:
        X               delete difference
        exchange        revert change

    Options:
    """
    source = args [0]
    wdiff = diffTools.iwdiffLib.IWDiff (options, source)
    wdiff.mainLoop ('iwdiff - ' + source, line = options.line)

def _options ():
    from diffTools.wdiffLib import _version, readOptions
    defaults = readOptions (nonum = None)
    return [
        # (optstring, varname, typechar, default, help)
        ('version', None, '', _version, 'version information'),
        ('line', None, ':', -1, 'move to this line'),
        ('nonum', None, '', defaults ['nonum'], 'don\'t display line numbers'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
