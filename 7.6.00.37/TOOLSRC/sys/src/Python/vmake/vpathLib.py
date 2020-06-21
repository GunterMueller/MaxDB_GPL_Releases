
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

import string
import os
pathjoin = os.path.join

def normpath (fname):
    return os.path.normcase (os.path.normpath (fname))

own = normpath (os.environ ['OWN'])
desc = normpath(os.environ ['DESC'])
src = normpath (os.environ ['SRC'])
vmakePath = map (normpath, string.split (os.environ ['VMAKE_PATH'], ','))
vmakeReverse = vmakePath [:]
vmakeReverse.reverse ()

def listdir (dirname):
    dirname = os.path.abspath (normpath (dirname))
    relative = dirname [len (own) + 1:]
    dict = {}
    for layer in vmakePath:
        fullDir = pathjoin (layer, relative)
        try:
            list = os.listdir (fullDir)
            for entry in list:
                if not dict.has_key (entry):
                    dict [string.lower (entry)] = (layer, pathjoin (relative, entry))
        except OSError:
            pass
    items = dict.items ()
    items.sort ()
    result = map (lambda t: t[1], items)
    return result

if __name__ == "__main__":
    import sys
    for entry in listdir (sys.argv [1]):
        print entry


