
# glue.py
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


import os

import mylib

def genOneLanguage (generator, module, typeMappings, aliases):
    generator.addTargetSpecificTypes (typeMappings)
    for newType, baseType in aliases:
        generator.addTypeAlias (newType, baseType)
    module.writeGlue (generator)

def genOneDoc (generator, module):
    module.writeGlue (generator)

def main (options, args):
    """Generates interfaces to scripting languages from module descriptions.

    Supported scripting environments are Python and Perl.
    """
    for arg in args:
        module, typeMappings, aliases = mylib.readConfig (arg, 'module',
                ('typeMappings', {}),
                ('aliases', []),
                )
        join = os.path.join
        if options.python:
            from PyGlue import PyGlue
            outfile = join (options.outdir, module.name + 'python.cpp')
            genOneLanguage (PyGlue (outfile),
                    module, typeMappings, aliases)
        if options.pythonDoc:
            from PyGlueHTML import PyGlueHTML
            outfile = join (options.outdir, module.name + 'Python.html')
            genOneDoc (PyGlueHTML (outfile), module)
        if options.perl:
            from PerlGlue import PerlGlue
            outfile = join (options.outdir, module.name + 'perl.cpp')
            genOneLanguage (PerlGlue (outfile),
                    module, typeMappings, aliases)
        if options.perlDoc:
            from PerlGlueHTML import PerlGlueHTML
            outfile = join (options.outdir, module.name + 'Perl.html')
            genOneDoc (PerlGlueHTML (outfile), module)
            

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('python', 'python', '', None, 'generate code for Python'),
        ('pythonDoc', 'pythonDoc', '', None, 'generate documentation for Python'),
        ('perl', 'perl', '', None, 'generate code for Perl'),
        ('perlDoc', 'perlDoc', '', None, 'generate documentation for Perl'),
        ('v', 'verbose', '', None, 'verbose execution'),
        ('o', 'outdir', ':', '.', 'output directory'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())
