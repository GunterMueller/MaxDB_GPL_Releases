# FOM.py
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


from UserList import UserList

class XMLTag:
    isList = None
    attributeNames = []
    subtags = []

    def __init__ (self, tag = None):
        if tag == None:
            self.__tag = self.__class__.__name__
        else:
            self.__tag = tag

    def __str__ (self):
        return '<' + self.__tag + '>'

    def getTag (self):
        return self.__tag

class XMLList (XMLTag, UserList):
    isList = 1
    def __init__ (self):
        XMLTag.__init__ (self)
        UserList.__init__ (self)

    def __str__ (self):
        return XMLTag.__str__ (self) + '(*)'


