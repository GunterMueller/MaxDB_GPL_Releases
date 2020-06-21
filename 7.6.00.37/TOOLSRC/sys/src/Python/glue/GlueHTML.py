
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
import time

import html

import Interface
from GlueDoc import GlueDoc, GlueExample, GlueImportExample, GlueExternExample, GlueDirectory

class GlueHTML:
    language = None

    def __init__ (self, fname):
        self.fname = fname

    def writeModuleGlue (self, module):
        self.openFile (module)
        self.writeTitle (module)
        self.writeOutline (module)
        self.writeContent (module)
        self.writeFooter (module)
        self.htmlstream.close ()

    def openFile (self, module):
        title = self.getTitle (module)
        attributes = self.getAttributes (module)
        self.htmlstream = html.HTMLFile (self.fname, title, attributes)

    def getTitle (self, module):
        result = module.pyname
        if self.language:
            result = '%s (%s)' % (result, self.language)
        return result

    def getAttributes (self, module):
        return [('BGCOLOR', "FFFFFF")]

    def writeTitle (self, module):
        self.htmlstream.titleHeader (self.getTitle (module))

    def writeFooter (self, module):
        now = time.localtime (time.time ())
        timeStamp = time.strftime ("%d. %b %Y", now)
        self.htmlstream.line ()
        self.htmlstream.write (timeStamp)

    def orderedItems (self, item):
        docorder = item.attributes.get ('docorder')
        if docorder == None:
            return item.items
        result = docorder [:]
        for subitem in item.items:
            if not subitem in docorder:
                result.append (subitem)
        return result

    def writeOutline (self, module):
        self.htmlstream.pseudoBreak ()
        self.htmlstream.pushTag ('UL')
        for item in self.orderedItems (module):
            self.writeOutlineItem (item, 1)
        if module.exceptions:
            self.writeExceptionOutline (module.exceptions)
        self.htmlstream.pseudoBreak ()
        self.htmlstream.popTag ()
        self.htmlstream.line ()

    def writeChapterOutline (self, items, ignoreConstructors = None):
        self.htmlstream.pseudoBreak ()
        self.htmlstream.pushTag ('UL')
        for item in items:
            self.writeOutlineItem (item, ignoreConstructors)
        self.htmlstream.pseudoBreak ()
        self.htmlstream.popTag ()

    def writeOutlineItem (self, item, ignoreConstructors = None):
        if not self.supports (item):
            return
        if isinstance (item, Interface.Class):
            self.htmlstream.pseudoBreak ()
            self.htmlstream.tag ('LI')
            self.writeClassOutline (item)
        elif item.isRealConstructor ():
            if not ignoreConstructors:
                self.htmlstream.pseudoBreak ()
                self.htmlstream.tag ('LI')
                self.writeConstructorOutline (item)
        elif isinstance (item, Interface.Method):
            self.htmlstream.pseudoBreak ()
            self.htmlstream.tag ('LI')
            self.writeMethodOutline (item)
        elif isinstance (item, GlueDoc):
            self.genDocOutline (item)
        else:
            self.htmlstream.pseudoBreak ()
            self.htmlstream.tag ('LI')
            name = item.pyname
            self.htmlstream.write (item.pyname + '\n')

    def makeLocalLink (self, name, display = None, comment = None):
        if not display:
            display = name
        result = html.href ('#' + name, display)
        shortComment = shortDesc (comment)
        if shortComment:
            result = result + " - " + shortComment
        return result

    def writeClassOutline (self, clazz):
        self.htmlstream.write (self.makeLocalLink (clazz.pyname, 'class ' + clazz.pyname))
        if clazz.constructor != None:
            allMethods = [clazz.constructor] + clazz.methods ()
        else:
            allMethods = clazz.methods ()
        self.writeChapterOutline (allMethods, None)

    def writeConstructorOutline (self, constructor):
        clazzName = constructor.clazz.pyname
        display = '%s %s' % ('constructor', clazzName)
        link = '%s_%s' % (clazzName, 'constructor')
        self.htmlstream.write (self.makeLocalLink (link, constructor.pyname, constructor.doc))

    def writeMethodOutline (self, method):
        if isinstance (method.context, Interface.Class):
            link = '%s_%s' % (method.pyname, method.context.pyname)
            self.htmlstream.write (self.makeLocalLink (link, method.pyname, method.doc))
        else:
            self.htmlstream.write (self.makeLocalLink (method.pyname, comment = method.doc))

    def genDocOutline (self, item):
        language = item.language
        if not language or (language == self.language):
            self.htmlstream.pseudoBreak ()
            self.htmlstream.tag ('li')
            self.htmlstream.write (self.makeLocalLink (item.pyname, None, item.doc))
            if isinstance (item, GlueDirectory):
                self.htmlstream.pushTag ('ul')
                for subitem in item.items:
                    self.genDocOutline (subitem)
                self.htmlstream.popTag ()

    def writeExceptionOutline (self, exceptions):
        self.htmlstream.tag ('li')
        self.htmlstream.write (self.makeLocalLink ('Exceptions'))
        self.htmlstream.pseudoBreak ()
        self.htmlstream.pushTag ('ul')
        for exception in exceptions:
            self.htmlstream.pseudoBreak ()
            self.htmlstream.tag ('li')
            self.htmlstream.write (self.makeLocalLink (exception.name))
        self.htmlstream.popTag ()

    def writeContent (self, module):
        self.writeChapterContent (self.orderedItems (module), 1)
        if module.exceptions:
            self.writeExceptionContent (module.exceptions)

    def writeChapterContent (self, items, ignoreConstructors = None):
        for item in items:
            self.writeContentItem (item, ignoreConstructors)

    def writeContentItem (self, item, ignoreConstructors = None):
        if not self.supports (item):
            return
        if isinstance (item, Interface.Class):
            self.writeClassContent (item)
        elif item.isRealConstructor ():
            if not ignoreConstructors:
                self.writeConstructorContent (item)
        elif isinstance (item, Interface.Method):
            self.writeMethodContent (item)
        elif isinstance (item, GlueDoc):
            self.genDocContent (item)
        else:
            name = item.pyname
            self.htmlstream.write (item.pyname + '\n')

    def writeClassContent (self, clazz):
        display = 'class ' + clazz.pyname
        anchor = html.anchor (clazz.pyname, display)
        self.htmlstream.write (html.header (anchor, 2) + '\n')
        self.htmlstream.pushTag ('blockquote')
        if clazz.doc:
            short, long = breakDesc (clazz.doc, None)
            self.htmlstream.write (html.paragraph (short))
            self.htmlstream.write (html.paragraph (long))
        if clazz.constructor != None:
            allMethods = [clazz.constructor] + clazz.methods ()
        else:
            allMethods = clazz.methods ()
        self.writeChapterContent (allMethods, None)
        self.htmlstream.popTag ()
        self.htmlstream.pseudoBreak ()
        self.htmlstream.line ()

    def writeConstructorContent (self, constructor):
        clazzName = constructor.clazz.pyname
        display = html.tagged ('b', '%s %s' % ('constructor', clazzName))
        link = '%s_%s' % (clazzName, 'constructor')
        self.htmlstream.write (html.anchor (link,display))
        self.htmlstream.pushTag ('blockquote')
        self.writeConstructorSynopsis (constructor)
        if constructor.doc:
            short, long = breakDesc (constructor.doc, None)
            self.htmlstream.write (html.paragraph (short))
            self.htmlstream.write (html.paragraph (long))
        self.htmlstream.popTag ()
        self.htmlstream.pseudoBreak ()

    def writeMethodContent (self, method):
        if isinstance (method.context, Interface.Class):
            link = '%s_%s' % (method.pyname, method.context.pyname)
        else:
            link = method.pyname
        self.htmlstream.write (html.anchor (link, html.tagged ('b', method.pyname)))
        self.htmlstream.pushTag ('blockquote')
        self.writeMethodSynopsis (method)
        if method.doc:
            short, long = breakDesc (method.doc, None)
            self.htmlstream.write (html.paragraph (short))
            self.htmlstream.write (html.paragraph (long))
        self.htmlstream.popTag ()
        self.htmlstream.pseudoBreak ()

    def genDocContent (self, item):
        language = item.language
        if not language or (language == self.language):
            self.htmlstream.write (html.header (html.anchor (item.pyname), 3))
            self.htmlstream.write (html.paragraph (item.doc))
            if isinstance (item, GlueDirectory):
                self.htmlstream.pushTag ('blockquote')
                for subitem in item.items:
                    self.genDocContent (subitem)
                self.htmlstream.popTag ()
                self.htmlstream.pseudoBreak ()
            elif isinstance (item, GlueExternExample):
                self.writeExternExample (item)
            elif isinstance (item, GlueExample):
                self.htmlstream.pushTag ('pre')
                code = item.getCode ()
                self.htmlstream.write (code)
                self.htmlstream.pseudoBreak ()
                self.htmlstream.popTag ()

    def writeExceptionContent (self, exceptions):
        self.htmlstream.line ()
        self.htmlstream.write (html.header (html.anchor ('Exceptions', 'Exceptions'), 2) + '\n')
        self.htmlstream.pushTags ('blockquote', 'dl')
        for exception in exceptions:
            self.htmlstream.tag ('dt')
            self.htmlstream.write (html.tagged ('b', (exception.name)))
            self.htmlstream.tag ('dd')
            self.htmlstream.write (exception.doc)
        self.htmlstream.popTag (2)
        self.htmlstream.pseudoBreak ()

    def getScriptingParms (self, method):
        inparms = []
        outparms = []
        for parm in method.parms:
            if parm.isScriptingOutput ():
                outparms.append (parm)
            elif parm.isScriptingInput ():
                inparms.append (parm)
        return inparms, outparms

    def supports (self, item):
        if not item.isDocumented ():
            return None
        return item.supportFor ('language', self.language)

def shortDesc (text):
    short, long = breakDesc (text, 1)
    return short

def breakDesc (text, ignoreLongDesc):
    if not text:
        return None, None
    lines = string.split (text, '\n')
    short = []
    long = []
    current = short
    for line in lines:
        line = string.strip (line)
        if line == '':
            if ignoreLongDesc:
                return (string.join (short, '\n'), None)
            else:
                current = long
        current.append (line)
    return (string.join (short, '\n'), string.join (long, '\n'))
