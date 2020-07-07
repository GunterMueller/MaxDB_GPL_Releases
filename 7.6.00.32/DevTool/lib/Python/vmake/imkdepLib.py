
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
import cStringIO
import re
import os
pathjoin = os.path.join
existsFile = os.path.exists
isabsolute = os.path.isabs
basename = os.path.basename
splitext = os.path.splitext
getsize = os.path.getsize
normcase = os.path.normcase
import shutil

import dynamicModule

_revision = "$Change: 31128 $"
_version = 0.4
_useInames = 1

def getExternalXmlImports ():
    vmakePathVar = os.environ.get ('VMAKE_PATH')
    if vmakePathVar is None:
        return []
    vmakePath = filter (None, string.split (vmakePathVar, ','))

    def addSRC (dirname):
        return os.path.join (dirname, 'sys', 'src')

    sys.path [:0] = map (addSRC, vmakePath)

    try:
        import imkdepSpecials
    except ImportError:
        return []

    try:
        result = imkdepSpecials.externalXmlImports
        return result
    except AttributeError:
        return []

externalXmlImports = getExternalXmlImports ()

def noInames ():
    global _useInames
    _useInames = None

class DepUpdater:
    def __init__ (self, options = None, **keywArgs):
        if options:
            self.__dict__.update (options.__dict__)
        self.__dict__.update (keywArgs)
        self.vpath = VMakePath ()
        self.loadSpecialCases ()

    def loadSpecialCases (self):
        srcPath = self.vpath.join ('sys', 'src')
        module = dynamicModule.DynamicModule ('imkdepSpecials',
            srcPath + sys.path,
            specialDependencies = {})
        self.specialDependencies = module.specialDependencies

    def updateDescription (self, descName):
        if self.verbose:
            print descName
        fname = self.deduceFilename (descName)
        if existsFile (fname):
            source = open (fname, 'r').read ()
        else:
            source = ''
        if not self.hasDependencySection (source):
            if self.force:
                source = self.initDescription (source)
            else:
                raise NotAnImkdepDescription (descName)
        else:
            source = self.cutDependencies (source)
        outstream = cStringIO.StringIO ()
        outstream.write (source)
        outstream.write (self.dependencySection)
        files = self.getFiles (fname)
        variants = self.getVariants (source)
        concomErrors = []
        for file in files:
            try:
                dependencies = self.createDependencyList (file, variants)
                dependencies.toStream (outstream)
            except WrongConcom, exc:
                exc.formatError ()
                outstream.write ('? error: %s:%d: %s\n'
                    % (exc.fullname, exc.lineno, exc.errorMessage))
                concomErrors.append (exc)
            except NotInSourceTree, exc:
                errorLine = "#%s: include source not found '%s'\n" % (
                    file [0], exc.fname)
                outstream.write (errorLine)
                if self.verbose:
                    print 'Error:', errorLine,
        outstream.write ('\n')
        openForEdit (fname).write (outstream.getvalue ())
        if self.verbose:
            print
        if concomErrors:
            raise concomErrors [0]

    def deduceFilename (self, descName):
        localFile = checkOut (descName)
        assert localFile != None
        return localFile
       ## test implementation
       #localFile = pathjoin (os.environ ['DESC'], descName)
       #if existsFile (localFile):
       #    os.remove (localFile)
       #remoteFile = self.vpath.findFile ('sys/desc/' + descName)
       #assert remoteFile != None
       #shutil.copy (remoteFile, localFile)
       #return localFile

    dependencySection = """#
# include dependencies (do not edit)
#
"""
    def hasDependencySection (self, source):
        pos = string.find (source, self.dependencySection)
        return (pos != -1)

    def initDescription (self, source):
        #source = re.sub (r'\\' + '\n' + r'\s+', ' ', source)
        source = string.replace (source, '\\\n', '<br>')
        lines = string.split (source ,'\n')
        lines = map (self.removeIncludeOpt, lines)
        source = string.join (lines, '\n')
        if not source:
            source = '\n'
        elif source [-1] != '\n':
            source = source + '\n'
        source = string.replace (source, '<br>', '\\\n')
        return source

    def removeIncludeOpt (self, line):
        words = string.split (line)
        if len (words) == 0:
            return line
        if not isCSource ( words[0]):
            return line
        pos = string.find (line, 'inc=')
        if pos == -1:
            return line
        wsre = re.compile (r'\s+')
        match = wsre.search (line, pos)
        while 1:
            if match:
                start, stop = match.span ()
                if line [start -4:start] == '<br>':
                    # skip space at start of continuation lines
                    match = wsre.search (line, stop)
                else:
                    result = line [:pos] + line [start:]
                    break
            else:
                result = line [:pos]
                break
        return string.strip (result)

    def cutDependencies (self, source):
        pos = string.index (source, self.dependencySection)
        result = source [:pos]
        return result

    def getFiles (self, descName):
        if self.unifiedSys:
            unifiedSys = 1
        else:
            unifiedSys = pathStartsWith (descName, os.environ ['SRC'])
        if unifiedSys:
            subdir = 'sys/src'
        else:
            subdir = 'sys/desc'
        descRelative = self.vpath.getRelativeName (descName, subdir)
        if unifiedSys:
            path= 'sys/src/' + os.path.dirname (descRelative)
        else:
            base, ext = os.path.splitext (descRelative)
            path = 'sys/src/' + base
        files = self.vpath.listdir (path)
        files = filter (isCSourceTuple, files)
        files = filter (hasData, files)
        return files

    def getVariants (self, source):
        result = {}
        lines = string.split (source ,'\n')
        variantRE = re.compile (r'(?P<base>[^-]+)(?P<variant>[*]|(-[^.]+))[.](?P<ext>hpp|h|cpp|c)\s?')
        for line in lines:
            match = variantRE.match (line)
            if match:
                base, variant, ext = match.group ('base', 'variant', 'ext')
                shortName = base + '.' + ext
                try:
                    variants = result [shortName]
                    variants.append (variant)
                except KeyError:
                    variants = [variant]
                    result [shortName] = variants
        return result

    def createDependencyList (self, file, variants):
        if self.verbose:
            sys.stdout.write ('.')
        shortName = file [0]
        ext = splitext (shortName) [1]
        if ext == '.xml':
            dependencies = self.XMLDependencies (file)
            dependencies.buildIncludeDeps (self.vpath, self.forAPO)
        else:
            if (ext == '.cpp') and getattr (self, 'except'):
                moreOptions = ' -except '
            else:
                moreOptions = ''
            dependencies = self.Dependencies (file, moreOptions,
                self.specialDependencies)
            dependencies.forAPO = self.forAPO
            dependencies.buildIncludeDeps (self.vpath)
            if variants.has_key (shortName):
                dependencies.addVariants (variants [shortName])
        return dependencies

    class Dependencies:
        forAPO = None
        def __init__ (self, fileTuple, moreOptions = '',
                specialDependencies = {}):
            self.fname, self.fullName = fileTuple
            self.deplist = []
            self.variants = {}
            self.moreOptions = moreOptions
            self.specialDependencies = specialDependencies

        def addVariants (self, variants):
            for variant in variants:
                if not self.variants.has_key (variant):
                    self.variants [variant] = self.deplist [:]

        def writeVariant (self, stream, variant, deplist):
            base, ext = splitext (self.fname)
            out = LineBreaker (stream.write, 72, '    ')
            out.write (base + variant + ext + self.moreOptions)
            fileDeps = self.extractFileDependencies (deplist)
            if fileDeps:
                lastWasDep = 1
                delimiter = ' dep=+'
                for filedep in fileDeps:
                    if filedep [0] == '&':
                        if lastWasDep:
                            out.writeLine (' \\')
                        out.writeLine (filedep)
                        lastWasDep = None
                        delimiter=' dep=+'
                    else:
                        out.writeNobreak (delimiter)
                        out.write (filedep)
                        delimiter = ','
                        lastWasDep = 1
                if lastWasDep:
                    out.writeLine (' \\')
            out.writeNobreak (' inc=')
            fileIncs = self.extractIncDependencies (deplist)
            if fileIncs:
                delimiter = ''
                lastWasInc = 1
                for fileinc in fileIncs:
                    if fileinc [0] == '&':
                        if lastWasInc:
                            out.writeLine ('\\')
                        out.writeLine (fileinc)
                        lastWasInc = None
                    else:
                        out.writeNobreak (delimiter)
                        out.write (fileinc)
                        delimiter = ','
                        lastWasInc = 1
            out.writeLine ('')

        def extractFileDependencies (self, deplist):
            result = []
            no_dep = 1
            for filedep in deplist:
                if filedep [0] == '&':
                    result.append (filedep)
                if filedep [:4] == 'dep=':
                    result.append (filedep [4:])
                    no_dep = None
            if no_dep:
                result = []
            return result

        def extractIncDependencies (self, deplist):
            result = []
            no_inc = 1
            for fileinc in deplist:
                if fileinc [0] == '&':
                    result.append (fileinc)         		
            	elif fileinc [:4] == 'dep=':
                    # already handled above
                    pass
                else:
                    result.append (fileinc)
                    no_inc = None
            if no_inc:
                result = []
            return result

        def toStream (self, stream):
            if self.forAPO:
                self.writeVariant (stream, '*', self.deplist)
            else:
                self.writeVariant (stream, '', self.deplist)
            items = self.variants.items ()
            items.sort ()
            for variant, deplist in items:
                self.writeVariant (stream, variant, deplist)

        includeRE = re.compile (r'\s*#\s*include\s+"([^"]+)"(.*)')
        importRE = re.compile (r'\s*import\s+"([^"]+)"\s*;(.*)')
        quoteIncludeRE = re.compile (r'\s*cpp_quote\s*[(]\s*"#include\s+\\"([^\\]+)\\"(.*)"\s*[)]')
        ppRE = re.compile (r'\s*#.*/[*]\s*(&.*)[*]/')

        def buildIncludeDeps (self, vpath):
            # print ('check file: ' + self.fullName + '... \n')
            lines = open (self.fullName, 'r').readlines ()
            self.deplist = []
            if splitext (self.fullName) [1] == '.idl':
                includeREs = [self.importRE, self.quoteIncludeRE, self.includeRE]
            else:
                includeREs = [self.includeRE]
            ppRE = self.ppRE
            concomStack = []
            lineno = 0
            for line in lines:
                lineno = lineno + 1
                # print line,
                for rex in includeREs:
                    match = rex.match (line)
                    if match:
                        break
                if match:
                    fname, remainder = match.group (1, 2)
                    if self.specialDependencies.has_key (fname):
                        replacement = self.specialDependencies [fname]
                        if replacement [:4] == 'inc=':
                            self.addDependency (replacement [4:])
                        elif replacement [:4] == 'dep=':
                            self.addDependency (replacement)
                        else:
                            self.moreOptions = (self.moreOptions
                                + ' ' + replacement)
                    elif isNocheck (remainder):
                        if string.find (remainder, 'but_imkdep') != -1:
                            # maxdb header, but nocheck as vmake doesn't
                            # scan conditional compiling
                            include = vpath.findInclude (fname)
                            self.addDependency (include)
                        else:
                            # this is an external header
                            pass
                    elif string.find (remainder, 'dep=') != -1:
                        depFile = getCommentContent (remainder)
                        depFile = depFile [4:] # remove dep=
                        fullName = vpath.findInclude (depFile)
                        self.addDependency ('dep=' + fullName)
                    else:
                        variantPos = string.find (remainder, '&variant')
                        if variantPos != -1:
                            dataEnd = string.find (remainder, '*/')
                            if dataEnd == -1:
                                dataEnd = len (remainder)
                            dataStart = variantPos + len ('&variant')
                            variantData = remainder [dataStart:dataEnd]
                            self.addVariantDependency (fname, variantData)
                        else:
                            include = vpath.findInclude (fname)
                            self.addDependency (include)
                else:
                    match = ppRE.match (line)
                    if match:
                        concom = match.group (1)
                        keyword = string.split (concom) [0]
                        if keyword in ['&elif', '&endif']:
                            if len (concomStack) == 0:
                                raise MissingIf (self.fullName, lineno, concom)
                            concomStack.pop ()
                        if keyword in ['&ifdef', '&if', '&elif']:
                            concomStack.append ((lineno, concom))
                        self.addDependency (concom)
            if len (concomStack) > 0:
                lineno, concom = concomStack [-1]
                raise MissingEndif (self.fullName, lineno, concom)

        def addDependency (self, text, exclude = []):
            self.deplist.append (text)
            for variant, deplist in self.variants.items ():
                if variant not in exclude:
                    deplist.append (text)

        def addVariantDependency (self, text, variantData):
            variantData = string.strip (variantData)
            if variantData [0] == '-':
                additional = None
            elif variantData [0] == '+':
                additional = 1
            else:
                raise 'Variants clause must start with + or -'
            vlist = string.split (variantData [1:], ',')
            vlist = map (lambda entry: '-' + string.strip (entry), vlist)
            self.addVariants (vlist)
            if additional:
                for variant in vlist:
                    deplist = self.variants [variant]
                    deplist.append (text)
            else:
                self.addDependency (text, vlist)


    class XMLDependencies (Dependencies):
        importRE = re.compile (r'<(IMPORT|INCLUDE)\s+FILE="([^"]+)"')
        def buildIncludeDeps (self, vpath, forAPO = None):
            if not forAPO:
                return
            stream = open (self.fullName, 'r')
            try:
                line = stream.readline ()
                while line:
                    match = self.importRE.search (line)
                    if match:
                        incl = match.group (2)
                        if incl not in externalXmlImports:
                            self.addDependency ('dep=:' + incl)
                    line = stream.readline ()
            finally:
                stream.close ()

        def isIDL2XML (self, stream):
            line = stream.readline ()
            while line:
                if string.find (line, '<IDL2XML ') != -1:
                    return 1
                line = stream.readline ()
            return None

