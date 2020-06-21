
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
import cStringIO
import time

from sql import sqlHelper
from FileSystem import FileSystem, File
from IndexGenerator import IndexGenerator
from docSakeLib import DocInfo, updatedSpan

def getEnvironmentInfo ():
    if os.name == 'nt':
        userKey = 'USERNAME'
    else:
        userKey = 'USER'
    user = os.environ.get (userKey, '<unknown>')
    relver = os.environ ['RELVER']
    release = relver [1] + '.' + relver [2]
    import sql.python15.sapdb
    sapdb = sql.python15.sapdb
    return user, release, sapdb

_user, _release, _sapdb = getEnvironmentInfo ()
_deployUser = 'ZZZZZZZ'
_noMatch = (None, None)

class SchemaManager:
    def __init__ (self, session, schemadef):
        self.session = session
        self.schemadef = schemadef

    schemaRE = re.compile (' *create +([a-z]+) +([a-z][a-z0-9_]*)', re.I)

    def extractInfo (self, cmd):
        match = self.schemaRE.match (cmd)
        if match:
            kind, name = match.group (1, 2)
        else:
            kind, name = _noMatch
        return kind, name

    def executeSafe (self, cmd, errorsToIgnore):
        try:
            self.session.sql (cmd)
        except _sapdb.SQLError, exc:
            if exc.errorCode in errorsToIgnore:
                pass
            else:
                raise

    def dropAll (self):
        execSQL = self.session.sql
        for cmd in self.schemadef:
            kind, name = self.extractInfo (cmd)
            if kind:
                self.executeSafe ("drop " + kind + " " + name, [-4004])
        self.session.commit ()

    def requiresExecution (self, cmd):
        result = 1
        kind, name = self.extractInfo (cmd)
        if kind:
            try:
                self.session.sql ("exists " + kind + " " + name)
                result = None   # exists, no execution necessary
            except _sapdb.SQLError, sqlExc:
                result = 1
        return result


    def createAll (self, verbose = None):
        execSQL = self.session.sql
        for cmd in self.schemadef:
            if self.requiresExecution (cmd):
                if verbose:
                    print self.shortCmd (cmd)
                execSQL (cmd)
        self.session.commit ()

    def shortCmd (self, cmd):
        lines = string.split (cmd, '\n')
        for line in lines:
            line = string.strip (line)
            if len (line) > 0:
                return line [:40] + '...'
        return '<empty command>'

class Weaver:
    classPseudoClass = 'class'
    functionPseudoClass = '::'
    keywordPseudoClass = '<keyword>'

    def sourceRelative (self, filename):
        path, base = os.path.split (filename)
        filePathList = string.split (path, '/')
        currentPathList = string.split (self.currentFile, '/') [:-1]
        while filePathList and currentPathList and (filePathList [0] == currentPathList [0]):
            del filePathList [0]
            del currentPathList [0]
        relativeList = (['..'] * len (currentPathList)) + filePathList
        result = string.join (relativeList + [base], '/')
        return result

    def indexRelative (self, filename):
        filedir, basename = os.path.split (filename)
        relative = relativePathList (self.relativePackage [:], filedir)
        result = string.join (relative + [basename], '/')
        return result

    def detectFileDir (self, targetFile):
        pos = string.rfind (targetFile, '/')
        if pos == -1:
            result = '/'
        else:
            result = targetFile [:pos]
        return result

    def fixLink (self, targetFile, targetAnchor = None, allowLocal = 1):
        targetDir = self.detectFileDir (targetFile)
        if allowLocal:
            if targetFile == self.currentFile:
                result = ''
            else:
                result = self.sourceRelative (targetFile)
        else:
            result = self.sourceRelative (targetFile)
        if targetAnchor:
            result = result + '#' + targetAnchor
        return result

    def formatLink (self, file, anchor, text, defaultText = None):
        if not text:
            text = defaultText
        href = self.fixLink (file, anchor)
        return '<A HREF="%s">%s</A>' % (href, text)

    def makeLink (self, contextName, symbol, text):
        """Returns a Link for the given symbol.

        returns <A HREF='%file#%anchor'>text</A> when a reference can
        be found or plain 'text' otherwise.
        """
        file, anchor = self.getLinkInfo (contextName, symbol)
        if file:
            result = self.formatLink (file, anchor, text, text)
        else:
            result = text
        return result

    def getClassLink (self, className, text = None):
        """returns a html link for the given class."""
        if text == None:
            text = className
        return self.makeLink (self.classPseudoClass, className, text)

    def getMethodLink (self, className, memberName, text):
        """returns a html link for the given class."""
        if text == None:
            text = className + '::' + memberName
        return self.makeLink (className, memberName, text)

    def getFunctionLink (self, functionName, text):
        """returns a html link for the given function."""
        if text == None:
            text = functionName
        return self.makeLink (self.functionPseudoClass, functionName, text)

    def getKeywordLink (self, keyword, text):
        """returns a html link for the given keyword."""
        if text == None:
            text = keyword
        return self.makeLink (self.keywordPseudoClass, keyword, text)

    def addClass (self, className, anchor, description = None):
        self.addEntry (self.classPseudoClass, className, 'C', anchor, description)

    def addMethod (self, className, memberName, anchor = None, description = None):
        if anchor == None:
            anchor = memberName
        self.addEntry (className, memberName, 'M', anchor, description)

    def addFunction (self, functionName, anchor = None, description = None):
        if anchor == None:
            anchor = functionName
        self.addEntry (self.functionPseudoClass, functionName, 'M', anchor, description)

    addRoutine = addFunction

    def addKeyword (self, keyword, anchor = None, description = None):
        if anchor == None:
            anchor = keyword
        self.addEntry (self.keywordPseudoClass, keyword, 'K', anchor, description)

    addAnything = addKeyword

