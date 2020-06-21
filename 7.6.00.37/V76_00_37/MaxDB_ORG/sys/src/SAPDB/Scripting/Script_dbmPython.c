/*
===========================================
   responsible:    DanielD
   description:    Interface to DB Manager
   target:         Python
   last changed:   11.04.2006

    ========== licence begin  GPL
    Copyright (c) 2000-2006 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
    ========== licence end
===========================================*/


#include <stddef.h>

#if defined (HPUX) || defined (AIX) || defined (SUN) || defined(SOLARIS) || defined (OSF1) || defined (LINUX) || defined (FREEBSD)
#define BAD_STATIC_FORWARD
#endif

#include "Python.h"/* no check */

#if PY_MAJOR_VERSION >= 2 && PY_MINOR_VERSION >= 4
#else
#if defined (HPUX) || defined (AIX) || defined (SUN) || defined (OSF1)
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

static PyObject *CommunicationErrorType;
static const char * CommunicationErrorCodeC = 
    "\n"
    "def __str__ (self):\n"
    "    return 'dbm.CommunicationError: [%d] %s' % (self.errorCode, self.message)\n"
    "\n";
static PyObject *DBMServErrorType;
static const char * DBMServErrorCodeC = 
    "\n"
    "def __str__ (self):\n"
    "    return 'dbm.DBMServError: [%d] %s' % (self.errorCode, self.message)\n"
    "\n";
/* snippet Python_precode */

#define CN72 1
#include "hsp100.h"
#include "hcn14.h"
#include "hsp77.h"
#include "hsp78_0.h"

typedef PyObject SL_ValueT;

#if defined (__cplusplus)
#define REFPARM(name)   name
#define INLINE          inline
#else
#define REFPARM(name)   &name
#define INLINE

#undef ROUTINE_DBG_MSP00
#define ROUTINE_DBG_MSP00(lit) static const char * funcname_glue_ = lit

#define REINTERPRET_CAST(type, expr) ((type) expr)
#define STATIC_CAST(type, expr) ((type) expr)
#define CONST_CAST(type, expr) ((type) expr)
#define REFCAST_MEO00(type)
#endif


static void trimPayload (
    const void * dataArg,
    tsp00_Int4 * len);

/*----------------------------------------*/

static void
raiseCommunicationError (
    int code,
    const char * msg)
{
    ROUTINE_DBG_MSP00 ("raiseCommunicationError");
    PyObject * exception = PyInstance_New (CommunicationErrorType, NULL, NULL);
    PyObject * pycode = NULL;
    PyObject * pymsg = NULL;

    pycode = PyInt_FromLong  (code);
    pymsg = PyString_FromString  (msg);
    if (exception != NULL) {
        PyObject_SetAttrString (exception, LIT ("errorCode"), pycode) ;
        PyObject_SetAttrString (exception, LIT ("message"), pymsg);
    }
    else {
        exception = Py_BuildValue (LIT ("NN"), pycode, pymsg);
    }
    PyErr_SetObject (CommunicationErrorType, exception);
    Py_XDECREF (pycode);
    Py_XDECREF (pymsg);
    Py_XDECREF (exception);
}

/*----------------------------------------*/

static int
strchrOrEnd (
    const char * str,
    int searchChar)
{
    ROUTINE_DBG_MSP00 ("strchrOrEnd");
    const char * chr = strchr (str, searchChar);
    int result;

    if (chr != NULL) {
        result = chr - str;
    }
    else {
        result = strlen (str);
    }
    return result;
}

/*----------------------------------------*/