def isNocheck (text):
    if string.find (text, 'nocheck') != -1:
        return 1
    if string.find (text, 'no check') != -1:
        return 1
    return None


class VmakePathError:
    def __init__ (self, fname):
        self.fname = fname

class NotInSourceTree (VmakePathError):
    def __str__ (self):
        return '<Not in source Tree: %s>' % self.fname

class CantDeduceSource (VmakePathError):
    def __str__ (self):
        return '<Cannot deduce source: %s>' % self.fname

class WrongConcom:
    def __init__ (self, fullname, lineno, concom):
        self.fullname = os.path.normpath (fullname)
        self.lineno = lineno
        self.concom = concom

    def formatError (self, stream = sys.stdout):
        stream.write ('ERROR: %s:%d: %s\n'
            % (self.fullname, self.lineno, self.errorMessage))

class MissingEndif (WrongConcom):
    errorMessage = '&endif missing'

class MissingIf (WrongConcom):
    errorMessage = '&if missing'

class NotAnImkdepDescription:
    def __init__ (self, descName):
        self.descName = descName


def normpath (fname):
    return os.path.normpath (os.path.normcase (fname))

class VMakePath:
    def __init__ (self):
        self.path = string.split (os.environ ['VMAKE_PATH'], ',')
        self.normpath = map (normpath, self.path)
        self.src = os.environ ['SRC']
        if self.src [-1] not in '\\/':
            self.src = self.src + '/'
        self.sapdbMapping = None
        self.includeCache = {}

    def findFile (self, fname):
        for root in self.path:
            fullName = pathjoin (root, fname)
            if existsFile (fullName):
                return fullName
        return None

    def openReader (self, fname, mode = ''):
        fullName = self.findFile (fname)
        if not fullName:
            fullName = pathjoin (self.path [0], fname)
        result = open (fullName, 'r' + mode)
        return result

    def fileData (self, fname, mode = ''):
        stream = self.openReader (fname, mode)
        data = stream.read ()
        stream.close ()
        return data

    vdnRE = re.compile ('[gvh]([a-z][a-z])[0-9]+')

    def findSource (self, fname):
        # check for absolute path
        srcName = self.makeRelativeSource (fname)
        if srcName:
            return srcName
        # check for src-absolute path
        if fname [0] == ':':
            return fname [1:]
        # check for relative path
        if isRelative (fname):
            return fname
        # check for traditional vdn pattern
        match = self.vdnRE.match (fname)
        if match:
            srcName = 'sys/src/' + fname [1:3] + '/' + fname
            return srcName
        # check for SAPDB source
        mappedDir = self.getMappedDirectory (fname)
        if mappedDir:
            srcName = pathjoin ('sys/src/SAPDB', mappedDir, fname)
            return srcName
        # give up
        raise CantDeduceSource (fname)

    def makeRelativeSource (self, fname):
        if not isabsolute (fname):
            return None
        src = self.src
        srcLen = len (src)
        if normcase (fname [:srcLen]) == normcase (src):
            result = fname [srcLen:]
            if result [0] == os.sep:
                result = result [1:]
            return result
        else:
            raise NotInSourceTree (fname)

    def getRelativeName (self, fullname, subdir):
        fullname = normpath (fullname)
        subdir = normpath (subdir)
        for path in self.normpath:
            testpath = os.path.join (path, subdir)
            testlen = len (testpath)
            if fullname [:testlen] == testpath:
                return fullname [testlen+1:]
        raise NotInSourceTree (fullname)

    def getMappedDirectory (self, fname):
        prefix = mappingPrefix (fname)
        if not prefix:
            return None
        mapping = self.getSAPDBMapping ()
        try:
            result = mapping [prefix]
        except KeyError:
            result = None
        return result

    def getSAPDBMapping (self):
        if self.sapdbMapping != None:
            return self.sapdbMapping
        result = {}
        self.sapdbMapping = result
        try:
            data = self.fileData ('sys/desc/DirectoryMapping')
        except IOError:
            return result
        mappingRE = re.compile ('([^,]+), *([^ ]+)')
        lines = string.split (data, '\n')
        for line in lines:
            match = mappingRE.match (line)
            if match:
                short, dir = match.group (1, 2)
                result [short] = dir
        return result

    def findInclude (self, fname):
        try:
            return self.includeCache [fname]
        except KeyError:
            pass
        #print 'checking', fname,
        match = self.vdnRE.match (fname)
        if match:
            #print 'found vdn match'
            fullName = 'sys/src/' + match.group (1) + '/' + fname
            if self.findFile (fullName):
                result = fname
            else:
                # possible pascal source
                fullPasname = splitext (fullName) [0]
                if not self.findFile (fullPasname):
                    raise NotInSourceTree (fname)
                result = splitext (fname) [0]
        elif isRelative (fname):
            #print 'is relative'
            if startsWith (fname, 'SAPDB/'):
                result = ':' + fname
            else:
                if self.findFile ('sys/src/SAPDB/' + fname):
                    result = ':SAPDB/' + fname
                else:
                    result = ':' + fname
        else:
            mappedDir = self.getMappedDirectory (fname)
            if mappedDir:
                # print 'is mapped'
                result = ':SAPDB/' + mappedDir + '/' + fname
            else:
                # print 'unknown', fname
                result = fname
        self.includeCache [fname] = result
        return result

    def listdir (self, dirname):
        dict = {}
        for root in self.path:
            fullDir = pathjoin (root, dirname)
            try:
                list = os.listdir (fullDir)
            except OSError:
                continue
            for entry in list:
                if not dict.has_key (entry):
                    fullName = pathjoin (fullDir, entry)
                    dict [entry] = fullName
        result = dict.items ()
        result.sort ()
        return result

    def join (self, *parts):
        result = []
        for path in self.path:
            result.append (apply (os.path.join, (path,) + parts))
        return result


