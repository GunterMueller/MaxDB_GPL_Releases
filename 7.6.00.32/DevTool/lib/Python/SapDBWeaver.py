
import sys
import string
import re
import os

import sqlHelper
from FileSystem import FileSystem, File
from IndexGenerator import IndexGenerator

def getEnvironmentInfo ():
    if os.name == 'nt':
        userKey = 'USERNAME'
    else:
        userKey = 'USER'
    user = os.environ.get (userKey, '<unknown>')
    releaseFile = os.environ ['OWN']
    fileContent = open (releaseFile + '/Release.dat', 'r').read ()
    release = string.strip (fileContent)
    if release == '7.3':
        import sapdb72
        sapdb = sapdb72
    else:
        import sapdb
    # print sapdb
    return user, release, sapdb

_user, _release, _sapdb = getEnvironmentInfo ()
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

    def dropAll (self):
        execSQL = self.session.sql
        for cmd in self.schemadef:
            kind, name = self.extractInfo (cmd)
            if kind:
                execSQL ("drop " + kind + " " + name)
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
    functionPseudoClass = '::'
    keywordPseudoClass = '<keyword>'

    def isPseudoClass (self, className):
        return className [0] in '<:'

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

    def makeLink (self, className, member, text):
        """Returns a Link for the given className and member.

        returns '<A HREF="%file#%anchor">text</A>' when a reference can
        be fouind or plain 'text' otherwise.
        """
        file, anchor = self.getLinkInfo (className, member)
        if file:
            result = self.formatLink (file, anchor, text, text)
        else:
            result = text
        return result

    def getClassLink (self, className, text = None):
        """returns a html link for the given class."""
        if text == None:
            text = className
        return self.makeLink (className, None, text)

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
        self.addEntry (className, '', 'C', anchor, description)

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
    'maxclassname': _identsize * 2, # allow for nested classes
    'maxident': _identsize,
    'maxfname': 255,
}

schemadef = [
"""create table SymbolIndex (
    %(stdkey)s,
    ClassName varchar (%(maxclassname)d),
    SymbolName varchar (%(maxident)d),
    kind char (1),
    DocFile varchar (%(maxfname)d),
    Anchor varchar (%(maxfname)d),
    shortdesc LONG ASCII)
    """ % (_constants),
"""create table ClassHierarchy (
    %(stdkey)s,
    ClassName varchar (%(maxclassname)d),
    SuperClass varchar (%(maxclassname)d))
    """ % (_constants),
"""create table File2Index (
    %(stdkey)s,
    DocFile varchar (%(maxfname)d),
    IndexName varchar (%(maxfname)d))
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
_defaultHost = 'p26223.bea.sap-ag.de'

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

_statements = {
    'clearSymbolsForFile' : """delete from SymbolIndex where
        Release = ? AND Author = ? AND DocFile = ?""",
    'insert': """insert into SymbolIndex
        (Release, Author, ClassName, Symbolname, Kind, DocFile, Anchor, shortdesc)
        values (?, ?, ?, ?, ?, ?, ?, ?)
    """,
    'selectByClass': """select Symbolname, DocFile, Anchor from SymbolIndex
        where Release = ? AND Author = ? AND ClassName = ?
    """,
    'selectByPattern': """select Symbolname, DocFile, Anchor from SymbolIndex
        where Release = ? AND Author = ? AND ClassName = ? AND SymbolName like ?
    """,
    'deleteFile2Index': """delete from File2Index
        where Release = ? AND Author = ? AND docfile = ? """,
    'insertFile2Index': """insert into File2Index
        (Release, Author, Docfile, IndexName) values (?, ?, ?, ?)""",
}

class SapDBWeaver (Weaver, sqlHelper.CommandCollection):
    docHeader = None

    def __init__ (self, session, deploySite, testSite = None):
        if session == None:
            session = defaultSession ()
        sqlHelper.CommandCollection.__init__ (self, session, _statements)
        self.deploySite = deploySite
        self.testSite = testSite
        self.symbols = {}
        # vars for indices
        self.selections = []
        self.targetDirs = []
        self.deflist = [('C', 'Class Index', 'classindex'),
            ('CMK', 'Index', 'index')]

    def setCurrentFile (self, fileName):
        self.symbols = {}   # clear completely, pretty difficult to cache correctly
        filestring = str (fileName)
        sqlIgnore100 (self, 'clearSymbolsForFile', [_release, _user, filestring])
        self.currentFile = filestring
        sqlIgnore100 (self, 'deleteFile2Index', [_release, _user, filestring])
        for selection in self.selections:
            self.insertFile2Index ([_release, _user, filestring, selection])

    def getLinkInfo (self, className, member):
        """Returns filename and anchor for the given class and member.
        """
        key = (className, member)
        try:
            file, anchor = self.symbols [key]
        except KeyError:
            file, anchor = self.loadSymbolsFor (key)
        return file, anchor

    def loadSymbolsFor (self, key):
        # todo: check this key hasn't been selected before
        className, memberName = key
        try:
            if self.isPseudoClass (className):
                pattern = memberName [0] + '*'
                cursor = self.selectByPattern ([_release, _user, className, pattern])
            else:
                cursor = self.selectByClass ([_release, _user, className])
        except _sapdb.SQLError, err:
            if err.errorCode == 100:
                return _noMatch
            else:
                raise
        symbols = self.symbols
        for symbol, docfile, anchor in cursor:
            if symbol == '':
                symbol = None
            newKey = (className, symbol)
            newValue = (docfile, anchor)
            symbols [newKey] = newValue
        try:
            result = symbols [key]
        except KeyError:
            result = _noMatch
        return result

    def addEntry (self, className, memberName, kind, anchor, description = None):
        if description == '':
            description = None
        try:
            self.insert ([_release, _user, className, memberName,
                kind, self.currentFile, anchor, description])
        except _sapdb.SQLError, sqlErr:
            if sqlErr.errorCode == 200:
                entry = self.entryString (className, memberName, kind)
                sys.stderr.write ('%s: Warning: Duplicate Entry: %s\n' % (
                    self.currentFile, entry))
            else:
                raise

    def entryString (self, className, memberName, kind):
        if kind == 'C':
            result = 'class ' + className
        elif kind == 'M':
            if className == '::':
                result = '::' + memberName
            else:
                result = className + '::' + memberName
        else:
            result = memberName
        return result

    def removeCurrentEntries (self):
        pass

    def save (self):
        self.endTransaction (1)

    def endTransaction (self, ok):
        if ok:
            self.session.commit ()
        else:
            self.session.rollback ()

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

_indexStatements = {
    'classSelect': """select classname, shortdesc, docfile, anchor
        from SymbolIndex
        where Release = ?
        and   author = ?
        and   kind = 'C'""",
    'anySelect': """select classname, symbolname, kind, shortdesc, docfile, anchor
        from SymbolIndex
        where Release = ?
        and   author = ?
        and   kind = ?""",
    'classSelectIndex': """select classname, shortdesc, docfile, anchor
        from SymbolIndex
        where Release = ?
        and   author = ?
        and   kind = 'C'
        and docfile in (
            select docfile from file2index where indexname = ?)""",
    'anySelectIndex': """select classname, symbolname, kind, shortdesc, docfile, anchor
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
            if description:
                description = description.read (1000)
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
            if description:
                description = description.read (1000)
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

