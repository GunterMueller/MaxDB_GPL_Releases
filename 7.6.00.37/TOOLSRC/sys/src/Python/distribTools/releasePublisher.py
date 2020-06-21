#!/usr/bin/env python
# releasePublisher.py

import sys
import string
import re
import os
import urllib
import shutil
import md5

_verbose = None

lineRel = '%(major)d_%(minor)d_%(cl)02d_%(build)02d'
dotRel = '%(major)d.%(minor)d.%(cl)d.%(build)d'

class Match:
    def __init__ (self, fname):
        self.fname = fname

    def filename (self, release = None, searchpath = None):
        return self.fname

class ReleaseMatch:
    def __init__ (self, pattern, platform = None, releasePattern = lineRel,
            windowsExtension = 'zip'):
        self.pattern = pattern
        self.platform = platform
        self.releasePattern = releasePattern
        self.windowsExtension = windowsExtension

    def filename (self, releaseTuple, searchPath):
        major, minor, cl, build = releaseTuple
        release = self.releasePattern % locals ()
        platform = self.platform
        if platform and platform [:3] == 'win':
            arc = self.windowsExtension
        else:
            arc = 'tgz'
        fullname = self.pattern % locals ()
        return fullname

class Searcher:
    def __init__ (self, pattern):
        self.pattern = self.breakPattern (pattern)

    def breakPattern (self, pattern):
        pos = string.index (pattern, '*')
        return (pattern [:pos], self.innerPattern, pattern [pos+1:])

    def filename (self, release, searchPath = None):
        rex = self.createPattern (locals ())
        index, matches = searchPath.searchForPattern (rex)
        if not matches:
            print 'ERR: no match for', rex.pattern
            return ''
        best = self.getFirstValue (matches [0])
        for match in matches [1:]:
            best = self.getBetterValue (best, match)
        return best [0]

    def createPattern (self, variables):
        p1, inner, p2 = self.pattern
        #r1 = re.escape (p1 % variables)
        #r2 = re.escape (p2 % variables)
        r1 = p1 % variables
        r2 = p2 % variables
        return re.compile (r1 + '(' + inner + ')' + r2)

class ChangelistSearch (Searcher):
    innerPattern = '[0-9]+'

    def getFirstValue (self, match):
        filename, match = match
        value = int (match.group (1))
        return filename, value

    def getBetterValue (self, best, match):
        filename, match = match
        value = int (match.group (1))
        bestfile, bestvalue = best
        if value > bestvalue:
            return filename, value
        else:
            return best

class RelSearch (Searcher):
    innerPattern = r'(\d)_(\d)_(\d+)_(\d+)'
    def __init__ (self, pattern, maxRelease = None):
        Searcher.__init__ (self, pattern)
        self.maxRelease = maxRelease

    def getFirstValue (self, match):
        filename, match = match
        reltuple = map (int, match.group (2, 3, 4, 5))
        return filename, reltuple

    def getBetterValue (self, best, match):
        filename, match = match
        reltuple = map (int, match.group (1, 2, 3, 4))
        bestfile, bestvalue = best
        if reltuple > bestvalue:
            return filename, reltuple
        else:
            return best

class ReleaseRedirect:
    def __init__ (self, comment):
        self.comment = comment

    def filename (self, *args):
        return self

def filter64to32 (platforms, excluded = []):
    result = []
    for platform in platforms:
        if platform in excluded:
            continue
        if platform == 'aix5-64bit-ppc':
            platform = 'aix5-32bit-ppc'
        elif platform == 'hpux-64bit-hppa':
            platform = 'hpux-32bit-hppa'
        elif platform == 'sun-64bit-sparc':
            platform = 'sun-32bit-sparc'
        result.append (platform)
    return result

