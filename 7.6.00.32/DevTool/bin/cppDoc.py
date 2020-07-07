#!/usr/bin/env python
# cppDoc.py
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
import os
import fileutil
from docTools import _version, cppDocLib
from docTools.cppDocLib import CppDoc, parseAttributes
from docTools.NoWeaver import NoWeaver
from docTools.FileSystem import File

def main (options, args):
    """Extracts comments from C++ sources and generates HTML.
    """
    bodyAttributes = parseAttributes (options.body, [('bgcolor', '#FFFFFF')])
    weaver = NoWeaver ()
    for arg in args:
        cppDoc = CppDoc (bodyAttributes)
        cppDoc.tabSize = options.tabsize
        cppDoc.doHighlight = options.highlight
        cppDocLib.strict = options.strict
        cppDoc.quoteHTML = options.quoteHTML
        cppDoc.weaver = weaver
        ok = cppDoc.readSource (File (arg), options.cleanForm)
        if ok:
            outfile = File (options.outdir).join (fileutil.basename (arg) + '.html')
            outname = os.path.join (options.outdir, fileutil.basename (arg) + '.html')
            cppDoc.genHTML (outfile)
    # weaver.save ()
    # weaver.genIndex ()

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('body', None, '::', None, '<BODY> attributes, e.g. -body bgcolor=#FFFFF0'),
        ('o', 'outdir', ':', '.', 'output directory'),
        ('clean', 'cleanForm', '', None, 'append clean declarations to source'),
        ('tabsize', None, ':', 8, 'size of tabulator'),
        ('highlight', None, '', None, 'class names appear in bold'),
        ('version', None, '', _version, 'version information'),
        ('strict', None, '', None, 'checks prototypes'),
        ('quoteHTML', None, '', None, 'quote HTML in description text'),
        ('package', None, ':', 'cppDoc', 'package name'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
