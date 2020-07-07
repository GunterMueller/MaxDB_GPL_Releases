#
# ptocprotoLb.py            support for ptocProto and ptocTccx
#
# Author: Daniel Dittmar


import string
import re
import time
import os
import stat
import cPickle
import md5
import StringIO

import fileutil
from vmake import yutil, vdnStream
from pasScanner import *
from pasParser import *
import parserUtil
from ptoc.ptoc00Lib import MultiFileStream

__version = 1.09

_cacheFormatVersion = "1.5"

_bufferTypes = ['tsp_moveobj', 'tsp_buf','tsp00_MoveObj','tsp00_Buf']

_defaultKind = 'C++'

class PtocProto:
    def __init__ (self, refKind = 'C++', iget = 1, iput = None):
        self.loadCache ()
        self.translateRefType (refKind)
        self.refOption = refKind
        self.iget = iget
        self.iput = iput

    def translateRefType (self, refKind):
        self.refKind = string.strip (refKind)
        refKind = string.lower (self.refKind)
        if refKind == 'c++':
            valueResult = '   '
            refResult = arrayRefResult = ' & '
        elif refKind == 'both':
            valueResult = '               '
            refResult = ' VAR_VALUE_REF '
            arrayRefResult = ' VAR_ARRAY_REF '
        elif refKind == 'pointer':
            valueResult = '   '
            refResult = ' * '
            arrayRefResult = '   '
        elif refKind == 'auto':
            valueResult = None
            refResult = None
            arrayRefResult = None
        else:
            raise "Invalid value '%s' for -R, must be 'C++', 'Both', 'Pointer' or 'Auto'" % refKind
        self.valueType = valueResult
        self.refType = refResult
        self.arrayRefType = arrayRefResult

    def convert (self, module, outdir = '.', verbose = None,
            unconditional = None):
        self.verbose = verbose
        self.outdir = outdir
        headerfile = yutil.findFileExt ('h' + module [1:], ['.hpp', '.h'])
        if self.refOption == 'Auto':
            self.detectCurrentMode (headerfile)
        source = yutil.openFile (module).read ()
        self.topic = '<unknown topic>'
        self.author = '<unknown>'
        headerMatch = headerRE.search (source)
        if headerMatch:
            self.topic, self.author = headerMatch.group (2, 1)
        else:
            moduleMatch = moduleRE.search (source)
            if moduleMatch:
                self.topic = string.strip (match.group (1))
            authorMatch = authorRE.search (source)
            if authorMatch:
                self.author = string.strip (authorMatch.group (1))
        self.includes = self.getIncludes (module)
        exportInfo = getExportInfo (module, headerfile, source)
        licence = self.extractLicence (source)
        iputNames = []
        for headerName, declarations, digest in exportInfo:
            if not unconditional:
                lastDigest = extractDigest (headerName)
                if digest == lastDigest:
                    continue
            print headerName
            iputNames.append (headerName)
            (self.outstream, self.currentIncludes) = (StringIO.StringIO (), {})
            self.genPrototypes (declarations)
            self.genOutfile (headerName, digest, licence,
                self.outstream, self.currentIncludes)
        if self.iput:
            if os.environ.get ('DEPOT'):
                print iputNames
                safeShell ('perl -d -S p4put.pl -d"implicit submit by ptocproto" ' + string.join (iputNames))
            else :
                safeShell ('iput ' + string.join (iputNames))
        self.verbose = None

    def extractLicence (self, source):
        licpos = string.find (source, '========== licence begin')
        if licpos == -1:
            return ''
        startpos = string.rfind (source, '\n', 0, licpos)
        endpos = string.find (source, '========== licence end', licpos)
        endpos = string.find (source, '\n', endpos)
        return source [startpos:endpos]


    def detectCurrentMode (self, headerName):
        source = open (headerName).read ()
        match = re.search ('RefType: +([^ ]+)', source)
        if match:
            refKind = match.group (1)
        else:
            refKind = _defaultKind
        self.translateRefType (refKind)

    def genOutfile (self, headerName, digest, licence, prototypeStream, includesDict):
        outstream = self.openHeader (headerName, digest, licence)
        includes = self.calculateMinimalIncludes (includesDict)
        self.writeIncludes (outstream, includes)
        outstream.write ("""

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

""")
        outstream.write (prototypeStream.getvalue ())
        self.closeOutstream (outstream)

    def calculateMinimalIncludes (self, includesDict):
        required = includesDict.keys ()
        result = []
        vmakeIncludes = self.includes
        for incl in vmakeIncludes:
            if incl in required:
                result.append (incl)
        for incl in required:
            if not (incl in vmakeIncludes):
                result.append (incl)
        return result

    def openHeader (self, hmodule, digest, licence):
        if self.iget:
            if os.environ.get ('DEPOT'):
                safeShell ('pget -m -e 0 ' + hmodule)
            else :
                safeShell ('iget -e 0 ' + hmodule)
        outstream = open (self.outdir + '/' + hmodule, "w")
        self.writeHeader (outstream, hmodule, licence)
        self.writeDigest (outstream, digest)
        return outstream

    def writeHeader (self, outstream, hmodule, licence):
        prep = string.replace (string.upper (hmodule), '.', '_')
        outstream.write (headerComment
            % (hmodule, self.author, self.topic, licence))
        outstream.write ("""
#ifndef %s
#define %s

""" % (prep, prep))

    def closeOutstream (self, outstream):
        outstream.write ("\n#endif\n\n")
        outstream.close ()

    def writeIncludes (self, outstream, includes):
        outstream.write ("""

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

""")
        for i in self.streamlineIncludes (includes):
            outstream.write ('#include "%s"\n' % i)

    def streamlineIncludes (self, includes):
        #
        # make sure that externPascal declaration is available
        #
        if len (includes) == 0:
            return ['RunTime/RTE_CompilerFeatures.h']
        result = []
        hasPascal = None
        #
        # add .h for pascal includes
        #
        for include in includes:
            if os.path.splitext (include) [1] == '':
                result.append (include + '.h')
                hasPascal = 1
            else:
                result.append (include)
        #
        # remove gsp00_2.h (added for pasbool)
        #    if another pascal include is used
        #
        if hasPascal:
            try:
                del result [result.index ('gsp00_2.h')]
            except ValueError:
                pass
        return result

    def getIncludes (self, module):
        if self.verbose:
            sys.stdout.write ('imf -UIn %s ... ' % module)
        lines = os.popen ("perl -S imf.pl -UIn " + module).readlines ()
        if self.verbose:
            sys.stdout.write ('finished\n')
        includes = []
        hasErrors = None
        for line in lines:
            if line [:5] == 'ERROR':
                print string.strip (line)
                hasErrors = 1
            include = getModule (line)
            if (include != None) and (include != module):
                self.parseInclude (include)
                includes.append (include)
        if hasErrors:
            raise 'files missing, please p4 sync first'
        return includes

    def writeDigest (self, outstream, digest):
        hexStrings = map (lambda ch: r'\x%02x' % ord (ch), digest)
        digestString = string.join (hexStrings, '')
        outstream.write ("""
/*===========================================================================*
 *     RefType: %-30s                               *
 *===========================================================================*/
""" % self.refKind)
        outstream.write ("""
/*===========================================================================*
 *     MD5: '%s'
 *===========================================================================*/
""" % digestString)

    def genPrototypes (self, part):
        outstream = self.outstream
        concom = TokenBoundConcom ()
        memStream = VDNMemoryStream (part, concom.handleLine)
        scanner = Scanner (memStream)
        scanner.keepComments = 1
        concom.attachScanner (scanner)
        try:
            while 1:
                token = scanner.next ()
                if token.id != keywordID:
                    raise TokenExpected (token, ['VAR', 'PROCEDURE', 'FUNCTION'])
                self.handleInlineC (token, outstream)
                handleConcom (token, outstream)
                if token.value == 'var':
                    self.genVar (outstream, scanner, token)
                elif token.value == 'procedure':
                    self.genProcedure (outstream, scanner)
                elif token.value == 'function':
                    self.genFunction (outstream, scanner)
                else:
                    raise TokenExpected (token, ['VAR', 'PROCEDURE', 'FUNCTION'])
                outstream.write ("""/* Don't edit this file by hand, it is generated by ptocProto.py */

""")

        except EOFError:
            pass
        handleConComLines (scanner.concomLine, outstream)

    def genVar (self, outstream, scanner, token):
        while 1:
            declaration = parserUtil.readUntil (scanner, operatorID, ';')
            if len (declaration) > 4:   # name : type ;
                varname = declaration [0].value
                outstream.write ('/* var ' + varname
                    + ' not generated, type too complex */\n')
            else:
                outstream.write ('externPascal %s;\n' % self.pasVar2cVar (declaration))
            token = scanner.next ()
            scanner.pushback (token)
            if token.value in ['var', 'procedure', 'function']:
                break
        outstream.write ('\n')

    def genProcedure (self, outstream, scanner):
        nameToken = scanner.next ()
        outstream.write ('externPascal void %s (' % nameToken.value)
        allParams = self.genParamString (scanner)
        outstream.write (allParams + ');\n\n')
        scanner.next () # skip ;

    def genFunction (self, outstream, scanner):
        nameToken = scanner.next ()
        allParams = self.genParamString (scanner)
        scanner.next ()
        returnToken = scanner.next ()
        returntype = returnToken.value
        typetag = self.getTypetag (returntype)
        if typetag == 'enum':
            returntype = returntype + '_Param'
        outstream.write ('externPascal %s %s (' % (returntype, nameToken.value))
        self.requireInclude (returnToken.value)
        outstream.write (allParams + ');\n\n')
        scanner.next () # skip ;

    def handleInlineC (self, token, outstream):
        comments = token.getAttribute ('preComment')
        if not comments:
            return None
        for comment in comments:
            words = string.split (comment)
            if words [0] == 'inlineC':
                outstream.write (string.join (words [1:]) + '\n')
        outstream.write ('\n')

    def genParamString (self, scanner):
        nextToken = scanner.next ()
        if nextToken.value == '(':
            paramTokens = parserUtil.readUntil (scanner, operatorID, ')') [:-1]
        else:
            paramTokens = []
            scanner.pushback (nextToken) # ; is consumed by next step
        if len (paramTokens) > 0:
            params = parserUtil.splitTokens (paramTokens, operatorID, ';')
            paramStrings = []
            for p in params:
                paramStrings.append (self.pasParam2cParam (p))
            allParams = '\n    ' + string.join (paramStrings, ',\n    ')
        else:
            allParams = ''
        return allParams

    def pasVar2cVar (self, tokens):
        nameToken = tokens [0]
        typeToken = tokens [2]
        self.requireInclude (typeToken.value)
        return '%s %s' % (typeToken.value, nameToken.value)

    def pasParam2cParam (self, tokens):
        if tokens [0].value == 'var':
            ref = self.refType
            nameToken = tokens [1]
            typeToken = tokens [3]
        else:
            ref = self.valueType
            nameToken = tokens [0]
            typeToken = tokens [2]
        typetag = self.getTypetag (typeToken.value)
        typeSynonym = getSpecialComment (typeToken, 'postComment', 'ptocSynonym')
        if typeSynonym != None:
            ref = self.valueType
            fname, typename = parseTypeSynonym (typeSynonym)
            if fname:
                self.currentIncludes [fname] = 1  # require this include
            typetag = 'scalar'    # so wie don't pass this by ref
        else:
            typename = typeToken.value
            self.requireInclude (typename)
            isConst = getCommentFlag (typeToken, 'postComment', 'ptocConst')
            if isConst:
                typename = 'const ' + typename
        if typename in _bufferTypes:
            typename = 'void'
            ref = ' * '
        elif (typetag == 'enum') and (ref == self.valueType):
            typename = typename + '_Param'
        elif (typetag == 'set') or (typetag == 'array'):
            ref = self.arrayRefType
        return '%-20s %s %s' % (typename, ref, nameToken.value)

    def typeTokenSynonym (self, typeToken):
        comment = token.getAttribute ('postComment')
        if not comment:
            return None
        return result

    def loadCache (self):
        try:
            fname = os.environ ['TMP'] + '/proto.cache'
            t = cPickle.load (open (fname, 'r'))
            if t[0] == _cacheFormatVersion:
                version, self.includeTimes, self.typedefs = t
            else:
                self.initDefaults ()
        except IOError:
            self.initDefaults ()

    def initDefaults (self):
        self.includeTimes = {}
        self.typedefs = {'pasbool': ('skalar', 'gsp00_2.h')}

    def saveCache (self):
        t = _cacheFormatVersion, self.includeTimes, self.typedefs
        fname = os.environ ['TMP'] + '/proto.cache'
        cPickle.dump (t, open (fname, 'w'))

    def parseInclude (self, module):
        #expanded = yutil.findFile (module)
        expandedList = self.expandInclude (module)
        expandedList = filter (os.path.exists, expandedList)
        if len (expandedList) == 0:
            raise NeedImfException (module)
        parsingNecessary = None
        for expanded in expandedList:
            lastModified = self.getChangedTime (expanded)
            try:
                cacheTime = self.includeTimes [module]
                parsingNecessary = (cacheTime < lastModified)
            except KeyError:
                parsingNecessary = 1
        if parsingNecessary:
            if self.verbose:
                sys.stdout.write ('Parsing %s ... ' % module)
            self.parsePascal00 (expandedList)
            if self.verbose:
                sys.stdout.write ('finished\n')
            self.includeTimes [module] = lastModified

    def parsePascal00 (self, includes):
        #instream = vdnStream.VDNStream (module, ignoreConcom)
        #instream.openPart ('code')
        instream = TccMultiFileStream (includes)
        scanner = Scanner (instream)
        scanner.keepComments = None
        self.scanner = scanner
        try:
            while 1:
                token = scanner.next ()
                if token.id != keywordID:
                    raise TokenExpected (token, ['CONST', 'TYPE'])
                    raise SyntaxError, (token, token.value, 'CONST or TYPE expected')
                if token.value == 'const':
                    readConstPart (scanner, self.handleConstantDecl)
                elif token.value == 'type':
                    readTypePart (scanner, self.handleTypeDecl)
                else:
                    raise TokenExpected (token, ['CONST', 'TYPE'])
                    raise SyntaxError, (token, token.value, 'CONST or TYPE expected')
        except EOFError:
            pass
        del self.scanner
        self.saveCache ()

    def expandInclude (self, module):
        wrkIncl = os.environ ['WRK'] + '/incl/'
        result = [wrkIncl + module + '.con', wrkIncl + module + '.typ']
        return result

    def getChangedTime (self, include):
        result = os.stat (include)[stat.ST_MTIME]
        return result

    def handleConstantDecl (self, tokens):
        pass

    def handleTypeDecl (self, startToken, type):
        typename = startToken.value
        typetag = 'scalar'
        metaclass = type.__class__
        if metaclass == EnumerationDcl:
            typetag = 'enum'
        elif metaclass == SetDcl:
            typetag = 'set'
        elif metaclass == ArrayDcl:
            typetag = 'array'
        elif metaclass == DerivedDcl:
            basetype = type.typename ()
            typetag = self.getTypetag (basetype)
        fname = self.scanner.stream.basename
        self.typedefs [typename] = (typetag, fname)

    def getTypetag (self, typename):
        try:
            typetag, fname = self.typedefs [typename]
        except KeyError:
            typetag = 'scalar'
        return typetag

    def requireInclude (self, typename):
        try:
            typetag, fname = self.typedefs [typename]
            self.currentIncludes [fname] = 1
        except KeyError:
            pass

