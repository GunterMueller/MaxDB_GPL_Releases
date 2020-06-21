
# downloads.py.py

import sys
import os
import re
import stat
import md5
from cStringIO import StringIO
from sets import Set, ImmutableSet

import releasePublisher

class Directory:
    cacheVersion = 1

    def __init__ (self, path):
        self.cache = {}
        self.init (path)

    def init (self, path):
        if os.path.isfile (path):
            path = os.path.dirname (path)
        self.path = path
        self.loadCache ()

    def loadCache (self):
        cacheFile = os.path.join (self.path, '.md5cache')
        if os.path.exists (cacheFile):
            variables = {'cacheVersion': None}
            try:
                execfile (cacheFile, variables)
            except:
                log ('exception when reading cache file')
                return
            if variables ['cacheVersion'] != self.cacheVersion:
                log ('wronmg cache version')
                return
            try:
                self.cache = variables ['cache']
            except NameError:
                log ('no cache variable')
                pass
            log ('cache read')
        else:
            log ('cachefile not found')
    
    def saveCache (self, trimFluff = None):
        cacheFile = os.path.join (self.path, '.md5cache')
        stream = open (cacheFile, 'w')
        stream.write("""#
# this file contains the MD5 sums
# if there are any problems, simply delete this file, it will be recreated
# contact DanielD for details
#
cacheVersion = %s

cache = {
""" % self.cacheVersion)
        items = self.cache.items ()
        items.sort ()
        for fname, info in items:
            if trimFluff:
                if not os.path.exists (os.path.join (self.path, fname)):
                    continue
            stream.write ('    %s: %s,\n' % (repr (fname), info))
        stream.write ('}\n')
        stream.close ()

    def getFileInfo (self, fname):
        fullname = os.path.join (self.path, fname)
        if not os.path.exists (fullname):
            print 'ERR: missing ' + fname
            return 0, 'unknown md5'
        statinfo = os.stat (fullname)
        size = statinfo [stat.ST_SIZE]
        mtime = statinfo [stat.ST_MTIME]
        md5result = None
        requiresUpdate = None
        try:
            cachemtime, cachemd5 = self.cache [fname]
            if cachemtime == mtime:
                md5result = cachemd5
                log ('used md5 from cache')
            else:
                log ('mtime differ')
        except KeyError:
            log ('no entry in cache for ' + fname)
            pass
        if not md5result:
            md5result = self.getMD5 (fullname)
            requiresUpdate = True
        self.cache [fname] = (mtime, md5result)
        if requiresUpdate:
            self.saveCache ()
        return size, md5result

    def getMD5 (self, fname):
        print 'calculating MD5 sum for', os.path.basename (fname)
        stream = open (fname, 'rb')
        m = md5.new ()
        mb = 1024 * 1024
        block = stream.read (mb)
        while block:
            m.update (block)
            block = stream.read (mb)
        result = m.hexdigest ()
        return result

    def listdir (self):
        result = []
        localOnly = self._loadFileList ('.not-in-archive')
        for fname in os.listdir (self.path):
            if fname not in localOnly:
                result.append (fname)
        return result

    def _loadFileList (self, fname):
        fullpath = os.path.join (self.path, fname)
        if os.path.exists (fullpath):
            entries = open (fullpath, 'r').read ().split ('\n')
            return ImmutableSet (entries)
        else:
            return ImmutableSet ()
    

class ArchiveDirectory:
    def __init__ (self, release):
        self.cache = []
        fname = '%d.%d.%02d.%02d.cache' % release
        lines = open (os.path.join ('archivecache', fname), 'r').read ().split ('\n')
        for line in lines:
            try:
                tuple = eval (line)
                self.cache.append (tuple)
            except SyntaxError:
                pass
    
    def getFileInfo (self, fname):
        size = None
        md5result = None
        for entry in self.cache:
            filename = entry [1]
            if filename == fname:
                size = entry [3]
        return size, md5result

    def listdir (self):
        result = []
        for entry in self.cache:
            result.append (entry [1])
        return result