_stringType = type ('')

def relativePathList (sourcePath, targetPath):
    if type (sourcePath) == _stringType:
        sourcePath = string.split (sourcePath, '/')
    if type (targetPath) == _stringType:
        targetPath = string.split (targetPath, '/')
    while sourcePath and targetPath and (sourcePath [0] == targetPath [0]):
        del sourcePath [0]
        del targetPath [0]
    relativePath = (['..'] * len (sourcePath)) + targetPath
    return relativePath

class DeploySite:
    """Contains information, where the url points to and where the files are written to."""
    def __init__ (self, url, rootdir):
        self.url = url
        self.rootdir = rootdir

_identsize = 64      # current limit in iput filename
_constants = {
    'stdkey': """    Release char (10),
    Author char (7)""",
    'maxcontextname': _identsize * 2, # allow for nested classes
    'maxident': _identsize,
    'maxfname': 255,
    'maxtitle': 255,
    'DescriptionCol': 'VARCHAR (250)',  # larger value uses too much temp space
}

schemadef = [
"""create table SymbolIndex (
    %(stdkey)s,
    ContextName varchar (%(maxcontextname)d),
    SymbolName varchar (%(maxident)d),
    kind char (1),
    DocFile varchar (%(maxfname)d),
    Anchor varchar (%(maxfname)d),
    abstract %(DescriptionCol)s)
    """ % (_constants),
"""create table ClassHierarchy (
    %(stdkey)s,
    ClassName varchar (%(maxcontextname)d),
    SuperClass varchar (%(maxcontextname)d),
    DocFile varchar (%(maxfname)d))
    """ % (_constants),
"""create table File2Index (
    %(stdkey)s,
    DocFile varchar (%(maxfname)d),
    IndexName varchar (%(maxfname)d))
    """ % (_constants),
"""create table FileCache (
    %(stdkey)s,
    SourceFile varchar (%(maxfname)d),
    DocFile varchar (%(maxfname)d),
    kind char (1),
    Title varchar (%(maxtitle)d),
    abstract %(DescriptionCol)s,
    lastUpdated Date)
    """ % (_constants),
"""create table DeploymentFiles (
    %(stdkey)s,
    DocFile varchar (%(maxfname)d)
    )
    """ % (_constants),
]

def createSchema (session, verbose = None):
    manager = SchemaManager (session, schemadef)
    manager.createAll (verbose)

def dropSchema (session):
    manager = SchemaManager (session, schemadef)
    manager.dropAll ()