class PtocTcc:
    def __init__ (self, protocolDir, nostrict):
        self.protocolDir = protocolDir
        self.strict = not nostrict
        self.hasErrors = None
        self.errorFile = None

    def checkModule (self, module):
        if module [0] == 'g':
            # globals are not checked
            return
        exportInfo = getExportInfo (module)
        for headerName, declarations, digest in exportInfo:
            matchObj = extractDigest (headerName, module, self.setInfo)
            if matchObj == digest:
                self.setInfo (module, headerName, 'Header matches MD5 digest of Pascal Defined part')
            elif matchObj == None:
                pass
                # self.setInfo (module, headerName, 'Header not generated by ptocProto')
            else:
                self.nonMatchingHeader (module, headerName) # , digest, matchObj)
        self.closeErrorFile ()
        return self.hasErrors

    def getErrorFile (self, module):
        errorFile = self.errorFile
        if errorFile == None:
            errorFile = open (os.path.join (self.protocolDir, module + '.check'), 'a')
            self.errorFile = errorFile
        return errorFile

    def closeErrorFile (self):
        errorFile = self.errorFile
        if errorFile != None:
            errorFile.write (
            '***************************** REAL END OF ERRORFILE ***************\n')
            errorFile.close ()
        self.errorFile = None

    def setError (self, module, header, msg):
        self.hasErrors = 1
        self.setMsg (module, header, 'ERROR', msg)

    def invalidHeader (self, module, header):
        msg = """Header must contain MD5 line
     Generate header using ptocProto or insert /* MD5: None */ into header"""
        self.setError (module, header, msg)

    def nonMatchingHeader (self, module, header, digestPascal = None, digestHeader = None):
        msg = """MD5 digest in header does not match Pascal Source
     Regenerate header using ptocProto"""
        if (digestPascal != None) and (digestHeader != None):
            msg = msg + (" Pascal: '%s', Header: '%s'" % (digestPascal, digestHeader))
        self.setError (module, header, msg)

    def setInfo (self, module, header, msg):
        self.setMsg (module, header, 'INFO', msg)

    def setMsg (self, module, header, kind, msg):
        errorFile = self.getErrorFile (module)
        errorFile.write ("""
     %s FOR %s
     %s

""" % (kind, header, msg))

