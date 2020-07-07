
# yutil.py      utilities working on a vmake path

import string
import regex
import os

_sourceRE = regex.compile ('[ghv]\([a-z][a-z]\)[0-9]+', regex.casefold)

notInHierarchy = 'ytools.notInHierarchy'
notFound = 'ytools.noFound'

def stripOwn (fname):
    """Remove $OWN from the filename, raise an exception if this is not possible"""
    own = os.environ ['OWN']
    if fname [:len (own)] != own:
        raise notInHierarchy, fname
    return fname [len (own) + 1:]

def ownName (fname, defaultPath = None):
    """Return fname relative to $OWN"""
    return stripOwn (fullName (fname, defaultPath))

def fullName (fname, defaultPath = None):
    """Return the full path of fname given some defaults"""
    if os.path.isabs (fname):
        result = fname
    elif defaultPath:
        result = defaultPath + '/' + fname
    elif _sourceRE.match (fname) != -1:
        result = '%s/%s/%s' % (os.environ ['SRC'], _sourceRE.group (1), fname)
    elif os.path.exists (fname):
        result = '%/%s' % (os.getcwd (), fname)
    else:
        try:
            result = '%s/%s' % (os.environ ['LAY'], fname)
        except KeyError:
            result = fname
    return result

def findFile (fname, defaultPath = None, startlevel = 1):
    """Searches fname in the VMAKE_PATH"""
    fname = ownName (fname, defaultPath)
    startlevel = startlevel - 1     # 1 means $OWN
    levels = string.splitfields (os.environ ['VMAKE_PATH'], ',')
    if startlevel > 0:
        levels = levels [startlevel:]
    for vpath in levels:
        expanded = '%s/%s' % (vpath, fname)
        if os.path.exists (expanded):
            return expanded
    raise notFound, fname

def openFile (fname, defaultPath = None, startlevel = 1):
    """Opens the first available fname in the VMAKE_PATH"""
    expanded = findFile (fname, defaultPath, startlevel)
    # this works only on NT; otherwise, access to vmake server required
    result = open (expanded, "r")
    return result

