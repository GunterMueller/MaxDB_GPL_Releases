
# fomc.py
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


import sys
import string
import os

from xmls.FOM.compiler.DTDReader import DTDReader
from xmls.FOM.compiler.SchemaCompiler import Schema2Python, Schema2SubPython

def getGenerator (options):
    if options.generator == 'dtd':
        from xmls.FOM.compiler.DTDGenerator import  DTDGenerator
        return DTDGenerator ()
    elif options.generator == 'FOM':
        return Schema2Python ()
    elif options.generator == 'sub':
        return Schema2SubPython ()
    else:
        raise 'unknown generator', options.generator

def getProperties (options):
    result = []
    for value in options.properties:
        assignPos = string.find (value, '=')
        if assignPos > 1:
            name = value [:assignPos]
            value = value [assignPos + 1:]
        else:
            name = value
            value = 'yes'
        result.append ((name, value))
    return result

def parseDTD (fname):
    reader = DTDReader ()
    reader.parseFile (fname)
    return reader.schema

def genReader (outname, schema):
    compiler =  Schema2Python (schema)
    compiler.compileSchema (outname)

def getInname (inname):
    if (not os.path.exists (inname)) and os.path.exists (inname + '.dtd'):
        inname = inname + '.dtd'
    return inname

def getOutname (options, generator, inname):
    basename = os.path.basename (inname)
    base = os.path.splitext (basename) [0]
    outname = generator.defaultName (base)
    if options.outname:
        if os.path.isdir (options.outname):
            outname = os.path.join (options.outname, outname)
        else:
            outname = options.outname
    return outname


def main (options, args):
    generator = getGenerator (options)
    properties = getProperties (options)
    for arg in args:
        inname = getInname (arg)
        outname = getOutname (options, generator, inname)
        schema = parseDTD (inname)
        for name, value in properties:
            schema.addProperty (name, value)
        outstream = open (outname, 'wt')
        generator.compileSchema (schema, outstream)

def _options ():
    return [
        # (optstring, varname, typechar, default, help)
        ('o', 'outname', ':', None, 'output file or directory'),
        ('G', 'generator', ':', 'FOM', 'generator class'),
        ('P', 'properties', '::', None, 'schema properties'),
        ]

if __name__ == '__main__':
    import optlib
    optlib.optMain2 (main, _options ())

