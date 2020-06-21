
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
"""cppDocLib.py: Extract special comments from C++-Source and generate HTML
"""

import os
import re
import regex
import regsub
import string
import sys
import traceback
import cStringIO

import fileutil
from html import *
import c2html

from docLib import AnyDoc, DocError, SimpleDocError, \
    removeIndentFromSource, AnchorBag, stripLicence
from DocItem import DocItem, DocGroup, ClassGroup, MastHead, DocLeaf
from DocEntry import DefListEntry
from FileSystem import File
from PidginXML import HTMLTranslate, HTMLContentRule
from acquisition import Acquest

version = "1.3.02"
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
        self.anchorBag = AnchorBag ()
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
            comment = stripLicence (comment)
            commentEnd = nextPos
            category, self.title, items = self.parseBlock (self.currentGroup, comment)
            if string.lower (category) != 'module':
                raise SimpleDocError ("The first special comment should be a 'Module', not a '%s'\n"
                        % category)
            self.masthead = MastHead (self.currentGroup, 'Module', items)
            self.currentGroup.addItem (self.masthead)
            pos = re.search (source, nextPos)
            while pos != -1:
                if makeClean:
                    cleanSource.append (source [commentEnd:pos])
                self.changeVisibility (source [commentEnd:pos])
                comment, nextPos = self.readComment (source, pos)
                category, title, items = self.parseBlock (self.currentGroup, comment)
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

    visibilityRE = re.compile ('\n\s*(private|protected|public)\s*:')

    def changeVisibility (self, chunk):
        lastMatch = None
        match = self.visibilityRE.search (chunk)
        while match:
            lastMatch = match
            match = self.visibilityRE.search (chunk, match.end ())
        if lastMatch:
            visibility = lastMatch.group (1)
            if visibility == 'public':
                visibility = ''
            self.currentGroup.setAcquiredAttribute ('visibility', visibility)

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
        visibility = context.acquire ('visibility')
        if category in ['function', 'method']:
            item = FunctionDoc (context, title, items, context.acquire ('className'))
            self.currentGroup.addItem (item)
        elif category == 'operator':
            item = OperatorDoc (context, title, items, context.acquire ('className'))
            self.currentGroup.addItem (item)
        elif category == 'macro':
            item = MacroDoc (context, title, items)
            self.currentGroup.addItem (item)
        elif category in ['declaration', 'declarations']:
            item = DeclarationDoc (context, title, items)
            self.currentGroup.addItem (item)
        elif category in ['notes', 'example']:
            item = self.currentGroup.addItem (DocLeaf (context, title, items))
        elif category == 'chapter':
            item = self.currentGroup = self.currentGroup.pushGroup (title, items)
        elif category == 'endchapter':
            self.currentGroup = self.currentGroup.popGroup ()
        elif category == 'class':
            item = self.currentGroup = self.currentGroup.pushGroup (
                title, items, ClassGroup)
            item.visibility = 'private'
            self.specialIdentifier.append (title)
        elif category == 'endclass':
            if strict:
                checkForGroupMismatch (category,
                    self.currentGroup.title, title)
            self.currentGroup.close ()
            self.currentGroup = self.currentGroup.popGroup ()
        elif category == 'endfunction':
            pass # simply ignore
        else:
            self.currentGroup.addItem (DocLeaf (context, title, items))
        if item:
            item.visibility = visibility
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
    # chapter XML formatting
    #
    def loadPxmlHandler (self, pxml):
        #
        # pass HTML through
        #
        for tag in ['ul', 'ol', 'li', 'a', 'br', 'var', 'pre']:
            uptag = string.upper (tag)
            rule = HTMLTranslate (tag, uptag)
            pxml.addRule (rule)
            rule = HTMLTranslate (uptag, uptag)
            pxml.addRule (rule)
        #
        # translate to HTML
        #
        for intag, outtag in [('Param', 'var')]:
            rule = HTMLContentRule (intag, outtag)
            pxml.addRule (rule)
        #
        # use Weaver for xref links
        #
        for tag in ['Class', 'Method', 'Function', 'SeeAlso']:
            constructor = getattr (self, 'CppXml' + tag)
            rule = constructor (tag)
            pxml.addRule (rule)

    class XmlClass (HTMLContentRule):
        pass

    class CppXmlClass (XmlClass):
        def translateAll (self, context, tag, attributes, content):
            className = string.strip (content)
            weaver = context.acquire ('weaver')
            text = '<CODE>' + className + '</CODE>'
            classLink = weaver.getClassLink (className, text)
            return 'class ' + classLink

    class CppXmlMethod (XmlClass):
        def translateAll (self, context, tag, attributes, content):
            methodName = string.strip (content)
            text = '<CODE>' + methodName + '</CODE>'
            weaver = context.acquire ('weaver')
            pos = string.find (content, '::')
            if pos == -1:
                className = context.acquire ('className')
                mname = methodName
            else:
                className = methodName [:pos]
                mname = methodName [pos + 2:]
            methodLink = weaver.getMethodLink (className, mname, text)
            return 'method ' + methodLink

    class CppXmlFunction (XmlClass):
        def translateAll (self, context, tag, attributes, content):
            functionName = string.strip (content)
            text = '<CODE>' + functionName + '</CODE>'
            weaver = context.acquire ('weaver')
            functionLink = weaver.getFunctionLink (functionName, text)
            return 'function ' + functionLink

    class CppXmlSeeAlso (XmlClass):
        def translateAll (self, context, tag, attributes, content):
            content = string.strip (content)
            return '[see also: ' + content + ']'

