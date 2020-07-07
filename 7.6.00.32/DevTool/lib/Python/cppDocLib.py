"""cppDocLib.py: Extract special comments from C++-Source and generate HTML
"""

import os
import re
import regex
import regsub
import string
import sys
import traceback
import urllib

import fileutil
from html import *
import c2html

from docLib import AnyDoc, DocError, SimpleDocError, removeIndentFromSource
from DocItem import DocItem, DocGroup, ClassGroup, MastHead, DocLeaf
from DocEntry import DefListEntry
from FileSystem import File

version = "1.3.00"
strict = None    # global should be replaced by acquisition

def stdwarn (msg):
    sys.stderr.write ('Warning: ' + msg + '\n')

_warn = stdwarn

class CppDoc (AnyDoc):
    doHighlight = None
    strict = None
    quoteHTML = None

    def __init__ (self, bodyAttributes = None):
        AnyDoc.__init__ (self, bodyAttributes);
        self.addCategoryHandler ({
            'template arguments': TemplateArgsEntry,
        })
        global _warn
        _warn = self.warning

    commentRE = regex.compile ('/[*]!')

    def tabWarning (self):
        return   # to make docSake a bit more quiet
        if self.tabFound:
            return
        self.warning ('found tabs')
        self.tabFound = 1

    def openHTML (self, fname, title):
        if self.doHighlight and self.specialIdentifier:
            htmlstream = HighlightStream (fname, title, self.bodyAttributes, self.specialIdentifier)
        else:
            htmlstream = AnyDoc.openHTML (self, fname, title)
        return htmlstream

    def readSource (self, fname, makeClean = None, requireComments = 1):
        global strict
        strict = self.acquire ('strict')
        self.initNewFile (fname)
        self.weaver = self.acquire ('weaver')
        self.weaver.setCurrentFile (fname.setExtension ('.html'))
        re = self.commentRE
        if isinstance (fname, File):
            source = fname.text ()
        else:
            source = open (fname, "r").read ()
        if makeClean:
            cleanSource = []
        pos = re.search (source)
        if pos == -1:
            if requireComments:
                raise 'Module ' + fname + 'has no special comments'
            else:
                return 0
        if makeClean:
            cleanSource.append (source [:pos])
        ok = 1
        try:
            comment, nextPos = self.readComment (source, pos)
            commentEnd = nextPos
            category, self.title, items = self.parseBlock (comment)
            if string.lower (category) != 'module':
                raise SimpleDocError ("The first special comment should be a 'Module', not a '%s'\n"
                        % category)
            self.masthead = MastHead ('Module', items)
            self.currentGroup.addItem (self.masthead)
            pos = re.search (source, nextPos)
            while pos != -1:
                if makeClean:
                    cleanSource.append (source [commentEnd:pos])
                comment, nextPos = self.readComment (source, pos)
                category, title, items = self.parseBlock (comment)
                item = self.createItem (category, title, items)
                commentEnd = nextPos
                if item != None:
                    nextPos = item.readSource (source, nextPos, self)
                    item.addToIndex (self.weaver)
                try:
                    pos = re.search (source, nextPos)
                except regex.error:
                    # quick fix, why is pos out of range?
                    pos = -1
            if makeClean:
                cleanSource.append (source [commentEnd:pos])
                self.makeClean (cleanSource, source, fname)
            if not self.currentGroup.isTop ():
                sys.stderr.write ('%s: group not closed %s\n'
                    % (fname, self.currentGroup.dumpString ()))
                ok = None
        except DocError, perr:
            perr.dump (fname, self.findErrorLine (source, pos))
            ok = None
        except:
            traceback.print_exc ()
            sys.stderr.write ('%s (line %d): Parse error\n' %
                (fname, self.findErrorLine (source, pos)))
            ok = None
        return ok

    def readComment (self, source, startPos):
        startPos = startPos + 3
        endPos = string.index (source, '*/', startPos)
        nextPos = endPos + 2
        if (source [nextPos] == '\n'):
            nextPos = nextPos + 1
        return source [startPos:endPos], nextPos

    def createItem (self, category, title, items):
        category = string.lower (category)
        item = None
        context = self.currentGroup
        if category == 'function':
            item = FunctionDoc (title, items, context.acquire ('className'))
            self.currentGroup.addItem (item)
        elif category == 'declaration':
            item = DeclarationDoc (title, items)
            self.currentGroup.addItem (item)
        elif category in ['notes', 'example']:
            item = self.currentGroup.addItem (DocLeaf (title, items))
        elif category == 'chapter':
            item = self.currentGroup = self.currentGroup.pushGroup (title, items)
        elif category == 'endchapter':
            self.currentGroup = self.currentGroup.popGroup ()
        elif category == 'class':
            item = self.currentGroup = self.currentGroup.pushGroup (
                title, items, ClassGroup)
            self.specialIdentifier.append (title)
        elif category == 'endclass':
            if strict:
                checkForGroupMismatch (category,
                    self.currentGroup.title, title)
            self.currentGroup = self.currentGroup.popGroup ()
        elif category == 'endfunction':
            pass # simply ignore
        else:
            self.currentGroup.addItem (DocLeaf (title, items))
        if item != None:
            item.setContext (context)
        return item

    def findOldClean (self, source, startRE, endRE):
        startPos = startRE.search (source)
        if startPos == -1:
            return -1, -1
        endPos = endRE.search (source, startPos)
        endPos = endRE.regs [0][1]
        return startPos, endPos

    def makeClean (self, snippets, source, fname):
        cleanSource = string.join (snippets, '')
        ifdef = '#if defined (CPPDOC_CLEANSOURCE)'
        endif = '#endif  /* CPPDOC_CLEANSOURCE */'
        endifre = '#endif  /[*] CPPDOC_CLEANSOURCE [*]/'
        ifdefRE = regex.compile (ifdef)
        endifRE = regex.compile (endifre)
        startPos, endPos = self.findOldClean (cleanSource, ifdefRE, endifRE)
        if startPos != -1:
            cleanSource = cleanSource [:startPos] + cleanSource [endPos:]
        # create new source
        startPos, endPos = self.findOldClean (source, ifdefRE, endifRE)
        if startPos != -1:
            source = "%s%s\n%s\n%s%s" % (source [:startPos],
                    ifdef, cleanSource, endif, source [endPos:])
        else:
            source = "%s\n%s\n%s\n%s\n" % (source, ifdef, cleanSource, endif)
        # make backup
        fileutil.makeBackup (fname)
        # write source to file
        open (fname, "w").write (source)

    def findErrorLine (self, source, commentStart):
        pos = 0
        line = 0
        index = string.index
        while pos < commentStart:
            pos = index (source, '\n', pos) + 1
            line = line + 1
        return line

    #
    # chapter TeX formatting
    #
    def loadTeXHandler (self, tex):
        for tag, html in [('param', 'code'),
                          ('ident', 'code')]:
            tex.addHTMLTranslation (tag, html)
        tex.addHandler ('br', self.texBr, None)
        for tag in ['Class', 'Method', 'Function', 'Link']:
            exitHandler = getattr (self, 'tex' + tag)
            tex.addHandler (string.lower (tag), None, exitHandler)

    def texBr (self, name):
        return '<BR>'

    def texClass (self, pendingData):
        return 'class <CODE>' + pendingData + '</CODE>'

    def texMethod (self, pendingData):
        return 'method <CODE>' + pendingData + '</CODE>'

    def texFunction (self, pendingData):
        return 'function <CODE>' + pendingData + '</CODE>'

    def texLink (self, pendingData):
        pos = string.find (pendingData, '|')
        if pos == -1:
            href = pendingData
            text = pendingData
        else:
            href = pendingData [pos + 1:]
            text = pendingData [:pos]
        result = '<A HREF="%s">%s</A>' % (string.strip (href), text)
        return result