static void
raiseDBMError (
    int code,
    const char * msg,
    const char * additionalMsg,
    int          additionalLen,
    SL_ValueT  * cmd)
{
    ROUTINE_DBG_MSP00 ("raiseDBMError");
    PyObject * exception = PyInstance_New (DBMServErrorType, NULL, NULL);
    PyObject * errorCode = NULL;
    PyObject * errorSymbol = NULL;
    PyObject * errorMessage = NULL;
    PyObject * additionalInfo = NULL;
    const char * endstr;
    int datalen;

    errorCode = PyInt_FromLong  (code);
    endstr = strchr (msg, ':');
    if (endstr != NULL) {
        errorSymbol = PyString_FromStringAndSize (msg, endstr - msg);
        msg = endstr + 2;  /* adjust for : and blank */
    }
    else {
        errorSymbol = PyString_FromStringAndSize ("", 0);
        /* keep msg, no error symbol found */
    }
    datalen = strchrOrEnd (msg, '\n');
    errorMessage = PyString_FromStringAndSize (msg, datalen);
    if (additionalLen > 0) {
        if (additionalMsg [additionalLen - 1] == '\n') {
            --additionalLen;
        }
        additionalInfo = PyString_FromStringAndSize (
            additionalMsg, additionalLen);
    }
    else {
        Py_INCREF (Py_None);
        additionalInfo = Py_None;
    }
    if (exception != NULL) {
        PyObject_SetAttrString (exception, LIT ("errorCode"), errorCode);
        PyObject_SetAttrString (exception, LIT ("errorSymbol"), errorSymbol);
        PyObject_SetAttrString (exception, LIT ("message"), errorMessage);
        PyObject_SetAttrString (exception, LIT ("additionalInfo"), additionalInfo);
        PyObject_SetAttrString (exception, LIT ("cmd"), cmd);
    }
    else {
        exception = Py_BuildValue (LIT ("NNNNN"), errorCode, errorSymbol,
            errorMessage, additionalInfo, cmd);
    }
    PyErr_SetObject (DBMServErrorType, exception);
    Py_XDECREF (errorCode);
    Py_XDECREF (errorSymbol);
    Py_XDECREF (errorMessage);
    Py_XDECREF (additionalInfo);
    Py_XDECREF (exception);
}

/*----------------------------------------*/

static int
errorOccured (
    int rc,
    tsp00_ErrTextc VAR_ARRAY_REF msg,
    char * croakBuf)
{
    ROUTINE_DBG_MSP00 ("errorOccured");
    if (rc != 0) {
        raiseCommunicationError (rc, msg);
        return 1;
    }
    else {
        return 0;
    }
}

/*----------------------------------------*/

static int
commErrOccured (
    int rc,
    tsp00_ErrTextc VAR_ARRAY_REF msg,
    char * croakBuf)
{
    ROUTINE_DBG_MSP00 ("commErrOccured");
    if (rc != 0) {
        raiseCommunicationError (rc, msg);
        return 1;
    }
    else {
        return 0;
    }
}

/*----------------------------------------*/

static int
dbmServErrOccured (
    void * nself,
    int rc,
    tsp00_ErrTextc VAR_ARRAY_REF msg,
    SL_ValueT * cmd,
    char * croakBuf)
{
    int           errOccurred = 0;
    tsp00_Int4      serverError;
    const void  * payLoad;
    tsp00_Int4      payLoadLen;
    tsp00_Int4      specificError;
    tsp00_ErrTextc errtext;


    if (rc == DBMAPI_COMMERR_CN14) {
        raiseCommunicationError (rc, msg);
        return 1;
    }
    else if (rc == DBMAPI_INVSESSION_CN14) {
        strcpy (msg, "Invalid Session");
        raiseCommunicationError (rc, msg);
        return 1;
    }
    serverError = cn14analyzeDbmAnswer (nself, &payLoad, &payLoadLen,
            &specificError, errtext);
    trimPayload (payLoad, &payLoadLen);
    if (serverError != DBMAPI_OK_CN14) {
        errOccurred = 1;
        raiseDBMError (specificError, errtext, (char *) payLoad, payLoadLen, cmd);
    }
    return errOccurred;
}

#define croakBuf NULL

/* endsnippet Python_precode */
typedef struct DBMObjectT {
    PyObject_HEAD
    void *nself;
/* no code for key DBM Python_/Generic_ in ['Generic_precode']*/
} DBMObjectT;

staticforward PyTypeObject DBMType;
#define isDBMObject(op) ((op)->ob_type == &DBMType)


