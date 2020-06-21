
import sys
import string
import regex
import time
import urllib

from html import HTMLFile
from acquisition import Acquest
from DocItem import DocGroup
from DocEntry import Entries, SimpleEntry, stdCategoryHandler
from PidginXML import PidginXML
from FileSystem import File, FileSystem

fullBacktrace = 1

class AnyDoc (Acquest):
    tabSize = 8

    def __init__ (self, bodyAttributes = None):
        Acquest.__init__ (self)
        self.bodyAttributes = bodyAttributes
        self.currentGroup = DocGroup (self)
        self.categoryHandler = stdCategoryHandler ()
        self.pxml = PidginXML ()
        self.loadPxmlHandler (self.pxml)

    def initNewFile (self, fname):
        self.fname = fname
        self.tabFound = None
        self.specialIdentifier = []

    def findCategoryHandler (self, category):
        handler = self.categoryHandler.get (string.lower(category), SimpleEntry)
        return handler

    def addCategoryHandler (self, handlerDict):
        self.categoryHandler.update (handlerDict)

    def loadPxmlHandler (self, pxml):
        pass

    def parseBlock (self, context, text):
        textLen = len (text)
        text = string.expandtabs (text, self.tabSize)
        if len (text) > textLen:
            self.tabWarning ()
        lines = string.split (text, '\n')
        lines = removeDecoLines (lines)
        leadingSkip = countSkip (lines, [_blankRE, _commentRE])
        lines = stripLead (lines, leadingSkip)
        rawItems = breakentries (lines)
        if len (rawItems) == 0:
            raise InvalidBlockError (lines)
        docCategory, categoryLines = rawItems [0]
        title = string.strip (categoryLines [0])
        entries = Entries (self)
        entries.setContext (context)
        context = entries
        for category, itemLines in rawItems [1:]:
            if self.quoteHTML and not (category in _htmlAllowed):
                itemLines = entityfyList (itemLines)
            handler = self.findCategoryHandler (category)
            try:
                try:
                    entry = handler (entries, itemLines)
                except:
                    sys.stderr.write (`handler` + '\n')
                    raise
                entries.addItem (category, entry)
            except IgnoreError:
                raise ParseError (category + ':' + string.join (itemLines, '\n'))
        return docCategory, title, entries

    def warning (self, msg):
        sys.stderr.write ('Warning: %s: %s\n' % (self.fname, msg))

    def openHTML (self, fname, title):
        htmlstream = HTMLFile (fname, title, self.bodyAttributes)
        return htmlstream

    def genHTML (self, fname):
        title = self.getTitle ()
        if isinstance (fname, File):
            file = fname
            fname = fname.shellName ()
        else:
            file = FileSystem ().file (fname)
        htmlstream = self.openHTML (fname, title)
        docHeader = self.acquire ('docHeader')
        if docHeader != None:
            htmlstream.write (docHeader.sourceHeader (file))
        htmlstream.titleHeader (title)
        htmlstream.pushTag ('blockquote')
        self.currentGroup.genTOC (htmlstream)
        htmlstream.line ()
        self.currentGroup.genHTML (htmlstream)
        genDate (htmlstream)
        htmlstream.popTag ()
        htmlstream.close ()

    def getTitle (self):
        specialEntry = self.masthead.entries.findCategory ('special area')
        if specialEntry == None:
            result = self.title
        else:
            result = self.title + ': ' + specialEntry.text ()
        return result

    def getShortdesc (self):
        specialEntry = self.masthead.entries.findCategory ('description')
        if specialEntry == None:
            result = None
        else:
            result = specialEntry.shortDesc ()
        return result

def genDate (htmlstream):
    now = time.localtime(time.time ())
    htmlstream.write (time.strftime ("<i>Generated %Y-%m-%d</i>\n", now))

class IgnoreError:
    def __init__ (self):
        pass


class DocError:
    def __init__ (self):
        self.excInfo = sys.exc_info ()

    def dump (self, sourcefile, lineno, text, stream):
        if fullBacktrace and (self.excInfo [0] != None):
            apply (traceback.print_exception, self.excInfo)
        stream.write ("%s (near line %d): %s\n"
            % (sourcefile, lineno, text))

class SimpleDocError (DocError):
    def __init__ (self, msg):
        DocError.__init__ (self)
        self.msg = msg

    def dump (self, sourcefile, lineno, stream = sys.stderr):
        DocError.dump (self, sourcefile, lineno, self.msg, stream)

class ParseError (DocError):
    def __init__ (self, text, pos = 0):
        DocError.__init__ (self)
        self.text = string.strip (text)
        self.pos = pos

    def dump (self, sourcefile, lineno, stream = sys.stderr):
        msg = "Parse error in '%s'" % self.text
        DocError.dump (self, sourcefile, lineno, msg, stream)

class GroupMismatchError (DocError):
    def __init__ (self, category, groupTitle, closeTitle):
        DocError.__init__ (self)
        self.category = category
        self.groupTitle = groupTitle
        self.closeTitle = closeTitle

    def dump (self, sourcefile, lineno, stream = sys.stderr):
        msg = "%s mismatch (got '%s', expected '%s')" % (
            self.category, self.closeTitle, self.groupTitle)
        DocError.dump (self, sourcefile, lineno, msg, stream)

