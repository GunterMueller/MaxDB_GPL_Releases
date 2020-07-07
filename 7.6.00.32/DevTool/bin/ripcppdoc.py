#!/usr/bin/env python
# ripcppdoc.py

import string
import re
import os
import time
import traceback
from UserDict import UserDict

from docTools.docLib import *
from docTools.DocEntry import *

_thisYear = str (time.localtime (time.time ()) [0])

_usePerforce = None

class AbstractRipCppdoc (AnyDoc):
    quoteHTML = None

    def extract (self, fname):
        self.fname = fname
        text = getfilecontent (fname)
        self.openStream (fname)
        codePos = 0
        commentPos = string.find (text, '/*!')
        while commentPos != -1:
            self.handleCode (text [codePos:commentPos])
            indent = self.findIndent (text, codePos, commentPos)
            codePos = string.index (text, '*/', commentPos + 3) + 2
            self.handleComment (text [commentPos + 3:codePos - 2],
                ' ' * indent)
            commentPos = string.find (text, '/*!', codePos)
        self.handleCode (text [codePos:])
        self.stream.close ()

    def openStream (self, fname):
        if _usePerforce:
            targetname = fname
        else:
            targetname = os.path.join (os.environ ['TMP'], 'newsrc', fname)
        self.stream = safeOpen (targetname)
        

    def handleCode (self, code):
        self.stream.write (code)
    
    def writeComment (self, comment):
        self.stream.write ('/*!' + comment + '*/')    
        

    def tabWarning (self, *args):
        pass
    
    def findIndent (self, text, codePos, commentPos):
        nlpos = string.rfind (text, '\n', codePos, commentPos)
        if nlpos == -1:
            return 0
        else:
            return commentPos - nlpos
    
        