static DBMObjectT*
newDBM ()
{
    DBMObjectT* result;

    result = PyObject_NEW (DBMObjectT, &DBMType);
    if (result != NULL) {
        memset (&result->nself, '\0', sizeof (DBMObjectT) - offsetof (DBMObjectT, nself));
    }
    return result;
}

static void
freeDBM (
    DBMObjectT * value)
{
    Py_DECREF (value);
}
/* snippet DBM Generic_precode */

static char * buildInfo ()
{
    static tsp00_Versionc versionString;
    static bool isInitialized;
#if defined (PYTHON_GLUE)
    tsp100_CompName compName = "dbmpy    ";
#elif defined (PERL_GLUE)
    tsp100_CompName compName = "dbmperl  ";
#else
    #error must be either Python or Perl
#endif

    if (!isInitialized) {
        sp100_GetVersionString (compName, s100buildnumber, versionString);
    }
    return versionString;
}

static void
trimPayload (
    const void * data,
    tsp00_Int4 * len)
{
    if (data != NULL) {
        const void * nullPos = memchr (data, '\0', *len);
        if (nullPos != NULL) {
            *len = (tsp00_Int4) (((const char *) nullPos) - ((const char *) data));
        }
    }
    else {
        *len = 0;
    }

}

static void
localRelease (
    void* nself)
{
    if (nself != NULL) {
        cn14release (&nself);
    }
}


#if defined (PYTHON_GLUE)
static void
destructor_DBM (
    PyObject * pyself)
{
    DBMObjectT* self = REINTERPRET_CAST (DBMObjectT*, pyself);

    localRelease (self->nself);
    PyObject_Del (pyself);
}
#endif

typedef struct StringT {
    SL_ValueT  * realObject;
    char       * utf8;
    int          utf8Len;
    int          isAllocated;
} StringT;

/*----------------------------------------*/

#if defined (PYTHON_GLUE)

/*----------------------------------------*/

static void
SL_getRawString (
    PyObject* value,
    const char ** chars,
    int * len)
{
    if (PyString_Check (value)) {
        *chars = PyString_AsString (value);
        *len = PyString_Size (value);
    }
    else {
        PyObject * str = PyObject_Str (value);
        *chars = PyString_AsString (str);
        *len = PyString_Size (str);
        Py_DECREF (str);
    }
}

/*----------------------------------------*/

static void
SL_getEncodedString (
    SL_ValueT * value,
    const char ** chars,
    int * characterCount,
    int * byteCount,
    const tsp77encoding ** srcEncoding)
{
#if (PY_MAJOR_VERSION >= 2)
#if ! defined (Py_UNICODE_SIZE)
#define Py_UNICODE_SIZE 2
#endif
    if (PyUnicode_Check (value)) {
        *chars = PyUnicode_AS_DATA (value);
        *characterCount = PyUnicode_GET_SIZE (value);
        *byteCount = PyUnicode_GET_DATA_SIZE (value);
        if (Py_UNICODE_SIZE == 2) {
            *srcEncoding = sp77encodingUCS2Native;
        }
        else {
            *srcEncoding = sp77encodingUCS4Native;
        }
    }
    else if (PyString_Check (value)) {
        *chars = PyString_AsString (value);
        *characterCount = *byteCount = PyString_Size (value);
        *srcEncoding = sp77encodingAscii;
    }
    else {
        PyObject * str = PyObject_Str (value);
        *chars = PyString_AsString (str);
        *characterCount = *byteCount  = PyString_Size (str);
        *srcEncoding = sp77encodingAscii;
        Py_DECREF (str);
    }
#else
    SL_getRawString (value, chars, characterCount);
    *byteCount = *characterCount;
    *srcEncoding = sp77encodingAscii;
#endif
}
#endif

/*----------------------------------------*/

#if defined (PERL_GLUE)

