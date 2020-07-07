
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

def getToolvars (symbols):
    cmd = '%(TOOLSHELL)s %(TOOL)s/bin/toolvarsDump%(TOOLEXT)s' % os.environ
    pipe = os.popen (cmd, 'rt')
    data = pipe.read ()
    entries = string.split (data, '\n\n')
    split = string.split
    for entry in entries:
        if not entry:
            continue
        list =  split (entry, '\n')
        if len (list) == 2:
            symbols [list [0]] = list [1]
    pipe.close ()

getToolvars (locals ())

if __name__ == "__main__":
    for key, value in globals ().items ():
        pass
        # print key, '=', value
