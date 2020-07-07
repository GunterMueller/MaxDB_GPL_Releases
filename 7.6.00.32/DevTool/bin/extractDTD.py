#!/usr/bin/env python
# extractDTD.py

import sys
import string
import re
import os
import glob

_dtdComment = '/*<!'

def getDTDFiles (dir, pattern):
    files = glob.glob1 (dir, pattern)
    result = []
    for file in files:
        fullpath = os.path.join (dir, file)
        source = open (fullpath, 'r').read ()
        if string.find (source, _dtdComment) != -1:
            result.append (file)
    return result

def updateDependencies (fname):
    dirname = os.path.dirname (fname)
    lines = open (fname, 'r').readlines ()
    startRE = re.compile (r'START\s+EXTRACT\s+SECTION\s*:\s+"([^"]+)"')
    endRE = re.compile (r'END\s+EXTRACT\s+SECTION')
    inIncludeSection = None
    outlines = []
    for line in lines:
        if inIncludeSection:
            if endRE.search (line):
                inIncludeSection = None
                outlines.append (line)
        else:
            outlines.append (line)
            match = startRE.search (line)
            if match:
                inIncludeSection = 1
                pattern = match.group (1)
                for file in getDTDFiles (dirname, pattern):
                    outlines.append ('<!--EXTRACT "%s"-->\n' % file)
    open (fname, 'w').writelines (outlines)

def getDTDSnippets (fname):
    source = open (fname, 'r').read ()
    result = []
    pos = 0
    startPos = string.find (source, _dtdComment)
    while startPos != -1:
        startPos = startPos + len (_dtdComment)
        endPos = string.find (source, '*/', startPos)
        chunk = source [startPos:endPos]
        for line in string.split (chunk, '\n'):
            result.append (line + '\n')
        startPos = string.find (source, _dtdComment, endPos)
    return result

def extractDTD (fname):
    lines = open (fname, 'r').readlines ()
    extractRE = re.compile (r'EXTRACT\s*"([^"]+)"')
    outlines = []
    for line in lines:
        outlines.append (line)
        match = extractRE.search (line)
        if match:
            sourceFile = match.group (1)
            dtdSnippet = getDTDSnippets (sourceFile)
            outlines.extend (dtdSnippet)
    open (fname, 'w').writelines (outlines)

def isDTD (fname):
    ext = os.path.splitext (fname) [1]
    return ext == '.dtd'

def allDTD (dirname = '.'):
    files = os.listdir (dirname)
    return filter (isDTD, files)

def main (options, args):
    #if len (args) == 0:
    #    args = allDTD ()
    for arg in args:
        if options.dependencies:
            updateDependencies (arg)
        else:
            extractDTD (arg)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('dep', 'dependencies', '', None, 'update dependencies in file'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
