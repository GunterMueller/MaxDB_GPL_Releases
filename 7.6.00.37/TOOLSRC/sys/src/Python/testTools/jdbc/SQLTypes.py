
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

from JDBCTypes import *

class SQLType:
    def __init__ (self, alias, declarations, compatibleJavaTypes):
        self.alias = alias
        if type (declarations) == type (''):
            declarations = [declarations]
        self.declarations = declarations
        self.compatibleJavaTypes = compatibleJavaTypes

    def __str__ (self):
        return '<SQLType ' + self.alias + '>'

CHAR = SQLType ('CHAR', ['CHAR (25)'], subSet (allTypes, [Bytes]))
INT = SQLType ('INT', ['INTEGER'], [String] + numericTypes)
MONEY = SQLType ('MONEY', ['FIXED (38, 4)'], [String] + numericTypes)
FLOAT = SQLType ('FLOAT', ['FLOAT'], [String] + numericTypes)
BOOLEAN = SQLType ('BOOLEAN', ['BOOLEAN'], [Boolean, String] + numericTypes)

allTypes = [
    CHAR,
    INT,
    MONEY,
    FLOAT,
    BOOLEAN,
]

