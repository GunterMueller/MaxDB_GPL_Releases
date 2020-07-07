#!/usr/bin/env python
# instdb.py
#
#  responsible:  DanielD
#
#  from a script by: BerndV
#
#  special area: DBMServer
#
#  description:  installation of database via DBMServer
#
#  prerequisite: Please install the DBM Srcipting Interface.
#                (for instance with "imf scriptdbm" in the 7.2
#                 development tree)
#
#  remarks:      For including a private configuration this script
#                looks for a file named "myparams.dbm" in the current
#                directory. In this file you can provide private
#                definitions of parameters and devspaces.
#
#                Lines starting with "param_put" contains a valid
#                "param_put" DBMServer command.
#                This lines will be used in the "Create param file..."
#                section of this script.
#
#                Lines starting with "param_adddevspace" contains a
#                a valid "param_adddevspace" DBMServer command.                definition. Use format from the DBMServer command
#                This lines will be used in the "Add devspaces..."
#                section of this script.
#
#                All other lines will be ignored.
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
import string
import re
import os

try:
    import sdb.dbm
    dbm = sdb.dbm
except ImportError:
    try:
        import sapdb.dbm
        dbm = sapdb.dbm
    except ImportError:
        import dbm

_protFile = 'instdb.prt'

def fatalError (msg):
    sys.stderr.write ('\ninstdb.py: ' + msg + '\n')
    sys.stderr.write ('(See ' + _protFile + ' for more information.)\n')
    sys.exit (3)

def show (comment):
    if comment:
        print '%-30s' % (comment + '...'),

def getLines (fname):
    result = []
    if os.path.exists (fname):
        data = open (fname, 'r').read ()
        result = string.split (data, '\n')
    return result

def dbmcmd (session, cmd, comment = None):
    log = open (_protFile, 'a')
    try:
        log.write ('DBMServer-Command ' + cmd + '\n')
        try:
            show (comment)
            result = session.cmd (cmd)
            log.write ('  => ' + result + '\n')
            if comment:
                print 'OK'
        except dbm.DBMServError, err:
            log.write (str (err) + '\n')
            fatalError ('ERR at DBMServer command ' + cmd + ': ' + str (err))
    finally:
        log.close ()
    return result

def dbm2cmd (session, comment, *cmds):
    show (comment)
    for cmd in cmds:
        dbmcmd (session, cmd)
    print 'OK'

def splitUserInfo (userInfo):
    pos = string.find (userInfo, ',')
    if pos != -1:
        name = userInfo [:pos]
        pwd = userInfo [pos + 1:]
    else:
        name = pwd = userInfo
    return name, pwd

def checkVersion (session, options):
    show ('Checking version')
    versionString = dbmcmd (session, 'version')
    match = re.search ('"(\d+)[.](\d+)[.](\d+)"', versionString)
    version = tuple (map (int, match.group (1, 2, 3)))
    if version < (7, 1, 3):
        fatalError ('ERR  Sorry, no support for version %d.%d.%d' % version)
    if (version < (7, 2, 2)) and options.instanceType:
        fatalError ('ERR  Sorry, no support for instancetype in version %d.%d.%d' % version)
    print 'OK'
    return version

def removeDB (options):
    session = dbm.DBM (options.node, options.dbname, '', options.dbmUser)
    dbmcmd (session, 'db_offline', 'Set serverdb offline')
    dbmcmd (session, 'db_drop', 'Drop serverdb')

