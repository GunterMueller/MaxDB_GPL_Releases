
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

import string
import regex
import re
import time
import rfc822

from html import *
from acquisition import Acquest
import docLib

class Entries (Acquest):
    def __init__ (self, context):
        self.__items = []

    def __len__ (self):
        return len (self.__items)

    def __getitem__ (self, index):
        return self.__items [index]

    def items (self):
        return self.__items

    def addItem (self, category, value):
        self.__items.append ((category, value))
        value.setContext (self)

    def insertItem (self, index, category, value):
        self.__items.insert (index, (category, value))
        value.setContext (self)

    def findCategory (self, categoryParm, default = None):
        for category, value in self.__items:
            if categoryParm == string.lower (category):
                return value
        return default

    def uncoupleSubDependents (self):
        self.uncoupleDependentsInList (self.__items)


class DocEntry (Acquest):
    def __init__ (self, context):
        self.setContext (context)

    def shortForm (self):
        return None

    def unXML (self, text):
        pxml = self.acquire ('pxml')
        result = pxml.translate (self, text)
        return result

    def unXMLLines (self, lines):
        text = string.join (lines, '\n')
        newText = self.unXML (text)
        result = string.split (newText, '\n')
        return result

class SimpleEntry (DocEntry):
    def __init__ (self, context, lines):
        DocEntry.__init__ (self, context)
        self.lines = lines

    def genHTML (self, htmlstream):
        for line in self.lines:
            htmlstream.write (line)
            htmlstream.lineBreak ()

    def text (self):
        return string.join (self.lines, '\n')

class ResponsibleEntry (DocEntry):
    def __init__ (self, context, lines):
        DocEntry.__init__ (self, context)
        self.lines = lines

    mailtoRE = re.compile ('[-a-z.0-9]+@[-a-z.0-9]+', re.I)

    def genHTML (self, htmlstream):
        outlines = []
        for line in self.lines:
            match = self.mailtoRE.search (line)
            if match:
                begin, end = match.span ()
                address = match.group (0)
                link = href ('mailto:' + address, address)
                line = line [:begin] + link + line [end:]
            outlines.append (line)
        full =string.join (outlines, '<BR>\n')
        htmlstream.write (full + '<BR>\n')

class PrototypesEntry (DocEntry):
    def __init__ (self, context, lines):
        DocEntry.__init__ (self, context)
        self.count = int (string.strip (lines [0]))

class SeeAlsoEntry (DocEntry):
    def __init__ (self, context, lines):
        DocEntry.__init__ (self, context)
        self.entries = lines

    def genHTML (self, htmlstream):
        for entry in self.entries:
            entry = string.strip (entry)
            if entry:
                htmlstream.write (href (entry) + '<BR>\n')


_listRE = regex.compile (' *[-*] *')

class DescriptionEntry (DocEntry):
    def __init__ (self, context, lines):
        DocEntry.__init__ (self, context)
        lines = self.unXMLLines (lines)
        self.breakParagraphs (lines)

    def breakParagraphs (self, lines):
        paragraphs = []
        inSpace = 1
        strip = string.strip
        currentParagraph = None
        for line in lines:
            bareLine = strip(line)
            if len (bareLine) == 0:
                if inSpace:
                    pass    # ignore
                else:
                    if currentParagraph != None:
                        paragraphs.append (self.createParagraph (currentParagraph))
                    currentParagraph = None
                    inSpace = 1
            else:
                if inSpace:
                    currentParagraph = [line]
                    inSpace = None
                else:
                    currentParagraph.append (line)
        if currentParagraph != None:
            paragraphs.append (self.createParagraph (currentParagraph))
        self.paragraphs = paragraphs

    def createParagraph (self, lines):
        re = _listRE
        if re.match (lines [0]) != -1:
            result = SimpleList (lines, re)
        else:
            result = SimpleParagraph (lines)
        return result

    def shortDesc (self):
        paragraphs = self.paragraphs
        if len (paragraphs) > 0:
            result = paragraphs [0].text
        else:
            result = ''
        return result

    def genHTML (self, htmlstream):
        for paragraph in self.paragraphs:
            paragraph.genHTML (htmlstream)


class ReleaseEntry (DocEntry):
    def __init__ (self, context, release):
        DocEntry.__init__ (self, context)
        self.release = release

    def genHTML (self, htmlstream):
        htmlstream.write (self.release)

    def text (self):
        return self.release

class SimpleParagraph:
    def __init__ (self, lines):
        self.text = string.join (lines, '\n')

    def genHTML (self, htmlstream):
        htmlstream.write (self.text)
        htmlstream.tag ('p')

    def genLastHTML (self, htmlstream):
        htmlstream.write (self.text)