_defaultUser = 'DOC'
_defaultPwd = 'DOC'
_defaultDB = 'docbase'
#
# jrg 18.Juli 2000
# New DHCP has problems with complete IP Address, so i stripped the domain suffix...
#
#_defaultHost = 'p48047.ber.sap-ag.de'
_defaultHost = 'p48047'

def defaultSession ():
    result = _sapdb.connect (_defaultUser, _defaultPwd,
        _defaultDB, _defaultHost)
    return result

def sqlIgnore100 (self, command, args):
    sql = getattr (self, command)
    try:
        result = sql (args)
    except _sapdb.SQLError, err:
        if err.errorCode == 100:
            result = []   # to allow for empty SELECTS
        else:
            # print command
            raise
    return result

def sqlXIgnore100 (self, command, args):
    try:
        result = self.sqlX (command, args)
    except _sapdb.SQLError, err:
        if err.errorCode == 100:
            result = []   # to allow for empty SELECTS
        else:
            # print command
            raise
    return result

_statements = {
    #
    # symbol management
    #
    'clearSymbolsForFile' : """delete from SymbolIndex where
        Release = ? AND Author = ? AND DocFile = ?
    """,
    'insert': """insert into SymbolIndex
        (Release, Author, Contextname, Symbolname, Kind, DocFile, Anchor, abstract)
        values (?, ?, ?, ?, ?, ?, ?, ?)
    """,
    'selectByContext': """select Symbolname, DocFile, Anchor from SymbolIndex
            where Release = ? AND Author = ? AND ContextName = ?
        union SELECT Symbolname, DocFile, Anchor from SymbolIndex
            where Release = ? AND Author = '%s' AND ContextName = ?
            AND DocFile <> ALL (SELECT DocFile from SymbolIndex where Author = ?)
    """ % _deployUser,
    'selectByPattern': """select Symbolname, DocFile, Anchor from SymbolIndex
            where Release = ? AND Author = ? AND SymbolName like ?
        union SELECT Symbolname, DocFile, Anchor from SymbolIndex
            where Release = ? AND Author = '%s' AND SymbolName like ?
            AND DocFile <> ALL (SELECT DocFile from SymbolIndex where Author = ?)

    """ % _deployUser,

    #
    # index groups
    #
    'deleteFile2Index': """delete from File2Index
        where Release = ? AND Author = ? AND docfile = ?
    """,
    'insertFile2Index': """insert into File2Index
        (Release, Author, Docfile, IndexName) values (?, ?, ?, ?)
    """,

    #
    # file info (title, abstract)
    #
    'deleteFile': """delete from FileCache
        where Release = ? AND Author = ? AND SourceFile = ? AND DocFile = ?
    """,
    'insertFile': """insert into FileCache
        (Release, Author, SourceFile, Docfile, kind, Title, abstract, lastUpdated)
        values (?, ?, ?, ?, ?, ?, ?, ?)
    """,
    'selectFile': """select Title, DocFile, abstract, kind, lastUpdated from FileCache
            where Release = ? AND Author = ? AND SourceFile = ?
        union select Title, DocFile, abstract, kind, lastUpdated from FileCache
            where Release = ? AND Author = '%s' AND SourceFile = ?
            AND DocFile <> ALL (SELECT DocFile from SymbolIndex where Author = ?)
    """ % _deployUser,

    #
    # list of deployment documents
    #
    'clearDeployment': """delete from DeploymentFiles
        where Release = ? AND Author = ?
    """,
    'addDeployment': """insert into DeploymentFiles
        (Release, Author, DocFile) values (?, ?, ?)
    """,

}

