# changed file
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
import os
import Ctype
from AbstractGlue import MappedType, AbstractGlue, AbstractModuleGlue

class PyType (MappedType):

    def __init__ (self, decl, typechar):
        MappedType.__init__ (self, decl)
        self.typechar = typechar

    def varDcls (self, name, typeString, defaultValue):
        result = "%s %s" % (typeString, name)
        if defaultValue:
            if string.strip (defaultValue) [0] == '"':
                defaultValue = 'LIT(' + defaultValue + ')'
            result = result + " = " + defaultValue
        return [result]

    def asParseTupleParameter (self, name):
        return '&' + name

    def asCallParameter (self, name):
        return name

    def asMakeTupleParameter (self, name):
        return name

    def initcall (self, name):
        return None

    def py2c (self, name):
        return None

    def c2py (self, name):
        return None

    def destructor (self, name):
        return None

class PyComplexType (PyType):
    def __init__ (self, decl, toC, toPy, destructor = None, typeChar = 'O'):
        PyType.__init__ (self, decl, typeChar)
        self.toC = toC
        self.toPy = toPy
        self.destructorFunction = destructor

    def varDcls (self, name, typeString, defaultValue):
        result = PyType.varDcls (self, name, typeString, defaultValue)
        baseobj = 'PyObject * %sObj'
        if defaultValue:
            baseobj = baseobj + ' = NULL'
        result.append ( baseobj % name)
        return result

    def asParseTupleParameter (self, name):
        return '&' + name + 'Obj'

    def initcall (self, name):
        return "memset (&%s, '\\0', sizeof (%s));" % (name, self.decl)

    def py2c (self, name):
        return '%s (self, %s, %s)' % (self.toC, name + 'Obj',
                '&' + self.asCallParameter (name))

    def c2py (self, name):
        return '%s (self, %s, %s)' % (self.toPy, self.asCallParameter (name),
                self.asParseTupleParameter (name))

    def destructor (self, name):
        if self.destructorFunction:
            return '%s (self, %s);' % (self.destructorFunction, self.asCallParameter (name))
        else:
            return None

    def asMakeTupleParameter (self, name):
        return name + 'Obj'

