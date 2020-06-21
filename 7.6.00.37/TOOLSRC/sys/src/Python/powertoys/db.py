
# db.py
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

from string import join
from os import environ
import sys
# sys.path.insert (1, 'c:\\python\\daniel')

def getMode (options):
    mode = None
    if options.fast:
        mode = 'f'
    if options.quick:
        mode = 'q'
    if options.slow:
        mode = 's'
    return mode

def getDBName (options):
    if options.dbname:
        return options.dbname
    #for envvar in ['DBNAME', 'SERVERDB']:
    #    result = environ.get (envvar)
    #    if result:
    #        return result
    return None

def main (options, args):
    from sapdbServer import manage, list, Error
    try:
        if options.list:
            list (options.configFile)
        mode = getMode (options)
        if args:
            cmd = join (args)
            options.commands.append (cmd)
        manage (options.configFile, getDBName (options),
            options.userinfo, mode, options.batchFile, options.commands)
    except Error, err:
        print err.getMessage ()

if __name__ == '__main__':
    import optlib
    optlib.optMain (main,  # (optstring, varname, typechar, default, help)
            ('d', 'dbname', ':', None, 'database name'),
            ('u', 'userinfo', ':', None, 'user info'),
            ('f', 'fast', '', None, 'use fast db'),
            ('q', 'quick', '', None, 'use quick db'),
            ('s', 'slow', '', None, 'use slow db'),
            ('c', 'commands', '::', [], 'command'),
            ('b', 'batchFile', ':', None, 'batch file'),
            ('l', 'list', '', None, 'list available DBs'),
            ('C', 'configFile', ':', None, 'configuration file'),
            )

