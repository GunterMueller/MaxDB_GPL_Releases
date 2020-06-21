
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

class DocHeader:
    #
    # context managament
    #
    def __init__ (self, top, indexNames):
        self.top = top
        self.packageStack = []
        self.indexNames = indexNames

    def pushPackage (self, packageFile):
        self.packageStack.append (packageFile)

    def popPackage (self):
        self.packageStack.pop ()

    #
    # generation of navigation bar
    #
    def topHeader (self, file):
        result = (
            _navbarStart
            + self.topTitle ()
            + self.upLink (file)
            + self.indexLink (file)
            + self.classIndexLink (file)
            + _navbarEnd)
        return result

    def packageHeader (self, file):
        result = (
            _navbarStart
            + self.topLink (file)
            + self.upLink (file)
            + self.indexLink (file)
            + self.classIndexLink (file)
            + _navbarEnd)
        return result

    def sourceHeader (self, file):
        result = (
            _navbarStart
            + self.topLink (file)
            + self.upLink (file)
            + self.indexLink (file)
            + self.classIndexLink (file)
            + _navbarEnd)
        return result
        pass

    def indexHeader (self, file):
        result = (
            _navbarStart
            + self.topLink (file)
            + self.upLink (file)
            + self.indexTitle ()
            + self.classIndexLink (file)
            + _navbarEnd)
        return result
        pass

    def classIndexHeader (self, file):
        result = (
            _navbarStart
            + self.topLink (file)
            + self.upLink (file)
            + self.indexLink (file)
            + self.classIndexTitle ()
            + _navbarEnd)
        return result

    #
    # navigation bar elements
    #
    def topLink (self, file):
        relative = self.top.relativeTo (file.directory ())
        # print 'topLink from', file, 'to', self.top, '=', relative
        return _linkCell % (relative, 'Top')

    def upLink (self, file):
        try:
            up = self.packageStack [-1]
            relative = up.relativeTo (file.directory ())
            # print 'upLink from', file, 'to', up, '=', relative
            result = _linkCell % (relative, 'Up')
        except IndexError:
            result = _invalidCell % 'Up'
        return result

    def basicIndexLink (self, file, no, title):
        try:
            indexFile = self.indexNames.getIndexFiles () [no]
            relative = indexFile.relativeTo (file.directory ())
            result = _linkCell % (relative, title)
        except IndexError:
            result = _invalidCell % title
        return result

    def indexLink (self, file):
        return self.basicIndexLink (file, 1, 'Index')

    def classIndexLink (self, file):
        return self.basicIndexLink (file, 0, 'Class Index')

    def topTitle (self):
        return _activeCell % 'Top'

    def packageTitle (self):
        return _activeCell % 'Package'

    def indexTitle (self):
        return _activeCell % 'Index'

    def classIndexTitle (self):
        return _activeCell % 'Class Index'


_navbarStart = """<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="3">
  <TR ALIGN="center" VALIGN="top">
"""

_navbarEnd = """
  </TR>
</TABLE>
"""

_activeCell = '<TD BGCOLOR="#FFFFFF" ID="NavBarCell1Rev"> &nbsp;<FONT ID="NavBarFont1Rev"><B>%s</B></FONT>&nbsp;</TD>\n'
_linkCell = '<TD BGCOLOR="#EEEEFF" ID="NavBarCell1">    <A HREF="%s"><FONT ID="NavBarFont1"><B>%s</B></FONT></A>&nbsp;</TD>\n'
_invalidCell = '<TD BGCOLOR="#FFFFFF" ID="NavBarCell1Rev"> &nbsp;<FONT ID="NavBarFont1">%s</FONT>&nbsp;</TD>\n'

