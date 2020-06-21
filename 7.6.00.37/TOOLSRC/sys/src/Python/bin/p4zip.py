
# p4zip.py
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

from perforce import perforceLib

_version = 0.1

def p4zip (options, args):
    depotList = perforceLib.depotList ('-s opened ' + args, 'info: ([^#]+)')
    localList = perforceLib.depot2local (depotList)
    command = 'zip -@ '
    if options.verbose:
        command = command + '-v '
    pipe = os.popen (command + options.outfile, 'wt')
    for file in localList:
        pipe.write (file + '\n')
    pipe.close ()

def main (options, args):
    """copies files opened in $OWN into a zip file

    Any arguments are passed to the 'p4 opened' command.
    """
    if len (args) == 0:
        args = [os.path.join (os.environ ['OWN'], '...')]
    p4zip (options, string.join (args))
    if options.verbose:
        print 'created', os.path.abspath (options.outfile)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('o', 'outfile', ':', 'changes.zip', 'name of generated zip file'),
        ('v', 'verbose', '', None, 'verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
