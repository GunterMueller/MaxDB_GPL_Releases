
# createIntree.py
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


import re
import os

import config

def createIntree ():
    dirRE = re.compile ('(.*)[ÃÀ]ÄÄÄ(.*)\n')
    fileRE = re.compile ('(³   )+ *([a-zA-Z0-9.]+.*)\n')
    currentPath = config.inDEV
    currentPathList = [currentPath]
    for line in open (config.inTree, 'rt').readlines ():
        match = dirRE.match (line)
        if match:
            indent, dirname = match.group (1, 2)
            if indent is None:
                levels = 0
            else:
                levels = len (indent) / 4
            currentPathList = currentPathList [:levels + 1] + [dirname]
            currentPath = apply (os.path.join, currentPathList)
            if not os.path.exists (currentPath):
                os.makedirs (currentPath)
        else:
            match = fileRE.search (line)
            if match:
                fname = match.group (2)
                fullpath = os.path.join (currentPath, fname)
                dspinput = os.path.join (config.inputDsp, fname)
                if os.path.exists (dspinput):
                    filecopy (dspinput, fullpath)
                else:
                    touch (fullpath)

def touch (fname):
    open (fname, 'w')

def filecopy (input, output):
    data = open (input, 'rb').read ()
    open (output, 'wb').write (data)

def main (options, args):
    createIntree ()

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
