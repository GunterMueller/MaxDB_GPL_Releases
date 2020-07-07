#!/usr/bin/env python
# whose.py

import string
import re
import os

import vmake.yutil

true, false = 'true', None
p4server = os.environ.get ('P4PORT', 'perforce3006.wdf.sap.corp:3006')
p4 = os.environ.get ('P4CALL', 'p4')
own = os.environ ['OWN']

class Whose:
    aliasData = None
    telData = None
    file2devData = None
    def __init__ (self, format, verbose = None, honorVacation = None, keepUnknown = None):
        self.fieldList2mapping (format)
        self.verbose = verbose
        self.honorVacation = honorVacation
        self.keepUnknown = keepUnknown

    def file2developer (self, file):
        if self.file2devData is None:
            dataFile = os.path.join (os.environ ['TOOL'], 'data', 'file2developer.txt')
            self.file2devData = open (dataFile, 'r').readlines ()
        for line in self.file2devData:
            line = stripComment (line)
            try:
                rex, nameString = string.split (line, ':', 1)
            except ValueError:
                # contains no rex: nameString
                continue
            if re.search (string.strip (rex), file):
                names = string.split (nameString, ',')
                names = map (string.strip, names)
                if self.verbose:
                    print '***', 'per file2developer', file
                    print '***', names
                return names
        return []

    def getResponsible (self, fname, searchSource = true):
        if searchSource:
            #
            # search content for responsible line
            #
            content = open (fname, 'r').read ()
            match = re.search (r'@author\s+([^\n]*)\n', content)
            if not match:
                match = re.search (r'responsible\s*:([^\n]*)\n', content)
            if match:
                names = fancySplit (match.group (1), ',')
                if names:
                    if self.verbose:
                        print '***', 'responsible in', fname
                        print '***', names
                    return false, names
        #
        # search by filename patterns
        #
        names = self.file2developer (fname)
        if names:
            return true, names
        return true, []

    def readAliasData (self):
        aliasFile = os.path.join (os.environ ['TOOL'], 'data', 'alias.txt')
        lines = open (aliasFile, 'r').readlines ()
        self.aliasData = parseDefinitionLists (lines)

    def resolveAlias (self, alias):
        if self.aliasData is None:
            self.readAliasData ()
        try:
            result = self.aliasData [alias]
            if self.verbose:
                print '***', 'found alias for', alias
                print '***', result
        except KeyError:
            result = [alias]
        return result

    def resolveAliases (self, aliases):
        result = []
        for alias in aliases:
            result.extend (self.resolveAlias (alias))
        return result

    def resolveAliases (self, aliases):
        result = []
        candidates = aliases [:]
        knownAliases = []
        while len (candidates) > 0:
            candidate = candidates [0]
            knownAliases.append (candidate)
            resolvedNames = self.resolveAlias (candidate)
            if resolvedNames == [candidate]:
                result.append (candidate)
            else:
                for resolvedName in resolvedNames:
                    if resolvedName not in knownAliases + candidates:
                        candidates.append (resolvedName)
            del candidates [0]
        return result

    def readTelFile (self):
        telFile = os.path.join (os.environ ['TOOL'], 'bin', 'tel.pl')
        stream = open (telFile, 'r')
        line = stream.readline ()
        while line != '__DATA__\n':
            line = stream.readline ()
        result = stream.readlines ()
        stream.close ()
        return result

    telFields = ['name', 'tel', 'duser', 'pub', 'email']
    def fieldList2mapping (self, fieldList):
        entries = string.split (fieldList, ',')
        result = []
        for entry in entries:
            result.append (self.telFields.index (entry) + 1)
        self.fieldMapping = result

    def formatPersonInfo (self, info):
        result = [info [0]]
        for index in self.fieldMapping:
            result.append (info [index])
        return tuple (result)

    def getPersonInfo (self, pattern):
        if self.telData is None:
            self.telData = self.readTelFile ()
        info = (None, pattern, 'unknown', 'unknown', 'unknown', 'unknown')
        pattern = string.lower (pattern)
        for line in self.telData:
            if string.find (string.lower (line), pattern) != -1:
                items = fancySplit (line, ',')
                info = [1] + items [:5]
                if self.verbose:
                    print '***', 'found tel info for', pattern
                    print '***', info
                break
        return self.formatPersonInfo (info)

    def basicIsThis (self, realname, searchSources):
        #
        # get responsible
        #
        searchedPatterns, responsibles = self.getResponsible (realname, searchSources)
        #
        # resolve alias
        #
        responsibles = self.resolveAliases (responsibles)
        responsibles = filter (None, responsibles)
        #
        # honor vacation entry
        #
        if self.honorVacation:
            onVacation = self.resolveAlias ('vacation')
            remaining = setMinusSet (responsibles, onVacation)
            if remaining:
                responsibles = remaining
        #
        # get entry from tel
        #
        result = []
        for resp in responsibles:
            result.append (self.getPersonInfo (resp))
        return searchedPatterns, result

    def isThis (self, realname, full = None):
        searchedPatterns, result = self.basicIsThis (realname, true)
        #
        # handle unknon responsibles
        #
        if not self.keepUnknown:
            result = self.filterUnknown (result)
            if not result and not searchedPatterns:
                searchedPatterns, result = self.basicIsThis (realname, false)
        #
        # reduce to first entry
        #
        if not full:
            result = result [:1]
        return result

    def filterUnknown (self, responsibles):
        result = []
        for responsible in responsibles:
            if responsible [0]:
                result.append (responsible)
        return result

    perforceRoot = 'undecided'
    depotfile = os.path.join ('sys', 'wrk', 'config', 'perforce.depot')
    def getPerforceRoot (self):
        if self.perforceRoot == 'undecided':
            fullDepotfile = os.path.join (own, self.depotfile)
            if not os.path.exists (fullDepotfile):
                self.perforceRoot = None
                return None
            depotRE = re.compile ('[$]File: ([^ ]+) [$]')
            self.perforceRoot = None
            for line in open (fullDepotfile, 'r').readlines ():
                match = depotRE.search (line)
                if match:
                    depot = match.group (1)
                    self.perforceRoot = depot [:- len (self.depotfile)]
                    break
        return self.perforceRoot

    def getLastChanged (self, realname):
        depotRoot = self.getPerforceRoot ()
        if depotRoot:
            queryFile = depotRoot + vmakeRelative (realname)
            queryFile = string.replace (queryFile, '\\', '/')
        else:
            queryFile = realname
        if p4server:
            portOption = '-p ' + p4server
        else:
            portOption = ''
        p4cmd = p4 + ' ' + portOption + ' changes -m 1 ' + queryFile
        line = os.popen (p4cmd, 'r').read ()
        if not string.strip (line):
            raise NoPerforceFileException (realname)
        words = string.split (line)
        user, node = string.split (words [5], '@')
        result = self.getPersonInfo (user)
        date = words [3]
        return result, date