class SapDBWeaver (Weaver,
        sqlHelper.CommandCollection,
        sqlHelper.ExtendedConnection):
    docHeader = None

    def __init__ (self, session, deploySite, testSite = None, deploying = None):
        if session == None:
            session = defaultSession ()
        sqlHelper.CommandCollection.__init__ (self, session, _statements)
        sqlHelper.ExtendedConnection.__init__ (self, session)
        self.deploySite = deploySite
        self.testSite = testSite
        self.deploying = deploying
        self.symbols = {}
        # vars for indices
        self.selections = []
        self.targetDirs = []
        self.deflist = [('C', 'Class Index', 'classindex'),
            ('CMK', 'Index', 'index')]
        if deploying:
            sqlIgnore100 (self, 'clearDeployment', [_release, _user])

    def close (self):
        if self.session:
            self.session.release ()
            self.session = None

    __del__ = close

    def setCurrentFile (self, fileName):
        self.symbols = {}   # clear completely, pretty difficult to cache correctly
        filestring = str (fileName)
        sqlIgnore100 (self, 'clearSymbolsForFile', [_release, _user, filestring])
        self.currentFile = filestring
        sqlIgnore100 (self, 'deleteFile2Index', [_release, _user, filestring])
        for selection in self.selections:
            self.insertFile2Index ([_release, _user, filestring, selection])
        if self.deploying:
            self.addDeployment ([_release, _user, filestring])

    def getLinkInfo (self, contextName, symbol):
        """Returns filename and anchor for the given symbol.
        """
        key = (contextName, symbol)
        try:
            file, anchor = self.symbols [key]
        except KeyError:
            file, anchor = self.loadSymbolsFor (key)
        return file, anchor

    def loadSymbolsFor (self, key):
        # todo: check this key hasn't been selected before
        contextName, memberName = key
        try:
            if contextName == None:
                pattern = memberName [0] + '*'
                cursor = self.selectByPattern (
                    [_release, _user, pattern, _release, pattern])
            else:
                cursor = self.selectByContext (
                    [_release, _user, contextName, _release, contextName])
        except _sapdb.SQLError, err:
            if err.errorCode == 100:
                return _noMatch
            else:
                raise
        symbols = self.symbols
        for symbol, docfile, anchor in cursor:
            if symbol == '':
                symbol = None
            newKey = (contextName, symbol)
            newValue = (docfile, anchor)
            symbols [newKey] = newValue
        try:
            result = symbols [key]
        except KeyError:
            result = _noMatch
        return result

    def addEntry (self, contextName, memberName, kind, anchor, description = None):
        if description == '':
            description = None
        try:
            self.insert ([_release, _user, contextName, memberName,
                kind, self.currentFile, anchor, description])
        except _sapdb.SQLError, sqlErr:
            if sqlErr.errorCode == 200:
                entry = self.entryString (contextName, memberName, kind)
                sys.stderr.write ('%s: Warning: Duplicate Entry: %s\n' % (
                    self.currentFile, entry))
            else:
                raise

    def entryString (self, contextName, symbol, kind):
        if kind == 'C':
            result = 'class ' + contextName
        elif kind == 'M':
            if contextName == '::':
                result = '::' + symbol
            else:
                result = contextName + '::' + symbol
        else:
            result = symbol
        return result

    def removeCurrentEntries (self):
        pass

    def save (self):
        if self.deploying:
            mover = DeployMove (self.session, ['SymbolIndex',
                'ClassHierarchy', 'File2Index', 'FileCache'])
            mover.move (_release, _user)
        self.endTransaction (1)

    def endTransaction (self, ok):
        if ok:
            self.session.commit ()
        else:
            self.session.rollback ()

    def addFileInfo (self, title, source, docfile, abstract, isPackage, lastUpdated):
        sqlIgnore100 (self, 'deleteFile', [_release, _user, source, docfile])
        if isPackage:
            kind = 'P'
        else:
            kind = 'H'
        updatedString = time.strftime ('%Y%m%d', time.gmtime (lastUpdated))
        self.insertFile ([_release, _user, source, docfile, kind, title, abstract, updatedString])

    def stream2string (self, stream):
        if type (stream) == type (''):
            # while description is in VARCHAR, not LONG
            return stream
        if not stream:
            return None
        strbuf = cStringIO.StringIO ()
        buf = stream.read (4096)
        while buf:
            strbuf.write (buf)
            buf = strbuf.read (4096)
        return strbuf.getvalue ()

    def getFileInfo (self, source):
        cursor = sqlIgnore100 (self, 'selectFile', [_release, _user, source, _release, source])
        if not cursor:
            print 'failed to find db record for', [_release, _user, source]
        title, docfile, abstractStream, kind, updatedString = None, source, 'error', 'H', None
        for title, docfile, abstractStream, kind, updatedString in cursor:
            pass
        if not title:
            title = docfile
        lastUpdated = self.dateString2time (updatedString)
        return title, docfile, self.stream2string (abstractStream), (kind == 'P'), lastUpdated

    def dateString2time (self, dateString):
        if not dateString:
            return 0
        year = int (dateString [:4])
        month = int (dateString [4:6])
        day = int (dateString [6:8])
        result = time.mktime ((year, month, day, 0, 0, 0, 0, 0, -1))
        return result

    #
    # methods for indices
    #
    def pushIndex (self, selection, targetDir):
        self.selections.append (selection)
        self.targetDirs.append (targetDir)

    def popIndex (self):
        self.selections.pop ()
        self.targetDirs.pop ()

    def genIndex (self, targetDir, selection):
        for kinds, title, namebase in self.deflist:
            if selection:
                title = title + ' ' + selection
                namebase = namebase + '-' + selection
            generator = SapDBIndexGenerator (self.session, kinds, title, namebase)
            generator.docHeader = self.docHeader
            generator.genIndex (targetDir, selection)

    def genCurrentIndex (self):
        selection = self.selections [-1]
        targetDir = self.targetDirs [-1]
        self.genIndex (targetDir, selection)

    def genMainIndex (self, targetDir):
        self.genIndex (targetDir, None)

    def getIndexFiles (self):
        result = []
        if len (self.selections) > 0:
            selection = self.selections [-1]
            targetDir = self.targetDirs [-1]
            for kind, title, namebase in self.deflist:
                base = namebase + '-' + selection + '.html'
                file = targetDir.join (base)
                result.append (file)
        else:
            targetDir = File ('/', FileSystem ())
            for kind, title, namebase in self.deflist:
                base = namebase + '.html'
                file = targetDir.join (base)
                result.append (file)
        return result

    #
    # methods to handle super classes
    #
    def setSuperclasses (self, className, superClasses, docFile):
        sqlXIgnore100 (self, """Delete from ClassHierarchy
            Where Release = ? AND Author = ? AND ClassName = ?
        """, [_release, _user, className])
        for superClass in superClasses:
            self.sqlX ("""Insert into ClassHierarchy
                (Release, Author, ClassName, Superclass, docFile) values (?, ?, ?, ?, ?)
            """, [_release, _user, className, superClass, docFile])

    def getSuperClasses (self, className):
        result = []
        cursor = sqlXIgnore100 (self, """Select distinct SuperClass from ClassHierarchy
                where Release = ? AND Author = ? AND ClassName = ?
            """, [_release, _user, className])
        for (superclass,) in cursor:
            result.append (superclass)
        return result

    def fetchMethodsForClass (self, clazz, knownMethods):
        result = []
        cursor = sqlXIgnore100 (self, """Select Symbolname, DocFile, Anchor, abstract
                from SymbolIndex
                where Release = ? AND Author IN (?, '%s') AND ContextName = ?
            """ % _deployUser, [_release, _user, clazz])
        for symbol, docfile, anchor, description in cursor:
            if symbol not in knownMethods:
                result.append ((symbol, docfile, anchor, description))
                knownMethods.append (symbol)
        return result

    def getClassInfo (self, className, methodNames):
        #
        # get superclasses
        #
        classList = [className]
        pos = 0
        while pos < len (classList):
            classList.extend (self.getSuperClasses (classList [pos]))
            pos = pos + 1
        #
        # get methods
        #
        result = []
        knownMethods = methodNames
        for clazz in classList [1:]:
            methods = self.fetchMethodsForClass (clazz, knownMethods)
            if methods:
                result.append ((clazz, methods))
        return result

    #
    # methods to handle updated entries
    #

    def getUpdatedForCurrentIndex (self):
        then = time.localtime (time.time () - updatedSpan)
        lastUpdated = time.strftime ('%Y%m%d', then)
        indexName = self.selections [-1]

        try:
            cursor = self.sqlX ("""select Title, DocFile, abstract, kind, lastUpdated from FileCache
                where Release = ? AND Author = ? AND lastUpdated >= ?
                and docfile in (select docfile from file2index where indexname = ?)
            union select Title, DocFile, abstract, kind, lastUpdated from FileCache
                where Release = ? AND Author = '%s' AND lastUpdated >= ?
                and docfile in (select docfile from file2index where indexname = ?)
                AND DocFile <> ALL (SELECT DocFile from SymbolIndex where Author = ?)
            order by 5 DESC, 1 ASC
            """ % _deployUser,
            [_release, _user, lastUpdated, indexName,
             _release, lastUpdated, indexName, _user])
        except _sapdb.SQLError, err:
            if err.errorCode == 100:
                print 'nothing updated'
                return []
            else:
                raise
        result = []
        for title, docFile, abstract, kind, lastUpdatedString in cursor:
            lastUpdated = self.dateString2time (lastUpdatedString)
            info = DocInfo (title, docFile, abstract, kind == 'P', lastUpdated)
            result.append (info)
        return result