class SimpleList:
    text = ''
    def __init__ (self, lines, re):
        entries = []
        currentEntry = ''
        for line in lines:
            matchSize = re.match (line)
            if matchSize == -1:
                currentEntry = currentEntry + ' ' + line
            else:
                entries.append (currentEntry)
                currentEntry = line [matchSize:]
        entries.append (currentEntry)
        self.entries = entries [1:]

    def genHTML (self, htmlstream):
        htmlstream.pushTag ('ul')
        for entry in self.entries:
            htmlstream.pseudoBreak ()
            htmlstream.tag ('li')
            htmlstream.write (entry)
        htmlstream.pseudoBreak ()
        htmlstream.popTag ()

    genLastHTML = genHTML

class CopyrightEntry (SimpleEntry):
    def __init__ (self, context, lines):
        DocEntry.__init__ (self, context)
        self.text = string.join (lines)

    def genHTML (self, htmlstream):
        htmlstream.write ('&copy; ' + self.text + '<br>')

def qbullet (text):
    return ('<IMG SRC="/Images/qbullets/%s.gif" ALT="%s" WIDTH="28 HEIGHT="8">'
        % (text, text))

def asText (text):
    return '<FONT COLOR="red">%s</FONT>' % text

emitter = asText

oneDay = 24 * 60 * 60
now = time.time ()

_ANSI_Format = re.compile ('(?P<year>[0-9]{4})-(?P<month>[0-9]{2})-(?P<day>[0-9]{2})( +[0-9]{2}:[0-9]{2})?')
_German_Format = re.compile ('(?P<day>[0-9]+). *(?P<month>[0-9]+). *(?P<year>[0-9]{4})')
_allowedFormats = [_ANSI_Format, _German_Format]

def parseDateString (text):
    # try RFC 822 date format.
    try:
        timeTuple = rfc822.parsedate(text)
    except:
        timeTuple = None
    if timeTuple == None:
        timeList = [0] * 9
        match = None
        for format in _allowedFormats:
            match = format.match (text)
            if match:
                break
            if not match:
                return 0   # error return
        year, month, day = match.group ('year', 'month', 'day')
        timeList [0] = int (year)
        timeList [1] = int (month)
        timeList [2] = int (day)
        timeTuple = tuple (timeList)
    return time.mktime (timeTuple), timeTuple

class DatedEntry (DocEntry):
    diffDays = 30

    def __init__ (self, context, lines):
        DocEntry.__init__ (self, context)
        text = string.strip (string.join (lines))
        self.text = text
        result = parseDateString (text)
        if result:
            self.date, self.dateTuple = result
        else:
            print "Can't parse data '%s'" % text
            self.date = 0
            self.dateTuple = time.gmtime (0)

    def shortForm (self):
        if now - self.date < self.diffDays * oneDay:
            return emitter (self.kind)
        else:
            return None

    def genHTML (self, htmlstream):
        htmlstream.write (time.strftime ("%Y-%m-%d<BR>\n", self.dateTuple))

class NewEntry (DatedEntry):
    kind = 'New!'

class UpdatedEntry (DatedEntry):
    kind = 'Updated!'

class ObsoleteEntry (DescriptionEntry):
    def shortForm (self):
        return asText ('Obsolete!')

class SuperclassesEntry (SimpleEntry):
    def __init__ (self, context, classlist, weaver):
        SimpleEntry.__init__ (self, context, '')
        self.classlist = classlist
        self.weaver = weaver

    def genHTML (self, htmlstream):
        for name, visibility, isTemplate in self.classlist:
            if visibility != 'private':
                classLink = self.weaver.getClassLink (name)
                htmlstream.write (classLink + '<BR>\n')

_argdescRE = regex.compile (' *\([a-zA-Z_][a-zA-Z0-9_]*\) +\(\[[^]]+]\)\(.*\)')
_varargdescRE = regex.compile (' *\([.][.][.]\) +\(\[[^]]+]\)\(.*\)')