class NoPerforceFileException:
    def __init__ (self, localname):
        self.localname = localname


def stripComment (line):
    hashPos = string.find (line, '#')
    if hashPos == -1:
        return line
    else:
        return line [hashPos + 1:]

def setMinusSet (set1, set2):
    result = []
    for item in set1:
        if item not in set2:
            result.append (item)
    return result

def fancySplit (text, delimiter):
    result = string.split (text, delimiter)
    result = map (string.strip, result)
    return result

def parseDefinitionList (line):
    defTerm, defData = string.split (line, ':', 1)
    defTerm = string.strip (defTerm)
    defData = fancySplit (defData, ',')
    return defTerm, defData

def parseDefinitionLists (lines):
    result = {}
    for line in lines:
        try:
            defTerm, defData = parseDefinitionList (line)
            result [defTerm] = defData
        except ValueError:
            pass
    return result

def strictPath (path):
    path = os.path.normpath (path)
    path = os.path.normcase (path)
    return path

def splitVmakePath ():
    result = []
    for path in string.split (os.environ ['VMAKE_PATH'], ','):
        path = strictPath (path)
        if path [-1] != os.sep:
            path = path + os.sep
        elif path [-2:] == os.sep + os.sep:
            path = path [:-1]
        result.append (path)
    return result

vmakePath = splitVmakePath ()

def vmakeRelative (absolute):
    for vpath in vmakePath:
        absolute = strictPath (absolute)
        if absolute [:len (vpath)] == vpath:
            return absolute [len (vpath):]
    raise 'not found in VMAKE_PATH', absolute

def output (options, fname, data):
    text = string.join (data [1:], '\t')
    if options.addFilename:
        text = fname + ':\t' + text
    print text

def main (options, args):
    """list the developer responsible for a module

        args must be given in iview notation

        Information is taken from:
        * responsible: in source
        * $TOOL/data/file2developer.txt
        * $TOOL/data/alias.txt

        Any names must be given in a format which can be found by
        searching tel.pl

    """
    if len (args) > 1:
        options.addFilename = 1
    whose = Whose (options.format, options.verbose, options.vacation,
        options.keepUnknown)
    for arg in args:
        realname = vmake.yutil.iview (arg, getOriginal = 1)
        if realname is None:
            raise 'unknown file', arg
        entries = whose.isThis (realname, options.full)
        if options.noLastChanged:
            lastChanged = None
        else:
            try:
                lastChanged, changedDate = whose.getLastChanged (realname)
                output (options, arg, lastChanged + ('<= last change at ' + changedDate,))
            except NoPerforceFileException, exc:
                print 'ERR: no perforce file for ' + exc.localname + ' (wrong case?)'
                # output (options, arg, ('ERR: no perforce file for ' + exc.localname + ' (wrong case?)',))
                lastChanged = None
        for entry in entries:
            if entry != lastChanged:
                output (options, arg, entry)


def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('l', 'addFilename', '', None, 'prefix output with filename'),
        ('full', None, '', 1, 'list all names'),
        ('version', None, '', 0.1, 'version information'),
        ('format', None, ':', 'name,tel', 'comma separated list of fields'),
        ('vacation', None, '', None, 'honor the vacation alias'),
        ('noLastChanged', None, '', None, 'don\'t search for last perforce change'),
        ('keepUnknown', None, '', None, 'keep entries for unknown names'),
        ('v', 'verbose', '', None, 'verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