class Cppdoc2Doxygen (AbstractRipCppdoc):
    def __init__ (self, deco, decosize):
        AbstractRipCppdoc.__init__ (self)
        self.categoryRefs = Bag ()
        self.entryRefs = Bag ()
        if deco:
            self.deco = deco * decosize
        else:
            self.deco = ''
        self.decosize = decosize

    def handleComment (self, comment, indent):
        try:
            comment, licence = self.extractLicence (comment)
            category, title, items = self.parseBlock (self, comment)
            try:
                handler = getattr (self, string.lower (category) + 'Handler')
            except AttributeError:
                print 'no handler for', category
                handler = None
            if handler:
                handler (category, title, items, indent, comment, licence)
            else:
                self.writeComment (comment)
            #self.categoryRefs.add (string.lower (category), self.fname)
            #for tag, item in items:
            #    self.entryRefs.add (item.__class__.__name__, self.fname)
        except InvalidBlockError:
            self.writeComment (comment)
        except:
            self.writeComment (comment)
            print '**** ERROR data ****'
            print comment
            print '**** ERROR exception ****'
            traceback.print_exc ()
        
    def writeCommentStart (self, indent):
        decosize = self.decosize - len (indent) - 3
        self.stream.write ('/*!' + self.deco [:decosize] + '\n')
    
    def writeCommentEnd (self, indent):
        decosize = self.decosize - len (indent) - 3
        self.stream.write ('\n' + indent + self.deco [:decosize] + '*/\n')
    
    def moduleHandler (self, category, title, entries, indent, comment, licence, *other):
        write = self.stream.write
        self.writeCommentStart (indent)
        write (self.formatEntry ('', 'file') + title + '\n')
        self.writeEntries (entries, indent)
        if licence:
            write (licence)
        #write (comment)
        self.writeCommentEnd (indent)
            
    def classHandler (self, category, title, entries, indent, comment, licence, *other):
        if string.find (title, '_I') != -1:
            tag = 'interface'
        else:
            tag = 'class'
        self.stdHandler (tag, title, entries, indent, comment, licence)

    def endclassHandler (self, category, title, entries, indent, comment, licence, *other):
        pass
    
    def functionHandler (self, category, title, entries, indent, comment, licence, *other):
        write = self.stream.write
        self.writeCommentStart (indent)
        self.writeEntries (entries, indent)
        self.writeCommentEnd (indent)
        
    operatorHandler = functionHandler
    methodHandler = functionHandler
    returnsHandler = functionHandler
    argumentsHandler = functionHandler

    def stdHandler (self, tag, title, entries, indent, comment, licence, *other):
        write = self.stream.write
        self.writeCommentStart (indent)
        write (self.formatEntry ('', tag) + title + '\n')
        self.writeEntries (entries, indent)
        self.writeCommentEnd (indent)
    
    def chapterHandler (self, category, title, entries, indent, comment, licence, *other):
        self.stream.write ('/*! @name ' + title + ' */\n')
        self.stream.write (indent + '/*@{*/')
    
    def endchapterHandler (self, category, title, entries, indent, comment, licence, *other):
        self.stream.write ('/*@}*/')
    
    def declarationHandler (self, category, title, entries, indent, comment, licence, *other):
        write = self.stream.write
        self.writeCommentStart (indent)
        self.writeEntries (entries, indent)
        self.writeCommentEnd (indent)
    
    declarationsHandler = declarationHandler
    typedefHandler = declarationHandler
    variableHandler = functionHandler
    
    def enddeclarationHandler (self, *args):
        pass
    
    enddeclarationsHandler = enddeclarationHandler

    def notesHandler (self, category, title, entries, indent, comment, licence, *other):
        write = self.stream.write
        self.writeCommentStart (indent)
        self.writeEntries (entries, indent)
        self.writeCommentEnd (indent)
        
    exampleHandler = notesHandler
    
    entryMapping = {
        'return value': 'return',
        'special area': 'ingroup',
    }
    
    def mapEntry (self, key):
        return self.entryMapping.get (key, key)
    
    def writeEntries (self, entries, indent):
        write = self.stream.write
        description = None
        for key, object in entries:
            if key in ['last change', 'version', 'prototypes', 
                    'copyright', 'last changed', 'new', 'updated']:
                continue
            className = object.__class__.__name__
            if className == 'DescriptionEntry':
                if key == 'description':
                    write (self.formatEntry (indent, 'brief'))
                    write (string.strip (object.shortDesc ()) + '\n')
                    description = object
                else:
                    doxygenkey = self.entryMapping.get (key, key)
                    write (self.formatEntry (indent, doxygenkey))
                    write (string.strip (object.shortDesc ()) + '\n')
                    self.writeDescription (object, entriesUsed = 1)
            elif className == 'ResponsibleEntry':
                write (self.formatEntry (indent, 'author'))
                validLines = filter (None, map (string.strip, object.lines))
                write (string.strip (string.join (validLines, ', ')) + '\n')
            elif className == 'SimpleEntry':
                write (self.formatEntry (indent, self.mapEntry (key)))
                write (string.strip (object.text ()) + '\n')
            elif className == 'SeeAlsoEntry':
                write (self.formatEntry (indent, 'see'))
                validLines = filter (None, map (string.strip, object.entries))
                if not validLines:
                    write ('\n')
                else:
                    write (validLines [0] + '\n')
                    for line in validLines [1:]:
                        write (indent + (' ' * 18) + line + '\n')
            elif className == 'ArgumentsEntry':
                for argname, direction, argdesc in object.args:
                    write (self.formatEntry (indent, 'param'))
                    write ('%s %s %s\n' % (argname, direction, 
                        string.strip (argdesc)))
            elif className == 'CodeEntry':
                write (self.formatEntry (indent, 'code') + '\n')
                lines = string.split (object.source, '\n')
                for line in lines:
                    write (indent + '   ' + line + '\n')
                write (indent + self.formatEntry (indent, 'endcode') + '\n')
            elif className == 'ExceptionsEntry':
                for classname, desc in object.exceptionList:
                    write (self.formatEntry (indent, 'exception'))
                    write ("%s %s\n" % (classname, desc))
            elif className == 'ObsoleteEntry':
                write (self.formatEntry (indent, 'deprecated'))
                self.writeDescription (object, indent)
            else:
                write (indent + '  ??' + key + str (object) + '\n')
        if description:            
            self.writeDescription (description, indent, entriesUsed = 1)
    
    def formatEntry (self, indent, key):
        return indent + '  @' + ('%-15s' % string.lower (key))
        
    def writeDescription (self, description, indent = '', entriesUsed = 0):
        # self.stream.write (indent + 'yadda yadda yadda')
        write = self.stream.write
        if len (description.paragraphs) <= entriesUsed:
            # first paragraph handled with @brief entry
            return
        write ('\n\n')
        for paragraph in description.paragraphs [entriesUsed:]:
            if isinstance (paragraph, SimpleParagraph):
                lines = string.split (paragraph.text, '\n')
                for line in lines:
                    write (indent + '  ' + line + '\n')
            elif isinstance (paragraph, SimpleList):
                for entry in paragraph.entries:
                    write (indent + '  - ' + entry + '\n')
                    
    
    copyrightRE = re.compile (r'Copyright \(C\) (\d+)(-\d+)? SAP AG')
    def extractLicence (self, comment):
        licpos = string.find (comment, '========== licence begin')
        if licpos == -1:
            return comment, None
        licstart = string.rfind (comment, '\n', 0, licpos)
        licend = string.find (comment, '========== licence end')
        licend = string.find (comment, '\n', licend)
        licence = comment [licstart:licend]
        match = self.copyrightRE.search (licence)
        if match:
            copstart, copend = match.span ()
            startyear = match.group (1)
            if startyear == _thisYear:
                yearstring = _thisYear
            else:
                yearstring = startyear + '-' + _thisYear
            copyright = 'Copyright (C) ' + yearstring + ' SAP AG'
            licence = '\n\\if EMIT_LICENCE' + licence [:copstart] + copyright + licence [copend:] + '\n\\endif'
        comment = comment [:licstart] + comment [licend:]
        return comment, licence
    
        
    def getMinimalSources (self):
        result  = []
        for category, values in self.categoryRefs.sorted ():
            fname = values [0]
            result.append ((fname, category))
        for category, values in self.entryRefs.sorted ():
            fname = values [0]
            result.append ((fname, category))
        result.sort ()
        for fname, category in result:
            print fname, '#', category
        
