
import sys
import string
import re
import os
splitext = os.path.splitext
pathjoin = os.path.join
isdir = os.path.isdir
basename = os.path.basename
import cStringIO

from refilter import RE_Filter

_version = 0.1

class ProjectRenamer:
    def __init__ (self, mappingFile, delimiter = '->', verbose = None):
        self.mapping = loadMapping (mappingFile, delimiter)
        self.verbose = verbose
        self.cRenamer = C_Renamer (self.mapping)
        self.pascalRenamer = Pascal_Renamer (self.mapping)
        self.vdnRenamer = VDN_Renamer (self.cRenamer, self.pascalRenamer)

    def filterFile (self, fname):
        base = basename (fname)
        if base [0] in ['_', '%']:
            return # skip backup files
        if self.verbose:
            print fname,
        ext = splitext (fname) [1]
        if ext in ['.c', '.cpp', '.h', '.hpp', '.skl', '.y', '.l']:
            self.cRenamer.filterFile (fname)
            result = '=> done'
        elif ext == '':
            self.vdnRenamer.filterFile (fname)
            esult = '=> done'
        else:
            pass # some other weird file
            result = 'skipped'
        if self.verbose:
            print result

    def filterDirectory (self, dirname, recursive):
        list = os.listdir (dirname)
        for entry in list:
            fullName = pathjoin (dirname, entry)
            if isdir (fullName):
                if recursive:
                    self.filterDirectory (fullName, recursive)
            else:
                self.filterFile (fullName)

    def filterAny (self, fname, isRecursive):
        if isdir (fname):
            self.filterDirectory (fname, isRecursive)
        else:
            self.filterFile (fname)

class VDN_Renamer:
    def __init__ (self, cRenamer, pascalRenamer):
        self.cRenamer = cRenamer
        self.pascalRenamer = pascalRenamer

    def filterFile (self, fname):
        pseudoExt = fname [-1]
        if pseudoExt in ['c', 'x']:
            filter =self.cRenamer
        elif pseudoExt == 'a':
            return    # don't do anything in assembler files
        else:
            filter = self.pascalRenamer
        source = open (fname, 'rt').read ()
        newSource = self.filterSource (source, filter)
        if source != newSource:
            open (fname, 'wt').write (newSource)

    def filterSource (self, source, filter):
        partStartRE = re.compile ('\n(define|use|synonym|code)\\s*:', re.I)
        partEndRE = re.compile ('\n[.]CM [*]-END-[*]', re.I)
        outstream = cStringIO.StringIO ()
        index = string.index
        dataPos = 0
        match = partStartRE.search (source)
        while match:
            # find end of line
            partPos = index (source, '\n', match.start ()) + 1
            # copy to outstream
            outstream.write (source [dataPos:partPos])
            # find end of part
            match = partEndRE.search (source, partPos)
            # get part data
            partData = source [partPos:match.start ()]
            # filter part data
            filtered = filter.filter (partData)
            # write filtered to stream
            outstream.write (filtered)
            # find next match
            dataPos = match.start ()
            match = partStartRE.search (source, dataPos)
        # write remaining source
        outstream.write (source [dataPos:])
        return outstream.getvalue ()

class SourceRenamer (RE_Filter):
    commentREs = []
    eolCOmmentREs = []
    stringREs = []
    identREs = ['[a-zA-Z_][a-zA-Z_0-9]+']

    def __init__ (self, renameDict):
        self.renameDict = renameDict
        callbackPairs = self.buildCallbackPairs ([
                ('identREs', self.renameIdentifier),
                ('commentREs', self.renameInComment),
                ('eolCOmmentREs', self.renameInEOLComment),
                #('stringREs', self.renameInString),
                ])
        RE_Filter.__init__ (self, callbackPairs)
        self.textFilter = RE_Filter (self.buildCallbackPairs ([
                ('identREs', self.renameIdentifier),
                ]))

    def buildCallbackPairs (self, callbackPairs):
        result = []
        for varname, callback in callbackPairs:
            list = getattr (self, varname)
            for entry in list:
                compiled = re.compile (entry)
                result.append ((compiled, callback))
        return result

    def renameIdentifier (self, match, data):
        oldName = match.group (0)
        try:
            replacement = self.renameDict [oldName]
        except KeyError:
            replacement = oldName
        # print oldName, '->', replacement
        return match.end (), replacement

    def renameInEOLComment (self, match, data):
        inText = match.group (0)
        outText = self.textFilter.filter (inText)
        return match.end (), outText

    def renameInComment (self, match, data, commentDelimiter):
        start, end = match.span ()
        eoComment = string.index (data, commentDelimiter, end) \
            + len (commentDelimiter)
        outText = self.textFilter.filter (data [start:eoComment])
        return eoComment, outText

    def renameInString (self, match, data):
        start, end = match.span ()
        eoString = self.findEOString (data, start)
        outText = self.textFilter.filter (data [start:eoString])
        return eoString, outText

class C_Renamer (SourceRenamer):
    commentREs = ['/[*]']
    eolCOmmentREs = ['//[^\n]*\n']
    stringREs = ['"', "'"]

    def renameInComment (self, match, data):
        return SourceRenamer.renameInComment (self, match, data, '*/')

    def findEOString (self, data, start):
        stopChar = data [start]
        found = None
        pos = start + 1
        index = string.index
        while not found:
            pos = index (data, stopChar, pos)
            if data [pos - 1] != '\\':
                found = 1
            pos = pos + 1
        return pos

class Pascal_Renamer (SourceRenamer):
    commentREs = ['\([*]']
    stringREs = ["'"]

    def renameInComment (self, match, data):
        return SourceRenamer.renameInComment (self, match, data, '*)')

    def findEOString (self, data, start):
        eos = string.index (data, start + 1, "'")
        return eos

def loadMappingError (fname, line, lineno, msg):
    sys.stderr.write ('%(line)sloadMapping: %(fname)s(%(lineno)d): %(msg)s\n'
        % locals ())

def loadMapping (fname, separator = None, doStrip = 1):
    lines = open (fname, 'rt').readlines ()
    result = {}
    split = string.split
    strip = string.strip
    for line, index in map (None, lines, xrange (len (lines))):
        xline = removeComment (line)
        if len (strip (xline)) == 0:
            continue
        if separator:
            items = split (xline, separator)
            if doStrip:
                items = map (strip, items)
        else:
            items = split (xline)
        if len (items) == 1:
            loadMappingError (fname, line, index, 'two items required for mapping')
        elif len (items) > 2:
            print separator, items
            loadMappingError (fname, line, index, 'only two items allowed for mapping')
        else:
            result [items [0]] = items [1]
    return result

def removeComment (line):
    commentPos = string.find (line, '#')
    if commentPos != -1:
        line = line [:commentPos]
    else:
        line = line [:-1]   # remove \n
    return line