static void
SL_getEncodedString (
    SL_ValueT * value,
    const char ** chars,
    int * characterCount,
    int * byteCount,
    const tsp77encoding ** srcEncoding)
{
    STRLEN len;

    if (SvUTF8 (value)) {
        *srcEncoding = sp77encodingUTF8;
        *chars = SvPV (value, len);
        *byteCount = len;
        *characterCount = 0;
        {
            unsigned int    charLength;
            unsigned int    byteLength;
            int             isTerminated;
            int             isCorrupted;
            int             isExhausted;
            int             infoOK;

            infoOK = sp77encodingUTF8->stringInfo (*chars,
                               *byteCount, true,
                               &charLength,
                               &byteLength,
                               &isTerminated,
                               &isCorrupted,
                               &isExhausted);
            *characterCount = charLength;
        }
    }
    else {
        *srcEncoding = sp77encodingAscii;
        *chars = SvPV (value, len);
        *byteCount = len;
        *characterCount = *byteCount;
    }
}

#endif

/*----------------------------------------*/

static int
string2C (
    void     * nself,
    SL_ValueT * stringObj,
    StringT  * stringVar)
{
    const char          * rawPython;
    int                   characterCount;
    int                   byteCount;
    const tsp77encoding * srcEncoding;
    tsp78ConversionResult convRC;
    int                   allocbufSize;
    tsp00_Uint4           utf8bufUsed;
    tsp00_Uint4           bytesParsed;

    stringVar->isAllocated = 0;
    SL_getEncodedString (stringObj, &rawPython, &characterCount, &byteCount, &srcEncoding);
    /*
     * alloc
     */
    allocbufSize = characterCount * 3;
    stringVar->utf8 = (char *) malloc (allocbufSize);
    if (stringVar->utf8 == NULL) {
        return 0;
    }
    stringVar->isAllocated = 1;
    /*
     * convert
     */
    convRC = sp78convertString (sp77encodingUTF8, 
        stringVar->utf8, allocbufSize, &utf8bufUsed, 0,
        srcEncoding, rawPython, byteCount, &bytesParsed);
    if (convRC != sp78_Ok) {
        return false;
    }
    stringVar->realObject = stringObj;
    stringVar->utf8Len = utf8bufUsed;
    return true;
}

/*----------------------------------------*/

#if defined (PYTHON_GLUE)

static int
string2Python (
    void      * nself,
    StringT     stringVar,
    PyObject ** stringObj)
{
    unsigned int    charLength;
    unsigned int    byteLength;
    int             isTerminated;
    int             isCorrupted;
    int             isExhausted;
    int             infoOK;
    int             useAsciiConversion;
    tsp78ConversionResult convRC;
    const tsp77encoding * targetEncoding;
    void                * targetPointer;
    int                   targetSize;
    tsp00_Uint4           targetUsed;
    tsp00_Uint4           bytesParsed;

    if (stringVar.utf8Len == 0) {
        *stringObj = PyString_FromStringAndSize ("", 0);
        return true;
    }
    infoOK = sp77encodingUTF8->stringInfo (stringVar.utf8,
                       stringVar.utf8Len, true,
                       &charLength,
                       &byteLength,
                       &isTerminated,
                       &isCorrupted,
                       &isExhausted);
#if (PY_MAJOR_VERSION < 2)
    useAsciiConversion = true;
#else
    useAsciiConversion = (charLength == byteLength);
#endif
    if (useAsciiConversion) {
        targetEncoding = sp77encodingAscii;
        *stringObj = PyString_FromStringAndSize (NULL, charLength);
        if (stringObj == NULL) {
            return false;
        }
        targetPointer = PyString_AS_STRING (*stringObj);
        targetSize = charLength;
    }
#if (PY_MAJOR_VERSION >= 2)
    else {
        if (Py_UNICODE_SIZE == 2) {
            targetEncoding = sp77encodingUCS2Native;
        }
        else {
            targetEncoding = sp77encodingUCS4Native;
        }
        *stringObj = PyUnicode_FromUnicode (NULL, charLength);
        if (stringObj == NULL) {
            return false;
        }
        targetPointer = (void*) PyUnicode_AS_DATA (*stringObj);
        targetSize = charLength * Py_UNICODE_SIZE;
    }
#endif
    convRC = sp78convertString (targetEncoding, 
        targetPointer, targetSize, &targetUsed, 0,
        sp77encodingUTF8, stringVar.utf8, stringVar.utf8Len, &bytesParsed);
    if (convRC != sp78_Ok) {
        return false;
    }
    return true;
}
#endif