_includeRE = re.compile ('[vg][a-z][a-z][0-9]+ *$')

def getModule (line):
    match = _includeRE.match (line)
    if match:
        result = string.strip (line)
    else:
        result = None
    return result

headerRE = re.compile ('.tt 3 [$]\([^$]*\)[$]\([^$]*\)[$]', re.I)
moduleRE = re.compile ('module[ ]*:\(.*\)', re.I)
authorRE = re.compile ('Author[ ]*:\(.*\)', re.I)

defineRE = re.compile ('Define *: *\n', re.I)

headerComment = """/*!******************************************************************************

  @file         %s

  ------------------------------------------------------------------------------

  @author       %s

  @brief        %s

\\if EMIT_LICENCE
%s
\\endif

********************************************************************************/
"""

class MemoryStream:
    def __init__ (self, source):
        self.data = string.split (source, '\n')
        self.lineno = -1

    def readline (self):
        try:
            self.lineno = self.lineno + 1
            return self.data [self.lineno] + '\n'
        except IndexError:
            raise EOFError

def ignoreConcom (line, stream):
    pass

def getDefinePart (source):
    defineMatch = defineRE.search (source)
    if not defineMatch:
        raise 'Source contains no Define part'
    partStart = defineMatch.end ()
    partEnd = string.find (source, '.CM', partStart)
    if partEnd == -1:
        raise 'End of Define part not found'
    return source [partStart:partEnd]

