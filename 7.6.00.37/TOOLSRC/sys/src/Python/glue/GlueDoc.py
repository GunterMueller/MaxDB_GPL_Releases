
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

class GlueDoc:
    isMethod = None
    def __init__ (self, name, doc, language = None):
        self.pyname = name
        self.doc = doc
        self.language = language

    def writeGlue (self, glue):
        pass

    def methods (self):
        return []

    def isRealConstructor (self):
        return None

    def supportFor (self, key, value):
        if key != 'language':
            return 1
        if self.language == None:
            return 1
        return self.language == value

    def isDocumented (self):
        return 1

class GlueExample (GlueDoc):
    language = None
    def __init__ (self, name, doc, code = []):
        self.pyname = name
        self.doc = doc
        self.code = code

    def getCode (self):
        return code

class GlueImportExample (GlueExample):
    def __init__ (self, name, comment, fname):
        GlueExample.__init__ (self, name, comment)
        self.fname = fname

    def getCode (self):
        try:
            data = open (self.fname, "r").read ()
        except IOException:
            data = "Access to '%s' has been denied" % self.fname
        return data

class GlueExternExample (GlueExample):
    def __init__ (self, name, comment, fname):
        GlueExample.__init__ (self, name, comment)
        self.fname = fname

    def getCode (self):
        return None

class GlueDirectory (GlueDoc):
    def __init__ (self, name, language, items):
        GlueDoc.__init__ (self, name, '', language)
        self.items = items

    def methods (self):
        return []