/*----------------------------------------*/

#if defined (PERL_GLUE)
static int
string2Perl (
    void      * nself,
    StringT   * stringVar,
    SL_ValueT * stringObj)
{
    unsigned int    charLength;
    unsigned int    byteLength;
    int             isTerminated;
    int             isCorrupted;
    int             isExhausted;
    int             infoOK;

    if (stringVar->utf8Len == 0) {
        sv_setpvn (stringObj, "", 0);
        return true;
    }
    infoOK = sp77encodingUTF8->stringInfo (stringVar->utf8,
                       stringVar->utf8Len, true,
                       &charLength,
                       &byteLength,
                       &isTerminated,
                       &isCorrupted,
                       &isExhausted);
    sv_setpvn (stringObj, stringVar->utf8, byteLength);
    if (charLength != byteLength) {
        SvUTF8_on (stringObj);
    }
    return true;
}

#endif

/*----------------------------------------*/

static void
stringDestructor (
    void     * nself,
    StringT    stringVar)
{
    if (stringVar.isAllocated) {
        free (stringVar.utf8);
    }
}
 
/*----------------------------------------*/

static tsp00_Int4 cmdAndRead (
   void * nself,
   StringT cmd,
   StringT * output,
   tsp00_ErrTextc VAR_ARRAY_REF errtext)
{
    tsp00_Int4 rc;
    tsp00_Int4 serverError;

    Glue_BEGIN_ALLOW_THREADS
    rc = cn14cmdExecute (nself, cmd.utf8, cmd.utf8Len, NULL, NULL, errtext);
    Glue_END_ALLOW_THREADS
    if (rc == DBMAPI_OK_CN14) {
        tsp00_Int4 datalen;
        rc = cn14analyzeDbmAnswer (nself, (const void**) &output->utf8, &datalen,
                &serverError, errtext);
        if (output->utf8 != NULL) {
            trimPayload (output->utf8, &datalen);
            output->utf8Len = datalen;
        }
    }
    return rc;
}

/*----------------------------------------*/

static tsp00_Int4 rawCommand (
   void * nself,
   StringT cmd,
   StringT * output,
   tsp00_ErrTextc VAR_ARRAY_REF errtext)
{
    tsp00_Int4 rc;

    Glue_BEGIN_ALLOW_THREADS
    rc = cn14cmdExecute (nself, cmd.utf8, cmd.utf8Len, NULL, NULL, errtext);
    Glue_END_ALLOW_THREADS
    if (rc == DBMAPI_OK_CN14) {
        tsp00_Int4 datalen = cn14bytesAvailable (nself);
        output->utf8 = (char *) cn14rawReadData (nself, &rc);
        if (output->utf8 != NULL) {
            trimPayload (output->utf8, &datalen);
            output->utf8Len = datalen;
        }
    }
    return rc;
}

/*----------------------------------------*/

static int cancelCmd (
   void * nself)
{
    Glue_BEGIN_ALLOW_THREADS
    cn14cmdCancel (nself);
    Glue_END_ALLOW_THREADS
    return 0;
}

/*----------------------------------------*/

static int doConnect (
   const char * servernode,
   const char * dbname,
   const char * dbroot,
   const char * userpwd,
   void      ** sessionOut,
   tsp00_ErrTextc VAR_ARRAY_REF errtext)
{
    int rc;

    Glue_BEGIN_ALLOW_THREADS
    if (userpwd != NULL) {
        rc = cn14connectDBMUsr (
                REFCAST_MEO00 (tsp00_NodeIdc) servernode,
                REFCAST_MEO00 (tsp00_DbNamec) dbname,
                REFCAST_MEO00 (tsp00_VFilenamec) dbroot,
                REFCAST_MEO00 (tsp00_C64c) userpwd,
                sessionOut, errtext);
    }
    else {
        rc = cn14connectDBM (
                REFCAST_MEO00 (tsp00_NodeIdc) servernode,
                REFCAST_MEO00 (tsp00_DbNamec) dbname,
                REFCAST_MEO00 (tsp00_VFilenamec) dbroot,
                sessionOut, errtext);
    }
    Glue_END_ALLOW_THREADS
    return rc;
}

