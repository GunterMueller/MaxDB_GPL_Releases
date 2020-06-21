
# incldump.py
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
import vmake.vmakeLib

class IncludeBuilder:
    def __init__ (self, module):
        self.module = module
        self.parser = parser = vmake.vmakeLib.VmakeParser ()
        parser.registerHandler (None, self.ignore)
        parser.registerHandler ('include', self.addModule)
        parser.registerHandler ('module', self.addModule)
        self.ntIncludes = []
        self.unixIncludes = []

    def addModule (self, item):
        if item.name == self.module:
            includes = []
            for name, speed in item.dependencies:
                if string.find (name, '.') == -1:
                    # no extension
                    name = name + '.h'
                includes.append (name)
            if self.currentOS == 'nt':
                self.ntIncludes = includes
            else:
                self.unixIncludes = includes

    def getNTInfo (self):
        self.currentOS = 'nt'
        self.parser.parseCollection (self.module)

    def getUnixInfo (self):
        self.currentOS = 'unix'
        self.parser.parseRemoteCollection ('sun', self.module)

    def buildIncludeStatements (self):
        if (self.ntIncludes == self.unixIncludes):
            for file in self.ntIncludes:
                print '#include "%s"' % file
        elif len (self.ntIncludes) == 0:
            # assume module is relevant only for Unix
            for file in self.unixIncludes:
                print '#include "%s"' % file
        elif len (self.unixIncludes) == 0:
            # assume module is relevant only for NT
            for file in self.ntIncludes:
                print '#include "%s"' % file
        else:
            print '#if defined (_WIN32) /*&if $OS = WIN32 */'
            for file in self.ntIncludes:
                print '#include "%s"' % file
            print '#else                /*&else*/'
            for file in self.unixIncludes:
                print '#include "%s"' % file
            print '#endif               /*&endif*/'

    def ignore (self, item):
        pass


def main (options, args):
    """extracts include information from vmake and builds
    #include statements
    """
    for arg in args:
        builder = IncludeBuilder (arg)
        builder.getNTInfo ()
        builder.getUnixInfo ()
        builder.buildIncludeStatements ()

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