def getExportInfo (module, mainName = None, source = None):
    if mainName is None:
        try:
            mainName = yutil.findFileExt ('h' + module [1:], ['.hpp', '.h'])
        except yutil.notFound:
            return []
    if source is None:
        source = yutil.openFile (module).read ()
    part = getDefinePart (source)
    currentEntry = [os.path.basename (mainName), '', '']
    result = [currentEntry]
    rex = re.compile ('[(][*] *ptocExport +([^ *]+) *[*][)]')
    pos = 0
    match = rex.search (part, pos)
    while match:
        currentEntry [1] = currentEntry [1] + part [pos:match.start ()]
        fname = fileutil.basename (match.group (1)) + '.h'
        currentEntry = findCurrentEntry (result, fname)
        pos = match.end ()
        match = rex.search (part, pos)
    currentEntry [1] = currentEntry [1] + part [pos:]
    for entry in result:
        declarations = entry [1]
        entry [2] = md5.new (declarations).digest ()
    return result

def findCurrentEntry (entries, fname):
    for entry in entries:
        if entry [0] == fname:
            return entry
    newEntry = [fname, '', '']
    entries.append (newEntry)
    return newEntry

def nullInfoFunction (*args):
    pass

md5RE = re.compile ('MD5: *([^\n\t]+)')