class ArchivePublisher:
    def __init__ (self, directory, groupname):
        self.directory = directory
        self.groupname = groupname

    patterns = {
        'all': re.compile ('^maxdb-all'),
        'sources': re.compile ('^maxdb-source'),
        'rpmx86': re.compile ('^maxdb-.*.i[3-8]86.rpm'),
        'rpmia64': re.compile ('^maxdb-.*.ia64.rpm'),
        'rpmx86_64': re.compile ('^maxdb-.*.x86_64.rpm'),
        'client': re.compile ('^maxdb-client'),
        'dbmgui': re.compile ('^maxdb-dbmgui'),
        'sqlstudio': re.compile ('^maxdb-sqlstudio'),
        'odbc': re.compile ('^maxdb-odbc'),
        'jdbc': re.compile ('^sapdbc'),
        'sqldbc': re.compile ('^maxdb-sqldbc.*[^m]$'),
        'perl': re.compile ('^maxdb-perl'),
        'perldbd': re.compile ('^DBD-MaxDB'),
        'php': re.compile ('^maxdb-php'),
        'python': re.compile ('^maxdb-python'),
        'plainhtml': re.compile ('^maxdb-htmldoc'),
        'htmlhelp': re.compile ('^maxdb-chmdoc'),
        'regressionTests': re.compile ('^maxdb-test-source'),
        'buildtools': re.compile ('^maxdb-buildtools'),

    }
    def allFiles (self, includeRedirects = None):
        result = []
        rex = self.patterns [self.groupname]
        for fname in self.directory.listdir ():
            if rex.match (fname):
                result.append (fname)
        return result

class TableOfContens:
    def __init__ (self):
        self.stream = StringIO ()
        self.inlist = None
        self.level1tag = None
        self.firstEntry = True

    def addLevel1 (self, title, tag):
        if self.inlist:
            self.stream.write ('</ul>\n')
            self.inlist = None
        self.level1info = (title, tag)

    def activateLevel1 (self):
        if self.level1info:
            title, tag = self.level1info
            if self.firstEntry:
                self.firstEntry = False
            else:
                pass 
                # self.stream.write ('<br>\n')
            self.stream.write ('<h3><a href="#%s">%s</a></h3>\n' % (tag, title))
            self.level1info = None
            return '<a name="%s"><!-- x --></a>\n' % tag
        else:
            return ''
    
    def addLevel2 (self, title, tag):
        if not self.inlist:
            self.stream.write ('<ul>\n')
            self.inlist = True
        self.stream.write ('    <li><a href="#%s">%s</a></li>\n' % (tag, title))

    def close (self):
        if self.inlist:
            self.stream.write ('</ul>\n')
        self.stream.write ('<br>\n')

    def updateHTML (self, htmlsource):
        self.close ()
        tochtml = self.stream.getvalue ()
        searchstring = '<!-- Table of contents -->'
        pos = htmlsource.index (searchstring)
        if pos != -1:
            pos = pos + len (searchstring)
            htmlsource = htmlsource [:pos] + tochtml + htmlsource [pos:]
        return htmlsource

