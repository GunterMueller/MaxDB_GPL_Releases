
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
"""undiffLib.py     remove std diffs from diff output
"""

from string import strip
import re

change   = '[0-9]+(,[0-9]+)?c[0-9]+(,[0-9]+)?'
addition = '[0-9]+(,[0-9]+)?a[0-9]+(,[0-9]+)?'
deletion = '[0-9]+(,[0-9]+)?d[0-9]+(,[0-9]+)?'
_emptyAdd = addition + '(\n>)+'
_emptyDel = deletion + '(\n<)+'

class Undiffer:
    def __init__ (self, pdiff):
        self.pdiff = pdiff
        self.data = open (pdiff, 'r').read ()

    def write (self):
        open (self.pdiff, 'w').write (strip (self.data))

    def removeAll (self, pattern):
        self.data = re.sub (pattern, '', self.data)

    def removeConditional (self, condition, pattern):
        self.data = re.sub (pattern,
            CondToReplacement (condition).replace,
            self.data)

    def removeEmptyLines (self):
        self.removeAll (_emptyAdd)
        self.removeAll (_emptyDel)

    def runSafe (self, callable):
        try:
            callable (self)
        except:
            import traceback, sys
            errfile = open (self.pdiff, 'a')
            traceback.print_exc (10, errfile)
            errfile.close ()
            sys.exit (5)

class CondToReplacement:
    def __init__ (self, condition):
        self.condition = condition

    def replace (self, match):
        if self.condition (match):
            result = ''
        else:
            result = match.group (0)
        return result

