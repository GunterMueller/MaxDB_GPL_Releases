#!/usr/bin/env python
# purifyLog.py

import sys
import string
import re
import os

true, false = 'true', None

_delayedMethods = [
    'Data_BasePage::CalculateChecksum(void)const',
    'Kernel_Page74::CalcParity(void)const',
]

class LogAnalyzer:
    def __init__ (self, all):
        self.all = all
        self.warnings = []

    def addLog (self, logfile):
        warnings = LogReader (logfile).warnings
        self.warnings.extend (filter (self.isValidWarning, warnings))
    
    def isValidWarning (self, warning):
        if warning.kind == 'PAR':
            return false
        if warning.containsAnyInErrorLocation (_delayedMethods):
            return false
        return true

    def summarize (self, outdir = '.'):
        #
        # remove certain warnings
        #
        if not self.all:
            self.warnings = filter (self.isValidWarning, self.warnings)
        #
        # sort by filename
        #
        sortable = []
        for warning in self.warnings:
            file, line = warning.getSourceLocation ()
            sortable.append ((string.lower (str (file)), file, line, warning.testcases [0], warning))
            
        sortable.sort ()
        #
        # remove duplicates
        #
        lastWarning = None
        compressed = [sortable [0]]
        lastWarning = sortable [0] [-1]
        for lowfile, file, line, name, warning in sortable [1:]:
            if lastWarning.getCallStack () == warning.getCallStack ():
                lastWarning.testcases.extend (warning.testcases)
            else:
                lastWarning = warning
                compressed.append ((lowfile, file, line, name, warning))
        #
        # print
        #
        stream = None
        lastfile = ''
        for lowfile, file, line, name, warning in compressed:
            if file != lastfile:
                if stream:
                    stream.close ()
                stream = open (os.path.join (outdir, str (file) + '.purify-log'), 'w')
                lastfile = file
            stream.write ("%s:%s %s\n" %  (file, line, unique (warning.testcases)))
            stream.write (warning.formatSourceInfo ())
            stream.write ('        ' + warning.description + '\n')
            for line in warning.lines:
                stream.write (line + '\n')
        if stream:
            stream.close ()
    
_ignoredFiles = ['vsp32.c', 'vsp32c.c', 'vsp36.c', 'gsp00_2.h', 
    'SAPDBFields_BasicRAMBuffer.hpp', 'ggg90.h', 'vgg10c.c']
                
_ignoredFunctions = ['GetSidSubAuthority']
    
class PurifyWarning:
    def __init__ (self, logLine, testcase, sourceInfo):
        self.kind = logLine [4:7]
        self.description = logLine [9:]
        self.testcases = [testcase]
        self.lines = []
        self.callStack = None
        self.sourceInfo = sourceInfo

    def __str__ (self):
        return '[%s] %s: %s' % (self.testcases [0], self.kind, self.description)
    
    locationRE = re.compile (r'\s*(.*)\s*\[([^:]+):?(\d+)?\]')
    def getSourceLocation (self):
        for file, line, function in self.getCallStack ():
            if line and (file not in _ignoredFiles) \
                    and (function not in _ignoredFunctions):
                return file, line
        return None, None
        
    def getCallStack (self):
        if self.callStack is None:
            result = []
            callStack = getStringRange (self.lines, 'Error location', 'Allocation location')
            for line in callStack:
                match = self.locationRE.match (line)
                if not match:
                    print 'no match for', line
                else:
                    function, file, line = match.group (1, 2, 3)
                    result.append (file, line, string.strip (function))
            self.callStack = result
        return self.callStack
    
    def containsInErrorLocation (self, methodName):
        return self.containsAnyInErrorLocation ([methodName])
        
    def containsAnyInErrorLocation (self, methodNames):
        for file, line, function in self.getCallStack():
            if function in methodNames:
                return true
        return false
    
    def formatSourceInfo (self):
        if self.sourceInfo is None:
            return ''        
        lineno, filename = self.sourceInfo
        lineno = int (lineno)
        fullpath = os.path.join (sourceDir, filename)
        if os.path.exists (fullpath):
            source = open (fullpath, 'r').readlines ()
            sourceLine = ' (' + string.strip (source [lineno - 1]) + ')'
        else:
            sourceLine = ''
        return '        source: %s:%d %s\n' % (filename, lineno, sourceLine)
    
def getStringRange (list, lower, upper):
    result = []
    for i in xrange (len (list)):
        if string.find (list [i], lower) != -1:
            break
    for i in xrange (i + 1, len (list)):
        if string.find (list [i], upper) != -1:
            break
        result.append (list [i])
    return result

def unique (list):
    result = []
    for entry in list:
        if entry not in result:
            result.append (entry)
    return result

class LogReader:
    def __init__ (self, logFile):
        self.name = os.path.basename (os.path.splitext (logFile) [0])
        self.warnings = []
        self.lastInfo = None
        self.readFile (logFile)
        
    def readFile (self, logFile):
        stream = open (logFile, 'r')
        line = stream.readline ()
        while line:
            line = line [:-1]
            if line [:3] == '[I]':
                self.addInfo (line)
            elif line [:3] == '[W]':
                self.addWarning (line)
            else:
                self.addLine (line)
            line = stream.readline ()
        stream.close ()
    
    abapInfoRE = re.compile (r'\[I\] Message: abap at: *(\d+) in: (.*)', re.I)
    def addInfo (self, line):
        match = self.abapInfoRE.match (line)
        if match:
            self.lastInfo = match.group (1, 2)
        self.addLine = self.ignoreLine
        
    def addWarning (self, line):
        self.warnings.append (PurifyWarning (line, self.name, self.lastInfo))
        self.addLine = self.addLineToWarning
    
    def ignoreLine (self, line):
        pass
    
    def addLineToWarning (self, line):
        self.warnings [-1].lines.append (line)
    
def isPurifyLog (fname):
    stream = open (fname, 'r')
    line = stream.readline ()
    stream.close ()
    text = '[I] Starting Purify'
    return line [:len (text)] == text

def getFullListDir (dirname):
    result = []
    for short in os.listdir (dirname):
        result.append (os.path.join (dirname, short))
    return result
    
def getLogFiles (args):
    result = []
    for arg in args:
        if os.path.isdir (arg):
            result.extend (getFullListDir (arg))
        else:
            result.append (arg)
    return filter (isPurifyLog, result)

def purifyLog (options, files):
    analyzer = LogAnalyzer (options.all)
    for log in files:
        if options.verbose:
            print os.path.basename (log)
        analyzer.addLog (log)
    analyzer.summarize (options.outdir)
    
sourceDir = os.environ.get ('SUT', '.')
    
def main (options, args):
    global sourceDir
    sourceDir = options.source
    purifyLog (options, getLogFiles (args))

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('o', 'outdir', ':', '.', 'directory for output files'),
        ('v', 'verbose', '', None, 'verbose output'),
        ('A', 'all', '', None, 'don\'t suppress some warnings'),
        ('source', None, ':', sourceDir, 'source directory'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
