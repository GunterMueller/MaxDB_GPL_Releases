#!/usr/bin/env python
# qaccMake.py

import sys
import string
import os
import re
import time
import urlparse
import ftplib

import mylib
from testTools.QACC.qaccLib import QACC_Session, stdOptions

def trace (msg):
    now = time.strftime ("%Y-%m-%d %H:%M:%S", time.localtime (time.time ()))
    sys.stdout.write (now + ' ' + msg + '\n')
    sys.stdout.flush ()

true = 'true'

def manageOptions (options):
    if options.slow:
        options.speed = 's'
    elif options.quick:
        options.speed = 'q'
    elif options.fast:
        options.speed = 'f'
    else:
        mylib.fatalError ('must specify -s, -q or -f', 'qaccMake')
    if options.release is None:
        mylib.fatalError ('must specify -relver', 'qaccMake')
    if options.OWN is None:
        mylib.fatalError ('must specify -OWN', 'qaccMake')
    if options.bit64:
        options.bits = '64'
    else:
        options.bits = '32'

def createConfiguration (options):
    properties = options.property
    for name in ['release', 'platform', 'bits']:
        properties [name] = getattr (options, name)
    properties ['kind'] = 'make'
    properties ['visibility'] = 'develop'
    return properties

def getLocalFile (fname):
    return open (fname, 'rt').readlines ()

def getFTPFile (own, tail):
    scheme, netloc, path, parameters, query, fragment = \
        urlparse.urlparse (own)
    fullPath = path + '/' + tail
    session = ftplib.FTP (netloc, 'remuser', 'Remuser')
    lines = []
    session.retrlines ('RETR ' + fullPath, lines.append)
    session.quit ()
    return lines


def getFile (options):
    for speed in ['fast', 'quick', 'slow']:
        if getattr (options, speed):
            break
    tail = 'sys/wrk/%s/prot/%s.mac.x0' % (speed, options.target)
    if options.OWN [:4] == 'ftp:':
        lines = getFTPFile (options.OWN, tail)
    else:
        fullName = os.path.join (options.OWN, tail)
        lines = getLocalFile (fullName)
    return lines


def parseStartDate (lines):
    startRE = re.compile (r'(\d\d)-(\d\d)-(\d\d) (\d\d):(\d\d):(\d\d) start\s+of\s+collection\s+phase')
    for line in lines:
        match = startRE.match (line)
        if match:
            when = '20' + string.join (match.group (3, 2, 1, 4, 5, 6), '') + ('0' * 6)
            return when
    return None


validTargets = ['mac', 'shm', 'lnk', 'dld']

def parseProtocoll (options, session):
    dir = 'im' + options.speed
    configuration = createConfiguration (options)
    configID = session.getConfigurationId (configuration)
    lines = getFile (options)
    #trace ('parseStartDate')
    when = parseStartDate (lines)
    if when is None:
        when = options.when
    #trace ('invalidateCache')
    session.invalidateCache (dir, when, configID)
    #trace ('createRun')
    runID = session.createRun (dir, when, configID)
    targetRE = '([a-zA-Z0-9_-]+)[.]([a-z]+)'
    finishedRE = re.compile (r'\d\d-\d\d-\d\d \d\d:\d\d:\d\d \(' + targetRE + ' finished\)')
    errorRE = re.compile ("Error -?[0-9]+ making '" + targetRE + "'")
    res = [(finishedRE, true), (errorRE, None)]
    #trace ('begin parse')
    for line in lines:
        for rex, ok in res:
            match = rex.match (line)
            if match:
                target, ext = match.group (1, 2)
                if ext in validTargets:
                    if ok:
                        success = 'OK'
                    else:
                        success = 'ERR'
                    session.insertData (dir, target + '.' + ext, runID, success)
                break

def main (options, args):
    manageOptions (options)
    session = QACC_Session ()
    try:
        parseProtocoll (options, session)
    except ftplib.error_perm, exc:
        mylib.fatalError (`exc`, 'qaccMake')
    session.commit ()
    del session

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('f', 'fast', '', None, 'insert data of fast make'),
        ('q', 'quick', '', None, 'insert data of quick make'),
        ('s', 'slow', '', None, 'insert data of slow make'),
        ('target', None, ':', 'all', 'target name of protocol file'),
        ] + stdOptions

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