class DownloadHTML:
    mirror = r'\\p26327\OpenSrc\pub\maxdb\current'
    archivemirror = r'\\p26327\OpenSrc\pub\maxdb\archive'
    def __init__ (self, release, forArchive):
        self.release = release
        self.forArchive = forArchive
        if forArchive:
            self.releaseString = '%d.%d.%02d.%02d' % release
            path = os.path.join (self.archivemirror, self.releaseString)
            if os.path.isdir(path):
                self.downloadDir = Directory (path)
            else:
                self.downloadDir = ArchiveDirectory (release)
        else:
            self.releaseString = '%d.%d.%02d' % release [:3]
            path = os.path.join (self.mirror, self.releaseString)
            self.downloadDir = Directory (path)

    def genHTML (self, stream = None):
        if stream is None:
            stream = sys.stdout
            stream.write ("<html>\n<head><title>MaxDB downloads</title></head>\n<body>\n")
        stream.write (tableHTML)
        for title, tag, groups in onMain + onClient:
            self.genGroup (stream, title, tag, groups)
        stream.write ('</table>\n')

    def patchPage (self):
        if self.forArchive:
            inname = r'\\p26327\OpenSrc\pub\maxdb\archive-template.html'
            outname = 'archives/%d.%d.%02d.%02d.html' % self.release
        else:
            inname = outname = '%d.%d.%02d.html' % self.release [:3]
        if not os.path.exists (inname):
            fatalError ('cannot open file ' + inname)
        source = open (inname, 'r').read ()
        if self.forArchive:
            source = source.replace ('XXXRELEASEXXX', self.releaseString)
        if self.forArchive:
            allGroups = allPackages + clientPackages + sourcePackages
        else:
            allGroups = allPackages + clientPackages + docPackages + sourcePackages
        toc = TableOfContens ()
        firstGroupInChapter = None
        for title, tag, groups in allGroups:
            if groups == chapterTitle:
                toc.addLevel1 (title, tag)
                firstGroupInChapter = (tag, title)
                continue
            oldsource = source
            stream = StringIO ()
            startRex, endRex = self.sectionPatterns (tag)
            #
            # write including start comment
            #
            match = re.search (startRex, source)
            if not match:
                source = (source  #('\n<h3><a name="%s">%s</h3>\n' % (tag, title))
                    + '<!-- Download section ' + tag + ' begin -->\n'
                    + '<!-- Download section ' + tag + ' end -->\n')
                match = re.search (startRex, source)
                # fatalError ("can't find 'Download section ' + groupname + ' begin' in " + fname)
            stream.write (source [:match.end ()])
            remainder = source [match.end ():]
            #
            # generate table
            #
            try:
                hasElements, firstGroupInChapter = self.genGroup (stream, 
                        title, tag, groups, toc, firstGroupInChapter)
            except AttributeError:
                raise
                print title, 'not defined in release', self.release
                source = oldsource
                continue
            #
            # write from closing comment
            #
            match = re.search (endRex, remainder)
            if not match:
                fatalError ("can't find 'Download section end' in " + fname)
            stream.write (remainder [match.start ():])
            #
            # 
            #
            source = stream.getvalue ()
        #
        # write to file
        #
        if self.forArchive:
            source = toc.updateHTML (source)
        output = open (outname, 'w')
        output.write (source)
        output.close ()
        if self.forArchive:
            print "\nwrote new Page '%s'" % outname
        else:
            print "\noverwrote Page '%s'" % outname

    def sectionPatterns (self, groupname):
        return (
            r'<!--\s*Download\s+section\s+%s\s+begin\s*-->' % groupname,
            r'<!--\s*Download\s+section\s+%s\s+end\s*-->' % groupname
            )
    
    def genGroup (self, stream, title, tag, groups, toc, firstGroupInChapter):
        titleWritten = False
        if self.forArchive:
            releasepath = '%d.%d.%02d.%02d' % self.release
        else:
            releasepath = '%d.%d.%02d' % self.release [:3]
        colors = ['#dae6ea', '#ffffff']
        colorIndex = 0
        for group in groups:
            if self.forArchive:
                publisher = ArchivePublisher (self.downloadDir, group)
            else:
                publisher = releasePublisher.Publisher (self.release, group, sort = None)
            fileinfos = []
            for fname in publisher.allFiles (includeRedirects = not self.forArchive):
                isredirect = isinstance (fname, releasePublisher.ReleaseRedirect)
                if isredirect:
                    order, pkgname, filerelease = None, None, None
                else:
                    pkgname, filerelease = self.getAttributesFromName (fname)
                    order = sortOrder (pkgname)
                fileinfos.append ((isredirect, order, pkgname, fname, filerelease))
            fileinfos.sort ()
            for isredirect, order, pkgname, fname, filerelease in fileinfos:
                if not titleWritten:
                    if firstGroupInChapter:
                        if self.forArchive:
                            stream.write ('<a name="%s"><h2>%s</h2></a>\n' % firstGroupInChapter)
                            firstGroupInChapter = None
                    else:
                        stream.write ('<br>\n')
                    anchor = toc.activateLevel1 ()
                    stream.write ('<a name="%s"><h3>%s</h3></a>\n' % (tag, title))
                    stream.write (tableHTML)
                    stream.write (anchor)
                    #stream.write (titleHTML % locals ())
                    toc.addLevel2 (title, tag)
                    titleWritten = True
                bgcolor = colors [colorIndex]
                if isredirect:
                    comment = fname.comment
                    stream.write (redirectHTML % locals ())
                else:
                    # pkgname, filerelease = self.getAttributesFromName (fname)
                    filesize, filemd5 = self.downloadDir.getFileInfo (fname)
                    filesize = formatSize (filesize)
                    # print pkgname, fname, filerelease, filesize, filemd5
                    # sys.stdout.write ('.')
                    if self.forArchive:
                        downloadText = 'Download'
                        downloadLink = 'http://downloads.mysql.com/archives/maxdb/%s/%s' 
                    else:
                        downloadText = 'Pick a mirror'
                        downloadLink = '/get/Downloads/MaxDB/%s/%s/from/pick'
                    downloadLink = downloadLink % (releasepath, fname)
                    stream.write (entryHTML % locals ())
                    if filemd5:
                        stream.write (md5HTML % locals ())
                colorIndex = not colorIndex
        if titleWritten:
            stream.write ('</table>\n')
        return titleWritten, firstGroupInChapter

    def getAttributesFromName (self, fname):
        #
        # all package 
        #
        match = re.match ('maxdb-[a-z]+-([a-z0-9]+)-([0-9]+)b?it-([a-z0-9_]+)-([0-9_]+)[.]...$', fname)
        if match:
            os, bitness, mach, release = match.group (1, 2, 3, 4)
            pkgname = self.platformTitle (os, mach, bitness)
            release = release.replace ('_', '.')
            return pkgname, release
        #
        # sources
        #
        match = re.match ('maxdb-(test-)?source-([0-9_]+)[.](...)', fname)
        if match:
            test, release, extension = match.group (1, 2, 3)
            pkgname = 'Source code'
            if extension == 'zip':
                pkgname = pkgname + ' (Microsoft Windows)'
            release = release.replace ('_', '.')
            return pkgname, release

        #
        # build tools 
        #
        match = re.match ('maxdb-buildtools-([a-z0-9]+)-([a-z0-9_]+)-([0-9]+)[.]...', fname)
        if match:
            os, mach, release = match.group (1, 2, 3)
            pkgname = self.platformTitle (os, mach)
            return pkgname, release
        #
        # build tools sourcec
        #
        match = re.match ('maxdb-buildtools-source-([0-9]+)[.]...', fname)
        if match:
            release = match.group (1)
            return 'Source code', release
        #
        # parse rpms
        #
        match = re.match ('maxdb-([a-z0-9]+)-([0-9.]+)-[0-9]+[.][0-9a-z_]+[.]rpm', fname)
        if match:
            package, release = match.group (1, 2)
            package = findMapping (package, rpmMap, 'rpmMap')
            release = tuple (map (int, release.split ('.')))
            release = '%d.%d.%02d.%02d' % release
            return package, release
        #
        # setups
        #
        match = re.match ('maxdb-([a-z0-9]+)-setup-([0-9_]+)[.]exe', fname)
        if match:
            release = match.group (2)
            release = release.replace ('_', '.')
            return 'Microsoft Windows installer', release
        #
        # setups in new release notation (+ changelist)
        #
        match = re.match ('maxdb-([a-z0-9]+)-setup-([0-9_]+)(-[0-9]+)[.]exe', fname)
        if match:
            release, changelist = match.group (2, 3)
            release = release.replace ('_', '.')
            return 'Microsoft Windows installer', release + changelist
        #
        # jdbc
        #
        match = re.match ('sapdbc-(source-)?([0-9_]+)[.]jar', fname)
        if match:
            source, release = match.group (1, 2)
            if source:
                package = 'Source code'
            else:
                package = 'Binary'
            release = release.replace ('_', '.')
            return package, release
        #
        # Perl DBD
        #
        match = re.match ('DBD-MaxDB-([0-9_]+)', fname)
        if match:
            release = match.group (1)
            release = release.replace ('_', '.')
            return 'Source code', release
        #
        # php
        #
        match = re.match ('maxdb-php(.*)-([0-9_]+)[.]', fname)
        if match:
            package, release = match.group (1, 2)
            if package.startswith ('-docu'):
                package = 'Documentation (HTML)'
            elif package.startswith ('-xmldocu'):
                package = 'Documentation (XML)'
            elif package.startswith ('-source'):
                package = 'Source code'
            elif package.startswith ('4') or package.startswith ('-v4'):
                package = 'Microsoft Windows (IA32, 32 bit) for PHP 4'
            elif package.startswith ('5') or package.startswith ('-v5'):
                package = 'Microsoft Windows (IA32, 32 bit) for PHP 5'
            elif package == '-win-32bit':
                package = 'Microsoft Windows (IA32, 32 bit) for PHP 4'
            release = release.replace ('_', '.')
            return package, release
        #
        # docs
        #
        match = re.match ('maxdb-.*doc(-de)?-([0-9]+)(_[0-9]+)?', fname)
        if match:
            language, release, saprelease = match.group (1, 2, 3)
            if not language:
                language = 'English'
            elif language == '-de':
                language = 'German'
            else:
                language = ''
            release = release [0] + '.' + release [1]
            if saprelease:
                release = release + '-' + saprelease [1:]
            return language, release
        #
        # sqldbc docu
        #
        match = re.match ('maxdb-.*-docu-([0-9_]+)[.]', fname)
        if match:
            release = match.group (1)
            release = release.replace ('_', '.')
            return 'Documentation', release
        #
        # scripting packages package with old fashioned release number 
        #
        match = re.match ('maxdb-[a-z]+-([a-z0-9]+)-([0-9]+)b?it-([a-z0-9_]+)-([0-9.]+)[.]...', fname)
        if match:
            os, bitness, mach, release = match.group (1, 2, 3, 4)
            pkgname = self.platformTitle (os, mach, bitness)
            return pkgname, release
        #
        # fallback, should really be an error
        #
        return 'unknown package ' + fname, 'unknown release'

    def platformTitle (self, os, mach, bitness = None):
        ostitle, machtitle = findMapping ((os, mach), platformMap, 'platformMap')
        if bitness is None:
            if machtitle:
                result = '%s (%s)' % (ostitle, machtitle)
            else:
                result = ostitle
        else:
            if machtitle:
                result = '%s (%s, %s bit)' % (ostitle, machtitle, bitness)
            else:
                result = '%s (%s bit)' % (ostitle, bitness)
        return result

