#!/usr/bin/env python
#
# concom2cpp.py    convert concom to C preprocessor
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


import sys
import string
import re
import os

concomRE = re.compile ('& *([a-zA-Z]+) *(.*)')

InvalidConcomExpression = 'Invalid Concom expression'

def convertLine (match, line):
    keyword, parmstring = match.group (1, 2)
    keyword = string.lower (keyword)
    if keyword == 'ifdef':
        line = '#if defined (%s)\n' % strip (parmstring)
    elif keyword == 'ifndef':
        line = '#if !defined (%s)\n' % strip (parmstring)
    elif keyword == 'else':
        line = '#else\n'
    elif keyword == 'endif':
        line = '#endif\n'
    elif keyword == 'if':
        try:
            line = '#if %s\n' % expression (parmstring)
        except InvalidConcomExpression:
            sys.stderr.write ('concom expression not translatable: \n' % parmstring )
            line = '#failed if %s\n' % parmstring
    else:
        sys.stderr.write ('Invalid concom: ' + line)
        line = '#failed %s %s\n' % (keyword, parmstring)
    return line

inRE = re.compile (' *([$a-z]+) *in *[[](.*)]', re.I)
eqRE = re.compile (' *([$a-z]+) *= *([a-z0-9]+)', re.I)

def expression (parmstring):
    eqMatch = eqRE.match (parmstring)
    if eqMatch:
        return 'defined (%s)' % eqMatch.group (2)
    inMatch = inRE.match (parmstring)
    if inMatch:
        words = string.splitfields (inMatch.group (2), ',')
        words = map (lambda word: 'defined (%s)' % string.strip (word), words)
        return string.joinfields (words, ' || ')
    raise InvalidConcomExpression, parmstring

def concom2cpp (options, fname):
    if not os.path.exists (fname):
        sys.stderr.write ("concom2cpp: file not found: '%s'\n" % fname)
        return
    lines = open (fname, 'rt').readlines ()
    resultlines = []
    for line in lines:
        match = concomRE.match (line)
        if match:
            line = convertLine (match, line)
        resultlines.append (line)
    if options.console:
        stream = sys.stdout
    else:
        stream = open (fname, 'wt')
    stream.writelines (resultlines)

def main (options, args):
    """converts concom to C preprocessor
    """
    for arg in args:
        concom2cpp (options, arg)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('c', 'console', '', 0, 'print result to stdout'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
