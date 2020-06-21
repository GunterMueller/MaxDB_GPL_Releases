
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
import os
import string
import re
import urllib

from html import *
from acquisition import Acquest
from DocEntry import *
import docLib
import mylib

_backlinkHTML = ('<A HREF="#%s">'
    + '<IMG SRC="http://pwww.ber.sap.corp:1111/Images/QBullets/scrollup.gif" '
    + 'ALT="Top" BORDER="0"></A>')

class DocItem (Acquest):
    closeWithLine = None
    def __init__ (self, context, title, entries = None):
        Acquest.__init__ (self, context)
        self.title = title
        anchorBag = self.acquire ('anchorBag')
        if anchorBag:
            self.anchor = anchorBag.addAnchor (title)
        else:
            self.anchor = urllib.quote (title, '')
        self.entries = entries
        if entries:
            entries.setContext (self)

    def uncoupleSubDependents (self):
        if self.entries:
            self.entries.uncoupleDependents ()

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

    def genBacklink (self):
        backlink = self.acquire ('tocTarget')
        if backlink:
            backlink = ' ' + (_backlinkHTML % backlink)
        else:
            backlink = ''
        return backlink

    headerFMT = '<H3><A NAME="%s">%s</A>%s</H3>\n'
    def genHeader (self, htmlstream):
        if not self.title:
            raise 'empty element'
        #fmt = '<H3><A NAME="%s">%s</A>%s</H3>'
        result = self.headerFMT % (self.anchor, self.title, self.genBacklink ())
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
    def __init__ (self, context, title = '', superGroup = None, entries = None):
        DocItem.__init__ (self, context, title, entries)
        self.items = []
        self.superGroup = superGroup
        if superGroup == None:
            self.nesting = 0
        else:
            self.nesting = superGroup.nesting + 1

    def uncoupleSubDependents (self):
        self.uncoupleDependentsInList (self.items)
        DocItem.uncoupleSubDependents (self)

    def addItem (self, item):
        self.items.append (item)
        item.setContext (self)

    def pushGroup (self, title, entries = None, constructor = None):
        if constructor == None:
            constructor = DocGroup
        newGroup = constructor (self, title, self, entries)
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

    headerFMT = ('<TABLE width="90%%" bgcolor="lavender"><TR><TD>\n'
            + '<H3><A NAME="%s">%s</A>%s</H3>\n'
            + '</TD></TR></TABLE>\n')

    def genHTML (self, htmlstream):
        DocItem.genHTML (self, htmlstream)
        htmlstream.line ()
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

    def close (self):
        pass

class ClassGroup (DocGroup):
    def __init__ (self, context, className, superGroup, entries):
        DocGroup.__init__ (self, context, 'Class ' + className, superGroup, entries)
        self.className = className
        self.superClasses = []
        self.methodNames = []

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
            if self.className != className:
                raise docLib.ClassMismatchError (self.className, className)
            self.parseSuperClass (data [match.end (0):])
            weaver = self.acquire ('weaver')
            assert weaver != None
            weaver.setSuperclasses (className, self.publicSuperClasses (),
                self.acquire ('fname'))

    def publicSuperClasses (self):
        result = []
        for name, visibility, dummy in self.superClasses:
            if visibility == 'public':
                result.append (name)
        return result


    superClassRE = re.compile (' *(private|protected|public)? *([a-zA-Z0-9_]+) *(<?)')

    def parseSuperClass (self, superInfo):
        superInfo = string.strip (docLib.removeComments (superInfo))
        if not superInfo:
            return
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

    def close (self):
        """get info about super classes
        """
        weaver = self.acquire ('weaver')
        if not weaver:
            return
        classInfo = weaver.getClassInfo (self.className, self.methodNames)
        for superName, methods in classInfo:
            newItem = Inherited (self, superName, methods)
            self.addItem (newItem)

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

    def __init__ (self, context, title, entries = None):
        # self.checkEntries (entries)
        DocItem.__init__ (self, context, title, entries)
        self.addReleaseEntry ()

    mandatoryItems = ['responsible', 'special area', 'description']
    followUps = [
        ('last changed', ['description']),
        ('see also', ['description', 'last changed']),
        ('see', ['description', 'last changed']),
    ]

    def checkEntries (self, entries):
        for expected, (actual, entry) in map (None, self.mandatoryItems, entries.items () [:3]):
            if expected != actual:
                msg = "Unexpected '" + actual + ":'"\
                    '\nthe following order is mandatory=> responsible:, special area:, description:, [last changed:], [see also:]\n'
                sys.stderr.write ('Warning: ' + msg)
                return
                #raise docLib.InvalidMasthead ('Invalid comment entries: must start with '
                #    + `self.mandatoryItems`)
        tag, entry = entries [-1]
        if tag != 'copyright':
            msg = "Last comment entry must be 'copyright'\n"
            sys.stderr.write ('Warning: ' + msg)
            return
            #raise docLib.InvalidMasthead ("Last comment entry must be 'copyright'")
        for searchtag, precedessors in self.followUps:
            pre = self.findPrecedessor (searchtag, entries)
            if (pre is not None) and (pre not in precedessors):
                msg = "Unexpected '" + searchtag + ":' after '" + pre + ":'"\
                    '\nthe following order is mandatory=> responsible:, special area:, description:, [last changed:], [see also:]\n'
                sys.stderr.write ('Warning: ' + msg)
                return
                #raise docLib.InvalidMasthead (
                #    "Invalid comment entry: '" + searchtag
                #    + "' must follow one of " + `precedessors`)


    def findPrecedessor (self, searchtag, entries):
        for index, tag, entry in mylib.IndexIterator (entries, 1):
            if searchtag == tag:
                prevtag, preventry = prev = entries [index - 1]
                return prevtag
        return None

    def getRelease (self):
        try:
            relver = os.environ ['RELVER']
            if relver [0] == 'R':
                result = relver [1] + '.' + relver [2]
            else:
                result = relver
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

class Inherited (DocItem):
    def __init__ (self, context, superclass, methods):
        DocItem.__init__ (self, context, 'Inherited from ' + superclass)
        self.superclass = superclass
        self.methods = methods

    def shortProperty (self, *args):
        return ''

    def genentries (self, htmlstream):
        weaver = self.acquire ('weaver')
        htmlstream.pushTag ('UL')
        for name, docfile, anchor, abstract in self.methods:
            link = weaver.formatLink (docfile, anchor, name)
            if abstract:
                abstract = ' - ' + abstract
            else:
                abstract = ''
            htmlstream.write ('<li>' + link + abstract + '\n')
        htmlstream.popTag ()