class R75:

    unixPlatforms = [
            'aix5-64bit-ppc',
            'hpux-64bit-hppa',
            'hpux-64bit-ia64',
            'linux-32bit-i386',
            'linux-64bit-x86_64',
            'linux-64bit-ia64',
            'linux-64bit-ppc',
            'sun-64bit-sparc',
            'tru64-64bit-alpha'
    ]
    windowsPlatforms = [
            'win-32bit-i386',
            'win-64bit-ia64',
            'win-64bit-x86_64',
    ]
    allPlatforms = unixPlatforms + windowsPlatforms
    rpmPlatforms = [
            'i586',
            'x86_64',
            'ia64',
    ]

    all = []
    for platform in allPlatforms:
        all.append (ReleaseMatch ('maxdb-all-%(platform)s-%(release)s.%(arc)s', platform))

    rpmPackages = ['ind', 'srv75', 'scriptif', 'callif', 'web'] # + ['sqldbc']
    rpmx86 = [ReleaseMatch ('maxdb-' + package + '-%(release)s-1.%(platform)s.rpm', 'i386', dotRel)
                for package in rpmPackages]
    rpmia64 = [ReleaseMatch ('maxdb-' + package + '-%(release)s-1.%(platform)s.rpm', 'ia64', dotRel)
                for package in rpmPackages]
    rpmx86_64 = [ReleaseMatch ('maxdb-' + package + '-%(release)s-1.%(platform)s.rpm', 'x86_64', dotRel)
                for package in rpmPackages]
    rpms = rpmx86 + rpmia64 + rpmx86_64
    sqldbc = []
    for platform in allPlatforms:
        sqldbc.append (ReleaseMatch ('maxdb-sqldbc-%(platform)s-%(release)s.tgz', platform))
    sqldbc.append (ReleaseMatch ('maxdb-sqldbc-docu-%(release)s.tgz'))
    #for platform in rpmPlatforms:
    #    sqldbc.append (ReleaseMatch ('maxdb-sqldbc-%(release)s-1.%(platform)s.rpm', platform, dotRel))

    lcpool = all +  rpms + sqldbc

    jdbc = [
        ChangelistSearch ('sapdbc-7_6_00_20_*.jar'),
        ChangelistSearch ('sapdbc-source-7_6_00_20_*.jar'),
    ]
    php = [
        ReleaseMatch ('maxdb-php-docu-%(release)s.tgz'),
        ReleaseMatch ('maxdb-php-xmldocu-%(release)s.tgz'),
        ReleaseMatch ('maxdb-php-source-%(release)s.tgz'),
        ReleaseMatch ('maxdb-php4-win-32bit-%(release)s.tgz'),
        ReleaseMatch ('maxdb-php5-win-32bit-%(release)s.tgz'),
    ]
    perldbd = [
        ReleaseRedirect ('Please use DBD::MaxDB from Release <a href="7.5.00.html#DBD_MaxDB">7.6</a>')
    ]
    odbc = [
        ReleaseMatch ('maxdb-odbc-setup-%(release)s.exe'),
    ]
    interfaces = sqldbc + jdbc + php + perldbd + odbc

    scriptingPlatforms = filter (lambda platform: platform != 'linux-64bit-ppc', allPlatforms)

    python = [ReleaseMatch ('maxdb-python-%(platform)s-%(release)s.%(arc)s', platform)
                for platform in filter64to32 (allPlatforms, excluded = ['win-64bit-x86_64'])]
    perl = [ReleaseMatch ('maxdb-perl-%(platform)s-%(release)s.%(arc)s', platform)
                for platform in filter64to32 (allPlatforms, excluded = ['win-64bit-x86_64', 'win-64bit-ia64'])]
    scripting = python + perl

    dbmgui = [RelSearch ('maxdb-dbmgui-setup-*.exe')]
    sqlstudio = [RelSearch ('maxdb-sqlstudio-setup-*.exe')]
    wingui = dbmgui + sqlstudio

    sources = [
        ReleaseMatch ('maxdb-source-%(release)s.tgz'),
        ReleaseMatch ('maxdb-source-%(release)s.zip'),
    ]
    regressionTests = [
        ReleaseMatch ('maxdb-test-source-%(release)s.tgz'),
        ReleaseMatch ('maxdb-test-source-%(release)s.zip'),
    ]
    buildtools = []
    buildPlatforms = [
            'aix5-ppc', 'hpux-hppa', 'hpux-ia64',
            'linux-i386', 'linux-ia64', 'linux-x86_64',
            'sun-sparc', 'tru64-alpha', 'source',
    ]
    buildtools.append (ChangelistSearch ('maxdb-buildtools-win32-i386-*.zip'))
    for platform in buildPlatforms:
        buildtools.append (ChangelistSearch ('maxdb-buildtools-' + platform + '-*.tgz'))

    plainhtml = [
        Match ('maxdb-htmldoc-75.tgz'),
        Match ('maxdb-htmldoc-de-75.tgz'),
    ]

    htmlhelp = [
        Match ('maxdb-chmdoc-75.chm'),
        Match ('maxdb-chmdoc-de-75.chm'),
    ]

    doc = plainhtml + htmlhelp
    complete = (all + interfaces + scripting + wingui 
        + sources + regressionTests + buildtools + doc)

    client = []

