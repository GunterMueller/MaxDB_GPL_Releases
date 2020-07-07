
import string

import html
import docLib
from FileSystem import File

_allKinds = "CMK"
_singlePageTreshold = 100

class IndexGenerator:
    def __init__ (self, kinds = None, title = None, namebase = None):
        self.docHeader = None
        if kinds == None:
            kinds = _allKinds
        self.kinds = kinds
        self.title = self.getTitle (title, kinds)
        self.namebase = self.getNamebase (namebase, kinds)

    def getTitle (self, title, kinds):
        if title != None:
            return title
        if kinds == "C":
            title = "Class Index"
        else:
            title = "Index"
        return title

    def getNamebase (self, namebase, kinds):
        if namebase != None:
            return namebase
        if kinds == "C":
            namebase = "classindex"
        else:
            namebase = "index" + kinds
        return namebase

    def genIndex (self, targetDir, selection = None):
        entries = []
        for kind in self.kinds:
            self.addEntries (kind, selection, entries)
        if len (entries) > _singlePageTreshold:
            page = self.genMultipageIndex (targetDir, entries)
        else:
            page = self.genSinglepageIndex (targetDir, entries)
        return page

    def addEntries (self, kind, selection, entries):
        if kind == 'C':
            self.addClassEntries (selection, entries)
        else:
            self.addAnyEntries (kind, selection, entries)

    def splitEntries (self, entries):
        entries.sort ()
        lower = string.lower
        result = []
        letters = []
        currentLetter = ''
        for entry in entries:
            name = entry [0]
            if not name:
                # empty entries are ignored
                continue
            letter = name [0]
            if letter != currentLetter:
                currentGroup = []
                result.append (currentGroup)
                currentLetter = letter
                letters.append (letter)
            currentGroup.append (entry)
        return result, letters

    def formatEntry (self, entry, targetDir, htmlstream):
        lowerName, name, context, description, docfile, anchor = entry
        linkFile = File (docfile, targetDir.filesystem)
        relative = linkFile.relativeTo (targetDir)
        link = relative + '#' + anchor
        htmlstream.tag ('dt')
        htmlstream.write (html.href (link, name))
        htmlstream.write (' (' + context + ')\n')
        if description:
            htmlstream.tag ('dd')
            htmlstream.write (description)

    def genSinglepageIndex (self, targetDir, entries):
        groups, letters = self.splitEntries (entries)
        targetFile = targetDir.join (self.namebase + '.html')
        htmlstream = self.htmlProlog (targetFile, self.title)
        htmlstream.write (self.singlePageLetterIndex (letters))
        for group in groups:
            # write anchor
            letter = string.lower (group [0][0][0])  # entry->name->[0]
            anchor = html.anchor (letter, letter)
            header = html.header (anchor, 3)
            htmlstream.write (header + '\n')
            # write entries
            htmlstream.pushTag ('dl')
            for entry in group:
                self.formatEntry (entry, targetDir, htmlstream)
            htmlstream.popTag ()
        self.htmlEpilog (htmlstream)
        return targetFile

    def singlePageLetterIndex (self, letters):
        if len (letters) == 0:
            return 'No entries found\n'
        href = html.href
        result = href ('#' + letters [0], letters [0])
        for letter in letters [1:]:
            result = result + ' | ' + href ('#' + letter, letter)
        return result + '\n'

    def genMultipageIndex (self, targetDir, entries):
        groups, letters = self.splitEntries (entries)
        #
        # create entry file
        #
        entryFile = targetDir.join (self.namebase + '.html')
        title = self.title
        # prolog
        htmlstream = self.htmlProlog (entryFile, title)
        htmlstream.write (self.multiPageLetterIndex ('', letters))
        # epilog
        self.htmlEpilog (htmlstream)
        #
        # create real index files
        #
        for group in groups:
            letter = string.lower (group [0][0][0])  # entry->name->[0]
            targetFile = targetDir.join (self.namebase + '-' + letter + '.html')
            title = self.title + ': ' + string.upper (letter)
            htmlstream = self.htmlProlog (targetFile, title)
            htmlstream.write (self.multiPageLetterIndex (letter, letters))
            # data
            htmlstream.pushTag ('dl')
            for entry in group:
                self.formatEntry (entry, targetDir, htmlstream)
            htmlstream.popTag ()
            # epilog
            self.htmlEpilog (htmlstream)
        return entryFile

    def multiPageLetterIndex (self, currentLetter, letters):
        href = html.href
        list = []
        for letter in letters:
            if letter == currentLetter:
                link = html.tagged ('b', letter)
            else:
                link = href (self.namebase + '-' + letter + '.html', letter)
            list.append (link)
        result = string.join (list, ' \n| ')
        return result

    def htmlProlog (self, file, title):
        filestream = file.open ('wt')
        htmlstream = html.HTMLStream (filestream, title, bgcolor = '#ffffff')
        if self.docHeader:
            htmlstream.write (self.docHeader.indexHeader (file))
        htmlstream.titleHeader (title)
        htmlstream.pushTag ('blockquote')
        return htmlstream

    def htmlEpilog (self, htmlstream):
        htmlstream.line ()
        docLib.genDate (htmlstream)
        htmlstream.popTag ()
        htmlstream.close ()