class PyModuleGlue (AbstractModuleGlue):
    language = 'Python'

    def __init__ (self, fname, typeDict = Ctype.TypeDict):
        AbstractModuleGlue.__init__ (self)
        self.typeDict = typeDict
        self.fname = fname

    def addStandardTypes (self):
        for decl, typechar in [('char *', 's'), ('int', 'i'), ('long', 'l')]:
            self.addType (PyType (decl, typechar))

    def defaultType (self, decl):
        return PyType (decl, 'O')

    def close (self):
        if self.cStream:
            self.cStream.close ()
            self.cStream = None

    __del__ = close

    def writeModuleGlue (self, module):
        self.openCStream (module)
        self.methodWriter = PyMethodGlue (self.cStream, self)
        self.constructorWriter = PyConstructorGlue (self.cStream, self)
        self.declareExceptions (module)
        self.writeSnippet (module.snippets, 'precode', 'Python_')
        for item in module.items:
            item.writeGlue (self)
        mtableName = module.pyname + 'ModuleMethods'
        self.writeMethodTable (mtableName, module.items)
        self.writeInitfunc (module, mtableName)
        self.close ()

    def openCStream (self, module):
        self.cStream = open (self.fname, "w")
        self.writeModuleHeader (self.cStream, module, 'Python')
        self.cStream.write ('''
#include <stddef.h>

#if defined (HPUX) || defined (AIX) || defined (SUN) || defined (SOLARIS) || defined (OSF1) || defined (LINUX) || defined (FREEBSD)
#define BAD_STATIC_FORWARD
#endif

#include "Python.h"/* no check */

#if PY_MAJOR_VERSION >= 2 && PY_MINOR_VERSION >= 4
#else
#if defined (HPUX) || defined (AIX) || defined (SUN) || defined (SOLARIS) || defined (OSF1)
#undef statichere
#define statichere
#endif
#endif

#if defined HP_IA64
#undef statichere
#define statichere static
#endif

#define MF__
#if defined (__cplusplus)
#define externC extern "C"
#define internC extern "C" static
#else
#define externC
#define internC static
#endif
#define PYTHON_GLUE 1

#define Glue_BEGIN_ALLOW_THREADS Py_BEGIN_ALLOW_THREADS
#define Glue_END_ALLOW_THREADS Py_END_ALLOW_THREADS

#if defined (__cplusplus)
#define LIT(c) CONST_CAST(char *, c)
#else
#define LIT(c) (c)
#endif

#if defined (_WIN32)
#define GLUEEXPORT __declspec(dllexport)
#else
#define GLUEEXPORT
#endif

#if PYTHON_API_VERSION < 1009
#define PyObject_Del(ptr) free (ptr)
#endif

#define SL_None Py_None
#define SL_isNone(val) (val == SL_None)
#define SL_isTrue(val) PyObject_IsTrue (val)

static PyObject *
createExceptionKind (
    const char * exceptionName,
    const char * exceptionCode)
{
    PyObject * result;
    PyObject * runResult = NULL;
    PyObject * globals = NULL;
    PyObject * locals = NULL;

    if (exceptionCode != NULL) {
        globals = PyDict_New ();
        locals = PyDict_New ();
        runResult = PyRun_String ((char *) exceptionCode, Py_file_input,
            globals, locals);
        if (PyErr_Occurred ()) {
            PyErr_Print ();
        }
    }
    result = PyErr_NewException ((char *) exceptionName, NULL, locals);
    Py_XDECREF (locals);
    Py_XDECREF (globals);
    Py_XDECREF (runResult);
    return result;
}

''')

    def docString2Cliteral (self, docString, indent = '', firstIndent = ''):
        if not docString:
            return 'NULL'
        str = string.replace (repr (docString) [1:-1], '"', '\\"')
        str = string.replace (str, '\\012', '\\n')
        return firstIndent + '"' + str + '"'

    def writeMethodTable (self, mtableName, items):
        stream = self.cStream
        stream.write ('static PyMethodDef %s [] = {\n' % mtableName)
        for item in items:
            if item.isMethod and self.supports (item): # and not item.attributes.get ('specialMethod'):
                mname = item.pyname
                docEntry = self.docString2Cliteral (item.doc,
                    firstIndent = '\n        ')
                stream.write ('    {LIT("%s"), (PyCFunction)%s_%s, METH_VARARGS|METH_KEYWORDS, %s},\n'
                    % (mname, mname, item.context.pyname, docEntry))
        stream.write ('    {NULL, NULL}\n')
        stream.write ('};\n\n')

    def declareExceptions (self, module):
        stream = self.cStream
        for exception in module.exceptions:
            stream.write ('static PyObject *%sType;\n'
                % exception.name)
            stream.write ('static const char * %sCodeC = ' % exception.name)
            if exception.pycode:
                lines = string.split (exception.pycode, '\n')
                for line in lines:
                    line = string.replace (line, '"', '\\"')
                    stream.write ('\n    "%s\\n"' % line)
                stream.write (';\n')
            else:
                stream.write ('NULL;\n')

    def getRelease (self):
        relver = os.environ.get ('RELVER')
        if (relver == None) or (relver [0] != 'R'):
            return None
        return relver [1:]

    def writeInitfunc (self, module, mtablename):
        write = self.cStream.write
        modname = module.pyname
        funcname = 'init%s' % modname
        write ('externC GLUEEXPORT void\n%sInternal (const char * moduleName)\n{\n' % funcname)
        functionTag (self.cStream, funcname)
        write ('    PyObject* module;\n    PyObject* dict;\n\n')
        write ('    module = Py_InitModule4 ((char *) moduleName, %s, %s, NULL, PYTHON_API_VERSION);\n'
            % (mtablename, self.docString2Cliteral (module.doc)))
        self.writeCompound ('if (module == NULL)', ['return;'], '    ')
        write ('    dict = PyModule_GetDict (module);\n')
        if not module.exceptionClasses:
            self.initClassExceptions (module, write)
        for clazz in module.classes:
            self.initClass (clazz, write)
        for i in xrange (module.constantParts):
            write ('    initConstants%d (PyModule_GetDict (module));\n' % i)
        self.writeSnippet (module.snippets, 'initroutine', 'Python_',
            indent = '    ')
        self.writeCompound ('if (PyErr_Occurred ())', [
                'Py_FatalError (LIT("can\'t initialize module %s"));' % modname], '    ')
        write ('}\n\n')
        exportFunction = """
externC GLUEEXPORT void
init%(currentModule)s ()
{
    init%(modname)sInternal ("%(currentModule)s");
}
"""
        currentModule = modname
        write (exportFunction % locals ())
        for currentModule in module.attributes.get ('additionalNames', []):
            write (exportFunction % locals ())

    def initStringExceptions (self, module, write):
        modname = module.pyname
        for exception in module.exceptions:
            excVar = "%s_%s" % (exception.name, modname)
            write ('    %s = Py_BuildValue (LIT("s"), "%s.%s");\n'
                % (excVar, modname, exception.name))
            write ('    PyDict_SetItemString (dict, LIT("%s"), %s);\n'
                % (exception.name, excVar))

    def initClassExceptions (self, module, write):
        modname = module.pyname
        for exception in module.exceptions:
            excVar = "%sType" % exception.name
            write ('    %s = createExceptionKind ("%s.%s", %sCodeC);\n'
                % (excVar, modname, exception.name, exception.name))
            write ('    PyDict_SetItemString (dict, LIT("%s"), %s);\n'
                % (exception.name, excVar))

    def initClass (self, clazz, write):
        typeVar = clazz.pyname + 'Type'
        write ('    %s.ob_type = &PyType_Type;\n' % typeVar)
        if not clazz.constructor or (clazz.pyname != clazz.constructor.pyname):
            write ('    PyDict_SetItemString (dict, LIT("%s"), REINTERPRET_CAST (PyObject*, &%s));\n'
                % (clazz.pyname, typeVar))

    # class generation

    def writeClassGlue (self, clazz):
        self.writeNativeStruct (clazz)
        self.cStream.write ("""
static %sObjectT*
new%s ()
{
    %sObjectT* result;

    result = PyObject_NEW (%sObjectT, &%sType);
    if (result != NULL) {
        memset (&result->nself, '\\0', sizeof (%sObjectT) - offsetof (%sObjectT, nself));
    }
    return result;
}

static void
free%s (
    %sObjectT * value)
{
    Py_DECREF (value);
}
""" % ((clazz.pyname,) * 9))
        self.writeSnippet (clazz.snippets, 'precode', 'Python_')
        for item in clazz.items:
            item.writeGlue (self)
        self.writeMethodTable (clazz.pyname + 'ClassMethods', clazz.items)
        self.writeTypeObject (clazz)

    def writeNativeStruct (self, clazz):
        stream = self.cStream
        selfname = clazz.pyname
        stream.write ('typedef struct %sObjectT {\n    PyObject_HEAD\n'
            % selfname)
        if clazz.isEmbedded:
            stream.write ('    %s nself;\n' % clazz.name)
        else:
            stream.write ('    %s *nself;\n' % clazz.name)
        self.writeSnippet (clazz.snippets, 'instance vars', 'Python_')
        stream.write ('} %sObjectT;\n\n' % selfname)
        stream.write ('staticforward PyTypeObject %sType;\n' % selfname)
        stream.write ('#define is%sObject(op) ((op)->ob_type == &%sType)\n\n'
            % (selfname, selfname))

    def fillSequenceSpecials (self, clazz):
        result = '0'
        specials = clazz.specialMethods
        hasSequenceMethod = None
        sequenceEntries = {}
        for name in ['__len__', '__add__', '__mul__', '__getitem__', '__getslize__']:
            method = specials.get (name)
            if method != None:
                sequenceEntries [name] = method.name
                hasSequenceMethod = 1
            else:
                sequenceEntries [name] = '0'
        if hasSequenceMethod:
            result = '&' + clazz.pyname + 'SequenceVMT'
            sequenceEntries ['clazzname'] = clazz.pyname
            self.cStream.write ("""
static PySequenceMethods %(clazzname)sSequenceVMT = {
    %(__len__)s,    /* sq_length */
    %(__add__)s,    /* sq_concat */
    %(__mul__)s,    /* sq_repeat */
    REINTERPRET_CAST (intargfunc, %(__getitem__)s),    /* sq_item */
    %(__getslize__)s,    /* sq_slice */
    0,    /* sq_ass_item */
    0,    /* sq_ass_slice */
};
""" % sequenceEntries)
        return result


    def fillSpecials (self, clazz, entries):
        specials = clazz.specialMethods
        entries ['sequenceVMT'] = self.fillSequenceSpecials (clazz)
        for name in ['__repr__', '__hash__', '__call__', '__str__']:
            method = specials.get (name)
            if method != None:
                entries [name] = method.name
            else:
                entries [name] = '0'


    def writeTypeObject (self, clazz):
        entries = {}
        entries ['clazzname'] = clazz.pyname
        self.fillSpecials (clazz, entries)
        entries ['docString'] = self.docString2Cliteral (clazz.doc)
        self.cStream.write ('''
static PyObject*
%(clazzname)sGetattr (PyObject* self, char* name)
{
    return Py_FindMethod (%(clazzname)sClassMethods, self, name);
}

statichere PyTypeObject %(clazzname)sType = {
    PyObject_HEAD_INIT (NULL)
    0,
    LIT("%(clazzname)s"),    /* tp_name */
    sizeof (%(clazzname)sObjectT),    /* tp_basicsize */
    0,    /* tp_itemsize */
    REINTERPRET_CAST (destructor, destructor_%(clazzname)s),    /* tp_dealloc */
    0,    /* tp_print */
    REINTERPRET_CAST (getattrfunc, %(clazzname)sGetattr),    /* tp_getattr */
    0,    /* tp_setattr */
    0,    /* tp_compare */
    REINTERPRET_CAST (reprfunc, %(__repr__)s),    /* tp_repr */
    0,    /* tp_as_number */
    %(sequenceVMT)s,    /* tp_as_sequence */
    0,    /* tp_as_mapping */
    REINTERPRET_CAST (hashfunc, %(__hash__)s),    /* tp_hash */
    REINTERPRET_CAST (ternaryfunc, %(__call__)s),    /* tp_call */
    REINTERPRET_CAST (reprfunc, %(__str__)s),    /* tp_str */
    0,    /* tp_getattro */
    0,    /* tp_setattro */
    0,    /* *tp_as_buffer */
    0,    /* tp_xxx4 */
    %(docString)s,    /* tp_doc */
};

''' % entries)

    # constant part generation

    def writeConstantPart (self, constpart):
        stream = self.cStream
        funcname = 'initConstants%d' % constpart.context.constantParts
        stream.write ('static void\n%s (PyObject* module)\n{\n' % funcname)
        functionTag (stream, funcname)
        constpart.context.constantParts = constpart.context.constantParts + 1
        stream.write ("""
    PyObject*   v;
    int         i;
    static struct {
        const char* name;
        %s          val;
    } items [] = {""" % constpart.argType (constpart.typechar))
        for name in constpart.names:
            stream.write ('\n        {"%s", %s},' % (name, name))
        stream.write ('''
        {NULL}};

    for (i = 0; i < %d; ++i) {
        v = Py_BuildValue (LIT("%s"), items [i].val);
        PyDict_SetItemString (module, items [i].name, v);
        Py_DECREF (v);
    }
}

''' % (len (constpart.names), constpart.typechar))

