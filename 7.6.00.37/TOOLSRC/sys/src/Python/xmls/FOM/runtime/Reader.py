
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#

import string
import xmllib
from FOM import CompiledElement

class SchemaViolation:
    def __init__ (self, msg):
        self.msg = msg

    def __str__ (self):
        return self.msg


class InvalidSubTag (SchemaViolation):
    def __init__ (self, container, tag):
        msg = '<' + tag + '> not allowed in <' + container + '>'
        SchemaViolation.__init__ (self, msg)

class InvalidAttribute (SchemaViolation):
    def __init__ (self, container, attribute):
        msg = attribute + '= not allowed in <' + container + '>'
        SchemaViolation.__init__ (self, msg)


class AbstractReader (xmllib.XMLParser):
    mapping = {}

    def __init__ (self):
        xmllib.XMLParser.__init__ (self)
        self.tagStack = [self.rootContainer ()]

    def new (self, name):
        return self.mapping [name] ()

    def readFile (self, fname):
        stream = open (fname, 'rt')
        result = self.readStream (stream)
        return result

    def readStream_xml (self, stream):
        pass

    def readStream_xmllib (self, stream):
        while 1:
            data = stream.read (4096)
            if not data:
                break
            self.feed (data)
        return self.getRootTag ()

    readStream = readStream_xmllib

    def handle_data (self, data):
        # print '#PCDATA', data
        self.tagStack [-1].addContent (data)

    handle_cdata = handle_data

    def unknown_starttag (self, tag, attributes):
        #print '<' + tag + '>'
        #
        # create tag
        #
        newTag = self.new (tag)
        #newTag.setAttributes (attributes)
        for attr, value in attributes.items ():
            try:
                getattr (newTag, 'set_' + attr) (value)
            except AttributeError:
                raise InvalidAttribute (newTag.__class__.name, attr)
        #
        #
        #
        superTag = self.tagStack [-1]
        self.tagStack.append (newTag)
        if superTag.__class__.isList:
            #print 'append', newTag, 'to', superTag
            superTag.append (newTag)
        else:
            #
            # set new tag as attribute of enclosing tag
            #
            #print 'insert', newTag, 'into', superTag
            # setattr (superTag, tag, newTag)
            try:
                getattr (superTag, 'add_' + tag) (newTag)
            except AttributeError:
                raise InvalidSubTag (superTag.__class__.name, tag)

    def unknown_endtag (self, tag):
        #print '</' + tag + '>'
        self.tagStack.pop ()

    def getRootTag (self):
        return self.tagStack [0].root
        #rootEntries = self.tagStack [0].__dict__.items ()
        #assert len (rootEntries) == 1
        #rootName, rootTag = rootEntries [0]
        #return rootTag

    def rootContainer (self):
        return RootContainer ()

class RootContainer:
    isList = None
    root = None

    def setRoot (self, value):
        self.root = value

    def addContent (self, pcdata):
        # ignore, no data in container allowed
        pass

    def __getattr__ (self, name):
        if name[:4] == 'add_':
            return self.setRoot
        else:
            raise AttributeError, name