class DeployMove:
    def __init__ (self, connection, tablenames):
        self.connection = connection
        self.tableDefinitions = self.getDefinitions (tablenames)

    def getDefinitions (self, tablenames):
        select = ('select tablename, columnname from columns '
            + 'where owner = USERGROUP and tablename in ('
            + self.quotedList (tablenames)
            + ') order by tablename, pos')
        cursor = self.connection.sql (select)
        result = []
        currentTable = ''
        for tablename, colname in cursor:
            if tablename != currentTable:
                currentCols = []
                result.append ((tablename, currentCols))
                currentTable = tablename
            currentCols.append (colname)
        return result

    def quotedList (self, tablenames):
        list = []
        for tablename in tablenames:
            list.append ("'" + string.upper (tablename) + "'")
        return string.join (list, ',')

    def showError (self, cmd, err):
        pos = err.errorPos - 1
        lines = string.split (cmd, '\n')
        printPos = 0
        for line in lines:
            print line
            if printPos <= pos < printPos + len (line):
                print (' ' * (pos - printPos)) + '^ ' + err.message
            printPos = printPos + len (line) + 1


    def ignore100 (self, cmd):
        try:
            result = self.connection.sql (cmd)
        except _sapdb.SQLError, err:
            if err.errorCode == 100:
                result = []   # to allow for empty SELECTS
            else:
                self.showError (cmd, err)
                raise
        return result

    def move (self, release, user):
        for tabledef in self.tableDefinitions:
            self.deleteCurrentEntries (tabledef, release, user)
            self.copyNewEntries (tabledef, release, user)
            self.deleteWorkEntries (tabledef, release, user)
        self.deleteDeploymentList (release, user)

    def deleteCurrentEntries (self, tabledef, release, user):
        tablename = tabledef [0]
        deployUser = _deployUser
        self.ignore100 ("""delete from %(tablename)s
            where Release = '%(release)s'
            and   Author = '%(deployUser)s'
            and   DocFile in
                (Select DocFile From DeploymentFiles
                    where Release = '%(release)s' AND Author = '%(user)s')
            """ % locals ())

    def copyNewEntries (self, tabledef, release, user):
        tablename, colnames = tabledef
        deployUser = _deployUser
        selectnames = []
        for colname in colnames:
            if colname == 'AUTHOR':
                selectnames.append ("'" + _deployUser + "'")
            else:
                selectnames.append (colname)
        selectString = string.join (selectnames, ',')
        self.ignore100 ("""insert into %(tablename)s
            select %(selectString)s
            from %(tablename)s
            where Release = '%(release)s'
            and   Author = '%(user)s'
            and   DocFile in
                (Select DocFile From DeploymentFiles
                    where Release = '%(release)s' AND Author = '%(user)s')
            Update Duplicates
        """ % locals ())

    def deleteWorkEntries (self, tabledef, release, user):
        tablename = tabledef [0]
        deployUser = _deployUser
        self.ignore100 ("""delete from %(tablename)s
            where Release = '%(release)s'
            and   Author = '%(user)s'
            and   DocFile in
                (Select DocFile From DeploymentFiles
                    where Release = '%(release)s' AND Author = '%(user)s')
            """ % locals ())

    def deleteDeploymentList (self, release, user):
        self.ignore100 ("""delete from DeploymentFiles
            where Release = '%(release)s' AND Author = '%(user)s'
            """)

