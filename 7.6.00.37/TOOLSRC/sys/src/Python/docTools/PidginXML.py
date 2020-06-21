
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
import re
import cStringIO

class PidginXML:
    def __init__ (self):
        self.rules = {}

    def addRule (self, rule):
        self.rules [rule.tag] = rule

    tagre = '[-a-zA-Z:_]+'
    tagStartRE = '(?P<start><' + tagre + ')'
    tagEndRE = '(?P<end></' + tagre + '>)'
    ltRE = '(?P<lt><)'
    ampRE = '(?P<amp>&)'

    xre = re.compile (string.join (
        [tagStartRE, tagEndRE, ltRE, ampRE], '|'))

    def translate (self, context, text):
        xre = self.xre
        pos = 0
        match = xre.search (text, pos)
        if not match:
            return text
        result = cStringIO.StringIO ()
        rules = self.rules
        while match:
            matchStart, matchEnd = match.span ()
            result.write (text [pos:matchStart])
            amp, lt, start, end = matches = match.group ('amp', 'lt', 'start', 'end')
            pos = matchEnd
            if amp:
                result.write ('&amp;')
            elif lt:
                result.write ('&lt;')
            elif start:
                tagname = start [1:]
                if tagname and rules.has_key (tagname):
                    rule = rules [tagname]
                    pos, attributes = self.readAttributes (text, matchEnd)
                    if rule.requiresContent:
                        pos, content = self.readContent (text, pos, tagname)
                        result.write (rule.translateAll (context, tagname, attributes, content))
                    else:
                        result.write (rule.translateStart (context, tagname, attributes))

                else:
                    result.write ('&lt;' + tagname)
            elif end:
                tagname = end [2:-1]
                if tagname and rules.has_key (tagname):
                    rule = rules [tagname]
                    result.write (rule.translateEnd (context, tagname))
                else:
                    result.write ('&lt;' + end [1:])
            match = xre.search (text, pos)
        result.write (text [pos:])
        return result.getvalue ()

    attributeRE = re.compile ('(' + tagre + ')="([^"]*)"')

    def readAttributes (self, text, pos):
        lastPos = string.find (text, '>', pos)
        attributeRE = self.attributeRE
        dict = {}
        match = attributeRE.search (text, pos)
        while (lastPos > pos) and match:
            key, value = match.group (1, 2)
            dict [key] = value
            # if lastPos inside match.span: new lastPos
            pos = match.end ()
            match = attributeRE.search (text, pos)
        return lastPos + 1, dict

    def readContent (self, text, pos, tagname):
        fullPattern = '</' + tagname + '>'
        fullPos = string.find (text, fullPattern, pos)
        if fullPos == -1:
            fullPos = len (text)
        abrevPattern = '</>'
        abrevPos = string.find (text, abrevPattern, pos)
        if abrevPos == -1:
            abrevPos = len (text)
        if fullPos < abrevPos:
            matchStart = fullPos
            matchEnd = matchStart + len (fullPattern)
        else:
            matchStart = abrevPos
            matchEnd = matchStart + len (abrevPattern)
        return matchEnd, text [pos:matchStart]

class HTMLRule:
    requiresContent = None

    def __init__ (self, tag, outtag = None):
        self.tag = tag
        if outtag == None:
            self.outtag = tag
        else:
            self.outtag = outtag

    def translateStart (self, context, tag, attributes):
        result = cStringIO.StringIO ()
        result.write ('<' + self.outtag)
        if attributes:
            for key, value in attributes.items ():
                result.write (' ' + key + '="' + value + '"')
        result.write ('>')
        return result.getvalue ()

    def translateEnd (self, context, tag, content = None):
        return '</' + self.outtag + '>'

    def translateAll (self, context, tag, attributes, content):
        return (self.translateStart (context, tag, attributes)
            + content + self.translateEnd (context, tag))

class HTMLContentRule (HTMLRule):
    requiresContent = 1

class HTMLTranslate (HTMLRule):
    def __init__ (self, intag, outtag):
        HTMLRule.__init__ (self, intag, outtag)

