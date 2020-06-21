
# tutoriallib.py
#
#
#    ========== licence begin  GPL
#    Copyright (c) 2000-2005 SAP AG
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

import sys
import string
import re
import os
import tempfile

import sdb.loader as loader
import sdb.sql as sql

_version = 0.2

_okExit = 0
_sqlErrorExit    = 2
_loaderErrorExit = 3
_pythonErrorExit = 4

#
#
# common code used by all installation modules
#

verbose = None

def loaderCmd (session, cmdStr):
    #if verbose:
    #    print cmdStr,
    try:
        result = session.cmd (cmdStr)
        #if verbose:
        #    print '=> OK', result
    except loader.LoaderError, err:
        #if verbose:
        #    print '=> loader failed', err
        raise loader.LoaderError, err, sys.exc_traceback
    except:
        #print '=> unexpected exception'
        kind, val, traceback = sys.exc_info()
        raise kind, val, traceback

def sendErrorIgnoreMessage (session, errorCode):
    if errorCode != 0:
        session.cmd ('REMARK "install script: error %d can be ignored"' % errorCode)

def sqlOK (session, cmd):
    try:
        loaderCmd (session, cmd)
        ok = 1
    except loader.LoaderError, err:
        if (err.sqlCode == 0) or (err.sqlCode == None):
            raise err  # this is a ReplMan error
        ok = None
        sendErrorIgnoreMessage (session, err.sqlCode)
    return ok

def stopIfSqlNotOK (session, cmd):
    try:
        loaderCmd (session, cmd)
    except loader.LoaderError, err:
        if (err.sqlCode == 0) or (err.sqlCode == None):
            raise err  # this is a ReplMan error
        stopInstall (session, err)

def sqlRC (session, cmd, sendIgnoreRemark = 1):
    try:
        loaderCmd (session, cmd)
        rc = 0
    except loader.LoaderError, err:
        if (err.sqlCode == 0) or (err.sqlCode == None):
#           print 'no match'
            raise err  # this is a ReplMan error
        rc = err.sqlCode
        if sendIgnoreRemark:
            sendErrorIgnoreMessage (session, rc)
    return rc

def stopInstall (session, err):
    if isinstance (err, loader.LoaderError):
        raise err
    else:
        raise InstallationAbort ('unexpected Error ' + `err`)

def stopIfSqlNotErr (session, expectErr, cmd):
    rc = sqlRC (session, cmd, None)
    if not (rc in [0, expectErr]):
        stopInstall(session, rc)
    else:
        sendErrorIgnoreMessage (session, rc)
    return rc

def createReplaceView (session, viewname, cmdtail):
    rc = stopIfSqlNotErr (session, -1026, 'CREATE OR REPLACE VIEW ' + viewname + ' ' + cmdtail)
    if rc == -1026:
        stopIfSqlNotErr (session, -4004, 'DROP VIEW ' + viewname)
        stopIfSqlNotOK (session, 'CREATE VIEW ' + viewname + ' ' + cmdtail)


def existsTable (session, tableName):
    return sqlOK (session, "EXISTS TABLE %s" % tableName)

def dropView (session, viewName):
    if existsTable (session, viewName):
        stopIfSqlNotOK (session, "DROP VIEW %s" % viewName)

def dropTable (session, tableName):
    if existsTable (session, tableName):
        stopIfSqlNotOK (session, "DROP TABLE %s" % tableName)

def getInstallComment (install, file):
    doc = install.__doc__
    if not doc:
        doc = file
    doc = string.rstrip (doc)
    return doc

def getSourceFile (module):
    result = module.__file__
    if result [-4:] == '.pyc':
        result = result [:-1]
    return result

def include (session, options, *files):
    for file in files:
        module = __import__ (file)
        install = module.install
        doc = getInstallComment (install, file)
        if options.verbose or not options.quiet:
            print doc
        session.cmd ('REMARK ">INCLUDE FILE \'%s\' (%s)"'
            % (file, getSourceFile (module)))
        if install.__doc__:
            session.cmd ('REMARK "' + install.__doc__ + '"')
        try:
            install (session, options)
        finally:
            session.cmd ('REMARK "<LEAVING FILE \'' + file + '\'"')

def switchToDefaultUser (session, options):
    pos = string.find (options.user, ',')
    username = options.user [:pos]
    pwd = options.user [pos + 1:]
    session.cmd ('use user %s %s' % (username, pwd))

def switchToSchema (session, schemaname):
    cmd = 'set current_schema = %s' % schemaname
    session.cmd (cmd)

def switchToDefaultSchema (session, options):
    pos = string.find (options.user, ',')
    username = options.user [:pos]
    switchToSchema (session, username)
    
class InstallationAbort:
    def __init__ (self, message):
        self.message = message

    def __str__ (self):
        return self.message

def flatString (value):
    return re.sub ('\n[ \t]*', ' ', value)

def fatalError (msg):
    sys.stderr.write (msg + '\n')
    sys.exit (2)

def getFileForTraceback (tb):
    fname = tb.tb_frame.f_code.co_filename
    return fname

def tracebackSize (tb):
    if tb == None:
        return 0
    nesting = tracebackSize (tb.tb_next)
    if nesting == 0:
        fname = getFileForTraceback (tb)
        if string.lower (os.path.basename (fname)) == 'tutoriallib.py':
            return 0
    return nesting + 1

