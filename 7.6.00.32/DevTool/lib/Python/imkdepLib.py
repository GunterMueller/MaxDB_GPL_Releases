
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

_version = 0.2

class DepUpdater:
    def __init__ (self, options = None, **keywArgs):
        if options:
            self.__dict__.update (options.__dict__)
        self.__dict__.update (keywArgs)
        self.vpath = VMakePath ()

    def updateDescription (self, descName):
        if self.verbose:
            print descName
        fname = self.deduceFilename (descName)
        source = open (fname, 'rt').read ()
        if not self.hasDependencySection (source):
            source = self.initDescription (source)
        else:
            source = self.cutDependencies (source)
        outstream = cStringIO.StringIO ()
        outstream.write (source)
        outstream.write (self.dependencySection)
        files = self.getFiles (fname)
        variants = self.getVariants (source)
        for file in files:
            try:
                dependencies = self.createDependencyList (file, variants)
                dependencies.toStream (outstream)
            except NotInSourceTree, exc:
                errorLine = "#%s: include source not found '%s'\n" % (
                    file [0], exc.fname)
                outstream.write (errorLine)
                if self.verbose:
                    print 'Error:', errorLine,
        outstream.write ('\n')
        open (fname, 'wt').write (outstream.getvalue ())
        if self.verbose:
            print

    def deduceFilename (self, descName):
        localFile = iget (descName)
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
        source = string.replace (source, '\\\n', '')
        lines = string.split (source ,'\n')
        lines = map (self.removeIncludeOpt, lines)
        source = string.join (lines, '\n')
        if source [-1] != '\n':
            source = source + '\n'
        return source

    def removeIncludeOpt (self, line):
        if not isCSource (line):
            return line
        pos = string.find (line, 'inc=')
        if pos == -1:
            return line
        match = re.search (r'\s', line [pos:])
        if match != None:
            start, stop = match.span ()
            result = line [:pos] + line [stop + pos:]
        else:
            result = string.strip (line [:pos])
        return result

    def cutDependencies (self, source):
        pos = string.index (source, self.dependencySection)
        result = source [:pos]
        return result

    def getFiles (self, descName):
        descDir = normcase (os.environ ['DESC'])
        descName = normcase (descName)
        assert startsWith (descName, descDir)
        descRelative = descName [len (descDir):]
        if descRelative [0] in "/\\":
            descRelative = descRelative [1:]
        base, ext = os.path.splitext (descRelative)
        path = 'sys/src/' + base
        files = self.vpath.listdir (path)
        files = filter (isCSourceTuple, files)
        files = filter (hasData, files)
        return files

    def getVariants (self, source):
        result = {}
        lines = string.split (source ,'\n')
        variantRE = re.compile ('([^-]+)-(.)[.](hpp|h|cpp|c)')
        for line in lines:
            match = variantRE.match (line)
            if match:
                base, variant, ext = match.group (1, 2, 3)
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
        dependencies = self.Dependencies (file)
        dependencies.buildIncludeDeps (self.vpath)
        shortName = file [0]
        if variants.has_key (shortName):
            dependencies.addVariants (variants [shortName])
        return dependencies

    class Dependencies:
        def __init__ (self, fileTuple):
            self.fname, self.fullName = fileTuple
            self.deplist = []
            self.variants = {}

        def addVariants (self, variants):
            for variant in variants:
                if not self.variants.has_key (variant):
                    self.variants [variant] = self.deplist [:]

        def writeVariant (self, stream, variant, deplist):
            maxLineLen = 72
            base, ext = splitext (self.fname)
            write = stream.write
            chunk = base + variant + ext + ' inc='
            write (chunk)
            linelen = len (chunk)
            sep = ''
            lastWasInclude = 1
            for dep in deplist:
                if dep [0] == '&':
                    if lastWasInclude:
                        write ('\\\n')
                    write (dep + '\n')
                    linelen = 0
                    lastWasInclude = None
                else:
                    chunk = sep + dep
                    if linelen + len (chunk) > maxLineLen:
                        write ('\\\n')
                        linelen = 0
                    write (chunk)
                    linelen = linelen + len (chunk)
                    sep = ','
                    lastWasInclude = 1
            write ('\n')

        def toStream (self, stream):
            self.writeVariant (stream, '', self.deplist)
            items = self.variants.items ()
            items.sort ()
            for variant, deplist in items:
                self.writeVariant (stream, '-' + variant, deplist)

        includeRE = re.compile (r'\s*#\s*include\s+"([^"]+)"(.*)')
        ppRE = re.compile (r'\s*#.*/[*]\s*(&.*)[*]/')

        def buildIncludeDeps (self, vpath):
            lines = open (self.fullName, 'rt').readlines ()
            self.deplist = []
            includeRE = self.includeRE
            ppRE = self.ppRE
            for line in lines:
                match = includeRE.match (line)
                if match:
                    fname, remainder = match.group (1, 2)
                    if string.find (remainder, 'no check') != -1:
                        # this is an external header
                        pass
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
                        self.addDependency (match.group (1))

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
            vlist = map (string.strip, vlist)
            self.addVariants (vlist)
            if additional:
                for variant in vlist:
                    deplist = self.variants [variant]
                    deplist.append (text)
            else:
                self.addDependency (text, vlist)



class VmakePathError:
    def __init__ (self, fname):
        self.fname = fname

class NotInSourceTree (VmakePathError):
    def __str__ (self):
        return '<Not in source Tree: %s>' % self.fname

class CantDeduceSource (VmakePathError):
    def __str__ (self):
        return '<Cannot deduce source: %s>' % self.fname

class VMakePath:
    def __init__ (self):
        self.path = string.split (os.environ ['VMAKE_PATH'], ',')
        self.src = os.environ ['SRC']
        if self.src [-1] not in '\\/':
            self.src = self.src + '/'
        self.sapdbMapping = None
        self.includeCache = {}

    def findFile (self, fname):
        for root in self.path:
            fullname = pathjoin (root, fname)
            if existsFile (fullname):
                return fullname
        return None

    def openReader (self, fname, mode = 't'):
        fullname = self.findFile (fname)
        if not fullname:
            fullname = pathjoin (self.path [0], fname)
        result = open (fullname, 'r' + mode)
        return result

    def fileData (self, fname, mode = 't'):
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
        if fname [:srcLen] == src:
            return fname [srcLen:]
        else:
            raise NotInSourceTree (fname)

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
                #print 'is mapped'
                result = ':SAPDB/' + mappedDir + '/' + fname
            else:
                #print 'unknown'
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

_csourceRE = re.compile ('[a-zA-Z0-9_-]+[.](h|hpp|c|cpp)')

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

def shellAndParse (cmd, regex, *groups):
    #rint 'shell', cmd
    data = os.popen (cmd, 'r').read ()
    if not data:
        #rint 'no data'
        return None
    match = regex.match (data)
    if not match:
        #rint 'no match in', '"' + data + '"'
        return None
    if len (groups) == 0:
        groups = (0,)
    matches = apply (match.group, tuple (groups))
    #rint 'matches:', matches
    return matches

_igetRE = re.compile (r'.*copied to (\S+)')

def iget (fname):
    match = shellAndParse ('iget -e 0 ' + fname, _igetRE, 1)
    if not match:
        match = localIview (fname)
    return match

_localiviewRE = re.compile ('([a-z]:[^:]+): local source', re.IGNORECASE)
_iviewCmd = '%(TOOLSHELL)s %(TOOL)s/bin/iview%(TOOLEXT)s -e 0 ' % os.environ

def localIview (fname):
    match = shellAndParse (_iviewCmd + fname, _localiviewRE, 1)
    return match