class FunctionDoc (DocItem):
    closeWithLine = 1

    def __init__ (self, title, entries, className = None):
        DocItem.__init__ (self, title, entries)
        self.prototypes = []
        if className:
            self.className = className
            self.anchor = urllib.quote (className + '::' + title, '')
        else:
            self.anchor = urllib.quote (title)

    def shortDesc (self):
        if self.entries == None:
            return ''
        result = DocItem.shortDesc (self)
        if not result:
            description = self.entries.findCategory ('return value')
            if description != None:
                result = 'returns ' + description.shortDesc ()
            else:
                result = ''
        return result

    def genSpecificHTML (self, htmlstream):
        self.genPrototypes (htmlstream)
        self.genentries (htmlstream, ['prototypes'])

    def genPrototypes (self, htmlstream):
        prototypes = self.prototypes
        if prototypes:
            htmlstream.pushTags ('pre', 'code')
            source = string.join (prototypes, '\n\n')
            source = _cColorizer.convertString (source)
            htmlstream.write (source)
            htmlstream.popTag (2)

    def readSource (self, source, pos, controller):
        count = self.prototypeCount ()
        nextPos = self.readPrototypes (source, count, pos, controller)
        return nextPos

    def prototypeCount (self):
        entry = self.entries.findCategory ('prototypes')
        if entry != None:
            result = int (entry.count)
        else:
            result = 1
        return result

    def readPrototypes (self, source, prototypeCount, startPos, controller):
        prototypes = []
        for i in xrange (prototypeCount):
            endPos = string.index (source, ';', startPos)
            prototype = source [startPos:endPos + 1]
            bracePos = string.find (prototype, '{')
            if bracePos != -1:
                prototype = string.strip (prototype [:bracePos]) + ';'
                endPos = self.skipInline (source, startPos + bracePos)
            protoLen = len (prototype)
            prototype = string.expandtabs (prototype, controller.tabSize)
            if len (prototype) > protoLen:
                controller.tabWarning ()
            prototype = removeIndentFromSource (prototype, None)
            prototypes.append (prototype)
            startPos = endPos + 1
        self.prototypes = prototypes
        if strict:
            self.checkPrototypes (prototypes)
        return startPos

    def skipInline (self, source, braceInPos):
        nesting = 1
        braceOutPos = findOrSkip (source, '}', braceInPos + 1)
        braceInPos = findOrSkip (source, '{', braceInPos + 1)
        while nesting > 0:
            if (braceInPos < braceOutPos):
                nesting = nesting + 1
                braceInPos = findOrSkip (source, '{', braceInPos + 1)
            else:
                nesting = nesting - 1
                nextPos = braceOutPos + 1
                braceOutPos = findOrSkip (source, '}', braceOutPos + 1)
        # search for ';', consume if only blanks between
        nextSemikolon = findOrSkip (source, ';', braceOutPos)
        if len (string.strip (source [braceOutPos:nextSemikolon])) == 0:
            nextPos = nextSemikolon + 1
        return nextPos

    def checkPrototypes (self, prototypes):
        if not prototypes:
            return
        parser = CppParser ()
        funcName, paramNames = parser.parseFunctionDeclaration (prototypes [0])
        className = self.acquire ('className', '')
        for prototype in prototypes [1:]:
            nextName, nextParams = parser.parseFunctionDeclaration (prototype)
            if funcName != nextName:
                _warn ('Multiple functions for comment: %s, %s'
                    % (funcName, nextName))
            for paramName in nextParams:
                if not (paramName in paramNames):
                    paramNames.append (paramName)
        if (funcName != self.title) and not (self.title in ['constructor', 'destructor']):
            _warn ('prototype %s::%s must match comment %s'
                % (className, funcName, self.title))
        commentedNames =self.getCommentedNames ()
        # check all parameters are documented
        for declared in paramNames:
            if not (declared in commentedNames):
                _warn ('Function %s::%s: Parameter %s not documented'
                    % (className, self.title, declared))
        # check all documented parameters are used
        for commented in commentedNames:
            if not (commented in paramNames):
                _warn ('Function %s::%s: Parameter %s not used'
                    % (className, self.title, commented))
        # not implemented: check for order of parameters

    def getCommentedNames (self):
        argumentPart = self.entries.findCategory ('arguments')
        if not argumentPart:
            return []
        names = argumentPart.argumentNames ()
        return names

    def addToIndex (self, weaver):
        className = self.acquire ('className', None)
        if className:
            weaver.addMethod (className, self.title, self.anchor, self.shortDesc ())
        else:
            weaver.addRoutine (self.title, self.anchor, self.shortDesc ())