def extractDigest (filename, module = None, infoFunction = nullInfoFunction):
    try:
        header = yutil.openFile (filename).read ()
    except:
        infoFunction (module, filename, 'no header source found')
        return None
    if len (header) == 0:
        # empty files are to be deleted
        infoFunction (module, filename, 'header source is empty')
        return None
    match = md5RE.search (header)
    if not match:
        infoFunction (module, filename, 'Header contains no MD5 digest => ignored')
        return None
    result = eval (string.strip (match.group (1)))
    return result

def getSpecialComment (token, kind, keyWord):
    comment = token.getAttribute (kind)
    if not comment:
        return None
    commentText = string.strip (comment)
    words = string.split (commentText)
    if words [0] != keyWord:
        result = None
    else:
        result = string.join (words [1:])
    return result

def getCommentFlag (token, kind, keyWord):
    comment = token.getAttribute (kind)
    if not comment:
        return None
    commentText = string.strip (comment)
    words = string.split (commentText)
    if words [0] != keyWord:
        result = None
    else:
        result = 1
    return result

def parseTypeSynonym (str):
    pos = string.find (str, ':')
    if pos == -1:
        fname = None
        typename = str
    else:
        fname = string.strip (str [:pos])
        typename = string.strip (str [pos + 1:])
    return fname, typename

