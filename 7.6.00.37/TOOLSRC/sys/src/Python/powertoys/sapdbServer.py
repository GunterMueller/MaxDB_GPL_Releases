
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
import os
from string import split, join, index, find
import re

import mylib
try:
    import sapdb
    import dbm
except ImportError:
    pass    # ignore, 6.* releases don't need them

_modeMap = {'f': '-f', 'q': '-q', 's': '-s'}

UsageError = 'SapdbServer.UsageError'

class Error:
    def __init__ (self, msg):
        self.msg = msg

    def __str__ (self):
        return self.__class__.__name__ + ": " + self.msg

class SapdbServer:
    def __init__ (self, name, dbroot = None, host = '', mode = 'f', wantVTrace = None,
            users = None, defaultUser = None, **options):
        self.name = name
        if dbroot == None:
            dbroot = os.environ ['DBROOT']
        self.dbroot = dbroot
        self.host = host
        self.mode = mode #_modeMap [mode]
        self.wantVTrace = wantVTrace
        if users == None:
            users = {}
        self.users = users
        self.defaultUser = defaultUser
        self.changed = None
        self.cachedDBMSession = None
        self.options = options

    def __del__ (self):
        self.cachedDBMSession = None   # implicitly releases dbm session

    def __repr__ (self):
        return '<SapdbServer %s>' % self.name

    def saveState (self):
        result = ("SapdbServer (%s, dbroot = '%s', fast = '%s', "
                    + "wantVTrace = %s, controlUser = '%s')"
                  % (self.name, self.dbroot, self.fast,
                          self.wantVTrace, self.controlUser))
        return result

    def setMode (self, mode):
        if mode != self.mode:
            self.mode = mode
            self.changed = 1

    def getuser (self, namepwd = None, default = None):
        if namepwd:
            pos = find (namepwd, ',')
            if pos != -1:
                user = namepwd [:pos]
                pwd = namepwd [pos + 1:]
                namepwd = (user, pwd)
            else:
                namepwd = self.users [namepwd]
        else:
            if default != None:
                namepwd = self.users [default]
            elif self.userinfo:
                namepwd = self.namePwdByString (self.userinfo)
            else:
                namepwd = self.getDefaultUser ()
        return namepwd

    def namePwdByString (self, namepwd):
        pos = find (namepwd, ',')
        if pos != -1:
            user = namepwd [:pos]
            pwd = namepwd [pos + 1:]
            namepwd = (user, pwd)
        else:
            namepwd = self.users [namepwd]
        return namepwd

    def getDefaultUser (self):
        return self.users [self.defaultUser]

    def getDbVersion (self):
        self.getDbmSession ()
        return self.dbversion

    def getDbmSession (self):
        if self.cachedDBMSession == None:
            user, pwd = self.users ['dbm']
            self.cachedDBMSession = dbm.DBM (self.host, self.name, '', user + ',' +  pwd)
            self.keepDatabaseVersion (self.cachedDBMSession)
        return self.cachedDBMSession

    def keepDatabaseVersion (self, dbm):
        versionString = dbm.cmd ('version')
        match = re.search ('"(\d+)[.](\d+)[.](\d+)"', versionString)
        self.dbversion = tuple (map (int, match.group (1, 2, 3)))

    def dbmCommand (self, *cmds):
        dbmSession = self.getDbmSession ()
        for cmd in cmds:
            sys.stdout.write ('dbm: ' + cmd + ' ')
            result, exception = self.unexceptionalDbm (cmd, dbmSession)
            if exception and (exception.errorCode == -24992):
                result, newException = self.unexceptionalDbm ('util_connect', dbmSession)
                if not newException:
                    result, exception = self.unexceptionalDbm (cmd, dbmSession)
            if exception:
                if exception.additionalInfo:
                    infoString = ' [' + exception.additionalInfo + ']'
                else:
                    infoString = ''
                sys.stdout.write ('==> %s%s\n' % (str (exception), infoString))
            else:
                if result == '':
                    result = 'OK'
                if result [-1] != '\n':
                    result = result + '\n'
                sys.stdout.write ('==> ' + result)

    def unexceptionalDbm (self, cmd, dbmSession):
        result = None
        exception = None
        try:
            result = dbmSession.cmd (cmd)
        except dbm.DBMServError, exception:
            pass
        return result, exception

    def dbm (self, *words):
        """sends command to dbm server"""
        self.dbmCommand (join (words))

    def restart (self):
        """starts database


        An explicit restart is done. VTrace is switched on
        if the database is configured that way (in db.conf).
        """
        self.dbmCommand ('db_warm -' + self.mode)
        if self.wantVTrace and (self.mode in ['f', 'q']):
            self.vtrace ('on')
        try:
            self.utility ('diagnose trigger off')
        except:
            sys.stderr.write ('diagnose trigger off failed\n')
        autolog = self.options.get ('autolog', None)
        if autolog:
            self.dbmCommand ('autosave_on ' + autolog)


    def shutdown (self):
        """stops the database"""
        #try:
        #    self.dbmCommand ('autosave_off')
        #except:
        #    pass
        if self.getDbVersion () < (7, 4, 3):
            self.dbmCommand ('db_offline -immediate')
        else:
            self.dbmCommand ('db_offline')

    def cold (self):
        """brings database to cold state"""
        self.dbmCommand ('db_cold')

    def bounce (self):
        """stops and restarts the database"""
        self.shutdown ()
        self.restart ()

    # some synonyms for less typing
    up = restart
    down = shutdown

    def kernprot (self, layers = 'ax', editor = None):
        """call the kernprot command

        Usage: db ... kernprot [layer [editor]]
        Default layer is 'ax'
        Default editor is %EDITOR%
        VTrace is flushed before calling kernprot
        """
        # self.shutdown ()     # while access to devspace is denied
        try:
            self.vtrace ('flush')
        except sapdb.CommunicationError:
            pass  # db down, no flush necessary
        rc = os.system ('xkernprot -d %s %s' % (self.name, layers))
        if rc == 0:
            if editor == '0':
                return
            if editor == None:
                editor = os.environ ['EDITOR']
            os.system ('%s %s.prt' % (editor, self.name))
        else:
            sys.stderr.write ('xkernprot failed with %d\n' % rc)

    def vtrace (self, *words):
        """switches the vtrace

        Possible arguments are:
            on      max 100 bytes per buffer
            all     complete request and reply
            off     no vtrace
            flush   flushes vtrace
            <other> send as 'diagnose vtrace <other>'
        """
        if len (words) == 0:
            words = ['flush']
        firstWord = words [0]
        if firstWord == 'on':
            cmd = 'diagnose vtrace default on'
        elif firstWord == 'off':
            cmd = 'diagnose vtrace default off'
        elif firstWord == 'all':
            cmd = 'diagnose vtrace default order on'
        elif firstWord == 'flush':
            cmd = 'diagnose vtrace flush'
        else:
            cmd = 'diagnose vtrace ' + join (words)
        #self.utilityCommands (1, cmd)
        self.simpleSQL (cmd)

    def utilityCommands (self, asSQL = None, *cmds):
        user, pwd = self.users ['dbm']
        try:
            utility = sapdb.utilitySession (user, pwd, self.name, self.host)
        except sapdb.CommunicationError, err:
            sys.stdout.write (err.message)
            return
        for cmd in cmds:
            try:
                sys.stdout.write ('utility: ' + cmd + ' ')
                if asSQL:
                    utility.sql (cmd)
                else:
                    utility.utility (cmd)
                sys.stdout.write ('\n')
            except sapdb.SQLError, err:
                sys.stdout.write (str (err))

    def utility (self, *options):
        """sends a utility command"""
        self.utilityCommands (None, join (options))

    def switch (self, *options):
        """switches kernel trace.

        arguments: <trace> <test>
        without arguments: switches trace off
        """
        if len (options) == 0:
            options = ['', '']
        elif len (options) == 0:
            options.append ('')
        user, pwd = self.users ['dbm']
        try:
            utility = sapdb.utilitySession (user, pwd, self.name, self.host)
            utility.switch (options [0], options [1])
        except sapdb.SQLError, err:
            sys.stdout.write (err)
        except sapdb.CommunicationError, err:
            sys.stdout.write (err)


    def getState (self):
        try:
            result = self.getDbmSession ().cmd ('db_state')
            lines = split (result, '\n')
            return lines [1]
        except dbm.DBMServError, err:
            return '<unknown>'

    def state (self):
        """show database state"""
        sys.stdout.write (self.getState () + '\n')

    def catalog (self, *words):
        """displays part of the sql catalog.

        Possible queries are:
            tables [<owner>.][<table>]
            table  [<owner>.]<table>
        """
        stmt = join (words)
        user, pwd = self.getDefaultUser ()
        session = sapdb.connect (user, pwd, self.name, self.host,
            'component=XCI&sqlmode=ORACLE')
        import sqlCatalog
        sqlCatalog.sqlCatalog (session, stmt)

    def layout (self, *words):
        """display the structure of a result set or a table
        """
        user, pwd = self.getDefaultUser ()
        session = sapdb.connect (user, pwd, self.name, self.host)
        withInOut = None
        if len (words) == 1:
            stmt = 'select * from ' + words [0]
            description = session.sql (stmt).getDescription ()
        else:
            prepared = session.prepare (join (words))
            description = prepared.getDescription ()
            if len (description) > 0:
                withInOut = 1
            else:
                description = prepared.execute ().getDescription ()
        self.dumpLayout (description, withInOut)

    def dumpLayout (self, description, withInOut = 0):
        maxname = 0
        for row in description:
            maxname = max (maxname, len (row[0]))
        namefmt = '%-' + repr (maxname) + 's'
        for name, kind, kindNumber, length, frac, optional, inout in description:
            line = namefmt % name
            line = line + (' %-10s' % kind)
            if kind == 'Fixed':
                size = '(%d, %d)' % (length, frac)
            elif kind in ['Char', 'Float']:
                size = '(%d)' % length
            else:
                size = ''
            line = line + ('%-10s' % size)
            if withInOut:
                line = line + ' ' + inout
            print line


    def simpleSQL (self, cmd):
        try:
            user, pwd = self.getuser ()
            session = sapdb.connect (user, pwd, self.name, self.host)
            session.sql (cmd)
        except sapdb.SQLError, err:
            sys.stdout.write ("SQLError: [%d] %s\n"
                % (err.errorCode, err.message))


    def sql (self, *words):
        """execute SQL command"""
        try:
            user, pwd = self.getuser ()
            session = sapdb.connect (user, pwd, self.name, self.host,
                'component=XCI&sqlmode=ORACLE')
            import report
            options, stmt = report.parseWords (words)
            result = session.sql (stmt)
            if isinstance (result, sapdb.SapDB_ResultSet):
                report.doReport (result, options)
            elif type (result) == type (()):
                sys.stdout.write (`result` + '\n')
            else:
                sys.stdout.write ('OK\n')
            session.commit ()
        except sapdb.SQLError, err:
            sys.stdout.write ("SQLError: [%d] %s\n"
                % (err.errorCode, err.message))


    def coredump (self, *words):
        """extract stack backtrace from knldiag"""
        dbmSession = self.getDbmSession ()
        cmdout = dbmSession.cmd ("param_directget RUNDIRECTORY")
        name, rundirectory = split (cmdout, '\t') [:2]
        rundirectory = strip (rundirectory)
        knldiag = os.path.join (rundirectory, 'knldiag')
        instream = open (knldiag, 'r')
        line = instream.readline ()
        while line:
            if find (line, '----> Stack Back Trace <----') != -1:
                break
            line = instream.readline ()
        procRE = re.compile (r'BTRACE\s+.*!(.*) [+]')
        sourceRE = re.compile ('BTRACE\s+Source:  (.*) [(] Line: ([0-9]+)')
        stack = []
        lastWasProc = None
        while line:
            match = procRE.search (line)
            if match:
                stack.append (match.group (1))
                lastWasProc = 1
            else:
                match = sourceRE.search (line)
                if match:
                    file, lineno = match.group (1, 2)
                    if lastWasProc:
                        proc = stack.pop ()
                        stack.append ('%s (%s:%s)' % (proc, lineno, file))
                    lastWasProc = None
            line = instream.readline ()
        stack.reverse ()
        for entry in stack:
            print entry [:79]


    def command (self, cmdLine):
        broken = split (cmdLine)
        cmd = broken [0]
        try:
            method = getattr (self, cmd)
        except AttributeError:
            method = self.findShortcut (cmd)
            if method == None:
                raise Error, cmd + ' not implemented'
        apply (method, tuple (broken [1:]))

    def findShortcut (self, cmd):
        keys = self.__class__.__dict__.keys ()
        keys.sort ()
        cmdLen = len (cmd)
        for key in keys:
            if key [:cmdLen] == cmd:
                return getattr (self, key)
        return None

    def help (self, *args):
        """show command help

        without argument: lists all commands
        with argument: shows help for the specified command
        """
        if args:
            cmdName = args [0]
            method = getattr (self, cmdName)
            print "Help for command '%s': %s" % (cmdName, method.__doc__)
        else:
            items = self.__class__.__dict__.items ()
            items.sort ()
            for name, member in items:
                self.methodHelp (name, member)

    def methodHelp (self, name, member):
        try:
            doc = member.__doc__
            if not doc:
                return
            try:
                summary = doc [:index (doc, '\n')]
                more = ' ...'
            except ValueError:
                summary = doc
                more = ''
            print '%-15s %s%s' % (name, summary, more)
        except AttributeError:
            pass

    def sqlSession (self, userName = None, **config):
        user, pwd = self.getuser (userName)
        result = sapdb.connect (user, pwd, self.name, self.host,
            _buildConfig (config))
        return result

    def dbmSession (self):
        user, pwd = self.getuser ('dbm')
        result = dbm.DBM (self.host, self.name, '', user + ',' + pwd)
        return result

    def repmanSession (self, userName = None):
        user, pwd = self.getuser (userName)
        import repman
        result = repman.RepMan (self.host, self.name)
        result.cmd ("use user %s %s;" % (user, pwd))
        return result

