
# yutil.py      utilities working on a vmake path
#
#    ========== licence begin  GPL
#    Copyright (C) 2001 SAP AG
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#    ========== licence end
#


import string
import re
import os

_sourceRE = re.compile ('[ghv]([a-z][a-z])[0-9]+', re.I)

notInHierarchy = 'ytools.notInHierarchy'
notFound = 'ytools.notFound'

_cachedVPath = None

def getVPath ():
    global _cachedVPath
    if _cachedVPath != None:
        return _cachedVPath
    list = string.splitfields (os.environ ['VMAKE_PATH'], ',')
    list = map (os.path.normpath, list)
    _cachedVPath = list
    return list

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
        return fname
    if defaultPath:
        return defaultPath + '/' + fname
    match = _sourceRE.match (fname)
    if match:
        return '%s/%s/%s' % (os.environ ['SRC'], match.group (1), fname)
    if os.path.exists (fname):
        return '%s/%s' % (os.getcwd (), fname)
    try:
        result = '%s/%s' % (os.environ ['LAY'], fname)
    except KeyError:
        result = fname
    return result

def findFile (fname, defaultPath = None, startlevel = 1):
    """Searches fname in the VMAKE_PATH"""
    fname = ownName (fname, defaultPath)
    startlevel = startlevel - 1     # 1 means $OWN
    levels = getVPath ()
    if startlevel > 0:
        levels = levels [startlevel:]
    for vpath in levels:
        expanded = '%s/%s' % (vpath, fname)
        if os.path.exists (expanded):
            return expanded
    raise notFound, fname

def findFileExt (fname, extensions, defaultPath = None, startlevel = 1):
    """Searches fname in the VMAKE_PATH"""
    fname = ownName (fname, defaultPath)
    startlevel = startlevel - 1     # 1 means $OWN
    levels = getVPath ()
    if startlevel > 0:
        levels = levels [startlevel:]
    for vpath in levels:
        for extension in extensions:
            expanded = '%s/%s%s' % (vpath, fname, extension)
            if os.path.exists (expanded):
                return expanded
    raise notFound, fname

def findRelativeFile (relPath, startlevel = 1, vpath = None):
    startlevel = startlevel - 1     # 1 means $OWN
    if vpath == None:
        vpath = getVPath ()
    if startlevel > 0:
        vpath = vpath [startlevel:]
    for vpath in vpath:
        expanded = os.path.join (vpath, relPath)
        if os.path.exists (expanded):
            return expanded
    return None

def openFile (fname, defaultPath = None, startlevel = 1):
    """Opens the first available fname in the VMAKE_PATH"""
    expanded = findFile (fname, defaultPath, startlevel)
    # this works only on NT; otherwise, access to vmake server required
    result = open (expanded, "r")
    return result

_iviewCmd = '%(TOOLSHELL)s %(TOOL)s/bin/iview%(TOOLEXT)s -e 0 ' % os.environ
_localiviewRE = re.compile ('(?P<local>[a-z]:[^:]+): local source', re.IGNORECASE)
_upperviewRE = re.compile ('(?P<original>[^ ]+) copied to (?P<local>.*)', re.IGNORECASE)
_notFoundRE = re.compile (r'\s+(?P<relative>\S+)\s+NOT found in')

def iview (source, level = 0, getOriginal = 0):
    cmd = _iviewCmd + ' -l' + `level` + ' ' + source
    data = os.popen (cmd, 'r').read ()
    if not data:
        #rint 'no data'
        return None
    hasOriginal = None
    match = _localiviewRE.match (data)
    if not match:
        match = _upperviewRE.match (data)
        hasOriginal = 1
    if match:
        if getOriginal and hasOriginal:
            result = match.group ('original')
        else:
            result = match.group ('local')
    else:
        result = None
    return result

_inewCmd = '%(TOOL)s/bin/inew -e 0 ' % os.environ

def inew (source):
    os.system (_inewCmd + source)

def findAndSplit (fname):
    fname = os.path.normpath (fname)
    for path in getVPath ():
        pathLen = len (path)
        if fname [:pathLen] == path:
            return (path, fname [pathLen + 1:])
    return (None, None)

def deduceRelative (fname):
    level, relative = findAndSplit (fname)
    return relative

def relativePath (source):
    cmd = _iviewCmd + ' ' + source
    data = os.popen (cmd, 'r').read ()
    if not data:
        #rint 'no data'
        return None
    result = None
    match = _localiviewRE.match (data)
    if not match:
        match = _upperviewRE.match (data)
    if match:
        try:
            fullName = match.group ('original')
        except IndexError:
            fullName = match.group ('local')
        result = deduceRelative (fullName)
    else:
        match = _notFoundRE.match (data)
        if match:
            result = os.path.normpath (match.group ('relative'))
    return result

def getDiffPair (source):
    base = iview (source, 0)
    upper = iview (source, 1)
    return base, upper

