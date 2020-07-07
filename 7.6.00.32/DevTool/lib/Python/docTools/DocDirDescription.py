#
#
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


from FOM.FOM import XMLTag, XMLList
from FOM.Reader import Reader

class Directory (XMLTag):
    pass

class Info (XMLTag):
    pass

class Documents (XMLList):
    pass

class FilesToIgnore (XMLList):
    pass

class LayoutGroup (XMLList):
    pass

class LayoutMember (XMLTag):
    pass

class Document (XMLTag):
    ref = None
    file = None
    pass

class File (XMLTag):
    pass

class Ref (XMLTag):
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

class FilePattern (XMLTag):
    pass

class DocDirDescriptionFactory (XMLTag):
    rootTag = 'directory'
    def create (self, tag):
        constructor = globals () [tag]
        return constructor ()

class DddReader (Reader):
    def __init__ (self):
        Reader.__init__ (self, DocDirDescriptionFactory() )

def parseData (data):
    reader = DddReader ()
    reader.feed (data)
    return reader.getRootTag ()

