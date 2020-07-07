#!/usr/bin/env python
# allsources.py

import string
import os

_platforms = [
    'linux',
    'aix64',
    'sun64',
    'sunx86',
    'hp64',
    'dec',
    'ia64']

_platformVars = [
    # MACH,  OS,      OSSPEC,  BIT64, other
    ('I386', 'WIN32', 'WIN32', None,  None),
    ('I386', 'WIN32', 'WIN32', 1,     None),
    ('I386', 'UNIX',  'LINUX', None,  'LNX_VER=GENERIC'),
    ('I386', 'UNIX',  'LINUX', None,  '__HAS_NPTL=1;LNX_VER=SLES9'),
    ('X86_64', 'UNIX',  'LINUX', 1,   '__HAS_NPTL=1;LNX_VER=SLES9'),
    ('IA64', 'UNIX',  'LINUX', 1,     '__HAS_NPTL=1;LNX_VER=SLES9'),
    ('SUN',  'UNIX',  'SVR4',  None,  None),
    ('SUN',  'UNIX',  'SVR4',  1,     None),
    ('SPARC',  'UNIX',  'SOLARIS',  None,  None),
    ('SPARC',  'UNIX',  'SOLARIS',  1,     None),
    ('I386',  'UNIX',  'SOLARIS',  None,  None),
    ('X86_64',  'UNIX',  'SOLARIS',  1,     None),
    ('_IBMR2', 'UNIX','AIX',   None,  None),
    ('_IBMR2', 'UNIX','AIX',   1,     None),
    ('PA11',  'UNIX', 'HPUX',  None,  None),
    ('PA20W', 'UNIX', 'HPUX',  1,     None),
    ('HP_IA64',  'UNIX', 'HPUX',  1,  None),
    ('ALPHA', 'UNIX', 'OSF1',  1,     None),
]

_fastTargets = ['allopensource.mac']
_quickTargets = ['quickknl']
_slowTargets = ['slowknl']
_cmdPrefix = 'perl %(TOOL)s/bin/' % os.environ

_platformSpecific = [
    'src/en/ven38NMP.cpp',
    'src/eo/heo35k.h',
    'src/eo/veo35k.cpp',
]

_vmakebugSources = [
    # imf -m gives src/Globals.com, src/ptoc.com
    'src/Globals/Globals.com',
    'src/ptoc/ptoc.com',
]

_additionalSources = [
]

_intentionalMissingSources = [
    'src/redist/bin/ALPHA_OSF1_64/python',
    'src/redist/bin/HP9_HPUX_32/python',
    'src/redist/bin/HP_IA64_HPUX_64/python',
    'src/redist/bin/IA64_LINUX_64/python',
    'src/redist/bin/IA64_WIN64/python',
    'src/redist/bin/IA64_WIN64/pyexpat.pyd',
    'src/redist/bin/IA64_WIN64/pysapdb.exe',
    'src/redist/bin/IA64_WIN64/python21.dll',
    'src/redist/bin/IA64_WIN64/python23.dll',
    'src/redist/bin/IA64_WIN64/zlib.pyd',
    'src/redist/bin/IA64_WIN64/_socket.pyd',
    'src/redist/bin/IA64_WIN64/_sre.pyd',
    'src/redist/bin/PA11_HPUX_32/python',
    'src/redist/bin/PA20W_HPUX_64/python',
    'src/redist/bin/S390X_LINUX_64/python',
    'src/redist/bin/SDBonPPC64_LINUX_64/python',
    'src/redist/bin/SUN_SVR4_32/python',
    'src/redist/bin/SUN_SVR4_64/python',
    'src/redist/bin/SPARC_SOLARIS_32/python',
    'src/redist/bin/SPARC_SOLARIS_64/python',
    'src/redist/bin/I386_SOLARIS_32/python',
    'src/redist/bin/X86_64_SOLARIS_64/python',
    'src/redist/bin/_IBMR2_AIX_32/python',
    'src/redist/bin/_IBMR2_AIX_64/python',
    'src/redist/bin/X86_64_LINUX_64/python',
    'src/redist/binpy23/ALPHA_OSF1_64/python',
    'src/redist/binpy23/HP9_HPUX_32/python',
    'src/redist/binpy23/HP_IA64_HPUX_64/python',
    'src/redist/binpy23/IA64_LINUX_64/python',
    'src/redist/binpy23/IA64_WIN64/python',
    'src/redist/binpy23/IA64_WIN64/pyexpat.pyd',
    'src/redist/binpy23/IA64_WIN64/pysapdb.exe',
    'src/redist/binpy23/IA64_WIN64/python23.dll',
    'src/redist/binpy23/IA64_WIN64/zlib.pyd',
    'src/redist/binpy23/IA64_WIN64/_socket.pyd',
    'src/redist/binpy23/IA64_WIN64/_sre.pyd',
    'src/redist/binpy23/PA11_HPUX_32/python',
    'src/redist/binpy23/PA20W_HPUX_64/python',
    'src/redist/binpy23/S390X_LINUX_64/python',
    'src/redist/binpy23/SDBonPPC64_LINUX_64/python',
    'src/redist/binpy23/SUN_SVR4_32/python',
    'src/redist/binpy23/SUN_SVR4_64/python',
    'src/redist/binpy23/SPARC_SOLARIS_32/python',
    'src/redist/binpy23/SPARC_SOLARIS_64/python',
    'src/redist/binpy23/I386_SOLARIS_32/python',
    'src/redist/binpy23/X86_64_SOLARIS_64/python',
    'src/redist/binpy23/_IBMR2_AIX_32/python',
    'src/redist/binpy23/_IBMR2_AIX_64/python',
    'src/redist/binpy23/X86_64_LINUX_64/python',
]

