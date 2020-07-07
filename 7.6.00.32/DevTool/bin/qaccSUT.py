#!/usr/bin/env python
# qaccSUT.py

import string
import os
import re
import time

import mylib
from testTools.QACC.qaccLib import QACC_Session, stdOptions

true = 'true'

def manageOptions (options):
    if options.release is None:
        mylib.fatalError ('must specify -relver', 'qaccMake')
    if options.bit64:
        options.bits = '64'
    else:
        options.bits = '32'

def createConfiguration (options, **sutProperties):
    properties = options.property
    for name in ['release', 'platform', 'bits']:
        properties [name] = getattr (options, name)
    properties.update (sutProperties)
    properties ['kind'] = 'sut'
    properties ['visibility'] = 'develop'
    return properties


def getAllsutContent (stream):
    result = []
    stream.readline () # skip -------------------
    line = stream.readline ()
    while line and (line [0] != '-'):
        result.append (line)
        line = stream.readline ()
    return result

def getSubProtocol (fname):
    stream = open (fname, 'rt')
    startRE = re.compile (r'(?P<when>\d\d-\d\d-\d\d)-\d\d.\d\d.\d\d\s+sutall\s+'
        + r'(?P<speed>\w+)\s+slowci([.]exe)?\s+(?P<dbname>\w+)\s?(?P<option>\w+)?')
    line = stream.readline ()
    while line:
        match = startRE.match (line)
        if match:
            entries = getAllsutContent (stream)
        line = stream.readline ()
    if not match:
        raise 'no valid SUT run found', fname
    when, speed, encoding = match.group ('when', 'speed', 'option')
    when = '20%s%s%s00000000000' % (when [0:2], when [3:5], when [6:8])
    return (when, speed, encoding), entries


def parseProtocol (options, session, fname):
    (when, speed, encoding), entries = getSubProtocol (fname)
    configuration = createConfiguration (options, speed = speed, encoding = encoding)
    configID = session.getConfigurationId (configuration)
    if options.when != None:
        when = options.when
    dir = 'SUT'
    session.invalidateCache (dir, when, configID)
    runID = session.createRun (dir, when, configID)
    for entry in entries:
        splitted = string.split (entry)
        if splitted [0] == 'NOT':   # handle NOT FOUND: <fname>
            continue
        sutName = splitted [3]
        result = splitted [4:]
        if 'core' in result:
            success = 'CORE'
            quality = -99999
        elif '*' in result:
            quality = - int (result [1])
            if 'interupted' in result:
                success = 'INT'
            else:
                success = 'DIFF'
        else:
            success = 'OK'
            quality = 0
        session.insertData (dir, sutName, runID, success, quality)
        session.commit ()

def main (options, args):
    manageOptions (options)
    session = QACC_Session ()
    for arg in args:
        parseProtocol (options, session, arg)
    del session

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ] + stdOptions

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

