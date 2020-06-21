#!/usr/bin/env python
# getLcProtocols.py

import sys
import string
import re
import htmllib
import formatter
import os
import time
import shutil

import vmake.protocol
import vmake.errorReportLib

def extractDirs (fname):
    parser = htmllib.HTMLParser (formatter.NullFormatter ())
    parser.feed (open (fname, 'r').read ())
    hrefs = parser.anchorlist
    parser.close ()
    result = []
    rex = re.compile ('/SAP_DB/([^/]+)/gen/opt/([^/]+)/sys/wrk/([^/]+)/prot')
    for href in hrefs:
        match = rex.search (href)
        if match:
            release, platform, speed = match.group (1, 2, 3)
            href = os.path.dirname (os.path.normpath (href))
            result.append ((href, release, platform, speed))
            # print href
    return result

def getErrorProtocols (href):
    files = os.listdir (href)
    result = []
    now = time.time ()
    for file in files:
        ext = os.path.splitext (file) [1]
        if ext == '.e0':
            fullpath = os.path.join (href, file)
            mtime = now # os.path.getmtime (fullpath)
            result.append ((mtime, fullpath))
    result.sort ()
    return result

def getPlatform (platform):
    if platform [-3:] == '_64':
        return platform [:-3], '64'
    if platform [-3:] == '_32':
        return platform [:-3], '32'
    if platform == 'NTintel':
        return 'NT', '32'
    return platform, '64'

class TimeStamp:
    def __init__ (self, tstampFile, referenceFile, unconditional):
        self.tstampFile = tstampFile
        self.referenceFile = referenceFile
        self.unconditional = unconditional

    def updateRequired (self):
        if self.unconditional:
            return 1
        if not os.path.exists (self.tstampFile):
            return 1
        stampTime = os.path.getmtime (self.tstampFile)
        refTime = os.path.getmtime (self.referenceFile)
        if stampTime < refTime:
            return 1
        else:
            return None

    def stamp (self):
        if not os.path.exists (self.tstampFile):
            stream = open (self.tstampFile, 'w')
            stream.close ()
        stampTime = os.path.getmtime (self.referenceFile)
        os.utime (self.tstampFile, (stampTime, stampTime))


def getLcProtocols (options, fname):
    outdir = os.path.join (options.outdir, 'cache')
    if not os.path.isdir (outdir):
        os.mkdir (outdir)
    timeStamp = TimeStamp (os.path.join (outdir, '.timestamp'),
        fname, options.force)
    if not timeStamp.updateRequired ():
        print 'no update needed for', fname
        return None
    hrefs = extractDirs (fname)
    for href, release, platform, speed in hrefs:
        platform, bitness = getPlatform (platform)
        #if platform != 'sun':
        #    continue
        #if release != '7403_DEV':
        #    continue
        if options.verbose:
            sys.stdout.write (href)
        try:
            protnames = getErrorProtocols (href)
            environment = vmake.protocol.Environment (release, platform, bitness, speed)
            parser = vmake.protocol.ErrorProtocol (environment)
            for mtime, protocol in protnames:
                if options.verbose:
                    sys.stdout.write ('.')
                parser.parseFile (protocol)
            parser.writeResult (outdir)
        except os.error, err:
            sys.stdout.write ('\nError reading ' + href)
        if options.verbose:
            sys.stdout.write ('\n')
    timeStamp.stamp ()
    return 1

def main (options, args):
    if options.quiet:
        options.verbose = None
    if len (args) == 0:
        args = [r'\\hs0100.wdf.sap-ag.de\bas\MAKES\prot\SAP_DB\html_logfiles\vmake_lc_main_night.html']
    for arg in args:
        newFiles = getLcProtocols (options, arg)
        #if newFiles:
        if 1:
            vmake.errorReportLib.createReports (options.outdir,
                os.path.join (options.outdir, 'cache', '*.dta'))
    print "view output at http://pwww.ber.sap.corp:1111/vmake-protocols"

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('o', 'outdir', ':', r'\\pwww\InetPub\wwwroot\vmake-protocols\livecache', 'output directory'),
        ('f', 'force', '', None, 'force, '),
        ('v', 'verbose', '', 1, 'verbose output'),
        ('q', 'quiet', ':', None, 'mo output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
