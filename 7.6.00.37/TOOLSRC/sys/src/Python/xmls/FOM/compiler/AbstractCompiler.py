
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

class NotImplemented:
    def __init__ (self, clazzName, methodName):
        self.clazzName = clazzName
        self.methodName = methodName

    def __str__ (self):
        return 'NotImplemented: ' + self.clazzName + '::' + self.methodName


class AbstractCompiler:
    def __init__ (self):
        pass

    def notImplemented (self, methodName):
        raise NotImplemented (self.__clazz__.__name__, methodName)

    def defaultName (self, schemaName):
        self.notImplemented ()

    def compileSchema (self, schemical, stream):
        self.genProlog (schemical, stream)
        self.genSchemaProperties (schemical, stream)
        self.genElements (schemical, stream)
        self.genEpilog (schemical, stream)

    def genProlog (self, schemical, stream):
        pass

    def genEpilog (self, schemical, stream):
        pass

    def genSchemaProperties (self, schemical, stream):
        pass

    def genElements (self, schemical, stream):
        for element in schemical.elements:
            self.genElement (element, stream)

    def genElement (self, element, stream):
        self.notImplemented ('genElement')


