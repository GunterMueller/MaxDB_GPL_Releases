#!/bin/env python
# ptoc00.py
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
from string import splitfields
from mylib import readConfig
from ptoc.ptoc00Lib import PtoC00VPath, PtoC00Vmake

def buildIncludes (fname, options):
    if options.include:
        return options.include
    try:
        includes = splitfields (options.includeDict [fname], ',')
    except KeyError:
        includes = []
    return includes

def translateVPath (options, fname):
    includes = buildIncludes (fname, options)
    ptoc00 = PtoC00VPath (fname, includes, options)
    errors = ptoc00.translate ()
    if errors == 0 and not options.no_genSizeof: # CTS 1106486
        ptoc00.genSizeof (fname)
    return errors

def shrubOptions (options):
    if options.config:
        options.includeDict = readConfig (options.config, 'includes')
    else:
        options.includeDict = {}
    optIncludes = options.include
    includes = []
    for include in optIncludes:
        includes = includes + splitfields (include, ',')
    options.include = includes

def translateVmake (options, args):
    ptoc00 = PtoC00Vmake (args, options.include, options)
    errors = ptoc00.translate ()
    if errors == 0 and not options.no_genSizeof:   # CTS 1106486
        fname = ptoc00.logicalName (args)
        ptoc00.genSizeof (fname)
    return errors

def main (options, args):
    """Converts Pascal declarations to their C++ equivalent
    """
    shrubOptions (options)
    if options.imake:
        errors = translateVmake (options, args)
    else:
        errors = 0
        for arg in args:
            errors = errors + translateVPath (options, arg)
    if errors > 0:
        sys.exit (3)

if __name__ == '__main__':
    import optlib
    optlib.trapKeyboardInterrupt = None
    optlib.optMain (main,  # (optstring, varname, typechar, default, help)
            ('I', 'include', '::', [], 'Include files'),
            ('o', 'outfile', ':', None, 'Name of outputfile'),
            ('v', 'verbose'),
            ('S', 'stripComments'),
            ('w', 'warnings'),
            ('C', 'config', ':'),
            ('n', 'no_genSizeof'),  # CTS 1106486
            ('i', 'imake', '', None, 'compile for vmake')
            )