def logInstallationAbort (session, message):
    try:
        message = 'INSTALLATION ABORTED: %s' % message
        decoration = '!' * len (message)
        if hasattr(session,'sqlX') == False :
           session.cmd ('REMARK "%s"' % decoration)
           session.cmd ('REMARK "%s"' % message)
           session.cmd ('REMARK "%s"' % decoration)
        if hasattr(session,'sqlX') == True: 
           pass  
    except:
        pass

def logInstallationSuccess (session, message):
    try:
        decoration = '=' * len (message)
        session.cmd ('REMARK "%s"' % decoration)
        session.cmd ('REMARK "%s"' % message)
        session.cmd ('REMARK "%s"' % decoration)
    except:
        pass

def dumpInstallError (err, logfile, session):
    import traceback
    write = logfile.write
    exctype, excvalue, tb = sys.exc_info ()
    nesting = tracebackSize (tb)
    write ('---------Traceback---------------\n')
    traceback.print_tb (tb, nesting, logfile)
    if hasattr (err, 'cmd') and err.cmd:
        write ('---------Command-----------------\n')
        write (err.cmd + '\n')
    write ( '---------Error-------------------\n')
    write (str (err) + '\n')
    logInstallationAbort (session, str (err))

def dumpSyntaxError (logfile, session):
    import traceback
    write = logfile.write
    exctype, excvalue, tb = sys.exc_info ()
    write ('---------Traceback---------------\n')
    #traceback.print_tb (tb, None, logfile)
    traceback.print_exception (exctype, excvalue, tb, None, logfile)
    write ( '---------Error-------------------\n')
    write ("%s: %s\n" % (exctype, excvalue))
    lines = traceback.format_exception_only(exctype, excvalue)
    logInstallationAbort (session, "%s: %s\n" % (exctype, excvalue))

def dumpPythonError (logfile, session):
    import traceback
    write = logfile.write
    exctype, excvalue, tb = sys.exc_info ()
    write ('---------Traceback---------------\n')
    traceback.print_tb (tb, None, logfile)
    write ( '---------Error-------------------\n')
    write ("%s: %s\n" % (exctype, excvalue))
    logInstallationAbort (session, "%s: %s\n" % (exctype, excvalue))

def locateModule (moduleName):
    import imp
    import os
    stream, fname, dummy = imp.find_module (moduleName)
    result = os.path.dirname (fname)
    if result == '':
        result = '.'
    return result
    
def connect (options):
    if not options.user:
        fatalError ('user name required (-u <user>,<pwd>)')
    if not options.db:
        fatalError ('database name required (-d <dbname>)')
    pos = string.find (options.user, ',')
    if pos == -1:
        fatalError ('wrong format, use -u <user>,<pwd>')
    username = options.user [:pos]
    pwd = options.user [pos + 1:]
    os.putenv('maxdbloaderserver','true')
    if options.remote:
        node = options.node
    else:
        node = ''
    if not options.dbroot:
        session = loader.Loader (node, options.db)
    else:
        session = loader.Loader (node, '', options.dbroot)
    if options.node == '':
        session.cmd ('use user %s %s serverdb "%s"' % (username, pwd, options.db))
    else:
        session.cmd ('use user %s %s serverdb "%s" on %s' % (username, pwd, options.db, options.node ))
    return session

class StdoutLog:
    def write (self, text):
        sys.stdout.write (text)

class TeeLog:
    def __init__ (self, fname):
        self.logfile = open (fname, 'w')

    def write (self, text):
        self.logfile.write (text)
        sys.stdout.write (text)

_dblngMapping = {
    'en': 'ENG',
    'de': 'DEU',
}

def connectAndInstall (installRoutine, docString = None, args = None):
    if docString == None:
        docSting = installRoutine.__doc__
    import optlib
    options, args = optlib.parseArgs (_options (), docString, args)
    global verbose
    verbose = options.verbose
    session = connect (options)
    options.dblng = _dblngMapping [options.lng]
    if options.errfile:
        options.errLog = TeeLog (options.errfile)
    else:
        options.errLog = StdoutLog ()
    try:
        installRoutine (session, options)
        successMessage = 'Installation successfully finished'
        decoration = '=' * len (successMessage)
        options.errLog.write (decoration + '\n' + successMessage + '\n'
            + decoration + '\n')
        logInstallationSuccess (session, successMessage)
        exitCode = _okExit
    except (loader.LoaderError, InstallationAbort), err:
        dumpInstallError (err, options.errLog, session)
        exitCode = _loaderErrorExit
    except SyntaxError:
        dumpSyntaxError (options.errLog, session)
        exitCode = _pythonErrorExit
    except:
        dumpPythonError (options.errLog, session)
        exitCode = _pythonErrorExit
    session = None
    sys.exit (exitCode)

def main (options, args):
    session = connect (options)
    install (session)
    session.release ()

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('u', 'user', ':', None, 'user name [<user>,<pwd>]'),
        ('dompwd', 'dompwd', ':', None, 'domain password'),
        ('usap', 'sapuser', ':', None, 'sap user <user>,<pwd>'),
        ('d', 'db', ':', None, 'db name'),
        ('n', 'node', ':', '', 'server name'),
        ('R', 'dbroot', ':', '', 'installation root'),
        ('r', 'remote',':', None, 'connect to remote Loader server'),
        ('l', 'lng', ':', 'en', 'language: de, en ...'),
        ('fdir', 'fdir', ':', '.', 'inputfile directory'),
        ('v', 'verbose', '', None, 'verbose output'),
        ('q', 'quiet', '', None, 'no output'),
        ('m', 'migrate', '', None, 'migrate catalog'),
        ('err', 'errfile', ':', None, 'name of error log'),
        ('version', None, '', _version, 'version information'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options())


