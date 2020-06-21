
# changeSut.py
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
import re

_replacements = [
    #(re.compile ('Pages Transferred    [0-9]+'), 'Pages Transferred    000'),
    #(re.compile ('First LOG Page       [0-9]+'), 'First LOG Page       000'),
    #(re.compile ('Page Count           [0-9]+'), 'Page Count           000'),
    #(re.compile ('(.+?) RELEASE', re.I), r'FILE RELEASE ( \1 !\n\nEnter command:\n'r'\1 RELEASE')
    (re.compile ('(Database ID[^\n]*\n)'),
        r'\1' + 'Max Used Data Page  000\n'),
    ]

_extensions = ['punix', 'uunix', 'upnix', 'tpunx',  'cunix']

_verbose = 1

def _translateExtensions ():
    result = []
    for extension in _extensions:
        result.append ((len (extension) + 1, '.' + extension))
    return result

def getFileList (filenames):
    if len (filenames) > 0:
        return filenames
    allFiles = os.listdir ('.')
    extensions = _translateExtensions ()
    result = []
    for file in allFiles:
        for length, extension in extensions:
            if file [-length:] == extension:
                result.append (file)
    return result

def changeFile (fname, replacements):
    if _verbose:
        print fname
    data = open (fname, 'r').read ()
    for pattern, replacement in replacements:
        data = re.sub (pattern, replacement, data)
    open (fname, 'w').write (data)

def changeFiles (fileList, replacements):
    for fname in fileList:
        changeFile (fname, replacements)

def main (options, args):
    """Changes strings in SUT protocols.

    To be actually useful, edit the _replacement list. This is
    a list of tuples (<compiled regular expression>, <replacement>).

    Unless specific files are given as arguments, all files
    in the current directory with the extensions
    ['punix', 'uunix', 'upnix', 'tpunx',  'cunix']
    are changed.

    """
    if options.quiet:
        global _verbose
        _verbose = None
    changeFiles (getFileList (args), _replacements)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('quiet', None, '', None, 'no progress output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