KB = 1024 * 1.0
MB = KB * 1024

def formatSize (size):
    if size < MB:
        kbs = size / KB
        return '%.1f KB' % kbs
    else:
        mbs = size / MB
        return '%.1f MB' % mbs
    
tableHTML = '<table border="0" cellpadding="2" cellspacing="0" width="99%">\n'

titleHTML = """<tr><td colspan="5"><a name="%(tag)s"><!-- x --></a><h3>%(title)s</h3></td></tr>
"""

entryHTML = """<tr bgcolor="%(bgcolor)s"><td width="42%%">%(pkgname)s</td><td>
  <td width="22%%">%(filerelease)s</td><td align="right" width="18%%">%(filesize)s</td>
  <td align="right" width="18%%"><a href="%(downloadLink)s">%(downloadText)s</a></td></tr>
"""

md5HTML = """<tr bgcolor="%(bgcolor)s"><td align="right" colspan="5"><small style="color:#777777">MD5: <code class="md5">%(filemd5)s</code></small></td></tr>
"""

redirectHTML = """<tr bgcolor="%(bgcolor)s"><td align="left" colspan="5">%(comment)s</td></tr>
<tr bgcolor="%(bgcolor)s"><td align="right" colspan="5">&nbsp;</td></tr>
"""

chapterTitle = 'chapterTitle'

