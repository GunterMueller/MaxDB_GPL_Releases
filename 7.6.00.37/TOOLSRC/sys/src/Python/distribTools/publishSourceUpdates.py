#!/usr/bin/env python
# publishSourceUpdates.py
#
#    ========== licence begin LGPL
#    Copyright (C) 2002 SAP AG
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    ========== licence end
#


import string
import re
from ftplib import FTP
from cStringIO import StringIO
import time

thisYear, thisMonth = time.localtime (time.time ()) [:2]

true = 'true'
false = None

def connectFTP (url, userInfo):
    host, basedir = string.split (url, '/', 1)
    basedir = '/' + basedir
    username, pwd = string.split (userInfo, ':', 1)
    # print 'FTP (', host, username, pwd, ')'
    result = FTP (host, username, pwd)
    if basedir:
        result.cwd (basedir)
    result.set_pasv (true)
    return result

def parseIIS (match):
    month, day, year, hour, minute, size = map (int, match.group (1, 2, 3, 4, 5, 7))
    if match.group (6) == 'P':
        hour = hour + 12
    filename = match.group (8)
    timestamp = '20%02d-%02d-%02d %02d:%02d' % (year, month, day, hour, minute)
    return filename, size, timestamp

_monthMapping = {
    'Jan': 1,
    'Feb': 2,
    'Mar': 3,
    'Apr': 4,
    'May': 5,
    'Jun': 6,
    'Jul': 7,
    'Aug': 8,
    'Sep': 9,
    'Oct': 10,
    'Nov': 11,
    'Dec': 12.
}

def parseCheckPoint (match):
    month = _monthMapping [match.group (2)]
    day = int (match.group (3))
    year = match.group (6)
    if year is not None:
        year = int (year)
        hour = 0
        minute = 0
    else:
        hour = int (match.group (4))
        minute = int (match.group (5))
        # calculate year
        if month > thisMonth:
            year = thisYear - 1
        else:
            year = thisYear
    size = int (match.group (1))
    filename = match.group (7)
    timestamp = '%04d-%02d-%02d %02d:%02d' % (year, month, day, hour, minute)
    return filename, size, timestamp

_rex = [
    ('IIS', re.compile (r'(\d\d)-(\d\d)-(\d\d)\s+(\d\d):(\d\d)([AP])M\s+(\d+) (.*)'), parseIIS),
    ('Check Point', re.compile (
        r'''[a-z-]+\s+    # privileges
            \d+\s+        # link count
            \S+\s+        # owner
            (\d+)\s+      # size
            ([a-zA-Z]+)\s+   # month
            (\d+)\s+      # day
            (?:
                (\d+):(\d+)\s+  # hour:minute
                |
                (\d+)\s+        # year
            )
            (.*)          # filename
        ''', re.VERBOSE), parseCheckPoint),
]

_ignorePatterns = [
    re.compile (r'total\s+\d+'),
]

def filterFluff (lines):
    result = []
    for line in lines:
        ignore = false
        for rex in _ignorePatterns:
            if rex.match (line):
                ignore = true
        if not ignore:
            result.append (line)
    return result


def findRe (sample):
    for server, rex, parse in _rex:
        if rex.match (sample):
            return rex, parse
    raise 'No matching rex', sample

def getDirectoryListing (ftpSession, dirname):
    entries = []
    ftpSession.dir (dirname, entries.append)
    entries = filterFluff (entries)
    result = {}
    if len (entries) == 0:
        return result
    rex, parse = findRe (entries [0])
    for entry in entries:
        match = rex.match (entry)
        assert match
        filename, size, timestamp = parse (match)
        result [filename] = (size, timestamp)
    return result

def sortedItems (dict):
    items = dict.items ()
    items.sort ()
    return items

def requiresCopy (filename, size, timestamp, targetDir, verbose = None):
    if not targetDir.has_key (filename):
        if verbose:
            print '::' + filename + ' does not exist'
        return true
    oldSize, oldTimestamp = targetDir [filename]
    if oldSize != size:
        if verbose:
            print '::' + filename + ' has new size %d (from %d)' % (size, oldSize)
        return true
    if oldTimestamp < timestamp:
        if verbose:
            print '::' + filename + ' is newer %s (from %s)' % (timestamp, oldTimestamp)
        return true
    return false

def getFile (ftpSession, dirname, filename):
    stream = StringIO ()
    ftpSession.retrbinary ('RETR %s/%s' % (dirname, filename), stream.write)
    return stream.getvalue ()

def storeFile (ftpSession, dirname, filename, data):
    stream = StringIO (data)
    ftpSession.storbinary ('STOR %s/%s' % (dirname, filename), stream, 4096)

def updateDir (sourceFTP, targetFTP, dirname, nocopy, verbose):
    sourceDir = getDirectoryListing (sourceFTP, dirname)
    items = sortedItems (sourceDir)
    targetDir = getDirectoryListing (targetFTP, dirname)
    for filename, (size, timestamp) in items:
        if requiresCopy (filename, size, timestamp, targetDir, verbose):
            print filename
            if not nocopy:
                data = getFile (sourceFTP, dirname, filename)
                putFile = storeFile (targetFTP, dirname, filename, data)

_directories = [
    '7.3/updates',
    'develop/tools/updates-linux',
    'develop/tools/updates-win32',
]

def checkOptions (options):
    assert options.targetUser, '-targetUser must be specified'

def main (options, args):
    checkOptions (options)
    sourceSession = connectFTP (options.sourceFTP, options.sourceUser)
    targetSession = connectFTP (options.targetFTP, options.targetUser)
    for dirname in _directories:
        updateDir (sourceSession, targetSession, dirname,
            options.nocopy, options.verbose)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('sourceFTP', None, ':', 'p48047/staging/sapdb', 'source ftp server'),
        ('sourceUser', None, ':', 'FileExchange:ExchangeFile', 'user on source ftp server'),
        ('targetFTP', None, ':', '194.39.131.91/pub/sapdb', 'target ftp server'),
        ('targetUser', None, ':', None, 'user on target ftp server [user:pass@securid]'),
        ('nocopy', None, '', None, 'simulate, but don\'t copy'),
        ('v', 'verbose', '', None, 'verbose output'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
