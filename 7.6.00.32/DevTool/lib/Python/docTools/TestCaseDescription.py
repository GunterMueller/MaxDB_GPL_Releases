#
# TestCaseDescription.py
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


import html

from FOM.FOM import XMLTag, XMLList
from FOM.Reader import Reader
from docLib import genDate

class TestCase (XMLTag):
    def genHTMLFile (self, fname):
        stream = open (fname, 'wt')
        self.genHTML (stream)

    def genHTML (self, stream):
        title = 'Testcase: ' + self.info.title.pcdata
        htmlstream = html.HTMLStream (stream, title, bgcolor="white")
        htmlstream.titleHeader (title)
        #
        # info
        #
        htmlstream.write (html.header ('Description', 3))
        htmlstream.pushTag ('BLOCKQUOTE')
        for paragraph in [self.info.abstract, self.info.description]:
            text = paragraph.pcdata
            if text:
                htmlstream.write (html.paragraph (text))
        htmlstream.popTag ()
        #
        # prescription
        #
        htmlstream.write (html.header ('Prescription', 3))
        htmlstream.pushTag ('BLOCKQUOTE')
        prescription = self.prescription
        htmlstream.write (html.paragraph (prescription.kind))
        # properties
        for property in prescription.properties:
            htmlstream.write (html.tagged ('B', property.kind + ':'))
            htmlstream.write (' ' + property.pcdata)
            htmlstream.lineBreak ()
        # files
        if len (prescription.files) > 0:
            htmlstream.write (html.header ('Files', 4))
            htmlstream.pushTag ('UL')
            for file in prescription.files:
                htmlstream.write (html.tagged ('LI', file.pcdata))
                htmlstream.pseudoBreak ()
            htmlstream.popTag ()
        htmlstream.popTag ()
        #
        # close
        #
        htmlstream.line ()
        genDate (htmlstream)
        htmlstream.close ()

    def getTitle (self):
        return self.info.title.pcdata

    def getAbstract (self):
        return self.info.abstract.pcdata

class Info (XMLTag):
    pass

class Title (XMLTag):
    pass

class Abstract (XMLTag):
    pass

class Description (XMLTag):
    pass

class Keywords (XMLList):
    pass

class Keyword (XMLTag):
    pass

class Responsible (XMLTag):
    pass

class Prescription (XMLTag):
    pass

class Properties (XMLList):
    pass

class Property (XMLTag):
    pass

class Files (XMLList):
    pass

class File (XMLTag):
    pass

class TestCaseDescriptionFactory (XMLTag):
    rootTag = 'testCase'
    def create (self, tag):
        constructor = globals () [tag]
        return constructor ()

class TcdReader (Reader):
    def __init__ (self):
        Reader.__init__ (self, TestCaseDescriptionFactory() )

def parseData (data):
    reader = TcdReader ()
    reader.feed (data)
    return reader.getRootTag ()