class LineBreaker:
    def __init__ (self, write, lineLength, indent = ''):
        self.writeFunc = write
        self.lineLength = lineLength
        self.indent = indent
        self.line = ''

    def write (self, text):
        if len (self.line) + len (text) > self.lineLength:
            self.writeFunc (self.line + '\\\n')
            self.line = self.indent + text
        else:
            self.line = self.line + text

    def writeNobreak (self, text):
        self.line = self.line + text

    def writeLine (self, text):
        self.writeFunc (self.line + text + '\n')
        self.line = ''


def startsWith (str, prefix):
    preflen = len (prefix)
    if str [:preflen] == prefix:
        return 1
    else:
        return None

def mappingPrefix (fname):
    pos = string.find (fname , '_')
    if pos > 0:
        result = fname [:pos]
    else:
        result = None
    return result

_csourceRE = re.compile ('[a-zA-Z0-9_-]+[.](h|hpp|c|cpp|idl|rc|xml)$')

def isCSource (text):
    match = _csourceRE.match (text)
    return match != None

def isCSourceTuple (nameTuple):
    return isCSource (nameTuple [0])

def hasData (nameTuple):
    if nameTuple [0] [0] == '_':
        # a backup file
        return None
    if getsize (nameTuple [1]) == 0:
        # a deleted file
        return None
    return 1

