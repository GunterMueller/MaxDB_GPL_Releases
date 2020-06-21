#!/usr/bin/env python
# errorReportLib.py.py

import string
import os
import glob
import urllib
import time

import vmake.protocol
import vmake.telLib

class Report:
    def __init__ (self):
        pass

    def loadFiles (self, fileList):
        self.errors = []
        self.protocolFiles = []
        prototype = {}
        for clazzName in ['Environment', 'ProtocolFile', 'ErrorEntry', 'WarningEntry']:
            prototype [clazzName] = getattr (vmake.protocol, clazzName)
        for fname in fileList:
            sandbox = {}
            sandbox.update (prototype)
            try:
                execfile (fname, sandbox)
                self.protocolFiles = self.protocolFiles + sandbox ['protocolFiles']
                self.errors = self.errors + sandbox ['errorList']
            except SystemError:
                pass

    def getPlatforms (self):
        platforms = {}
        for protocolFile in self.protocolFiles:
            platforms [protocolFile.platform] = 1
        platforms = platforms.keys ()
        platforms.sort ()
        return platforms

    def sort (self, fields):
        return map (lambda t: t [-1], self.sorted (fields))

    def sorted (self, fields):
        sortList = []
        for entry in self.errors:
            sortList.append (entry.makeSortable (fields))
        sortList.sort ()
        sortList.reverse ()
        return sortList

    def sortedRuns (self, fields):
        sortList = []
        for entry in self.protocolFiles:
            sortList.append (entry.makeSortable (fields))
        sortList.sort ()
        return sortList

    def dump (self):
        for error in self.errors:
            print error

    def dumpOverview (self):
        sorted = self.sorted (['error', 'warning', 'release', 'platform',
            'bitness', 'source', 'speed'])
        env = () * 5
        elements = {}
        for error, warning, release, platform, bitness, source, speed, entry in sorted:
            newenv = (error, warning, release, platform, bitness)
            if newenv != env:
                print '    ' + string.join (sortedElements (elements), ', ')
                elements = {}
                print errortag (error, warning), release, platform, bitness, ':'
                env = newenv
            elements [source] = 1
        print '    ' + string.join (sortedElements (elements), ', ')

    def dumpByError (self):
        sorted = self.sorted (['source', 'release', 'platform', 'bitness',
            'error', 'warning'])
        lastSource = ''
        lastEnv = (None, None, None)
        lastText = None
        print
        for source, release, platform, bitness, error, warning, entry in sorted:
            if source != lastSource:
                print source + '\n' + ('=' * len (source)) + '\n'
                lastSource = source
                lastEnv = (None, None, None)
            env = (release, platform, bitness)
            if env != lastEnv:
                print '    %s %s %s:' % env
                lastEnv = env
            text = entry.text
            if text == lastText:
                continue
            else:
                lastText = text
            lines = string.split (entry.text, '\n')
            text = '        ' + string.join (lines, '\n        ')
            print text


