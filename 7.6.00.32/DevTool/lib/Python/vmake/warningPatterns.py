
import sys
import re
import os

clWarnings = map (re.compile, [
    # warning.c(6) : warning C4018: '!=' : signed/unsigned mismatch
    r'[(]\d+[)] : warning [A-Z]+\d+:',
    ])

gccWarnings = map (re.compile, [
    r'[^:]+:\d+: warning:',
    # ../obj/cn/vcn14-p.o(.text+0x28de): the use of `tmpnam' is dangerous, better use `mkstemp'
    r'.*.o[(][.]text+0x[0-9a-f]+[)]:'
    ])

sunccWarnings = map (re.compile, [
    # "warnings.c", line 3: warning: macro redefined: something
    r'"[^"]+", line \d+: warning:',
    ])

hpccWarnings = map (re.compile, [
    # "warnings.c", line 3: warning 2001:
    r'"[^"]+", line \d: warning [a-fA-F0-9]+:',
    ])

aixccWarnings = map (re.compile, [
    # "warnings.c", line 3.9: 1506-236 (W) Macro name something has been redefined.
    r'"[^"]+", line \d+[.]\d+: \d+-\d+ ([WI])',
    ])

decccWarnings = map (re.compile, [
    # cc: Warning: warnings.c, line 3: The redefinition
    r'cc: Warning: '
    ])

vmakeWarnings = map (re.compile, [
    # gsp00.h in include list for hsp43.h but not included
    '[^ ]+ in include list for [^ ]+ but not included',
    # Include gsp00_2.h not in include list for hsp43.h
    'Include [^ ]+ not in include list for'
    ])

platform2warning = {
    'NT':       clWarnings,
    'WIN32':    clWarnings,
    'LINUX':    gccWarnings,
    'sun':      sunccWarnings,
    'hp':       hpccWarnings,
    'HPUX':     hpccWarnings,
    'rs6000':   aixccWarnings,
    'rs6000_51':aixccWarnings,
    'alphaosf': decccWarnings,
    'OSF1':     decccWarnings,
}

class WarningFilter:
    def containsWarning (self, platform, text, filename = None):
        platformWarnings = platform2warning.get (platform, [])
        for rex in platformWarnings + vmakeWarnings:
            if rex.search (text):
                return 1
        return None