class InvalidBlockError (DocError):
    def __init__ (self, lines):
        DocError.__init__ (self)
        self.text = string.strip (firstTextLine (lines))

    def dump (self, sourcefile, lineno, stream = sys.stderr):
        msg = "Parse error in '%s' (missing ':' or wrong indentation?)" % self.text
        DocError.dump (self, sourcefile, lineno, msg, stream)

class ClassMismatchError (DocError):
    def __init__ (self, classGroupName, actualName):
        DocError.__init__ (self)
        self.classGroupName = classGroupName
        self.actualName = actualName

    def dump (self, sourcefile, lineno, stream = sys.stderr):
        msg = "Mismatch: declared class %s, comment title %s" % (
            self.actualName, self.classGroupName)
        DocError.dump (self, sourcefile, lineno, msg, stream)

class InvalidMasthead (DocError):
    def __init__ (self, msg):
        DocError.__init__ (self)
        self.msg = msg

    def dump (self, sourcefile, lineno, stream = sys.stderr):
        DocError.dump (self, sourcefile, lineno, self.msg, stream)


def checkForGroupMismatch (category, groupTitle, closeTitle):
    closeTitle = string.strip (closeTitle)
    if closeTitle == '':
        return
    if string.lower (category) == 'endclass':
        closeTitle = 'Class ' + closeTitle
    if groupTitle != closeTitle:
        raise GroupMismatchError (category, groupTitle, closeTitle)

_categoryRE = regex.compile ('[ \n]*\(.*\)\([a-zA-Z]+\):')

_blankRE = regex.compile (' *')
_commentRE = regex.compile (' *\*+* *')

def countSkip (lines, patterns, ignoreFirstLine = 1):
    result = 1000
    trim = string.strip
    if ignoreFirstLine:
        bottomLines = lines [1:]
    else:
        bottomLines = lines
    for re in patterns:
        for line in bottomLines:
            if len (trim(line)) == 0:
                continue
            matchSize = re.match (line)
            if (matchSize >= 0) and (matchSize < result):
                result = matchSize
    if result == 1000:
        result = 0
    return result

def stripFirstLine (line, leadingSkip):
    leftTrimmed = string.lstrip (line)
    leadingBlanks = len (line) - len (leftTrimmed)
    if leadingBlanks < leadingSkip:
        result = leftTrimmed
    else:
        result = line [leadingSkip:]
    return result

def stripLead (lines, leadingSkip):
    if len (lines) == 0:
        return lines
    if leadingSkip == 0:
        lines [0] = string.strip (lines [0])
        return lines
    result = [stripFirstLine (lines [0], leadingSkip)]
    for line in lines [1:]:
        result.append (line [leadingSkip:])
    return result

def removeIndentFromLines (lines, ignoreFirstLine = 1):
    leadingSkip = countSkip (lines, [_blankRE, _commentRE], ignoreFirstLine)
    lines = stripLead (lines, leadingSkip)
    return lines

def removeIndentFromSource (source, ignoreFirstLine = 1):
    lines = string.split (source, '\n')
    lines = removeIndentFromLines (lines, ignoreFirstLine)
    return string.join (lines, '\n')

_decoRE = regex.compile (' *[=*-]+ *$')

def removeDecoLines (lines):
    result = filter (lambda line: _decoRE.match (line) == -1, lines)
    return result

_categoryRE = regex.compile ('[a-zA-Z][^:]*:')

def breakentries (lines):
    result = []
    currentLines = []
    re = _categoryRE
    for line in lines:
        matchSize = re.match (line)
        if matchSize == -1:
            currentLines.append (line)
        else:
            category = string.strip (line [:matchSize - 1])
            currentLines = [line [matchSize + 1:]]
            result.append ((category, currentLines))
    return result

def firstTextLine (lines):
    for line in lines:
        if line:
            return line
    return ''

class AnchorBag:
    def __init__ (self):
        self.dict = {}

    def addAnchor (self, title):
        title = urllib.quote (title, '')
        count = self.dict.get (title, 0)
        count = count + 1
        if count == 1:
            result = title
        else:
            result = title + '_' + ('%03d' % count)
        self.dict [title] = count
        return result

def stripLicence (comment):
    licenceStart = string.find (comment, "========== licence begin")
    if licenceStart == -1:
        return comment
    licenceEnd = string.find (comment, "========== licence end", licenceStart)
    if licenceEnd == -1:
        return comment
    realStart = string.rfind (comment, '\n', 0, licenceStart) - 1
    realEnd = string.find (comment, '\n', licenceEnd) + 1
    return comment [:realStart] + comment [realEnd:]

def removeComments (snippet):
    result = removeSpecificComments (snippet, '//', '\n')
    result = removeSpecificComments (result, '/*', '*/')
    return result

def removeSpecificComments (snippet, commentStart, commentEnd):
    if string.find (snippet, commentStart) == -1:
        return snippet
    result = ''
    searchPos = 0
    startPos = string.find (snippet, commentStart, searchPos)
    while startPos != -1:
        result = snippet [searchPos:startPos]
        endPos = string.find (snippet, commentEnd, startPos + len (commentStart))
        if endPos == -1:
            endPos = len (snippet)
        else:
            endPos = endPos + len (commentEnd)
        searchPos = endPos
        startPos = string.find (snippet, commentStart, searchPos)
    result = result + snippet [searchPos:]
    return result
    
