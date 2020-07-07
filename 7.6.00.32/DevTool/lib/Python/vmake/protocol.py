
import string
import re
import os

import warningPatterns

class ErrorProtocol:
    def __init__ (self, environment):
        self.environment = environment
        self.errors = []
        self.subProtocols = []
        self.warningFilter = warningPatterns.WarningFilter ()

    def parseStream (self, stream):
        self.skipToCollection (stream)
        self.skipToMakePhase (stream)
        self.parseErrors (stream)

    def parseFile (self, fname):
        stream = open (fname, 'r')
        self.currentSubProtocol = ProtocolFile (fname, self.environment)
        self.subProtocols.append (self.currentSubProtocol)
        self.parseStream (stream)
        stream.close ()

    def skipToCollection (self, stream):
        rex = re.compile (r'PHASE: COLLECTING [(](\d\d)-(\d\d)-(\d\d) (\d\d):(\d\d):(\d\d) started[)]')
        line = stream.readline ()
        match = rex.match (line)
        while line and not match:
            line = stream.readline ()
            match = rex.match (line)
        if not line:
            raise "Can't find 'PHASE: COLLECTING' line"
        year, month, day, hour, minute, second = match.group (3, 2, 1, 4, 5, 6)
        self.currentSubProtocol.setStarttime ('20%s-%s-%s %s:%s:%s' % (year, month, day, hour, minute, second))

    def skipToMakePhase (self, stream):
        rex = re.compile (r'PHASE:\s*MAKE\s+TARGET: ([^ ]+)\s+VERSION:([a-z]+)')
        line = stream.readline ()
        match = rex.match (line)
        while line and not match:
            line = stream.readline ()
            match = rex.match (line)
        if not line:
            raise "Can't find 'PHASE: MAKE' line"
        self.currentSubProtocol.setTarget (match.group (1))
        self.currentSubProtocol.setSpeed (match.group (2))

    def parseErrors (self, stream):
        targetRE = re.compile (r"TARGET:\s*(.*)\s+STATUS: ([A-Z]+)")
        line = stream.readline ()
        while line:
            match = targetRE.match (line)
            if match:
                source, status = match.group (1, 2)
                source, addOn = self.extractSource (source)
                text = addOn + self.readOutput (stream)
                if (status == 'ERROR'):
                    if self.isErrorOutput (text):
                        self.addError (source, text)
                elif self.isWarningOutput (text):
                    self.addWarning (source, text)
            line = stream.readline ()

    def extractSource (self, targetText):
        text = string.strip (targetText)
        if (text [0] == "'") and (text [-1] == "'"):
            return text [1:-1], ''
        words = string.split (text)
        source = words [-1]
        addOn = text
        return source, addOn

    def readOutput (self, stream):
        result = []
        line = stream.readline ()
        compareLine = '=' * 20
        while (line [:20] != compareLine) and line:
            result.append (line)
            line = stream.readline ()
        return string.join (result, '')

    def isErrorOutput (self, text):
        if string.find (text, 'not made because of dependency errors of') != -1:
            return None
        return 1

    def isWarningOutput (self, text):
        return self.warningFilter.containsWarning (self.environment.platform, text)

    def addError (self, source, text):
        self.errors.append (ErrorEntry (source, self.environment, text))
        self.currentSubProtocol.addError ()

    def addWarning (self, source, text):
        self.errors.append (WarningEntry (source, self.environment, text))
        self.currentSubProtocol.addWarning ()

    def writeResult (self, outdir = '.'):
        stream = open (self.environment.makeFilename (outdir), 'w')
        stream.write ('\nenvironment = ' + repr (self.environment) + '\n\n')
        #
        # write sub protocols
        #
        stream.write ('protocolFiles = [\n')
        for subProtocol in self.subProtocols:
            stream.write ('    %s,\n' % subProtocol.toRepr ('environment'))
        stream.write (']\n\n')
        #
        # write errors
        #
        stream.write ('errorList = [\n')
        for error in self.errors:
            stream.write ('    %s,\n' % error.toRepr ('environment'))
        stream.write (']\n\n')
        stream.write ('errors, warnings = %s, %s # status\n' % self.getErrorCount ())
        stream.close ()

    def getErrorCount (self):
        errors = 0
        warnings = 0
        for entry in self.errors:
            errors = errors + entry.error
            warnings = warnings = entry.warning
        return errors, warnings


class Environment:
    properties = ['release', 'platform', 'bitness', 'speed']
    def __init__ (self, release, platform, bitness, speed):
        self.release = release
        self.platform = platform
        self.bitness = bitness
        self.speed = speed

    def __repr__ (self):
        return 'Environment (%s, %s, %s, %s)' \
            % tuple (map (repr, (self.release, self.platform, self.bitness, self.speed)))

    def makeFilename (self, outdir):
        base = '%s-%s-%s-%s.dta' % (
            self.release, self.platform, self.bitness, self.speed)
        return os.path.join (outdir, base)

class Entry:
    properties = ['source', 'author', 'text']
    author = None
    def __init__ (self, source, environment, text):
        if source [0] != ':':
            source = '::' + source
        self.source = source
        self.environment = environment
        for name in Environment.properties:
            setattr (self, name, getattr (environment, name))
        self.text = text

    def makeSortable (self, names):
        result = []
        for name in names:
            result.append (getattr (self, name))
        result.append (self)
        return result

    def toRepr (self, envvarName):
        return '%s (%s, %s, """%s""")' % (
            self.__class__.__name__, repr (self.source), envvarName, self.text)

    def __str__ (self):
        return "%c: %.20s in %s" % (self.tagChar, self.source, repr (self.environment))

class ErrorEntry (Entry):
    error = 1
    warning = 0
    tagChar = "E"

class WarningEntry (Entry):
    error = 0
    warning = 1
    tagChar = "W"

class ProtocolFile:
    def __init__ (self, url, environment, target = None, starttime = None,
            errors = 0, warnings = 0):
        self.url = url
        self.environment = environment
        for name in Environment.properties:
            setattr (self, name, getattr (environment, name))
        self.target = target
        self.starttime = starttime
        self.errors = errors
        self.warnings = warnings

    def setTarget (self, target):
        self.target = target

    def setStarttime (self, timestring):
        self.starttime = timestring

    def setSpeed (self, speed):
        self.spped = speed

    def makeSortable (self, names):
        result = []
        for name in names:
            result.append (getattr (self, name))
        result.append (self)
        return result

    def addError (self):
        self.errors = self.errors + 1

    def addWarning (self):
        self.warnings = self.warnings + 1

    def toRepr (self, envvarName):
        return '%s (%s, %s, %s, %s, %d, %d)' % (
            self.__class__.__name__, repr (self.url),
            envvarName, repr (self.target), repr (self.starttime),
            self.errors, self.warnings)

if __name__ == "__main__":
    import sys
    env = Environment ('RTest', 'any', '32', 'unknown')
    prot = ErrorProtocol (env)
    prot.parseFile (sys.argv [1])
    prot.writeResult ()

