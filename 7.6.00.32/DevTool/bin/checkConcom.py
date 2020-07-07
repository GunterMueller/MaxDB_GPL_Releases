#!/usr/bin/env python
# checkConcom.py

import string
import re
import os

concomRE = re.compile ('(&.*)')
ppRE = re.compile (r'\s*#.*/[*]\s*(&.*)[*]/')

def checkConcom (fname, rex):
    lineno = 0
    lines = open (fname, 'r').readlines ()
    concomStack = []
    for line in lines:
        lineno = lineno + 1
        match = rex.match (line)
        if match:
            concom = match.group (1)
            keyword = string.split (concom) [0]
            if keyword in ['&elif', '&endif']:
                if len (concomStack) == 0:
                    print '%s:%d: missing &if' % (fname, lineno)
                    return 1
                concomStack.pop ()
            if keyword in ['&ifdef', '&if', '&elif']:
                concomStack.append ((lineno, keyword))
    if len (concomStack) > 0:
        lineno, keyword = concomStack [-1]
        print '%s:%d: missing &endif' % (fname, lineno)
        return 1
            
    return 0    
    
def checkPascalConcom (fname):
    return checkConcom (fname, concomRE)

def checkCConcom (fname):
    return checkConcom (fname, ppRE)

def getFiles (args):
    result = []
    for arg in args:
        if os.path.isdir (arg):
            for entry in os.listdir (arg):
                result.append (os.path.join (arg, entry))
    return result

pascalVDN = re.compile ('[vg][a-z][a-z][0-9]+$')    
    
def isPascalFile (fname):
    return pascalVDN.match (os.path.basename (fname))

def getExt (fname):
    return string.lower (os.path.splitext (fname) [1])

def isCFile (fname):
    return getExt (fname) in ['.h', '.hpp', '.c', '.cpp', '.idl']

def main (options, args):
    hasErrors = 0
    for arg in getFiles (args):
        if isPascalFile (arg):
            if options.verbose:
                print arg
            hasErrors = checkPascalConcom (arg) or hasErrors
        elif isCFile (arg):
            if options.verbose:
                print arg
            hasErrors = checkCConcom (arg) or hasErrors
        else:
            if options.veryVerbose:
                print 'Cannot check', arg
    return hasErrors

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('v', 'verbose', '', None, 'verbose output'),
        ('V', 'veryVerbose', '', None, 'very verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