def isRelative (fname):
    if ((string.find (fname, '/') != -1)
            or (string.find (fname, '\\') != -1)):
        return 1
    else:
        return None

def openForEdit (fname):
    try:
        result = open (fname, 'w')
    except IOError:
        os.system ('p4 edit ' + fname)
        result = open (fname, 'w')
    return result

def shellAndParse (cmd, rex, *groups):
    #rint 'shell', cmd
    data = os.popen (cmd, 'r').read ()
    if not data:
        #rint 'no data'
        return None
    match = rex.match (data)
    if not match:
        print 'no match in', '"' + data + '"'
        return None
    if len (groups) == 0:
        groups = (0,)
    matches = apply (match.group, tuple (groups))
    #rint 'matches:', matches
    return matches

def getCommentContent (text):
    text = string.strip (text)
    if text [:2] == '//':
        return string.strip (text [2:])
    elif (text [:2] == '/*') and (text [-2:] == '*/'):
        return string.strip (text [2:-2])
    else:
        return text


_igetRE = re.compile (r'.*copied to (\S+)')

def iget (fname):
    if not _useInames:
        return os.path.abspath (fname)
    match = shellAndParse ('iget -e 0 ' + fname, _igetRE, 1)
    if not match:
        match = localIview (fname)
    return match