static INLINE void doSaveUser (
   const char * dbname,
   const char * servernode,
   const char * userpwd)
{
    Glue_BEGIN_ALLOW_THREADS
    cn14saveUser (
                REFCAST_MEO00 (tsp00_NodeIdc) servernode,
                REFCAST_MEO00 (tsp00_DbNamec) dbname,
                REFCAST_MEO00 (tsp00_C64c) userpwd);
    Glue_END_ALLOW_THREADS
}

static INLINE tsp00_Int4 doCheckUser (
   const char * dbname,
   const char * servernode,
   const char * userpwd)
{
    tsp00_Int4 result;

    Glue_BEGIN_ALLOW_THREADS
    result = cn14checkUser (
                REFCAST_MEO00 (tsp00_NodeIdc) servernode,
                REFCAST_MEO00 (tsp00_DbNamec) dbname,
                REFCAST_MEO00 (tsp00_C64c) userpwd);
    Glue_END_ALLOW_THREADS
    return result;
}

static INLINE void doDeleteUser (
   const char * dbname,
   const char * servernode)
{
    Glue_BEGIN_ALLOW_THREADS
    cn14deleteUser (
        REFCAST_MEO00 (tsp00_NodeIdc) servernode,
        REFCAST_MEO00 (tsp00_DbNamec) dbname);
    Glue_END_ALLOW_THREADS
}

/* endsnippet DBM Generic_precode */
/*----------------------------------------*/

