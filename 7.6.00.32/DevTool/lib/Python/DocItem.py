
import os
import string
import re
import urllib

from html import *
from acquisition import Acquest
from DocEntry import ReleaseEntry, SuperclassesEntry

_backlinkHTML = ('<A HREF="#%s">'
    + '<IMG SRC="http://pwww.bea.sap-ag.de/Images/QBullets/scrollup.gif" '
    + 'ALT="Top" BORDER="0"></A>')

class DocItem (Acquest):
    closeWithLine = None
    def __init__ (self, title, entries = None):
        self.title = title
        self.anchor = urllib.quote (title, '')
        self.entries = entries

    def genTOC (self, htmlstream):
        pass

    def shortDesc (self):
        if self.entries == None:
            return ''
        description = self.entries.findCategory ('description')
        if description != None:
            result = description.shortDesc ()
        else:
            result = ''
        return result

    def tocLink (self):
        property = self.shortProperty ('new', 'created', 'updated', 'obsolete')
        link = href ('#' + self.anchor, self.title)
        shortDesc = property + self.shortDesc ()
        if shortDesc:
            shortDesc = ' - ' + shortDesc
        else:
            shortDesc = ''
        return link + shortDesc + '\n'

    def shortProperty (self, *names):
        for name in names:
            property = self.entries.findCategory (name)
            if property:
                text = property.shortForm ()
                if text:
                    return text + ' '
        return ''

    def genHeader (self, htmlstream):
        if not self.title:
            raise 'empty element'
        backlink = self.acquire ('tocTarget')
        if backlink:
            backlink = ' ' + (_backlinkHTML % backlink)
        else:
            backlink = ''
        fmt = '<H3><A NAME="%s">%s</A>%s</H3>\n'
        result = fmt % (self.anchor, self.title, backlink)
        htmlstream.write (result)
        return result

    def genentries (self, htmlstream, exceptlist = []):
        relevant = []
        for category, value in self.entries.items ():
            if not string.lower (category) in exceptlist:
                relevant.append ((category, value))
        if len (relevant) == 0:
            return
        htmlstream.pushTag ('dl')
        for category, value in relevant:
            htmlstream.tag ('dt')
            htmlstream.write (tagged ('b', category) + ':\n')
            htmlstream.tag ('dd')
            value.genHTML (htmlstream)
            htmlstream.lineBreak ()
        htmlstream.popTag ()

    def genSpecificHTML (self, htmlstream):
        self.genentries (htmlstream)

    def genHTML (self, htmlstream):
        if not self.title:
            return
        self.genHeader (htmlstream)
        htmlstream.pushTag ('blockquote')
        self.genSpecificHTML (htmlstream)
        htmlstream.popTag ()
        if self.closeWithLine:
            htmlstream.line ()

    def readSource (self, source, pos, controller):
        """Read any additional info from the source.

        An example would be prototypes. The method returns the new
        position inside the source.
        """
        return pos

    def addToIndex (self, weaver):
        weaver.addAnything (self.title)

class DocGroup (DocItem):
    def __init__ (self, title = '', superGroup = None, entries = None):
        DocItem.__init__ (self, title, entries)
        self.items = []
        self.superGroup = superGroup
        if superGroup == None:
            self.nesting = 0
        else:
            self.nesting = superGroup.nesting + 1

    def addItem (self, item):
        self.items.append (item)
        item.setContext (self)

    def pushGroup (self, title, entries = None, constructor = None):
        if constructor == None:
            constructor = DocGroup
        newGroup = constructor (title, self, entries)
        self.addItem (newGroup)
        return newGroup

    def popGroup (self):
        if (self.superGroup == None):
            return self
        else:
            return self.superGroup

    def tocLink (self):
        link = DocItem.tocLink (self)
        if self.title:
            self.tocTarget = urllib.quote (self.title + '_TOC', '/')
            result = anchor (self.tocTarget, '') + link
        else:
            result = link
        return result

    def genTOC (self, htmlstream):
        htmlstream.pushTag ('ul')
        htmlstream.pseudoBreak ()
        for item in self.items:
            htmlstream.pushTag ('li')
            htmlstream.write (item.tocLink ())
            item.genTOC (htmlstream)
            htmlstream.popTagSafe ('li')
        htmlstream.popTag ()
        htmlstream.pseudoBreak ()

    def genHTML (self, htmlstream):
        DocItem.genHTML (self, htmlstream)
        for item in self.items:
            item.genHTML (htmlstream)

    def __str__ (self):
        return "<%s '%s' %x>" % (
            self.__class__.__name__, self.title, id (self))

    def dumpString (self):
        selfString = str (self)
        if self.superGroup != None:
            superString = self.superGroup.dumpString ()
            selfString = selfString + ' in ' + superString
        return selfString

    def isTop (self):
        return self.superGroup == None