class HTMLWriter:
    def __init__ (self, tel = None, platform = None):
        if tel is None:
            tel = vmake.telLib.Tel ()
        self.tel = tel
        self.platform = platform


    def write (self, report, outfile):
        stream = self.openFile (outfile)        
        self.writeByPlatform (report, stream)
        stream.write ('<hr>\n')
        self.writeByVmake (report, stream)
        stream.write ('<hr>\n')
        self.writeBySource (report, stream)
        self.closeHTML (stream)

    def openFile (self, outfile):
        stream = open (outfile, 'w')
        stream.write ('<html><head><title>SAP DB Protocoll Summary</title></head>\n')
        stream.write ('<body>\n')
        timestamp = time.strftime ("%Y-%m-%d %H:%M:%S", time.localtime (time.time ()))
        stream.write ('<i>generated %s by getLcProtocols.py</i><br><br>\n' % timestamp)
        stream.write (self.sourceLink ('Errors') + '<br>\n')
        stream.write (self.sourceLink ('Platform Overview') + '<br>\n')
        stream.write (self.sourceLink ('Messages by Source') + '<br>\n')
        return stream

    def closeHTML (self, stream):
        stream.write ('</body></html>\n')
        stream.close ()

    def writeByPlatform (self, report, stream):
        sorted = report.sorted (['error', 'warning', 'release', 'platform',
            'bitness', 'source', 'speed'])
        if self.platform != None:
            sorted = filterByField (sorted, 3, self.platform)
        env = (None, ) * 5
        elements = {}
        stream.write ('<h3>' + self.sourceAnchor ('Errors') + '</h3>\n')
        stream.write ('<dl>\n')
        for error, warning, release, platform, bitness, source, speed, entry in sorted:
            newenv = (error, warning, release, platform, bitness)
            if newenv != env:
                if elements:
                    stream.write ('<dd>'
                        + self.sourcesToLinks (elements)
                        + '</dd>\n')
                if env [0] and warning:
                    # switch from error to warning
                    stream.write ('</dl>\n')
                    stream.write ('<h3>Warnings</h3>\n')
                    stream.write ('<dl>\n')
                stream.write ('<dt>%s %s %s</dt>\n' % (release, platform, bitness))
                env = newenv
                elements = {}
            elements [source] = 1
        stream.write ('<dd>'
                    + self.sourcesToLinks (elements)
                    + '</dd>\n')
        stream.write ('</dl>\n')

    def sourcesToLinks (self, set, delimiter = ', '):
        sources = set.keys ()
        sources.sort ()
        links = map (self.sourceLink, sources)
        return string.join (links, delimiter)


    def sourceLink (self, source):
        href = urllib.quote (source)
        return '<a href="#%s">%s</a>' % (href, source)

    def sourceAnchor (self, source):
        name = urllib.quote (source)
        return '<a name="%s">%s</a>' % (name, source)

    def timeFromString (self, timeString):
        # 2002-01-01 12:00:00
        dateval, timeval = string.split (timeString)
        year, month, day = map (int, string.split (dateval, '-'))
        hour, minutes, seconds = map (int, string.split (timeval, ':'))
        return time.mktime ((year, month, day, hour, minutes, seconds, -1, -1, -1))
        
    def writeByVmake (self, report, stream):
        now = time.time ()
        week = (7 * 24 * 60 * 60)
        old = now - (2 * week)
        veryold = now - (4 * week)
        stream.write ('<h3>' + self.sourceAnchor ('Platform Overview') + '</h3>\n')
        stream.write ('<table border="2">\n')
        stream.write ('<tr><th>Platform</th><th>Release</th><th>Target</th><th>Version</th><th>Err/Warn</th><th>started</th><th>comment</th></tr>\n')
        runs = report.sortedRuns (['platform', 'release', 'starttime', 'target', 'speed'])
        if self.platform != None:
            runs = filterByField (runs, 0, self.platform)
        lastPlatform = ''
        lastRelease = ''
        for platform, release, starttime, target, speed, run in runs:
            comment = ''
            if platform == lastPlatform:
                platform = '&nbsp;'
            else:
                lastPlatform = platform
            if release == lastRelease:
                release = '&nbsp;'
            else:
                lastRelease = release
            if self.timeFromString (starttime) < veryold:
                comment = comment + ' VERY OLD '            
            elif self.timeFromString (starttime) < old:
                comment = comment + ' OLD '            
            if run.errors > 0:
                errorString = '<font color="red">%s</font>' % run.errors
                comment = comment + ' ERRORS '
            else:
                errorString = repr (run.errors)            
            if not comment:
                comment = '&nbsp;'
            stream.write ('<tr><td>%s</td><td>%s</td><td><a href="%s">%s</a></td><td>%s</td><td>%s/%s</td><td>%s</td><td>%s</td></tr>\n'
                % (platform, release, run.url, run.target, run.speed, errorString, run.warnings, run.starttime, comment))
        stream.write ('</table>\n')


    def writeBySource (self, report, stream):
        sorted = report.sorted (['source', 'release', 'platform', 'bitness',
            'error', 'warning'])
        if self.platform != None:
            sorted = filterByField (sorted, 2, self.platform)
        lastSource = ''
        lastEnv = (None, None, None)
        lastText = None
        stream.write ('<h3>' + self.sourceAnchor ('Messages by Source') + '</h3>\n')
        for source, release, platform, bitness, error, warning, entry in sorted:
            if source != lastSource:
                if lastSource:
                    stream.write ('</dl>\n')
                stream.write ('<h4>' + self.sourceAnchor (source) + '</h4>\n')
                date, who = self.getChangeInfo (release, source)
                if who:
                    stream.write ('<blockquote><i>%s, %s</i></blockquote>\n'
                        % (who, date))
                lastSource = source
                lastEnv = (None, None, None)
                stream.write ('<dl>\n')
            env = (release, platform, bitness)
            if env != lastEnv:
                stream.write ('<dt>%s %s %s</dt>\n' % env)
                lastEnv = env
            text = entry.text
            if text == lastText:
                continue
            else:
                lastText = text
            stream.write ('<PRE>' + text + '</PRE>\n')
        stream.write ('</dl>\n')

    def getChangeInfo (self, release, source):
        p4dir = _release2perforce.get (release)
        if p4dir is None:
            print 'no perforcedir for', release
            return (None, None)
        if source [:2] == '::':
            fullSource = p4dir + '/sys/desc/' + source [2:]
        else:
            fullSource = p4dir + '/sys/src/' + source [1:]
        p4output = os.popen ('p4 changes -m 1 ' + fullSource, 'r').read ()
        words = string.split (p4output)
        if len (words) == 0:
            return None, None
        date = words [3]
        who, host = string.split (words [5], '@')
        try:
            whoFields = self.tel.byDnumber (who)
            who = whoFields [0]
        except KeyError:
            pass
        return date, who

