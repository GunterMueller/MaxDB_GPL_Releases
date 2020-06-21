#!/usr/bin/env python
#
# create links for posix and parser tools
#

import sys
import string
import os

pathList = string.split (os.environ ['PATH'], os.pathsep)

def findInPath (program, strict):
    for dir in pathList:
        fullpath = os.path.join (dir, program)
        if os.path.exists (fullpath):
            return fullpath
    if strict:
        raise 'not in $PATH', program
    else:
        return None

def createLinks (targetDir, programs, strict):
    if not os.path.isdir (targetDir):
        os.makedirs (targetDir)
    for program in programs:
        target = os.path.join (targetDir, program)
        if not os.path.exists (target):
            fullpath = findInPath (program, strict)
            if fullpath:
                print fullpath, '=>', target
                os.symlink (fullpath, target)

def createPosixLinks (toolRoot):
    target = os.path.join (toolRoot, 'Posix')
    createLinks (target, ["bash", "cat", "cp", "diff", "echo", 
         "egrep", "fgrep", "find", "grep", "gzip", "ln", "ls", 
         "make", "mkdir", "mv", "ps", "rm", "rmdir", "sed", 
	 "sh", "sort", "tar", "touch", "uniq", "wc"], 1)

def createParserLinks (toolRoot):
    target = os.path.join (toolRoot, 'gnu', 'bin')
    createLinks (target, ['flex', 'bison'], None)

def main (args):
    toolRoot = os.path.abspath (args [0])
    global pathList
    pathList = args [1:] + pathList
    createPosixLinks (toolRoot)
    createParserLinks (toolRoot)

if __name__ == "__main__":
    main (sys.argv [1:])