class DeclarationDoc (DocItem):
    closeWithLine = 1
    endDeclRE = regex.compile ('/[*]!\([^*]*\)[*]/')

    def __init__ (self, title, entries):
        DocItem.__init__ (self, title, entries)
        self.source = None

    def genSpecificHTML (self, htmlstream):
        if self.source:
            self.genCode (htmlstream)
        self.genentries (htmlstream, ['source'])

    def genCode (self, htmlstream):
        htmlstream.pushTags ('pre', 'code')
        source = _cColorizer.convertString (self.source)
        htmlstream.write (source)
        htmlstream.popTag (2)

    def readSource (self, source, pos, controller):
        re = self.endDeclRE
        commentStart = re.search (source, pos)
        if commentStart == -1:
            self.source = 'No declarations found!'
            controller.warning ('missing end of declaration')
            return pos
        content = re.group (1)
        if string.find (string.lower (content), 'enddecl') != -1:
            self.source = source [pos:commentStart]
            nextPos = re.regs [0][1]
        else:
            self.pos = 'No declarations found!'
            nextPos = commentStart
        return nextPos

class TemplateArgsEntry (DefListEntry):
    def __init__ (self, context, lines):
        DefListEntry.__init__ (self, context, 'template arguments', lines)

    def formatDefinition (self, name):
        return name