allPackages = [
    # title, tag, filegroups (see releasePublisher.R76 for valid names)
    ('Complete Software Packages', 'complete', chapterTitle),
    ('MaxDB Installer (exe/tgz Packages)', 'complete_installer', ['all', 'sources']),
    ('Linux IA32, 32 bit (rpm Packages)', 'Linux_x86_RPM', ['rpmx86']),
    ('Linux IA64, 64 bit (rpm Packages)', 'Linux_IA64_RPM', ['rpmia64']),
    ('Linux x86_64, 64 bit (rpm Packages)', 'Linux_AMD_Opteron_RPM', ['rpmx86_64']),
]

clientPackages = [
    ('Client Packages', 'clients', chapterTitle),
    ('MaxDB Installer (Client Baskets)', 'client_installer', ['client']),
    ('DBMGUI', 'DBMGUI', ['dbmgui']),
    ('SQL Studio', 'SQL_Studio', ['sqlstudio']),
    ('ODBC Driver', 'ODBC_Driver', ['odbc']),
    ('JDBC Driver', 'JDBC_Driver', ['jdbc']),
    ('SQLDBC', 'SQLDBC', ['sqldbc']),
    ('Perl', 'Perl', ['perl']),
    ('DBD::MaxDB', 'DBD__MaxDB', ['perldbd']),
    ('PHP', 'PHP', ['php']),
    ('Python', 'Python', ['python']),
]

