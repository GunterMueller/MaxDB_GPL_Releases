
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
import re
import cStringIO

tagEntry = '@([a-z][a-z0-9_]*)[{]'
tagExit = '}@'

class PseudoTeX:
    def __init__ (self):
        self.handler = {}

    tagRE = re.compile ('(%s)|(%s)' % (tagEntry, tagExit), re.IGNORECASE)

    def convertString (self, input):
        output = cStringIO.StringIO ()
        #output = sys.stdout
        tagStack = []
        pos = 0
        tagRE = self.tagRE
        match = tagRE.search (input, pos)
        while match != None:
            start, end = match.span ()
            pendingData = input [pos:start]
            if (end - start) > 2:
                # handle entry tag
                output.write (pendingData)
                name = match.group (2)
                entry, exit = self.findHandler (name)
                output.write (entry (name))
                tagStack.append (exit)
            else:
                # handle exit tag
                exit = tagStack.pop ()
                output.write (exit (pendingData))
            pos = end
            match = tagRE.search (input, pos)
        output.write (input [pos:])
        return output.getvalue ()

    def findHandler (self, name):
        try:
            result = self.handler [name]
        except KeyError:
            result = self.defaultEntry, self.defaultExit
        return result

    def defaultEntry (self, name):
        return '@' + name + '{'

    def defaultExit (self, pendingData):
        return pendingData + '}@'

    def addHTMLTranslation (self, tag, html):
        translator = Translation (html)
        self.handler [tag] = (translator.entry, translator.exit)

    def addHandler (self, tag, entry, exit):
        if entry == None:
            entry = nothingEntry
        if exit == None:
            exit = nothingExit
        self.handler [tag] = (entry, exit)

def nothingEntry (name):
    return ''

def nothingExit (pendingData):
    return pendingData


class Translation:
    def __init__ (self, tag):
        self.tag = tag

    def entry (self, name):
        return '<' + self.tag + '>'

    def exit (self, pendingData):
        return pendingData + '</' + self.tag + '>'