_indexStatements = {
    'classSelect': """select Symbolname, abstract, docfile, anchor
        from SymbolIndex
        where Release = ?
        and   author = ?
        and   kind = 'C'""",
    'anySelect': """select contextname, symbolname, kind, abstract, docfile, anchor
        from SymbolIndex
        where Release = ?
        and   author = ?
        and   kind = ?""",
    'classSelectIndex': """select symbolname, abstract, docfile, anchor
        from SymbolIndex
        where Release = ?
        and   author = ?
        and   kind = 'C'
        and docfile in (
            select docfile from file2index where indexname = ?)""",
    'anySelectIndex': """select contextname, symbolname, kind, abstract, docfile, anchor
        from SymbolIndex
        where Release = ?
        and   author = ?
        and   kind = ?
        and docfile in (
            select docfile from file2index where indexname = ?)""",
}

class SapDBIndexGenerator (IndexGenerator, sqlHelper.CommandCollection):
    def __init__ (self, session = None, kinds = None, title = None, namebase = None):
        if session == None:
            session = defaultSession ()
        IndexGenerator.__init__ (self, kinds, title, namebase)
        sqlHelper.CommandCollection.__init__ (self, session, _indexStatements)

    def addClassEntries (self, selection, entries):
        if selection == None:
            cursor = sqlIgnore100 (self, 'classSelect', [_release, _user])
        else:
            cursor = sqlIgnore100 (self, 'classSelectIndex', [_release, _user, selection])
        lower = string.lower
        for classname, description, docfile, anchor in cursor:
            #if description:
            #    description = description.read (1000)
            entry = (lower (classname), classname, 'class', description, docfile, anchor)
            entries.append (entry)
            pos = string.find (classname, '_')
            if pos != -1:
                shortName = classname [pos + 1:]
                entry = (lower (shortName), shortName, 'class ' + classname, description, docfile, anchor)
                entries.append (entry)

    def addAnyEntries (self, kind, selection, entries):
        if selection == None:
            cursor = sqlIgnore100 (self, 'anySelect', [_release, _user, kind])
        else:
            cursor = sqlIgnore100 (self, 'anySelectIndex', [_release, _user, kind, selection])
        lower = string.lower
        for classname, symbol, kind, description, docfile, anchor in cursor:
            #if description:
            #    description = description.read (1000)
            if kind == 'K':
                entry = (lower (symbol), symbol, 'keyword', description, docfile, anchor)
            elif kind == 'M':
                if classname == '::':
                    context = 'global function'
                else:
                    context = 'in class ' + classname
                entry = (lower (symbol), symbol, context, description, docfile, anchor)
            else:
                entry = (lower (symbol), symbol, kind, description, docfile, anchor)
            entries.append (entry)