class FunctionDoc (DocItem):
    closeWithLine = 1

    def __init__ (self, context, title, entries, className = None):
        DocItem.__init__ (self, context, title, entries)
        self.prototypes = []
        if className:
            self.className = className
            title = className + '::' + title
        self.anchor = self.acquire ('anchorBag').addAnchor (title)

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

    def genHeader (self, htmlstream):
        if not self.title:
            raise 'empty element'
        visibility = self.visibility
        if visibility:
            title = self.title + ' (' + visibility + ')'
        else:
            title = self.title
        result = self.headerFMT % (self.anchor, title, self.genBacklink ())
        htmlstream.write (result)
        return result

    def genSpecificHTML (self, htmlstream):
        self.genPrototypes (htmlstream)
        self.genentries (htmlstream, ['prototypes'])

    def genPrototypes (self, htmlstream):
        prototypes = self.prototypes
        if prototypes:
            htmlstream.pushTags ('pre', 'code')
            source = string.join (prototypes, '\n\n')
            source = _cColorizer.convertString (source, self.acquire ('weaver'))
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
                if i == (prototypeCount - 1):
                    endPos = startPos + bracePos
                else:
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

class OperatorDoc (FunctionDoc):
    def __init__ (self, context, title, entries, className = None):
        FunctionDoc.__init__ (self, context, title, entries, className)
        self.title = 'operator ' + self.title

class DeclarationDoc (DocItem):
    closeWithLine = 1
    endDeclRE = regex.compile ('/[*]!\([^*]*\)[*]/')

    def __init__ (self, context, title, entries):
        DocItem.__init__ (self, context, title, entries)
        self.source = None

    def genSpecificHTML (self, htmlstream):
        if self.source:
            self.genCode (htmlstream)
        self.genentries (htmlstream, ['source'])

    def genCode (self, htmlstream):
        htmlstream.pushTags ('pre', 'code')
        source = _cColorizer.convertString (self.source, self.acquire ('weaver'))
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

class MacroDoc (DocItem):
    def __init__ (self, context, title, entries):
        DocItem.__init__ (self, context, title, entries)
        self.title = 'macro ' + title
        self.source = 'no source available'

    def genSpecificHTML (self, htmlstream):
        if self.source:
            self.genCode (htmlstream)
        self.genentries (htmlstream, ['source'])

    def genCode (self, htmlstream):
        htmlstream.pushTags ('pre', 'code')
        source = _cColorizer.convertString (self.source, self.acquire ('weaver'))
        htmlstream.write (source)
        htmlstream.popTag (2)

    defineRE = re.compile ("# *define *(?P<name>[a-zA-Z0-9_]+) *(?P<args>[(][^)]*[)])?")
    def readSource (self, source, pos, controller):
        match = self.defineRE.search (source, pos)
        if not match:
            print 'no match'
            return pos
        name, args = match.group("name", "args")
        self.source = "#define " + name
        if args:
            self.source = self.source + args
        nextPos = match.end ()
        nextPos = string.find (source, '\n', nextPos)
        while source [nextPos - 1] == '\\':
            nextPos = string.find (source, '\n', nextPos + 1)
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

# _cColorizer = c2html.C2html ()

class WeavingC2HTML:
    def __init__ (self):
        keywordSet = {}
        for word in c2html.keywords:
            keywordSet [word] = 1
        self.keywordSet = keywordSet
        self.identifierRE = re.compile ('[a-z][a-z0-9_]*', re.IGNORECASE)

    xre = re.compile ('([a-z][a-z0-9_]*)|(//)|(/[*])', re.IGNORECASE)

    def convertString (self, source, weaver):
        source = string.replace (source, '&', '&amp;')
        source = string.replace (source, '<', '&lt;')
        result = cStringIO.StringIO ()
        write = result.write
        find = string.find
        xre = self.xre
        keywordSet = self.keywordSet
        pos = 0
        match = xre.search (source, pos)
        while match != None:
            matchStart = match.start ()
            write (source [pos:matchStart])
            identifier, eolComment, comment = match.group (1, 2, 3)
            if identifier:
                if keywordSet.has_key (identifier):
                    write ('<strong>' + identifier + '</strong>')
                else:
                    if find (identifier, '_') == -1:
                        # this cannot be an official class name
                        write (identifier)
                    else:
                        write (weaver.getClassLink (identifier))
                pos = matchStart + len (identifier)
            else:
                if eolComment:
                    end = find (source, '\n', matchStart)
                else:
                    # must be a /* */ comment
                    end = find (source, '*/', matchStart + 2) + 2
                if end == -1:
                    end = len (source)
                write ('<font color="green">')
                write (source [matchStart:end])
                write ('</font>')
                pos = end
            match = xre.search (source, pos)
        write (source [pos:])
        return result.getvalue ()

_cColorizer = WeavingC2HTML ()
#_cColorizer = c2html.C2html ()

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