class PyMethodGlue (AbstractGlue):
    language = 'Python'

    def __init__ (self, stream, moduleGlue):
        self.cStream = stream
        self.moduleGlue = moduleGlue

    def getClassKind (self, method):
        context = method.context
        isCppClass = isCClass = None
        if context.isClass:
            if context.isCpp:
                isCppClass = 1
            else:
                isCClass = 1
        return isCClass, isCppClass

    def getRealParameters (self, method):
        """parameters without any self argument
        """
        isCClass, isCppClass = self.getClassKind (method)
        if isCClass:
            parms = method.parms [1:]
        else:
            parms = method.parms
        return parms

    def writeGlue (self, method):
        if not self.supports (method) or method.attributes.get ('specialMethod'):
            return
        stream = self.cStream
        self.writeProcDeclaration (method)
        snippetsFound = self.writeSnippet (method.snippets, 'code', 'Python_')
        if not snippetsFound:
            self.needsCleanup = 0
            self.writeMethodBody (method)
            self.writeMethodEnd (method)
        self.cStream.write ('}\n\n')

    def writeMethodBody (self, method):
        self.writeInArgs (method)
        snippetsFound = self.writeSnippet (method.snippets, 'nativeCall', 'Python_')
        if not snippetsFound:
            self.writeNativeCall (method)
        self.writeOutArgs (method)

    def writeProcDeclaration (self, method):
        funcname = '%s_%s' % (method.pyname, method.context.pyname)
        self.cStream.write ('/*----------------------------------------*/\n\n')
        self.cStream.write ('static PyObject *\n%s (PyObject *pyself, PyObject* args, PyObject* keywds)\n{\n'
                % funcname)
        functionTag (self.cStream, funcname)

    def writeInArgs (self, method):
        stream = self.cStream
        context = method.context
        fmtstr = ""
        callList = []
        isCClass, isCppClass = self.getClassKind (method)
        stream.write ('    int ok = 1;\n')
        if context.isClass:
            stream.write ('    %sObjectT* self = REINTERPRET_CAST (%sObjectT*, pyself);\n'
                    % (context.pyname, context.pyname))
            if context.isEmbedded:
                stream.write ('    %s* nself = &self->nself;\n'
                        % context.name)
            else:
                stream.write ('    %s* nself = self->nself;\n'
                        % context.name)
        self.declareResultVar (stream, method)

        if isCClass:
            parms = method.parms [1:]
        else:
            parms = method.parms
        if len (parms) > 0:
            self.writeKeywordList (stream, parms)
            defaultFrontier = None
            for parm in parms:
                argstr = parm.type
                typeMapper = self.moduleGlue.findType (argstr)
                if parm.isScriptingInput ():
                    if parm.defaultVal and not defaultFrontier:
                        fmtstr = fmtstr + '|'
                        defaultFrontier = 1
                    fmtstr = fmtstr + typeMapper.typechar
                    callList.append (typeMapper.asParseTupleParameter (parm.name))
                for dcl in typeMapper.varDcls (parm.name, parm.type, parm.defaultVal):
                    stream.write ('    %s;\n' % dcl)
            stream.write ('\n')
            for parm in parms:
                typeMapper = self.moduleGlue.findType (parm.type)
                initcall = typeMapper.initcall (parm.name)
                if initcall:
                    self.cStream.write ('    ' + initcall + '\n')
            if method.context.precondition:
                self.writeErrorCondition ('!' + (method.context.precondition % 'self'), method)
            self.writeObjectConstruction (method)
            if len(callList) > 0:
                fmtstr = fmtstr + ':' + method.context.pyname + '.' + method.pyname
                parseCall = ('!PyArg_ParseTupleAndKeywords (args, keywds, LIT("%s"), (char **) kwlist, %s)'
                    % (fmtstr, string.joinfields (callList, ', ')))
                self.writeErrorCondition (parseCall, method);
                for parm in parms:
                    if parm.isInput ():
                        typeMapper = self.moduleGlue.findType (parm.type)
                        conversion = typeMapper.py2c (parm.name)
                        if conversion:
                            self.writeErrorCondition ('!' + conversion, method)
        else:
            if method.isConstructor:
                self.writeObjectConstruction (method)

    def writeKeywordList (self, stream, parms):
        stream.write ('    static const char * kwlist [] = {\n        ')
        for parm in parms:
            if parm.isInput () and not parm.isHelper:
                stream.write ('LIT("%s"), ' % parm.name)
        stream.write ('NULL};\n');


    def declareResultVar (self, stream, method):
        if method.returns:
            pass
            #stream.write ('    %s result;\n' % method.returns.type)

    def writeObjectConstruction (self, method):
        pass

    def writeNativeCall (self, method):
        stream = self.cStream
        stream.write ('    ')
        if method.returns:
            stream.write (self.resultLVal () + ' = ')
        isCClass, isCppClass = self.getClassKind (method)
        if isCppClass:
            stream.write ('nself->')
        stream.write (method.name + ' (')
        args = []
        if method.parms:
            if isCClass:
                parms = method.parms [1:]
            else:
                parms = method.parms
            for parm in parms:
                if parm != method.returns:
                    typeMapper = self.moduleGlue.findType (parm.type)
                    callParmString = typeMapper.asCallParameter (parm.name)
                    if parm.isOutput ():
                        callParmString = '&' + callParmString
                    args.append (callParmString)
        if isCClass:
            args.insert (0, 'nself')
        stream.write (string.joinfields (args, ', '))
        stream.write (');\n')
        if method.exception:
            self.writeResultException (method)

    def resultLVal (self):
        return 'result'

    def writeResultException (self, method):
        self.writeErrorCondition (method.exception (self.resultLVal ()), method)

    def writeOutArgs (self, method):
        self.cStream.write ('    /* outargs */\n')
        for parm in method.parms:
            if parm.isOutput ():
                typeMapper = self.moduleGlue.findType (parm.type)
                conversion = typeMapper.c2py (parm.name)
                if conversion:
                    self.writeErrorCondition ('!' + conversion, method)

    def writeErrorCondition (self, condition, method):
        self.needsCleanup = 1
        self.writeCompound ('if (%s)' % condition, [
                            'ok = 0;',
                            'goto cleanup_label;'],
                            '    ')

    def writeMethodCleanup (self, method, indent = '    '):
        for parm in self.getRealParameters (method):
            typeMapper = self.moduleGlue.findType (parm.type)
            destructor = typeMapper.destructor (parm.name)
            if destructor:
                self.cStream.write (indent + destructor + '\n')

    def writeErrorCleanup (self, method, indent = ''):
        pass

    def writeMethodEnd (self, method):
        self.writeCleanup (method)
        self.writeReturn (method)

    def writeCleanup (self, method):
        if self.needsCleanup:
            stream = self.cStream
            stream.write ('  cleanup_label:\n')
            indent = ' ' * 4
            self.writeMethodCleanup (method, indent)
            stream.write ('    if (!ok) {\n')
            self.writeErrorCleanup (method, indent * 2)
            stream.write ('        return NULL;\n')
            stream.write ('    }\n')

    def writeReturn (self, method):
        stream = self.cStream
        parms = method.parms
        callList = []
        fmtstr = ""
        for parm in parms:
            if parm.isScriptingOutput ():
                typeMapper = self.moduleGlue.findType (parm.type)
                fmtstr = fmtstr + typeMapper.typechar
                callList.append (typeMapper.asMakeTupleParameter (parm.name))
        if (len (callList) == 1) and (fmtstr == 'O'):
            # Py_BuildValue increases refcount of args
            # so don't use it to return a single value
            stream.write ('    return %s;\n' % callList [0])
        elif len (callList) > 0:
            argstr = string.joinfields (callList, ', ')
            stream.write ('    return Py_BuildValue (LIT("%s"), %s);\n'
                    % (fmtstr, argstr))
        else:
            stream.write ('    Py_INCREF(Py_None);\n')
            stream.write ('    return Py_None;\n')

    def errorReturnString (self, method = None):
        return 'return NULL;'

class PyConstructorGlue (PyMethodGlue):
    def writeObjectConstruction (self, method):
        tname = method.clazz.pyname
        self.cStream.write ('    newObj = new%s ();\n' % tname)
        self.writeErrorCondition ('newObj == NULL', method)

    def declareResultVar (self, stream, method):
        stream.write ('    %sObjectT * newObj = NULL;\n' % method.clazz.pyname)

    def resultLVal (self):
        return 'newObj->nself'

    def writeErrorCleanup (self, method, indent):
        PyMethodGlue.writeErrorCleanup (self, method, indent)
        self.cStream.write (indent + '''if (newObj != NULL) {
            Py_DECREF (newObj);
        };\n''')

    def writeReturn (self, method):
        self.cStream.write ('    return REINTERPRET_CAST (PyObject *, newObj);\n')


def functionTag (stream, name):
    stream.write ('    ROUTINE_DBG_MSP00 ("%s");  /* generated by Glue */\n' % name)
    # stream.write ('#undef MF__\n#define MF__ MOD__"%s"\n' % name)

PyGlue = PyModuleGlue