class R76:

    unixPlatforms = [
            'aix5-64bit-ppc',
            'hpux-64bit-hppa',
            'hpux-64bit-ia64',
            'linux-32bit-i386',
            'linux-64bit-x86_64',
            'linux-64bit-ia64',
            'linux-64bit-ppc',
            'sun-64bit-sparc',
            'sun-64bit-x86_64',
    ]
    windowsPlatforms = [
            'win-32bit-i386',
            'win-64bit-ia64',
            'win-64bit-x86_64',
    ]
    allPlatforms = unixPlatforms + windowsPlatforms
    rpmPlatforms = [
            'i586',
            'x86_64',
            'ia64',
    ]

    all = []
    for platform in allPlatforms:
        all.append (ReleaseMatch ('maxdb-all-%(platform)s-%(release)s.%(arc)s', platform,
            windowsExtension = 'exe'))

    client = []
    for platform in allPlatforms:
        client.append (ReleaseMatch ('maxdb-client-%(platform)s-%(release)s.%(arc)s', platform,
            windowsExtension = 'exe'))

    rpmPackages = ['ind', 'srv76', 'scriptif', 'callif', 'web'] # + ['sqldbc']
    rpmx86 = [ReleaseMatch ('maxdb-' + package + '-%(release)s-1.%(platform)s.rpm', 'i586', dotRel)
                for package in rpmPackages]
    rpmia64 = [ReleaseMatch ('maxdb-' + package + '-%(release)s-1.%(platform)s.rpm', 'ia64', dotRel)
                for package in rpmPackages]
    rpmx86_64 = [ReleaseMatch ('maxdb-' + package + '-%(release)s-1.%(platform)s.rpm', 'x86_64', dotRel)
                for package in rpmPackages]
    rpms = rpmx86 + rpmia64 + rpmx86_64

    sqldbc = []
    for platform in allPlatforms:
        sqldbc.append (ReleaseMatch ('maxdb-sqldbc-%(platform)s-%(release)s.tgz', platform))
    sqldbc.append (ReleaseMatch ('maxdb-sqldbc-docu-%(release)s.tgz'))
    #for platform in rpmPlatforms:
    #    sqldbc.append (ReleaseMatch ('maxdb-sqldbc-%(release)s-1.%(platform)s.rpm', platform, dotRel))

    lcpool = all +  rpms + sqldbc

    jdbc = [
        ChangelistSearch ('sapdbc-7_6_00_24_*.jar'),
        # ChangelistSearch ('sapdbc-source-7_6_00_16_*.jar'),
    ]
    php = [
        ReleaseMatch ('maxdb-php-docu-%(release)s.tgz'),
        ReleaseMatch ('maxdb-php-xmldocu-%(release)s.tgz'),
        ReleaseMatch ('maxdb-php-source-%(release)s.tgz'),
        ReleaseMatch ('maxdb-php4-win-32bit-%(release)s.tgz'),
        ReleaseMatch ('maxdb-php5-win-32bit-%(release)s.tgz'),
    ]
    perldbd = [
        RelSearch ('DBD-MaxDB-*.tar.gz'),
    ]
    odbc = [
        ReleaseMatch ('maxdb-odbc-setup-%(release)s.exe'),
    ]
    interfaces = sqldbc + jdbc + php + perldbd + odbc

    python = [ReleaseMatch ('maxdb-python-%(platform)s-%(release)s.%(arc)s', platform)
                for platform in filter64to32 (allPlatforms, excluded = ['win-64bit-x86_64'])]
    perl = [ReleaseMatch ('maxdb-perl-%(platform)s-%(release)s.%(arc)s', platform)
                for platform in filter64to32 (allPlatforms, excluded = ['win-64bit-x86_64', 'win-64bit-ia64'])]
    scripting = python + perl

    dbmgui = [ChangelistSearch ('maxdb-dbmgui-setup-7_6_00-*.exe')]
    sqlstudio = [ChangelistSearch ('maxdb-sqlstudio-setup-7_6_00-*.exe')]
    wingui = dbmgui + sqlstudio

    sources = [
        #ReleaseMatch ('maxdb-source-%(release)s.tgz'),
        #ReleaseMatch ('maxdb-source-%(release)s.zip'),
    ]

    regressionTests = [
        #ReleaseMatch ('maxdb-test-source-%(release)s.tgz'),
        #ReleaseMatch ('maxdb-test-source-%(release)s.zip'),
    ]
    buildtools = []
    buildPlatforms = [
            'aix5-ppc', 'hpux-hppa', 'hpux-ia64',
            'linux-i386', 'linux-ia64', 'linux-x86_64',
            'sun-sparc', 'source',
    ]
    #buildtools.append (ChangelistSearch ('maxdb-buildtools-win-i386-*.zip'))
    #for platform in buildPlatforms:
    #    buildtools.append (ChangelistSearch ('maxdb-buildtools-' + platform + '-*.tgz'))

    plainhtml = [
        ChangelistSearch ('maxdb-htmldoc-76_*.tgz'),
        ChangelistSearch ('maxdb-htmldoc-de-76_*.tgz'),
    ]

    htmlhelp = [
        ChangelistSearch ('maxdb-chmdoc-76_*.chm'),
        ChangelistSearch ('maxdb-chmdoc-de-76_*.chm'),
    ]

