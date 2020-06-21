#!/usr/bin/env python
# imkdep.py

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
import os
path = os.path

from vmake.imkdepLib import DepUpdater, noInames, \
    WrongConcom, NotAnImkdepDescription, getDirectoryMapping, _version

def strictnorm (pathname):
    return path.normcase (path.normpath (pathname))


src = strictnorm (os.environ ['SRC'])
desc = strictnorm (os.environ ['DESC'])

def startsWith (str, other):
    compareValue = path.normcase (str [:len (other)])
    result = compareValue == path.normcase (other)
    return result

def descnameFromFilename (fnameArg, unifiedSys):
    #
    #
    #
    if fnameArg [-4:] == '.com':
        return fnameArg
    #
    # get from DirectoryMapping
    #
    mapping = getDirectoryMapping ()
    if mapping.has_key (fnameArg):
        directory = string.strip (mapping [fnameArg])
        descName = path.join (os.environ ['DESC'], 'SAPDB', directory + '.com')
        if path.exists (descName):
            return descName
        srcName = path.join (os.environ ['SRC'], 'SAPDB', directory, path.basename (directory) + '.com')
        if path.exists (srcName):
            return srcName
    #
    #
    #
    fname = path.normpath (path.abspath (fnameArg))
    if startsWith (fname, desc):
        return fname
    if not startsWith (fname, src):
        raise 'Argument not in %SRC% or %DESC%', fnameArg
    if path.isfile (fname):
        fname = path.dirname (fname)
    relative = fname [len (src):]
    layer = os.path.basename (relative)
    inSrc = src + relative + '/' + layer + '.com'
    if unifiedSys or os.path.exists (inSrc):
        result = inSrc
    else:
        result = desc + relative + '.com'
    return result

def descnamesFromFilenames (args, unifiedSys):
    set = {}
    for arg in args:
        description = descnameFromFilename (arg, unifiedSys)
        set [description] = 1
    result = set.keys ()
    result.sort ()
    return result

def signalNotAnImkdepDescription (err):
    print """ERR: %s is not managed with imkdep yet

    Use imkdep -force to convert this file for use by imkdep.
    (But consult with your colleagues first)
""" % err.descName
    sys.exit (2)


def checkVersion ():
    import sys
    if sys.version < '1.5.2':
        sys.stdout.write ("""
imkdep needs at least Python 1.5.2.
You're running Python %s.
""" % sys.version)
        sys.exit (1)


def main (options, args):
    """updates the include dependencies of a vmake .com description

    Arguments can be
    - a .com description with a directory prefix
    - the absolute or relative path to a .com description
    - the absolute or relative path to a source file
    - the absolute or relative path to a source directory
    - the prefix as defined in DirectoryMapping

    imkdep works by building a file list of the source
    directory and scanning the sources for #include "..." statements.

    On first use, existing inc= directives are removed from the
    description.

    ========================================================
    #ifdef directives can be transferred to the .com-Description
    by adding the concom directive as a comment:
        #if defined(_WIN32) /*&if $OSSPEC = WIN32*/
        #include "gos00.h"  /* nocheck but_imkdep */
        #else               /*&else*/
        #include "gen00.h"  /* nocheck but_imkdep */
        #endif              /*&endif*/

    imkdep will ignore #includes followed by a comment containing nocheck,
    unless the comment contains also the word but_imkdep.

    This is necessary because vmake will not parse conditional compiling
    when trying to match #includes with inc=.

    ========================================================
    Dependencies for variants can also be specified:

    Include only in variants -a and -b
    #include "..."  /* &variant +a,b */

    Include everywhere except in variants -c and -d
    #include "..."  /* &variant -c,d */

    &variant implies nocheck!
    ========================================================
    options:
    """
    checkVersion ()
    if options.noinames:
        noInames ()
    updater = DepUpdater (options)
    args = descnamesFromFilenames (args, options.unifiedSys)
    hasError = 0
    for arg in args:
        try:
            updater.updateDescription (arg)
        except WrongConcom, err:
            hasError = 1
        except NotAnImkdepDescription, err:
            signalNotAnImkdepDescription (err)
    return hasError

def _options ():
    if desc == src:
        unifiedSys = 1
    else:
        unifiedSys = None
    return [
        # (optstring, varname, typechar, default, help)
        ('except', None, '', None, 'add -except to C++ modules'),
        ('noinames', None, '', None, 'don\'t use iview to find full names'),
        ('unifiedSys', None, '', unifiedSys, 'obsolete, this is automatically detected'),
        ('force', None, '', None, 'change .com even if not imkdep\'ed so far'),
        ('forAPO', None, '', None, 'special handling of variants for dsp2vmake'),
        ('v', 'verbose', '', None, 'verbose output'),
        ('version', None, '', _version, 'version information'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