class AllSources:
    def __init__ (self, verbose, fastTargets, quickTargets, slowTargets,
            debug = None):
        self.verbose = verbose
        self.debug = debug
        self.fastTargets = fastTargets
        self.quickTargets = quickTargets
        self.slowTargets = slowTargets
        self.sources = {}
        for source in _additionalSources + _vmakebugSources:
            self.sources [source] = 1

    def addLocalSources (self):
        if self.verbose:
            print 'local',
        self.readSourcesFromPipe ('imf.pl', self.fastTargets)
        self.readSourcesFromPipe ('imq.pl', self.quickTargets)
        self.readSourcesFromPipe ('ims.pl', self.slowTargets)
        if self.verbose:
            print '=>', len (self.sources.keys ())

    def addRemoteSources (self, platform):
        if self.verbose:
            print platform,
        self.readSourcesFromPipe ('remimf.pl ' + platform, self.fastTargets)
        self.readSourcesFromPipe ('remimq.pl ' + platform, self.quickTargets)
        self.readSourcesFromPipe ('remims.pl ' + platform, self.slowTargets)
        if self.verbose:
            print '=>', len (self.sources.keys ())

    def addPlatformSources (self, mach, osvar, osspec, isbit64, other):
        envvar = 'MACH=%s;OS=%s;OSSPEC=%s' % (mach, osvar, osspec)
        if isbit64:
            envvar = envvar + ';BIT64=1'
            bit = 64
        else:
            bit = 32
        if other:
            envvar = envvar + ';' + other
        if osvar == 'UNIX':
            envvar = envvar + ';COREPERL_INCLUDE=YES'
        if self.verbose:
            print '%-6s %-6s %d' % (osspec, mach, bit),
        os.putenv ('TOOLVARS_PATCH', envvar)
        if self.debug:
            self.debugfile = open (envvar, 'w')
        else:
            self.debugfile = None
        self.readSourcesFromPipe ('imf.pl', self.fastTargets)
        self.readSourcesFromPipe ('imq.pl', self.quickTargets)
        self.readSourcesFromPipe ('ims.pl', self.slowTargets)
        if self.verbose:
            print '=>', len (self.sources.keys ())
        if self.debugfile:
            self.debugfile.close ()


    def writeResult (self, outfile):
        for source in _intentionalMissingSources:
            try:
                del self.sources [source]
            except KeyError:
                pass
        sources = self.sources.keys ()
        sources.sort ()
        stream = open (outfile, 'w')
        for source in sources:
            stream.write (source + '\n')
        stream.close ()

    def readSourcesFromPipe (self, cmd, targets):
        cmdString = '%s -m %s' % (cmd, string.join (targets))
        lines = os.popen (cmdString, 'r').readlines ()
        for line in lines:
            if self.debugfile:
                self.debugfile.write (line)
            startDir = string.split (line, '/') [0]
            if startDir in ['src', 'desc']:
                self.sources [string.strip (line)] = 1

def writeMissingSources ():
    stream = open ('missing.lst', 'w')
    for fname in _intentionalMissingSources:
        stream.write (fname + '\n')
    stream.close ()


def checkOptions (options):
    if not options.ftarget and not options.qtarget and not options.starget:
        options.ftarget = _fastTargets
        options.starget = _slowTargets
        global _additionalSources
        _additionalSources = _platformSpecific
    if not hasattr (options, 'debug'):
        options.debug = None
    if not hasattr (options, 'pstart'):
        options.pstart = 0
    if not hasattr (options, 'pend'):
        options.pend = 0

def main (options, args):
    if os.environ.has_key ('SAPDB_INTERNAL'):
        os.putenv ('SAPDB_INTERNAL', '')
    checkOptions (options)
    if options.missing:
        writeMissingSources ()
    else:
        sources = AllSources (options.verbose, options.ftarget,
            options.qtarget, options.starget, options.debug)
        #sources.addLocalSources ()
        #for platform in _platforms:
        #    sources.addRemoteSources (platform)
        if options.pstart and options.pend:
            platformVars = _platformVars [options.pstart:options.pend]
        elif options.pstart:
            platformVars = _platformVars [options.pstart:]
        elif options.pend:
            platformVars = _platformVars [:options.pend]
        else:
            platformVars = _platformVars
        for mach, osvar, osspec, isbit64, other in platformVars:
            sources.addPlatformSources (mach, osvar, osspec, isbit64, other)
        sources.writeResult (options.outfile)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('o', 'outfile', ':', 'sources.lst', 'name of output file'),
        ('ftarget', None, '::', None, 'fast vmake targets to list'),
        ('qtarget', None, '::', None, 'fast vmake targets to list'),
        ('starget', None, '::', None, 'fast vmake targets to list'),
        ('missing', None, '', None, 'write list of missing files only'),
        ('v', 'verbose', '', None, 'verbose output'),
        #('debug', None, '', None, 'write debug infos'),
        #('pstart', None, ':', 0, 'restrict platfgorms from start'),
        #('pend', None, ':', 0, 'restrict platfgorms from end'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

