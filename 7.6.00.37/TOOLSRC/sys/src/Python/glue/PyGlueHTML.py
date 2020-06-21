
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

import html
from Interface import Class
from GlueHTML import GlueHTML
import python2html

class PyGlueHTML (GlueHTML):
    language = 'Python'

    def buildInParms (self, inparms):
        defaultCount = 0
        parmIndex = 0
        result = ''
        for parm in inparms:
            if parm.defaultVal:
                result = result + ' ['
                defaultCount = defaultCount + 1
            if parmIndex > 0:
                result = result + ', '
            result = result + parm.name
            parmIndex = parmIndex + 1
        result = result + "]" * defaultCount
        return result

    def lowlevelWriteMethod (self, synopsis, outparms):
        if outparms:
            names = map (lambda parm: parm.name, outparms)
            resultString = string.join (names, ', ')
            synopsis = resultString + ' = ' + synopsis
        self.htmlstream.write (html.paragraph (html.tagged ('code', synopsis)))

    def writeConstructorSynopsis (self, constructor):
        inparms, outparms = self.getScriptingParms (constructor)
        moduleName = constructor.context.pyname
        #synopsis = 'newObject = %s.%s (%s)' % (moduleName,
        #    constructor.pyname, self.buildInParms (inparms))
        synopsis = 'new%s = %s.%s (%s)' % (constructor.clazz.pyname,
            moduleName, constructor.pyname, self.buildInParms (inparms))
        self.lowlevelWriteMethod (synopsis, outparms)

    def writeMethodSynopsis (self, method):
        inparms, outparms = self.getScriptingParms (method)
        if isinstance (method.context, Class):
            callPrefix = inparms [0].name + '.'
            del inparms [0]
        else:
            callPrefix = method.context.pyname + '.'
        synopsis = '%s%s (%s)' % (callPrefix, method.pyname, self.buildInParms (inparms))
        self.lowlevelWriteMethod (synopsis, outparms)

    def writeExternExample (self, example):
        fname = example.fname
        htmlName = fname + '.html'
        python2html.convertFile (fname, htmlName)
        self.htmlstream.write ("%s (%s)\n" %
                (html.href (htmlName, fname), html.href (fname, 'Raw Source')))
