
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
import re
import html
import cStringIO

class Source2html:
    def __init__ (self, keywords, eolComment = None):
        self.keywordsRE = re.compile ('\\b(' + string.join (keywords, '|') + ')\\b')
        self.patterns = [(self.keywordsRE, r'<strong>\1</strong>')]
        if eolComment:
            self.eolCommentRE = re.compile ('(' + eolComment + '[^\n]*)')
            self.patterns.append ((self.eolCommentRE, r'<font color="green">\1</font>'))
        else:
            self.eolCommentRE = None

    def convertString (self, source, *otherArgs):
        source = string.replace (source, '&', '&amp;')
        source = string.replace (source, '<', '&lt;')
        result = cStringIO.StringIO ()
        write = result.write
        keywordsRE = self.keywordsRE
        keywordReplace = r'<strong>\1</strong>'
        eolCommentRE = self.eolCommentRE
        eolCommentReplace = r'<font color="green">\1</font>'
        pos = 0
        match = eolCommentRE.search (source, pos)
        while match != None:
            matchStart, matchEnd = match.span (0)
            write (re.sub (keywordsRE, keywordReplace, source [pos:matchStart]))
            write ('<font color="green">')
            write (source [matchStart:matchEnd])
            write ('</font>')
            pos = matchEnd
            match = eolCommentRE.search (source, pos)
        write (re.sub (keywordsRE, keywordReplace, source [pos:]))
        return result.getvalue ()

    def convertFile (self, inname, outname = None):
        if outname == None:
            outname = inname + '.html'
        result = self.convertString (open (inname, "r").read ())
        htmlstream = html.HTMLFile (outname, inname, bgcolor = "FFFFFF")
        htmlstream.pushTag ('pre')
        htmlstream.write (result)
        htmlstream.close ()