def dbInfo (fname = None, dbname = None):
    if fname == None:
        fname = os.environ ['OWN'] + '/db.conf'
    defaultName, dbs = mylib.readConfig (fname,
            ('default', os.environ.get ('SERVERDB', '')), 'dbs')
    if dbname == None:
        dbname = defaultName
    db = None
    for dbInstance in dbs:
        if dbname == dbInstance.name:
            db = dbInstance
            break
    return db

def _buildConfig (configDict):
    list = map (lambda t: t [0] + '=' + t[1], configDict.items ())
    return join (list, '&')

def defaultDB ():
    return dbInfo ()

class FileNotFound (Error):
    def __init__ (self, filekind, filenames):
        self.filekind = filekind
        self.filenames = filenames

    def getMessage (self):
        return '%s not found [%s]' % (self.filekind, join (self.filenames, ', '))

class dbManager:
    def __init__ (self, configFile = None):
        self.loadConfig (configFile)

    def detectConfigFile (self, fname = None):
        ospath = os.path
        # checking for explicit name
        if fname != None:
            if ospath.exists (fname):
                return fname
            else:
                raise FileNotFound ('configuration', fname)
        # checking in $OWN
        configName = 'db.conf'
        missedFiles = []
        own = os.environ.get ('OWN')
        if own:
            ownpath = ospath.join (own, configName)
            if ospath.exists (ownpath):
                return ownpath
            missedFiles.append (ownpath)
        # checking in home directory
        if os.name == 'nt':
            homepath = (os.environ.get ('HOMEDRIVE', '')
                + os.environ.get ('HOMEPATH', '')
                + '\\' + configName)
        else:
            homepath = ospath.expanduser ('~/' + configName)
        if (homepath [0] != '~') and ospath.exists (homepath):
            return homepath
        missedFiles.append (homepath)
        raise FileNotFound ('configuration', missedFiles)

    def loadConfig (self, fname = None):
        fname = self.detectConfigFile (fname)
        self.configName = fname
        self.defaultName, self.dbs = mylib.readConfig (fname,
                ('default', os.environ.get ('SERVERDB', '')), 'dbs')
        self.byName = {}
        for db in self.dbs:
            self.byName [db.name] = db

    def getDB (self, name):
        if not name:
            name = self.defaultName
            if not name and (len (self.dbs) == 1):
                # only one db, so this is the default
                return self.dbs [0]
        db = self.byName [name]
        return db

    def save (self):
        pass

    def listDBs (self, stream = sys.stdout):
        for db in self.dbs:
            stream.write (db.name + '\n')

def manage (configFile = None, dbname = None,
        userinfo = None, mode = None, batchfile = None,
        commands = None):
    manager = dbManager (configFile)
    db = manager.getDB (dbname)
    if mode:
        db.setMode (mode)
    db.userinfo = userinfo
    if batchfile:
        for line in open (batchfile, "r").readlines ():
            line = line [:-1]
            db.command (line)
    if commands:
        for command in commands:
            try:
                db.command (command)
            except Error, err:
                sys.stderr.write (str (err) + '\n')
    manager.save ()

def list (configFile = None):
    manager = dbManager (configFile)
    manager.listDBs ()