class ClassGroup (DocGroup):
    def __init__ (self, className, superGroup, entries):
        DocGroup.__init__ (self, 'Class ' + className, superGroup, entries)
        self.className = className
        self.superClasses = []

    def readSource (self, source, pos, controller):
        bracePos = string.find (source, '{', pos)
        if bracePos == -1:
            result = pos
        else:
            self.parseClassInfo (source [pos:bracePos])
            result = bracePos
        return result

    templateRE = re.compile ('template *<')
    classRE = re.compile ('(.*)class +(?:AFX_EXT_CLASS +)?([a-zA-Z0-9_]+)', re.DOTALL)

    def parseClassInfo (self, data):
        data = string.strip (data)
        match = self.templateRE.match (data)
        if match:
            pos = findMatching ('<>', data, match.end (0))
            data = data [pos:]
        match = self.classRE.match (data)
        if match != None:
            className, templateInfo = match.group (2, 1)
            assert self.className == className
            superInfo = string.strip (data [match.end (0):])
            if superInfo:
                self.parseSuperClass (superInfo)

    superClassRE = re.compile (' *(private|protected|public)? *([a-zA-Z0-9_]+) *(<?)')

    def parseSuperClass (self, superInfo):
        superInfo = string.replace (superInfo, '\n', ' ')
        superInfo = string.replace (superInfo, 'virtual', '')
        pos = string.index (superInfo, ':') + 1
        match = self.superClassRE.match (superInfo, pos)
        while match != None:
            visibility, name, templateStart = match.group (1, 2, 3)
            if not visibility:
                visibility = 'private'
            pos = match.end (0)
            if templateStart == '<':
                pos = findMatching ('<>', superInfo, pos)
            self.superClasses.append ((name, visibility, templateStart))
            stopPos = string.find (superInfo, ',', pos)
            if stopPos == -1:
                match = None
            else:
                match = self.superClassRE.match (superInfo, pos + 1)

    def addToIndex (self, weaver):
        weaver.addClass (self.className, self.anchor, self.shortDesc ())
        if self.superClasses:
            superclassesEntry = SuperclassesEntry (self, self.superClasses, weaver)
            self.entries.insertItem (0, 'superclasses', superclassesEntry)

def findMatching (pair, data, pos):
    intro = pair [0]
    exit = pair [1]
    nesting = 1
    while nesting > 0:
        current = data [pos]
        if current == intro:
            nesting = nesting + 1
        elif current == exit:
            nesting = nesting - 1
        pos = pos + 1
    return pos

class MastHead (DocItem):
    closeWithLine = 1

    def __init__ (self, title, entries = None):
        DocItem.__init__ (self, title, entries)
        self.addReleaseEntry ()

    def getRelease (self):
        try:
            releaseFile = os.path.join (os.environ ['OWN'], 'Release.dat')
            result = open (releaseFile, 'r').read ()
            result = string.strip (result)
        except KeyError:
            result = None
        return result

    def addReleaseEntry (self):
        release = self.getRelease ()
        if release != None:
            self.entries.addItem ('release', ReleaseEntry (self, release))

class DocLeaf (DocItem):
    closeWithLine = 1

    def genSpecificHTML (self, htmlstream):
        description = self.entries.findCategory ('description')
        if description != None:
            htmldescription = self.entries.findCategory ('htmldescription')
            if htmldescription != None:
                description.paragraphs = (description.paragraphs
                    + htmldescription.paragraphs)
            description.genHTML (htmlstream)
        self.genentries (htmlstream, ['description', 'htmldescription', 'prototypes'])