class ArgumentsEntry (DocEntry):
    def __init__ (self, context, lines):
        DocEntry.__init__ (self, context)
        lines = self.unXMLLines (lines)
        args = []
        re = _argdescRE
        vre = _varargdescRE
        argname = ''
        direction = ''
        currentDesc = ''
        for line in lines:
            for matchRE in [re, vre]:
                matchSize = matchRE.match (line)
                if matchSize != -1:
                    break
            if matchSize == -1:
                currentDesc = currentDesc + '\n' + line
            else:
                args.append ((argname, direction, currentDesc))
                argname, direction, currentDesc = matchRE.group (1, 2, 3)
        args.append ((argname, direction, currentDesc))
        self.args = args [1:]

    def genHTML (self, htmlstream):
        if self.args:
            htmlstream.pushTag ('dl')
            for argname, direction, argdesc in self.args:
                htmlstream.tag ('dt')
                htmlstream.write (tagged ('code', argname))
                htmlstream.write (' ' + tagged ('i', direction))
                htmlstream.tag ('dd')
                htmlstream.write (argdesc)
            htmlstream.popTag ()
            htmlstream.lineBreak ()
        else:
            htmlstream.write ('none')
            htmlstream.lineBreak ()

    def argumentNames (self):
        return map (lambda t: t[0], self.args)

class CodeEntry (SimpleEntry):
    def __init__ (self, context, text):
        SimpleEntry.__init__ (self, context, 'code')
        source = string.join (text, '\n')
        self.source = docLib.removeIndentFromSource (source, None)

    def genHTML (self, htmlstream):
        htmlstream.pushTags ('pre', 'code')
        source = _cColorizer.convertString (self.source)
        htmlstream.write (source)
        #for line in self.lines:
        #    htmlstream.write (line)
        #    htmlstream.pseudoBreak ()
        htmlstream.popTag (2)

class DefListEntry (SimpleEntry):
    def __init__ (self, context, kind, text):
        SimpleEntry.__init__ (self, context, kind)
        self.parseText (self.unXMLLines (text))
        self.weaver = None

    excRE = re.compile ('^([a-z0-9_]+)', re.M | re.I)

    def parseText (self, text):
        strip = string.strip
        text = string.join (docLib.removeIndentFromLines (text, 1), '\n')
        try:
            parts = self.excRE.split (text)
            items = []
            if not strip (parts [0]):
                del parts [0]
            for i in xrange (0, len (parts), 2):
                items.append ((parts[i], strip (parts [i + 1])))
            self.items = items
        except IndexError:
            # quick fix for badly formatted comment
            self.tmpText = text

    def genHTML (self, htmlstream):
        # quick fix for badly formatted comment
        if hasattr (self, 'tmpText'):
            htmlstream.pushTag ('pre')
            htmlstream.write (self.tmpText)
            htmlstream.popTag ()
            return
        # normal formatting
        htmlstream.pushTag ('dl')
        for name, description in self.items:
            htmlstream.pseudoBreak ()
            htmlstream.tag ('dt')
            htmlstream.write (self.formatDefinition (name))
            htmlstream.tag ('dd')
            if description:
                htmlstream.write (description)
        htmlstream.pseudoBreak ()
        htmlstream.popTag ()
        htmlstream.pseudoBreak ()

class ExceptionsEntry (SimpleEntry):
    def __init__ (self, context, text):
        SimpleEntry.__init__ (self, context, 'exceptions')
        self.parseText (text)
        self.weaver = None

    excRE = re.compile ('^([a-z0-9_]+)', re.M | re.I)

    def parseText (self, text):
        strip = string.strip
        text = string.join (docLib.removeIndentFromLines (text, 1), '\n')
        parts = self.excRE.split (text)
        exceptions = []
        if not strip (parts [0]):
            del parts [0]
        for i in xrange (0, len (parts), 2):
            exceptions.append ((parts[i], strip (parts [i + 1])))
        self.exceptionList = exceptions

    def genHTML (self, htmlstream):
        htmlstream.pushTag ('ul')
        for classname, description in self.exceptionList:
            htmlstream.pseudoBreak ()
            htmlstream.tag ('li')
            if self.weaver:
                classLink = self.weaver.getClassLink (classname)
            else:
                classLink = classname
            htmlstream.write (tagged ('b', classname))
            if description:
                htmlstream.write (' - ' + description)
        htmlstream.pseudoBreak ()
        htmlstream.popTag ()
        htmlstream.pseudoBreak ()

def stdCategoryHandler ():
    return {
        'description': DescriptionEntry,
        'htmldescription': DescriptionEntry,
        'prototypes' : PrototypesEntry,
        'return value': DescriptionEntry,
        'returns': DescriptionEntry,
        'see also': SeeAlsoEntry,
        'see': SeeAlsoEntry,
        'responsible': ResponsibleEntry,
        'copyright': CopyrightEntry,
        'new': NewEntry,
        'created': NewEntry,
        'updated': UpdatedEntry,
        'obsolete': ObsoleteEntry,
        'exceptions': ExceptionsEntry,
        'arguments': ArgumentsEntry,
        'code': CodeEntry,
        'example': CodeEntry,
        }

import c2html
_cColorizer = c2html.C2html ()

