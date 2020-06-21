
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

class Ifdef:
    def __init__ (self, filename, mainDef = None):
        self.filename = filename
        self.mainDef = mainDef

    def __getitem__ (self, var):
        result = '#define %s\n#include "%s"\n#undef %s\n' % (var, self.filename, var)
        if self.mainDef:
            result = '#define %s\n%s#undef %s\n' % (self.mainDef, result, self.mainDef)
        return result

    def subSnippets (self, subkey):
        return Ifdef (self.filename, subkey)