static PyObject *
cmd_DBM (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("cmd_DBM");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    DBMObjectT* self = REINTERPRET_CAST (DBMObjectT*, pyself);
    void* nself = self->nself;
    static const char * kwlist [] = {
        LIT("cmd"), NULL};
    StringT cmd;
    PyObject * cmdObj;
    StringT output;
    PyObject * outputObj;
    tsp00_ErrTextc msg;
    tsp00_Int4 result;

    memset (&cmd, '\0', sizeof (StringT));
    memset (&output, '\0', sizeof (StringT));
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("O:DBM.cmd"), (char **) kwlist, &cmdObj)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!string2C (self, cmdObj, &cmd)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = cmdAndRead (nself, cmd, &output, msg);
    if (dbmServErrOccured (nself, result, msg, cmd.realObject, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!string2Python (self, output, &outputObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    stringDestructor (self, cmd);
    stringDestructor (self, output);
    if (!ok) {
        return NULL;
    }
    return outputObj;
}

/*----------------------------------------*/

static PyObject *
rawCmd_DBM (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("rawCmd_DBM");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    DBMObjectT* self = REINTERPRET_CAST (DBMObjectT*, pyself);
    void* nself = self->nself;
    static const char * kwlist [] = {
        LIT("cmd"), NULL};
    StringT cmd;
    PyObject * cmdObj;
    StringT output;
    PyObject * outputObj;
    tsp00_ErrTextc msg;
    tsp00_Int4 result;

    memset (&cmd, '\0', sizeof (StringT));
    memset (&output, '\0', sizeof (StringT));
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("O:DBM.rawCmd"), (char **) kwlist, &cmdObj)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!string2C (self, cmdObj, &cmd)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = rawCommand (nself, cmd, &output, msg);
    if (commErrOccured (result, msg, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!string2Python (self, output, &outputObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    stringDestructor (self, cmd);
    stringDestructor (self, output);
    if (!ok) {
        return NULL;
    }
    return outputObj;
}

/*----------------------------------------*/

static PyObject *
cancelCmd_DBM (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("cancelCmd_DBM");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    DBMObjectT* self = REINTERPRET_CAST (DBMObjectT*, pyself);
    void* nself = self->nself;
    static const char * kwlist [] = {
        NULL};
    int result;

/* no code for key Python_/Generic_nativeCall */
    result = cancelCmd (nself);
    /* outargs */
    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static PyObject *
release_DBM (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("release_DBM");  /* generated by Glue */
/* no code for key release Python_/Generic_ in ['Generic_nativeCall']*/
    int ok = 1;
    DBMObjectT* self = REINTERPRET_CAST (DBMObjectT*, pyself);
    void* nself = self->nself;
/* snippet release Generic_nativeCall */
    localRelease (self->nself);
    self->nself = NULL;
    /* endsnippet release Generic_nativeCall */
    /* outargs */
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef DBMClassMethods [] = {
    {LIT("cmd"), (PyCFunction)cmd_DBM, METH_VARARGS|METH_KEYWORDS, 
        "exeute a command and return resulting buffer as string"},
    {LIT("rawCmd"), (PyCFunction)rawCmd_DBM, METH_VARARGS|METH_KEYWORDS, 
        "exeute a command and return resulting buffer as string.\n\n            This doesn't scan the buffer for error information.\n            "},
    {LIT("cancelCmd"), (PyCFunction)cancelCmd_DBM, METH_VARARGS|METH_KEYWORDS, 
        "cancels a pending request"},
    {LIT("release"), (PyCFunction)release_DBM, METH_VARARGS|METH_KEYWORDS, 
        "closes the connection"},
    {NULL, NULL}
};


static PyObject*
DBMGetattr (PyObject* self, char* name)
{
    return Py_FindMethod (DBMClassMethods, self, name);
}

statichere PyTypeObject DBMType = {
    PyObject_HEAD_INIT (NULL)
    0,
    LIT("DBM"),    /* tp_name */
    sizeof (DBMObjectT),    /* tp_basicsize */
    0,    /* tp_itemsize */
    REINTERPRET_CAST (destructor, destructor_DBM),    /* tp_dealloc */
    0,    /* tp_print */
    REINTERPRET_CAST (getattrfunc, DBMGetattr),    /* tp_getattr */
    0,    /* tp_setattr */
    0,    /* tp_compare */
    REINTERPRET_CAST (reprfunc, 0),    /* tp_repr */
    0,    /* tp_as_number */
    0,    /* tp_as_sequence */
    0,    /* tp_as_mapping */
    REINTERPRET_CAST (hashfunc, 0),    /* tp_hash */
    REINTERPRET_CAST (ternaryfunc, 0),    /* tp_call */
    REINTERPRET_CAST (reprfunc, 0),    /* tp_str */
    0,    /* tp_getattro */
    0,    /* tp_setattro */
    0,    /* *tp_as_buffer */
    0,    /* tp_xxx4 */
    NULL,    /* tp_doc */
};

/*----------------------------------------*/

static PyObject *
DBM_dbm (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("DBM_dbm");  /* generated by Glue */
/* no code for key constructor Python_/Generic_ in ['Generic_nativeCall']*/
    int ok = 1;
    DBMObjectT * newObj = NULL;
    static const char * kwlist [] = {
        LIT("servernode"), LIT("dbname"), LIT("dbroot"), LIT("userpwd"), NULL};
    char * servernode = LIT( "");
    char * dbname = LIT( "");
    char * dbroot = LIT( "");
    char * userpwd =  NULL;
    void * session;
    tsp00_ErrTextc msg;
    tsp00_Int4 result;

    newObj = newDBM ();
    if (newObj == NULL) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("|ssss:dbm.DBM"), (char **) kwlist, &servernode, &dbname, &dbroot, &userpwd)) {
        ok = 0;
        goto cleanup_label;
    }
/* snippet constructor Generic_nativeCall */
    result = doConnect (servernode, dbname, dbroot, userpwd, &session, msg);
    newObj->nself = session;
    ok = ! errorOccured (result, msg, croakBuf);
    /* endsnippet constructor Generic_nativeCall */
    /* outargs */
  cleanup_label:
    if (!ok) {
        if (newObj != NULL) {
            Py_DECREF (newObj);
        };
        return NULL;
    }
    return REINTERPRET_CAST (PyObject *, newObj);
}

/*----------------------------------------*/

static PyObject *
saveUser_dbm (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("saveUser_dbm");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    static const char * kwlist [] = {
        LIT("dbname"), LIT("servernode"), LIT("userpwd"), NULL};
    char * dbname;
    char * servernode;
    char * userpwd;

    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("sss:dbm.saveUser"), (char **) kwlist, &dbname, &servernode, &userpwd)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    doSaveUser (dbname, servernode, userpwd);
    /* outargs */
  cleanup_label:
    if (!ok) {
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static PyObject *
checkUser_dbm (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("checkUser_dbm");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    static const char * kwlist [] = {
        LIT("dbname"), LIT("servernode"), LIT("userpwd"), NULL};
    char * dbname;
    char * servernode;
    char * userpwd;
    tsp00_Int4 result;

    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("sss:dbm.checkUser"), (char **) kwlist, &dbname, &servernode, &userpwd)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = doCheckUser (dbname, servernode, userpwd);
    /* outargs */
  cleanup_label:
    if (!ok) {
        return NULL;
    }
    return Py_BuildValue (LIT("i"), result);
}

/*----------------------------------------*/

static PyObject *
_buildInfo_dbm (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("_buildInfo_dbm");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    static const char * kwlist [] = {
        NULL};
    char * result;

/* no code for key Python_/Generic_nativeCall */
    result = buildInfo ();
    /* outargs */
    return Py_BuildValue (LIT("s"), result);
}

/*----------------------------------------*/

static PyObject *
deleteUser_dbm (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("deleteUser_dbm");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    static const char * kwlist [] = {
        LIT("dbname"), LIT("servernode"), NULL};
    char * dbname;
    char * servernode;

    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("ss:dbm.deleteUser"), (char **) kwlist, &dbname, &servernode)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    doDeleteUser (dbname, servernode);
    /* outargs */
  cleanup_label:
    if (!ok) {
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef dbmModuleMethods [] = {
    {LIT("DBM"), (PyCFunction)DBM_dbm, METH_VARARGS|METH_KEYWORDS, 
        "open a connection\n\n                An empty servernode will search for a dbm server on\n                the local machine. Username and passsword must be specified as\n                user,pwd"},
    {LIT("saveUser"), (PyCFunction)saveUser_dbm, METH_VARARGS|METH_KEYWORDS, 
        "saves a user in the client environment for authorization\n                against DBMServer"},
    {LIT("checkUser"), (PyCFunction)checkUser_dbm, METH_VARARGS|METH_KEYWORDS, 
        "checks a user in the client environment against the\n                current value"},
    {LIT("_buildInfo"), (PyCFunction)_buildInfo_dbm, METH_VARARGS|METH_KEYWORDS, 
        "returns a string containing the release and build number"},
    {LIT("deleteUser"), (PyCFunction)deleteUser_dbm, METH_VARARGS|METH_KEYWORDS, 
        "deletes a user in the client environment"},
    {NULL, NULL}
};

externC GLUEEXPORT void
initdbmInternal (const char * moduleName)
{
    ROUTINE_DBG_MSP00 ("initdbm");  /* generated by Glue */
    PyObject* module;
    PyObject* dict;

    module = Py_InitModule4 ((char *) moduleName, dbmModuleMethods, "Interface to DB Manager", NULL, PYTHON_API_VERSION);
    if (module == NULL) {
        return;
    }
    dict = PyModule_GetDict (module);
    CommunicationErrorType = createExceptionKind ("dbm.CommunicationError", CommunicationErrorCodeC);
    PyDict_SetItemString (dict, LIT("CommunicationError"), CommunicationErrorType);
    DBMServErrorType = createExceptionKind ("dbm.DBMServError", DBMServErrorCodeC);
    PyDict_SetItemString (dict, LIT("DBMServError"), DBMServErrorType);
    DBMType.ob_type = &PyType_Type;
/* no code for key Python_/Generic_ in ['Python_precode', 'Perl_precode', 'DBM Generic_precode', 'constructor Generic_nativeCall', 'release Generic_nativeCall']*/
    if (PyErr_Occurred ()) {
        Py_FatalError (LIT("can't initialize module dbm"));
    }
}


externC GLUEEXPORT void
initdbm ()
{
    initdbmInternal ("dbm");
}
