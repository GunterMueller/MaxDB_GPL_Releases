
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

    def convertString (self, source):
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


