# Reader.py
#
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
import xmllib

from FOM import XMLTag

class Reader (xmllib.XMLParser):
    def __init__ (self, factory):
        xmllib.XMLParser.__init__ (self)
        self.factory = factory
        self.tagStack = [XMLTag ()]
        self.currentText = ''

    def handle_data (self, data):
        self.currentText = self.currentText + data

    def unknown_starttag (self, tag, attributes):
        #print '<' + tag + '>'
        #
        # create tag
        #
        newTag = self.factory.create (tag)
        for key, value in attributes.items ():
            setattr (newTag, key, value)
        self.currentText = ''
        #
        #
        #
        superTag = self.tagStack [-1]
        self.tagStack.append (newTag)
        if superTag.isList:
            #print 'append', newTag, 'to', superTag
            superTag.append (newTag)
        else:
            #
            # set new tag as attribute of enclosing tag
            #
            #print 'insert', newTag, 'into', superTag
            varname = string.lower (tag [0]) + tag [1:]
            setattr (superTag, varname, newTag)

    def unknown_endtag (self, tag):
        #print '</' + tag + '>'
        currentTag = self.tagStack.pop ()
        currentTag.pcdata = self.currentText

    def getRootTag (self):
        return getattr (self.tagStack [0], self.factory.rootTag)

def parseData (factory, data):
    reader = Reader (factory)
    reader.feed (data)
    return reader.getRootTag ()