def createDB (options):
    session = dbm.DBM (options.node, '', options.dbroot)
    version = checkVersion (session, options)
    #
    # create database
    #
    dbmcmd (session,
        'db_create %s %s %s' % (options.dbname, options.dbmUser, options.osUser),
        'Create registry entry')
    if version < (7, 2, 0):
        session = dbm.DBM (options.node, options.dbname, '', options.dbmUser)

    localParams = getLines (options.paramfile)

    #
    # init kernel parameters
    #
    show ('Create param file')
    dbmcmd (session, 'param_startsession')
    dbmcmd (session, 'param_init ' + options.instanceType)
    dbmcmd (session, 'param_put MAXUSERTASKS  10')
    if options.dbEncoding <> 'ascii':
        dbmcmd (session, 'param_put _UNICODE  YES')
        dbmcmd (session, 'param_put DEFAULT_CODE  UNICODE')
	# using myparams.dbm
    for line in localParams:
        if line [:9] == 'param_put':
            dbmcmd (session, line)
    #
    dbmcmd (session, "param_checkall");
    dbmcmd (session, "param_commitsession");
    print 'OK'

    #
    # set devspaces
    #
    show ('Add devspaces')
    if version < (7, 4, 0):
        dbmcmd (session, 'param_adddevspace 1 SYS  sys_001  F')
    dbmcmd (session, 'param_adddevspace 1 LOG  LOG_001  F  2000 ');
    dbmcmd (session, 'param_adddevspace 1 DATA DAT_001  F 10000 ');
    for line in localParams:
        if line [:len ('param_adddevspace')] == 'param_adddevspace':
            dbmcmd (session, line)
    print 'OK'

    #
    # start database
    #
    dbmcmd (session, 'db_cold', 'Start cold')

    if version < (7, 4, 4):
        dbm2cmd (session, 'Init config', 'util_connect', 'util_execute INIT CONFIG')
        dbm2cmd (session, 'Activate serverdb', 'util_activate ' + options.dbaUser,
            'util_release')
    else:
        dbm2cmd (session, 'Activate serverdb', 'db_activate ' + options.dbaUser)
    dbm2cmd (session, 'Load system tables', 'load_systab -ud domp')

    if version < (7, 2, 0):
        dbm2cmd (session, 'Restart serverdb', 'db_restart')

    if version >= (7, 5, 0):
        dbm2cmd (session, 'Disable Log backup', 'util_execute SET LOG AUTO OVERWRITE ON')

    #
    # create demo users
    #
    if options.sqluser:
        dbmcmd (session, 'sql_connect ' + options.dbaUser)
        for userInfo in options.sqluser:
            username, pwd = splitUserInfo (userInfo)
            dbm2cmd (session, 'Create ' + username,
                'sql_execute CREATE USER %s PASSWORD %s DBA NOT EXCLUSIVE' % (username, pwd))
        dbmcmd (session, 'sql_release')


def loadDemo (options):
    show ('Load demo')
    os.chdir (os.path.join (os.environ ['TOOL'], 'bin', 'demodb'))
    if os.environ ['RELVER'] >= 'R73':
        cmd = 'x_python demo.py -d %s -u %s' % (options.dbname, options.dbaUser)
    else:
        cmd = 'xload -d %s -u %s -b demo.ins' % (options.dbname, options.dbaUser)
    rc = os.system (cmd)
    if rc != 0:
        fatalError ('load demo failed')
    print 'OK'

def main (options, args):
    open (_protFile, 'w')
    if options.remove:
        removeDB (options)
    else:
        createDB (options)
        if options.loaddemo:
            loadDemo (options)

def _options ():
    defaultDBRoot = os.environ.get ('INSTROOT', '')
    return [
        # (optstring, varname, typechar, default, help)
        ('l', 'loaddemo', '', None, 'loads demo schema'),
        ('r', 'remove', '', None, 'removes database'),
        ('d', 'dbname', ':', 'testdb', 'specifies serverdb to create'),
        ('u', 'dbmUser', ':', 'dbm,dbm', 'specifies DBMServer user'),
        ('n', 'node', ':', '', 'specifies servernode'),
        ('R', 'dbroot', ':', defaultDBRoot, 'specifies INSTROOT path'),
        ('a', 'dbaUser', ':', 'dba,dba', 'specifies SYSDBA user'),
        ('s', 'osUser', ':', '', 'specifies operating system user'),
        ('i', 'paramfile', ':', 'myparams.dbm', 'specifies a private param file'),
        ('t', 'instanceType', ':', '', 'specifies the type of the instance to create'),
		('e', 'dbEncoding',':','ascii', 'specifies encoding type of database'),
        ('sqluser', None, '::', None, 'create these sql users (name[,pwd])'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