class SapDBIndexGeneratorFactory:
    def __init__ (self, session = None):
        if session == None:
            session = defaultSession ()
        self.session = session

    def create (self, kinds = None, title = None, namebase = None):
        return SapDBIndexGenerator (self.session, kinds,
            title, namebase)

def connect ():
    import docConfig
    connectArgs = docConfig.connectParameters
    session = apply (_sapdb.connect, connectArgs)
    return session

def main (options, args):
    """
    'SapDBWeaver create'    creates tables of schema
    'SapDBWeaver drop'      drops tables of schema

    """
    if options.userInfo:
        name, pwd = string.split (options.userInfo, ',')
        session = _sapdb.connect (name, pwd, options.dbname, options.node)
    else:
        session = connect ()
    manager = SchemaManager (session, schemadef)
    command = args [0]
    if command == 'create':
        manager.createAll (options.verbose)
    elif command == 'drop':
        manager.dropAll ()
    elif command == 'recreate':
        manager.dropAll ()
        manager.createAll (options.verbose)
    else:
        print "command must be one of 'create', 'drop'"

def _options ():
    return [
        #(optstring, varname, typechar, default, help)
        ('u', 'userInfo', ':', _defaultUser + ',' + _defaultPwd, 'user info'),
        ('d', 'dbname', ':', _defaultDB, 'database name'),
        ('n', 'node', ':', _defaultHost, 'database host/node'),
        ('v', 'verbose', '', None, 'verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