#     plainhtml = [
#         Match ('maxdb-htmldoc-76.tgz'),
#         Match ('maxdb-htmldoc-de-76.tgz'),
#     ]
#
#     htmlhelp = [
#         Match ('maxdb-chmdoc-76.chm'),
#         Match ('maxdb-chmdoc-de-76.chm'),
#     ]

    doc = plainhtml + htmlhelp

    complete = (all + client + interfaces + scripting + wingui + 
        sources + regressionTests + buildtools + doc)

class Publisher:
    def __init__ (self, release, subset, sort = 1):
        self.release = release
        major = release [:2]
        if major == (7, 5):
            filelist = R75
        elif major == (7, 6):
            filelist = R76
        else:
            raise 'release not yet supported', major
        self.generators = getattr (filelist, subset)
        self.createSearchPath (release)
        self.hasError = None
        self.sort = sort

    def createSearchPath (self, release):
        localMirror = r'\\p26327\OpenSrc\pub\maxdb'
        (major, minor, cl, build) = release
        current = localMirror + r'\current\%d.%d.%02d' % (major, minor, cl)
        if not os.path.isdir (current):
            fatalError ("cannot find 'current' directory " + current)
        candidates = os.listdir (localMirror + r'\archive')
        bestFit = (0, 0, 0, 0)
        archive = None
        for candidate in candidates:
            astuple = tuple (map (int, string.split (candidate, '.')))
            if (bestFit < astuple) and (astuple < release):
                bestFit == astuple
                archive = candidate
        if archive != None:
            path = [current, localMirror + '\\archive\\' + archive]
        else:
            path = [current]
        self.path = SearchPath (path)

    def do_help (self):
        """list the available commands
        """
        names = dir (self.__class__)
        for name in names:
            if name [:3] == 'do_':
                method = getattr (self, name)
                doc = method.__doc__
                if not doc:
                    doc = ''
                print name [3:] + ':', doc

    def do_list (self):
        """list all the files that have to be put on the FTP server
        """
        for filename in self.allFiles ():
            print filename

    def do_test (self):
        """show where the files are located
        """
        for filename in self.allFiles ():
            full = self.path.find (filename)
            if full:
                print full
            else:
                print "ERR:", filename

    def do_fillLocal (self):
        """copy files from archive that have not been updated
        """
        for filename in self.allFiles ():
            source, destination = self.path.findForCopy (filename)
            if not source:
                print "ERR:", filename
            elif source == destination:
                pass # already local
            else:
                print source, '\n    =>', destination
                shutil.copy (source, destination)

    def do_checkftp (self, checkMd5 = None):
        """show all files that are missing locally or on the ftp server
        """
        ftpFiles = self.getFTP ()
        for filename in self.allFiles ():
            if filename not in ftpFiles:
                print 'ftp   missing:', filename
                self.setError ()
                continue
            full = self.path.find (filename)
            if not full:
                print "local missing:", filename
                self.setError ()
                continue
            if checkMd5:
                if _verbose:
                    print 'md5', filename, '...'
                localmd5 = self.getMD5 (full)
                ftpmd5 = self.getFtpMD5 (filename)
                if localmd5 != ftpmd5:
                    print 'ERR MD5 Diff:', filename
                    print '       local:', localmd5
                    print '         ftp:', ftpmd5
                    self.setError ()


    def do_checkftpmd5 (self):
        """compare md5 sums for local files and ftp files
        """
        self.do_checkftp (checkMd5 = 1)

    def do_obsolete (self):
        """show all files that can be deleted from the ftp server
        """
        currentFiles = self.allFiles ()
        ftpFiles = self.getFTP ()
        for ftpFile in ftpFiles:
            if ftpFile not in currentFiles:
                print 'obsolete:', ftpFile

    def allFiles (self, includeRedirects = None):
        complete = []
        for generator in self.generators:
            filename = generator.filename (self.release, self.path)
            if not isinstance (filename, ReleaseRedirect) or includeRedirects:
                complete.append (filename)
        if self.sort:
            complete.sort ()
        result = []
        last = ''
        for filename in complete:
            if filename != last:
                result.append (filename)
                last = filename
        return result

    def getFTP (self):
        os.environ ['ftp_proxy'] = 'http://proxy:8080'
        url = 'ftp://ftp.sap.com/pub/maxdb/current/%d.%d.%02d/' % self.release [:3]
        data = urllib.urlopen (url).read ()
        rex = re.compile ('<A HREF="' + url + '([^"]+)')
        match = rex.search (data)
        result = []
        while match:
            file = match.group (1)
            if file [-1] != '/':
                result.append (file)
            match = rex.search (data, match.end ())
        return result

    def getFtpMD5 (self, fname):
        current = 'ftp://ftp.sap.com/pub/maxdb/current/%d.%d.%02d/' % self.release [:3]
        data = urllib.urlopen (current + fname + '.md5').read ()
        result, remotename = string.split (data)
        return result

    def getMD5 (self, fname):
        stream = open (fname, 'rb')
        m = md5.new ()
        mb = 1024 * 1024
        block = stream.read (mb)
        while block:
            m.update (block)
            block = stream.read (mb)
        result = m.hexdigest ()
        return result

    def setError (self):
        self.hasError = 1