# concom specifics

class TokenBoundConcom:
    def attachScanner (self, scanner):
        self.scanner = scanner

    def handleLine (self, line, stream):
        self.scanner.concomLine.append (line)

def handleConcom (token, outstream):
    #print 'trying %s (%x)' % (token, hash(token))
    concomLines = token.getAttribute ('concomLine')
    if concomLines:
        #print '    found', concomLine
        handleConComLines (concomLines, outstream)

concomRE = re.compile (' *& *([a-zA-Z]+) *(.*)')

def handleConComLines (lines, outstream):
    rex = concomRE
    for line in lines:
        match = rex.match (line)
        if not match:
            raise 'Concom error', line
        keyword, parmstring = match.group (1, 2)
        keyword = lower (keyword)
        parmstring = string.strip (parmstring)
        if parmstring == 'TRACE':
            parmstring = 'SAPDB_SLOW'
        if keyword == 'ifdef':
            outstream.write ('\n#if defined (%s)\n\n' % parmstring)
        elif keyword == 'ifndef':
            outstream.write ('\n#if !defined (%s)\n\n' % parmstring)
        elif keyword == 'else':
            outstream.write ('\n#else\n\n')
        elif keyword == 'endif':
            outstream.write ('\n#endif\n\n')
        elif keyword == 'if':
            outstream.write ('\n#if %s\n\n' % string.strip (parmstring))
        elif keyword == 'define':
            outstream.write ('\n#define %s\n\n' % string.strip (parmstring))
        else:
            raise 'Invalid Concom keyword', ('%s in %s' % (keyword, line))

inRE = re.compile (' *\([$a-z]+\) *in *[[]\(.*\)]', re.I)
eqRE = re.compile (' *\([$a-z]+\) *= *\([a-z0-9]+\)', re.I)

def expression (parmstring):
    inRE = inRE
    eqRE = eqRE
    match = eqRE.match (parmstring)
    if match:
        return 'defined (%s)' % match.group (2)
    match = inRE.match (parmstring)
    if match:
        words = string.split (match.group (2), ',')
        words = map (lambda word: 'defined (%s)' % string.strip (word), words)
        return string.join (words, ' || ')
    raise 'Invalid Concom expression', parmstring


class VDNMemoryStream:
    def __init__ (self, part, concomHandler):
        self.part = part
        self.pos = 0
        self.lineno = 0
        self.concomHandler = concomHandler

    def readline (self):
        part = self.part
        pos = self.pos
        eolPos = string.find (part, '\n', pos)
        if eolPos == -1:
            line = part [pos:]
            if line == '':
                raise EOFError
        else:
            line = part [pos:eolPos + 1]
        self.pos = pos + len (line)
        if line [0] == '&':
            self.concomHandler (line, self)
            line = '\n'
        return line

class TccMultiFileStream (MultiFileStream):
    def __init__ (self, fnames, scanner = None):
        MultiFileStream.__init__ (self, fnames, ignoreConcom)
        self.scanner = scanner

    def openNext (self):
        MultiFileStream.openNext (self)
        self.basename = fileutil.basename (self.fname)

class NeedImfException:
    def __init__ (self, include):
        self.include = include

    def show (self, main):
        return "Missing include: %s, try running 'imf %s'" \
            % (self.include, main)




def safeShell (cmd):
    os.system (cmd)