docPackages = [
    ('Documentation', 'docu', chapterTitle),
    ('Plain HTML', 'Plain_HTML', ['plainhtml']),
    ('HTMLHelp', 'HTMLHelp', ['htmlhelp']),
]

sourcePackages = [
    ('Development', 'develop', chapterTitle),
    # ('Sources', 'Sources', ['sources']),
    ('Regression Tests', 'Regression_Tests', ['regressionTests']),
    ('Build Tools', 'Build_Tools', ['buildtools']),
]

def findMapping (key, mapping, mappingname):
    try:
        return mapping [key]
    except KeyError:
        fatalError ("ERR: please  add an entry for '%s' into %s in file downloads.py"
            % (key, mappingname))

platformMap = {
    ('aix', 'ppc')      : ('IBM AIX', None),
    ('aix5', 'ppc')     : ('IBM AIX', None),
    ('hpux', 'hppa')    : ('HP-UX', 'PA-RISC'),
    ('hpux', 'ia64')    : ('HP-UX', 'IA64'),
    ('linux', 'i386')   : ('Linux', 'IA32'),
    ('linux', '386')    : ('Linux', 'IA32'),
    ('linux', 'ia64')   : ('Linux', 'IA64'),
    ('linux', 'ppc')    : ('Linux', 'Power'),
    ('linux', 'x86_64') : ('Linux', 'x86_64'),
    ('sun', 'sparc')    : ('SUN Solaris', 'SPARC'),
    ('sun', 'x86_64')   : ('SUN Solaris', 'x64'),
    ('win', 'i386')     : ('Microsoft Windows', 'IA32'),
    ('win', 'ia64')     : ('Microsoft Windows', 'IA64'),
    ('win', 'x86_64')   : ('Microsoft Windows', 'x64'),
    ('tru64', 'alpha')  : ('Tru64', 'Alpha'),
}

rpmMap = {
    'callif': 'ODBC, JDBC',
    'ind': 'Release independent',
    'precompiler': 'C/C++ Precompiler',
    'sqldbc': 'SQLDBC',
    'scriptif': 'Perl, Python',
    'srv75': 'Server',
    'srv76': 'Server',
    'testdb75': 'Test Database',
    'testdb76': 'Test Database',
    'web': 'Web tools',
}

_explicitSortOrder = [
    # installation platforms
    'Microsoft Windows (IA32, 32 bit)',
    'Microsoft Windows (IA64, 64 bit)',
    'Microsoft Windows (x64, 64 bit)',
    'Linux (IA32, 32 bit)',
    'Linux (IA64, 64 bit)',
    'Linux (x86_64, 64 bit)',
    'Linux (Power, 64 bit)',
    'SUN Solaris (SPARC, 32 bit)',
    'SUN Solaris (SPARC, 64 bit)',
    'SUN Solaris (x64, 64 bit)',
    'IBM AIX (32 bit)',
    'IBM AIX (64 bit)',
    'HP-UX (PA-RISC, 32 bit)',
    'HP-UX (PA-RISC, 64 bit)',
    'HP-UX (IA64, 64 bit)',
    'Tru64 (Alpha, 64 bit)',
    # rpm packages
    'Release independent',
    'Server',
    'Test Database',
    'ODBC, JDBC',
    'Perl, Python',
    'Web tools',
    # other
    'Documentation',
    'Source code',
    'Source code (Microsoft Windows)',
]

sortMap = dict ([(item, index) for index, item in enumerate (_explicitSortOrder)])

def sortOrder (packageName):
    return sortMap.get (packageName, 300)

def fatalError (msg):
    print 'ERR:', msg
    sys.exit (2)

def log (*args):
    pass
    #sys.stdout.write ('<LOG text="*** ' + ' '.join (args) + '">\n')

def checkOptions (options):
    if not options.release:
        fatalError ('option -R <release> not set')
    release = options.release.split ('.')
    if len (release) != 4:
        fatalError ('argument to option -R must be of form X.X.XX.XX')
    try:
        release = map (int, release)
    except ValueError:
        fatalError ('argument to option -R must be of form X.X.XX.XX with X being digits')
    options.release = tuple (release)

def main (options, args):
    """generates the mysql.com download pages
    """
    checkOptions (options)
    generator = DownloadHTML (options.release, options.archive)
    generator.patchPage ()

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('R', 'release', ':', None, 'release X.X.XX.XX'),
        ('archive', None, '', None, 'generate archive page'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