_localiviewRE = re.compile ('([a-z]:[^:]+): local source', re.IGNORECASE)
_iviewCmd = '%(TOOLSHELL)s %(TOOL)s/bin/iview%(TOOLEXT)s -e 0 ' % os.environ

def localIview (fname):
    match = shellAndParse (_iviewCmd + fname, _localiviewRE, 1)
    return match

_pgetCmd = '%(TOOLSHELL)s %(TOOL)s/bin/pget%(TOOLEXT)s -e 0 ' % os.environ
_pgetRE = re.compile (r'(?:.*checked out exclusive to (\S+))|(?:.*/sys/(.*) already locked)')

def pget (fname):
    if not _useInames:
        return os.path.abspath (fname)
    if os.path.exists (fname):
        return os.path.abspath (fname)
    match = shellAndParse (_pgetCmd + fname, _pgetRE, 1, 2)
    if match [0]:
        result = match [0]
    elif match [1]:
        result = os.path.join (os.environ ['OWN'], 'sys', match [1])
    else:
        result = None
    return result

if os.environ.has_key ('DEPOT'):
    checkOut = pget
else:
    checkOut = iget

def pathStartsWith (path, startpath):
    path = os.path.normpath (os.path.normcase (path))
    startpath = os.path.normpath (os.path.normcase (startpath))
    return path [:len (startpath)] == startpath

_directoryMapping = None

def getDirectoryMapping ():
    global _directoryMapping
    if _directoryMapping != None:
        return _directoryMappingMapping
    result = _directoryMapping = {}
    try:
        mappingFile = os.path.join (os.environ ['OWN'], 'sys/desc/DirectoryMapping')
        lines = open (mappingFile, 'r').readlines ()
    except IOError:
        return result
    mappingRE = re.compile ('([^,]+), *([^ ]+)')
    for line in lines:
        match = mappingRE.match (line)
        if match:
            short, dir = match.group (1, 2)
            result [short] = dir
    return result