class Cppdoc2Clean (AbstractRipCppdoc):
    def handleComment (self, comment, indent):
        pass

class Cppdoc2DoxygenSimple (AbstractRipCppdoc):
    def handleComment (self, comment, indent):
        for search, replace in [
                ('module', 'file'),
                ('class',  'class'),
                ('return value', 'return'),
                ('returns', 'return'),
                ('arguments', 'param'),
                ('see also','see'),
                ('responsible', 'author'),
                ('function', 'fn'),
                ('description', 'brief'),
                ]:
            comment = string.replace (comment, ' ' + search + ':', ' @' + replace)
        self.stream.write ('/*!' + comment + '*/')
            
class Bag (UserDict):
    def add (self, key, value):
        try:
            set = self [key]
        except KeyError:
            set = {}
            self[key] = set
        set [value] = 1
    
    def sorted (self):
        items = self.items ()
        items.sort ()
        result = []
        for key, values in items:
            sortedValues = values.keys ()
            sortedValues.sort ()
            result.append ((key, sortedValues))
        return result
    
    def dump (self):
        for category, values in self.sorted ():
            print '=== ' + category + ':'
            for value in values:
                print '   ', value
    
def getfilecontent (fname):
    if _usePerforce:
        os.system ('p4 edit ' + fname)
        stream = os.popen ('p4 print -q ' + fname + '#head', 'r')
        data = stream.read ()
        stream.close ()
    else:
        if not os.path.exists (fname):
            return ''
        stream = open (fname, 'r')
        data = stream.read ()
        stream.close ()
    return data

def safeOpen (fname, mode = 'w'):
    dir = os.path.dirname (fname)
    if dir and not os.path.isdir (dir):
        os.makedirs (dir)
    return open (fname, mode)
    
def stripComment (line):
    pos = string.find (line, '#')
    if pos == -1:
        return line
    else:
        return string.strip (line[:pos])
    
def getArgs (options, args):
    if options.inputfiles:
        text = getfilecontent (options.inputfiles)
        lines = string.split (text, '\n')
        lines = map (stripComment, lines)
        lines = filter (None, lines)
        args = lines + args
    return args

def main (options, args):
    """Converts cppDoc comments to doxygen comments
    """
    global _usePerforce
    _usePerforce = options.perforce
    if options.clean:
        extractor = Cppdoc2Clean ()
    elif options.simple:
        extractor = Cppdoc2DoxygenSimple ()
    else:
        extractor = Cppdoc2Doxygen (options.deco, options.decosize)
    try:
        for arg in getArgs (options, args):
            if options.verbose:
                print arg
            extractor.extract (arg)
    except KeyboardInterrupt:
        pass
    #extractor.categoryRefs.dump ()
    #extractor.entryRefs.dump ()
    #extractor.getMinimalSources ()

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('I', 'inputfiles', ':', None, 'file containing input files'),
        ('deco', None, ':', None, 'fill first and last line of comment with this string'),
        ('decosize', None, ':', 72, 'fill deco to this col'),
        ('simple', None, '', None, 'simple replacement of tags'),
        ('clean', None, '', None, 'remove cppdoc comments'),
        ('perforce', None, '', None, 'read file from perforce server'),
        ('v', 'verbose', '', None, 'verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