_release2perforce = {
    '7402':     '//sapdb/V74/c_02/develop',
    '7402_DEV': '//sapdb/V74/c_02/develop',
    '7403':     '//sapdb/V74/c_03/develop',
    '7403_COR': '//sapdb/V74/c_03/cor',
    '7403_DEV': '//sapdb/V74/c_03/develop',
    '7404_DEV': '//sapdb/V74/develop',
    'TOOLSRC':   '//sapdb/TOOLSRC/develop',
}

def filterByField (list, colno, matchingValue):
    result = []
    for entry in list:
        if entry [colno] == matchingValue:
            result.append (entry)
    return result


def errortag (error, warning):
    if error:
        tag = 'Error'
    elif warning:
        tag = 'Warning'
    else:
        tag = 'OK'
    return tag

def sortedElements (set):
    elements = set.keys ()
    elements.sort ()
    return elements

def expandArgs (arg):
    return glob.glob (arg)

def outname (outdir, fname):
    return os.path.join (outdir, fname)

def createReports (outdir, filePattern):
    tel = vmake.telLib.Tel ()
    expanded = expandArgs (filePattern)
    report = Report ()
    report.loadFiles (expanded)
    HTMLWriter (tel).write (report, outname (outdir, 'all-platforms.html'))
    for platform in report.getPlatforms ():
        HTMLWriter (tel, platform).write (report, outname (outdir, platform + '.html'))

def main (options, args):
    if len (args) == 0:
        args = [r'\\pwww\InetPub\wwwroot\vmake-protocols\livecache\cache\*.dta']
    for arg in args:
        createReports (options.outdir, filePattern)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('1', 'oneReport', '', 'None', 'create a single report'),
        ('o', 'outdir', ':', r'\\pwww\InetPub\wwwroot\vmake-protocols\livecache', 'output directory'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
