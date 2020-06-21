
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

class NoWeaver:
    def __init__ (self):
        pass

    def setCurrentFile (self, fname):
        pass

    def removeCurrentEntries (self):
        pass

    def addClass (self, className, anchor, description = None):
        pass

    def addMethod (self, className, memberName, anchor = None, description = None):
        pass

    def addFunction (self, functionName, anchor = None, description = None):
        pass

    addRoutine = addFunction

    def getClassLink (self, classname, text = None):
        if text == None:
            result = classname
        else:
            result = text
        return result

    def getMethodLink (self, className, memberName, text):
        """returns a html link for the given class."""
        if text == None:
            text = className + '::' + memberName
        return text

    def getFunctionLink (self, functionName, text):
        return text

    def addAnything (self, name, description = None):
        pass

    def endTransaction (self, ok):
        pass

    def setSuperclasses (self, className, superClasses, docFile = None):
        pass

    def getClassInfo (self, className, methodNames):
        return []