_htmlAllowed = ['htmldescription', 'code', 'example']

def parseAttributes (attributes, default):
    if not attributes:
        return default
    result = []
    find = string.find
    for attribute in attributes:
        pos = find (attribute, '=')
        if pos == -1:
            attrTuple = (attribute, None)
        else:
            attrTuple = (attribute [:pos], attribute [pos+1:])
        result.append (attrTuple)
    return result

class HighlightStream (HTMLStream):
    def __init__ (self, fname, title = None, bodyList = None, specialIdentifier = None):
        stream = open (fname, "w")
        self.changeRE = re.compile ('\\b(' + string.join (specialIdentifier, '|') + ')\\b')
        HTMLStream.__init__ (self, stream, title, bodyList)

    def write (self, text):
        text = self.changeRE.sub (r'<strong>\1</strong>', text)
        HTMLStream.write (self, text)

_cColorizer = c2html.C2html ()

def findOrSkip (source, pattern, start):
    result = string.find (source, pattern, start)
    if result == -1:
        result = len (source)
    return result

def entityfy (text):
    text = string.replace (text, '&', '&amp;')
    text = string.replace (text, '<', '&lt;')
    return text

def entityfyList (list):
    result = map (entityfy, list)
    return result

_cidentifier = '\([~a-zA-Z_][a-zA-Z0-9_]*\)'
_operatorName = '\(operator *[][()+-*/%&|~^!><=,]+\)'
_operatorNewName = '\(operator *\(new\|delete\) *\([[] *]\)?\)'

class CppParser:
    paramPattern = ' *(\([^)]*\))'
    #funcDclRE = regex.compile (_cidentifier + ' *(\([^]*\))')
    funcDclRE = regex.compile (_cidentifier + ' *(\([^)]*\))')
    operatorDclRE = regex.compile (_operatorName + ' *(\([^)]*\))')
    operatorNewDclRE = regex.compile (_operatorNewName + paramPattern)
    paramRE = regex.compile (_cidentifier + '\( *[[] *[]]\)?\( *=.*\)? *$')

    def parseFunctionDeclaration (self, text):
        for re in [self.funcDclRE, self.operatorDclRE, self.operatorNewDclRE]:
            matchPos = re.search (text)
            if matchPos != -1:
                break
        else:
            return None
        funcName, paramString = re.group (1, 2)
        paramNames = []
        if string.strip (paramString) != '':
            paramStrings = string.split (paramString, ',')
            for str in paramStrings:
                str = string.strip (str)
                if self.paramRE.search (str) != -1:
                    paramNames.append (self.paramRE.group (1))
                else:
                    paramNames.append ('parse error')
        return funcName, paramNames

