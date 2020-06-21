
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
from string import upper, join

from vdnStream import VDNStream
from pasScanner import *
from ptoc00lib import ConcomCopy
from parserUtil import readUntil, readNested, tokenUntil, tokenUntilAny
from pasParser import parseRoutDcl, nestedStatements

class PtoC:
    def __init__ (self, fname, includes):
        self.fname = fname
        self.includes = includes

    def translate (self):
        self.streamSetup (self.fname)
        # read includes (implement later)
        self.writeIncludes (self.outstream, self.includes)
        self.readExports ()
        # self.readImports ()
        self.collectCode ()
        self.generateImplementation ()

    def streamSetup (self, fname):
        self.outstream = self.openOutstream (fname)
        self.concom = ConcomCopy (self.outstream)
        self.instream = VDNStream (fname, self.concom.handleLine)

    def openOutstream (self, fname):
        result = open (fname + '.c', "w")
        return result

    def openPart (self, partName):
        self.instream.openPart (partName)
        self.scanner = Scanner (self.instream)

    def writeIncludes (self, stream, includes):
        stream.write ("""
/***************************************
 *           I N C L U D E S           *
 ***************************************/
""")
        for include in includes:
            moduleDefine = upper (include)
            stream.write ('#ifndef %s\n#include "%s.h"\n#endif\n'
                    % (moduleDefine, include))

    def readExports (self):
        self.outstream.write ("""
/***************************************
 *           E X P O R T D             *
 ***************************************/
""")
        self.openPart ('define')
        self.exports = {}
        modifier = 'global'
        scanner = self.scanner
        try:
            while 1:
                token = scanner.next ()
                keyword = token.value
                if keyword == 'var':
                    vars = self.readVarDecls ()
                    self.declareVars (vars, modifier)
                elif keyword in ['procedure', 'function']:
                    dcl = parseRoutDcl (scanner, token)
                    dcl.modifier = modifier
                    self.exports [dcl.name] = dcl
                    self.writeRoutDcl (dcl)
                else:
                    raise 'SyntaxError', 'VAR/PROCEDURE/FUNCTION expected instead of ' + token.value
                
        except EOFError:
            pass

    def collectCode (self):
        self.outstream.write ("""
/***************************************
 *             L O C A L S             *
 ***************************************/
""")
        self.openPart ('code')
        self.routines = []
        scanner = self.scanner
        modifier = 'static'
        try:
            while 1:
                token = scanner.next ()
                keyword = token.value
                if keyword == 'var':
                    vars = self.readVarDecls ()
                    self.declareVars (vars, modifier)
                elif keyword in ['procedure', 'function']:
                    dcl = parseRoutDcl (scanner, token)
                    try:
                        exportDcl = self.exports [dcl.name]
                        # possibly compare declarations
                        dcl = exportDcl
                    except KeyError:
                        dcl.modifier = modifier
                        self.writeRoutDcl (dcl)
                    self.routines.append (dcl)
                    dcl.codeTokens = self.skipBody ()
                else:
                    raise 'SyntaxError', (
                            'VAR/PROCEDURE/FUNCTION expected instead of '
                            + token.value, token.getAttribute ('pos'))
        except EOFError:
            pass

    def writeRoutDcl (self, dcl, forDefinition = None):
        if forDefinition:
            closeString = ''
        else:
            closeString = ';\n'
        stream = self.outstream
        if dcl.returnType:
            returnType = dcl.returnType
        else:
            returnType = 'void'
        stream.write ('%s %s %s (' % (dcl.modifier, returnType, dcl.name))
        comma = ''
        for var, parmname, type in dcl.parms:
            if var:
                var = '*'
            stream.write ('%s\n            %-19s%1s%s' % (comma, type, var, parmname))
            comma = ','
        stream.write (')%s\n' % closeString)

    def skipBody (self):
        self.concom.doCopy = None
        vars = readUntil (self.scanner, keywordID, 'begin')
        begin = vars [-1]
        del vars [-1]
        codeBody = readNested (self.scanner, begin, nestedStatements)
        codeBody.append (self.scanner.next ())
        self.concom.hideText = 1
        return vars + codeBody

    def generateImplementation (self):
        self.outstream.write ("""
/***************************************
 *     I M P L E M E N T A T I O N     *
 ***************************************/
""")
        for routine in self.routines:
            self.generateRoutine (routine)

    def generateRoutine (self, routine):
        outstream = self.outstream
        self.writeRoutDcl (routine, 1)
        outstream.write ('{\n    ROUTINE_TRACE_DBG_MSP00 ("%s");\n'
                % routine.name)
        tokens = routine.codeTokens
        declTokens = tokenUntil (tokens, 0, keywordID, 'begin')
        pos = len (declTokens)
        endRoutine = len (tokens) - pos
        stack = ['begin']
        indent = '    ' * len (stack)
        stmtEnd = [ (operatorID, ';'), (keywordID, 'end'),
                (keywordID, 'until'), (keywordID, 'else')]
        while pos < endRoutine:
            indent = '    ' * len (stack)
            current = tokens [pos]
            if current.id != keywordID:
                stmtTokens = tokenUntilAny (tokens, pos, stmtEnd)
                if stmtTokens [-1].value != ';':
                    del stmtTokens [-1]
                pos = pos + len (stmtTokens)
                self.writeSimpleStmt (stmtTokens, indent)
                while stack [-1] != 'begin':
                    del stack [-1]
            else:
                keyword = current.value
                print keyword, len (stack)
                doIndent = 1
                if keyword == 'if':
                    expr = tokenUntil (tokens, pos + 1, keywordID, 'then')
                    outstream.write ('%sif (<expr>)\n' % indent)
                    pos = pos + len (expr)
                elif keyword == 'else':
                    outstream.write ('%selse\n' % indent)
                elif keyword == 'while':
                    expr = tokenUntil (tokens, pos + 1, keywordID, 'do')
                    outstream.write ('%swhile (<expr>)\n' % indent)
                    pos = pos + len (expr)
                elif keyword == 'repeat':
                    outstream.write ('%sdo\n%s{\n' % (indent, indent))
                elif keyword == 'until':
                    expr = tokenUntil (tokens, pos + 1, operatorID, ';')
                    outstream.write ('%s} while (<cond>);\n')
                    doIndent = None
                    pos = pos + len (expr)
                elif keyword == 'for':
                    forBounds = tokenUntil (tokens, pos + 1, keywordID, 'do')
                    outstream.write ('%sfor ()\n' % indent)
                    pos = pos + len (forBounds)
                elif keyword == 'begin':
                    outstream.write ('%s{\n' % indent [:-4])
                    stack.append ('begin')
                    doIndent = None
                elif keyword == 'end':
                    print 'found end'
                    outstream.write ('%s}\n' % indent [:-4])
                    doIndent = None
                elif keyword == 'with':
                    vars = tokenUntil (tokens, pos + 1, keywordID, 'do')
                    outstream.write ('%s/* with statement */\n' % indent)
                    pos = pos + len (vars)
                else:
                    raise 'SyntaxError', 'invalid keyword ' + keyword
                if doIndent:
                    stack.append (keyword)
                else:
                    del stack [-1]
                pos = pos + 1
        outstream.write ('\n\n/*----------------------------------------*/\n\n')

    def writeSimpleStmt (self, tokens, indent):
        if tokens [-1].value != ';':
            tokens.append (Operator (';'))
        strlist = map (lambda token: token.value, tokens)
        tokenString = join (strlist)
        self.outstream.write ('%s%s\n' % (indent, tokenString))