class SearchPath:
    def __init__ (self, path):
        self.path = path
        self.fileCache = [None] * len (path)

    def listdir (self, index):
        if self.fileCache [index] is None:
            self.fileCache [index] = os.listdir (self.path [index])
        return self.fileCache [index]

    def searchForPattern (self, pattern):
        result = []
        for i in xrange (len (self.path)):
            filenames = self.listdir (i)
            for filename in filenames:
                match = pattern.match (filename)
                if match:
                    result.append ((filename, match))
            if len (result) > 0:
                return (i, result)
        return (0, [])

    def find (self, requested):
        for i in xrange (len (self.path)):
            filenames = self.listdir (i)
            if requested in filenames:
                result = os.path.join (self.path [i], requested)
                return result
        return None


    def findForCopy (self, requested):
        for i in xrange (len (self.path)):
            filenames = self.listdir (i)
            if requested in filenames:
                source = os.path.join (self.path [i], requested)
                destination = os.path.join (self.path [0], requested)
                return (source, destination)
        return (None, None)

_validSubsets = ['complete', 'all', 'rpms', 'sqldbc', 'lcpool',
    'jdbc', 'php', 'perldbd', 'odbc', 'interfaces', 'scripting',
    'wingui', 'sources', 'doc']

def checkOptions (options):
    if not options.release:
        fatalError ('option -R <release> must be set')
    match = re.match ('([0-d]+)[.]([0-d]+)[.]([0-d]+)[.]([0-d]+)', options.release)
    if not match:
        fatalError ('option -R <release> must be of the form x.x.xx.xx')
    major, minor, cl, build = map (int, match.group (1, 2, 3, 4))
    options.releaseTuple = (major, minor, cl, build)
    if options.verbose:
        global _verbose
        _verbose = 1
    if options.subset not in _validSubsets:
        fatalError ('options -subset must be in %s' % _validSubsets)

def fatalError (msg):
    sys.stderr.write ('releasePublisher.py: ' + msg + '\n')
    sys.exit (2)

def main (options, args):
    checkOptions (options)
    publisher = Publisher (options.releaseTuple, options.subset)
    if len (args) == 0:
        args = ['help']
    for arg in args:
        try:
            method = getattr (publisher, 'do_' + arg)
        except AttributeError:
            fatalError ('command ' + arg + ' not implemented')
        method ()
    if publisher.hasError:
        sys.exit (3)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('R', 'release', ':', None, 'Release to publish x.x.xx.xx'),
        ('v', 'verbose', '', None, 'verbose output'),
        ('subset', None, ':', 'complete', 'use subset of files %s' % _validSubsets),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

