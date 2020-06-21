/*
===========================================
   responsible:    DanielD
   description:    Interface to the MaxDB Loader
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

#if defined (HPUX) || defined (AIX) || defined (SUN) || defined (SOLARIS) || defined (OSF1) || defined (LINUX) || defined (FREEBSD)
#define BAD_STATIC_FORWARD
#endif

#include "Python.h"/* no check */

#if PY_MAJOR_VERSION >= 2 && PY_MINOR_VERSION >= 4
#else
#if !defined (WIN32)
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
    "    return \"loader.CommunicationError: [%d] %s\" % (self.errorCode, self.message)\n"
    "\n";
static PyObject *LoaderErrorType;
static const char * LoaderErrorCodeC = 
    "\n"
    "def __str__ (self):\n"
    "    if self.sqlCode:\n"
    "        return 'loader.LoaderError: [%d] SQL [%d] %s' % (\n"
    "            self.errorCode, self.sqlCode, self.sqlMessage)\n"
    "    else:\n"
    "        return 'loader.LoaderError: [%d] %s' % (self.errorCode, self.message)\n"
    "\n";
/* snippet Python_precode */
#define CN72 1
#include "hsp100.h"
#include "hcn14.h"
#if defined (__cplusplus)
#include "heo02.h"      /* &variant +a */
#else
#include "heo102.h"     /* &variant +c */
#endif
#include "RunTime/RTE_UniqueId.h"
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

static const char * SQLErrorTagC = "SQL error ";

typedef struct ReplyInfoT {
    tsp00_Int4        errCode;
    const _TCHAR  * errText;
    tsp00_Int4        errLen;
    tsp00_Int4        sqlCode;
    const _TCHAR  * sqlErrText;
    tsp00_Int4        sqlErrLen;
} ReplyInfoT;

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

static void
raiseLoaderError (
    ReplyInfoT * replyInfo,
    SL_ValueT  * cmd)
{
    ROUTINE_DBG_MSP00 ("raiseLoaderError");
    PyObject * exception = PyInstance_New (LoaderErrorType, NULL, NULL);
    PyObject * errorCode;
    PyObject * message;
    PyObject * sqlCode;
    PyObject * sqlMessage;

    errorCode = PyInt_FromLong  (replyInfo->errCode);
    if ((replyInfo->errLen > 0) && (replyInfo->errText [replyInfo->errLen - 1] == '\n')) {
        --replyInfo->errLen;
    }
    message = PyString_FromStringAndSize (replyInfo->errText, replyInfo->errLen);
    sqlCode = PyInt_FromLong  (replyInfo->sqlCode);
    sqlMessage = PyString_FromStringAndSize (replyInfo->sqlErrText, replyInfo->sqlErrLen);
    if (exception != NULL) {
        PyObject_SetAttrString (exception, LIT ("errorCode"), errorCode) ;
        PyObject_SetAttrString (exception, LIT ("message"), message);
        PyObject_SetAttrString (exception, LIT ("sqlCode"), sqlCode);
        PyObject_SetAttrString (exception, LIT ("sqlMessage"), sqlMessage);
        PyObject_SetAttrString (exception, LIT ("cmd"), cmd);
    }
    else {
        exception = Py_BuildValue (LIT ("NNNNN"), errorCode, message,
            sqlCode, sqlMessage, cmd);
    }
    PyErr_SetObject (LoaderErrorType, exception);
    Py_XDECREF (errorCode);
    Py_XDECREF (message);
    Py_XDECREF (sqlCode);
    Py_XDECREF (sqlMessage);
    Py_XDECREF (exception);
}

/*----------------------------------------*/

static int
errorOccured (int rc, tsp00_ErrTextc msg, char * croakBuf)
{
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
commErrOccured (int rc, tsp00_ErrTextc msg, char * croakBuf)
{
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
loaderErrOccured (
    void * nself,
    int rc,
    tsp00_ErrTextc msg,
    SL_ValueT * cmd,
    char * croakBuf,
    int raiseSqlErrors)
{
    ReplyInfoT replyInfo;
    int errOccurred = 0;

    if (rc == DBMAPI_COMMERR_CN14) {
        raiseCommunicationError (1, msg);
        return 1;
    }
    else if (rc == DBMAPI_OK_CN14) {
        return 0;
    }
    cn14analyzeRpmAnswer (nself,
        &replyInfo.errCode, &replyInfo.errText, &replyInfo.errLen,
        &replyInfo.sqlCode, &replyInfo.sqlErrText, &replyInfo.sqlErrLen);
    if ((replyInfo.sqlCode != 0) && !raiseSqlErrors) {
        /* ignore */
    }
    else if ((replyInfo.errCode != 0) || (replyInfo.sqlCode != 0)) {
        errOccurred = 1;
        raiseLoaderError (&replyInfo, cmd);
    }
    return errOccurred;
}

/*----------------------------------------*/

typedef struct CommErrorT {
    int             code;
    tsp00_ErrTextc  msg;
} CommErrorT;

/*----------------------------------------*/

typedef struct LoaderErrorT {
    int             code;
    int             sqlCode;
    tsp00_ErrTextc  msg;
} LoaderErrorT;

#define croakBuf NULL

/* endsnippet Python_precode */
typedef struct LoaderObjectT {
    PyObject_HEAD
    void *nself;
/* no code for key Loader Python_/Generic_ in ['Generic_precode']*/
} LoaderObjectT;

staticforward PyTypeObject LoaderType;
#define isLoaderObject(op) ((op)->ob_type == &LoaderType)


static LoaderObjectT*
newLoader ()
{
    LoaderObjectT* result;

    result = PyObject_NEW (LoaderObjectT, &LoaderType);
    if (result != NULL) {
        memset (&result->nself, '\0', sizeof (LoaderObjectT) - offsetof (LoaderObjectT, nself));
    }
    return result;
}

static void
freeLoader (
    LoaderObjectT * value)
{
    Py_DECREF (value);
}
/* snippet Loader Generic_precode */

/*----------------------------------------*/

static void c2p (
    void * instanceData,
    int    instanceSize,
    const char * str)
{
    int len = strlen (str);

    if (len > instanceSize)
        len = instanceSize;
    memcpy (instanceData, str, len);
    if (len < instanceSize)
        memset ((char *) instanceData + len, ' ', instanceSize - len);
}

/*----------------------------------------*/

static boolean
initializeRTE ()
{
    ROUTINE_DBG_MSP00 ("initializeRTE");
    tsp00_CompName compName;

    c2p (compName, sizeof (compName), "R SDB Scripting");
    sqlinit (compName, NULL);
    RTE_InitIdGenerator();
    return true;
}

/*----------------------------------------*/

static char * buildInfo ()
{
    static tsp00_Versionc versionString;
    static bool isInitialized;
#if defined (PYTHON_GLUE)
    tsp100_CompName compName = "loaderpy ";
#elif defined (PERL_GLUE)
    tsp100_CompName compName = "loaderprl";
#else
    #error must be either Python or Perl
#endif

    if (!isInitialized) {
        sp100_GetVersionString (compName, s100buildnumber, versionString);
    }
    return versionString;
}

/*----------------------------------------*/

void
createGUID (
    RTE_UniqueIdBuffer * buf)
{
    RTE_UniqueId guid;
    RTE_UniqueIdBuffer withDash;

    (*buf) [0] = '\0';
    if (RTE_FillUniqueId(&guid)) {
        const char * source = (const char *) withDash;
        char * target = (char *) buf;
        RTE_DumpUniqueId (&guid, withDash);
        for (; source [0]; ++source) {
            if (source [0] != '-') {
                target [0] = source [0];
                ++target;
            }
        }
        target [0] = '\0';
    }
}

/*----------------------------------------*/

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

/*----------------------------------------*/

static void
localRelease (
    void* nself)
{
    if (nself != NULL) {
        cn14release (&nself);
    }
}


/*----------------------------------------*/

#if defined (PYTHON_GLUE)
static void
destructor_Loader (
    PyObject * pyself)
{
    LoaderObjectT* self = REINTERPRET_CAST (LoaderObjectT*, pyself);

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
    ReplyInfoT replyInfo;

    Glue_BEGIN_ALLOW_THREADS
    rc = cn14ExecuteLoaderCmd (nself, (const char *) cmd.utf8, cmd.utf8Len, NULL, NULL, errtext);
    Glue_END_ALLOW_THREADS
    if (rc == DBMAPI_OK_CN14) {
        rc = cn14analyzeRpmAnswer (nself,
            &replyInfo.errCode, &replyInfo.errText, &replyInfo.errLen,
            &replyInfo.sqlCode, &replyInfo.sqlErrText, &replyInfo.sqlErrLen);
        if (replyInfo.errCode == 0) {
            output->utf8 = (char *) replyInfo.errText;
            output->utf8Len = replyInfo.errLen;
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
    rc = cn14ExecuteLoaderCmd (nself, cmd.utf8, cmd.utf8Len, NULL, NULL, errtext);
    Glue_END_ALLOW_THREADS
    if (rc == DBMAPI_OK_CN14) {
        output->utf8Len = cn14bytesAvailable (nself);
        output->utf8 = (char *) cn14rawReadData (nself, &rc);
        if (output->utf8 != NULL) {
            trimPayload (output->utf8, &output->utf8Len);
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

static int sql (
   void * nself,
   StringT cmd,
   tsp00_ErrTextc VAR_ARRAY_REF errtext)
{
    int result = 0;
    ReplyInfoT replyInfo;

    Glue_BEGIN_ALLOW_THREADS
    result = cn14ExecuteLoaderCmd (nself, cmd.utf8, cmd.utf8Len, NULL, NULL, errtext);
    Glue_END_ALLOW_THREADS
    if (result == DBMAPI_OK_CN14) {
        cn14analyzeRpmAnswer (nself,
            &replyInfo.errCode, &replyInfo.errText, &replyInfo.errLen,
            &replyInfo.sqlCode, &replyInfo.sqlErrText, &replyInfo.sqlErrLen);
        result = replyInfo.sqlCode;
    }
    return result;
}

/*----------------------------------------*/

static int doConnect (
   const char * servernode,
   const char * dbname,
   const char * instroot,
   void      ** sessionOut,
   tsp00_ErrTextc VAR_ARRAY_REF errtext)
{
    int rc;

    Glue_BEGIN_ALLOW_THREADS
    rc = cn14connectRPM (
        REFCAST_MEO00 (tsp00_NodeIdc) servernode,
        REFCAST_MEO00 (tsp00_DbNamec) dbname,
        REFCAST_MEO00 (tsp00_VFilenamec) instroot,
        NULL, sessionOut, errtext);
    Glue_END_ALLOW_THREADS
    return rc;
}


/* endsnippet Loader Generic_precode */
/*----------------------------------------*/

static PyObject *
cmd_Loader (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("cmd_Loader");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    LoaderObjectT* self = REINTERPRET_CAST (LoaderObjectT*, pyself);
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
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("O:Loader.cmd"), (char **) kwlist, &cmdObj)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!string2C (self, cmdObj, &cmd)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = cmdAndRead (nself, cmd, &output, msg);
    if (loaderErrOccured (nself, result, msg, cmd.realObject, croakBuf, 1)) {
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
rawCmd_Loader (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("rawCmd_Loader");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    LoaderObjectT* self = REINTERPRET_CAST (LoaderObjectT*, pyself);
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
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("O:Loader.rawCmd"), (char **) kwlist, &cmdObj)) {
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
sql_Loader (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("sql_Loader");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    LoaderObjectT* self = REINTERPRET_CAST (LoaderObjectT*, pyself);
    void* nself = self->nself;
    static const char * kwlist [] = {
        LIT("cmd"), NULL};
    StringT cmd;
    PyObject * cmdObj;
    tsp00_ErrTextc msg;
    int result;

    memset (&cmd, '\0', sizeof (StringT));
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("O:Loader.sql"), (char **) kwlist, &cmdObj)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!string2C (self, cmdObj, &cmd)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = sql (nself, cmd, msg);
    if (loaderErrOccured (nself, result, msg, cmd.realObject, croakBuf, 0)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
  cleanup_label:
    stringDestructor (self, cmd);
    if (!ok) {
        return NULL;
    }
    return Py_BuildValue (LIT("i"), result);
}

/*----------------------------------------*/

static PyObject *
cancelCmd_Loader (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("cancelCmd_Loader");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    LoaderObjectT* self = REINTERPRET_CAST (LoaderObjectT*, pyself);
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
release_Loader (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("release_Loader");  /* generated by Glue */
/* no code for key release Python_/Generic_ in ['Generic_nativeCall']*/
    int ok = 1;
    LoaderObjectT* self = REINTERPRET_CAST (LoaderObjectT*, pyself);
    void* nself = self->nself;
/* snippet release Generic_nativeCall */
    localRelease (self->nself);
    self->nself = NULL;
    /* endsnippet release Generic_nativeCall */
    /* outargs */
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef LoaderClassMethods [] = {
    {LIT("cmd"), (PyCFunction)cmd_Loader, METH_VARARGS|METH_KEYWORDS, 
        "exeute a command and return resulting buffer as string"},
    {LIT("rawCmd"), (PyCFunction)rawCmd_Loader, METH_VARARGS|METH_KEYWORDS, 
        "exeute a command and return resulting buffer as string\n\n            This doesn't scan the buffer for error information."},
    {LIT("sql"), (PyCFunction)sql_Loader, METH_VARARGS|METH_KEYWORDS, 
        "exeute a sql command and return the error code\n\n                This method doesn't raise an exception unless an error\n                unreleated to the SQL command occurs.\n                "},
    {LIT("cancelCmd"), (PyCFunction)cancelCmd_Loader, METH_VARARGS|METH_KEYWORDS, 
        "cancels a pending request"},
    {LIT("release"), (PyCFunction)release_Loader, METH_VARARGS|METH_KEYWORDS, 
        "closes the connection"},
    {NULL, NULL}
};


static PyObject*
LoaderGetattr (PyObject* self, char* name)
{
    return Py_FindMethod (LoaderClassMethods, self, name);
}

statichere PyTypeObject LoaderType = {
    PyObject_HEAD_INIT (NULL)
    0,
    LIT("Loader"),    /* tp_name */
    sizeof (LoaderObjectT),    /* tp_basicsize */
    0,    /* tp_itemsize */
    REINTERPRET_CAST (destructor, destructor_Loader),    /* tp_dealloc */
    0,    /* tp_print */
    REINTERPRET_CAST (getattrfunc, LoaderGetattr),    /* tp_getattr */
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
Loader_loader (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("Loader_loader");  /* generated by Glue */
/* no code for key constructor Python_/Generic_ in ['Generic_nativeCall']*/
    int ok = 1;
    LoaderObjectT * newObj = NULL;
    static const char * kwlist [] = {
        LIT("servernode"), LIT("dbname"), LIT("instroot"), NULL};
    char * servernode = LIT( "");
    char * dbname = LIT( "");
    char * instroot = LIT( "");
    void * session;
    tsp00_ErrTextc msg;
    tsp00_Int4 result;

    newObj = newLoader ();
    if (newObj == NULL) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("|sss:loader.Loader"), (char **) kwlist, &servernode, &dbname, &instroot)) {
        ok = 0;
        goto cleanup_label;
    }
/* snippet constructor Generic_nativeCall */
    result = doConnect (servernode, dbname, instroot, &session, msg);
    newObj->nself = session;
    if (commErrOccured (result, msg, croakBuf)) {
    ok = 0;
    goto cleanup_label;
    }
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
_buildInfo_loader (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("_buildInfo_loader");  /* generated by Glue */
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
_createGUID_loader (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("_createGUID_loader");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    static const char * kwlist [] = {
        NULL};
    RTE_UniqueIdBuffer buf;

/* no code for key Python_/Generic_nativeCall */
    createGUID (&buf);
    /* outargs */
    return Py_BuildValue (LIT("s"), buf);
}

static PyMethodDef loaderModuleMethods [] = {
    {LIT("Loader"), (PyCFunction)Loader_loader, METH_VARARGS|METH_KEYWORDS, 
        "open a connection\n\n                An empty servernode will search for a loader server on\n                the local machine."},
    {LIT("_buildInfo"), (PyCFunction)_buildInfo_loader, METH_VARARGS|METH_KEYWORDS, 
        "returns a string containing the release and build number"},
    {LIT("_createGUID"), (PyCFunction)_createGUID_loader, METH_VARARGS|METH_KEYWORDS, 
        "returns a string containing a GUID in hex"},
    {NULL, NULL}
};

externC GLUEEXPORT void
initloaderInternal (const char * moduleName)
{
    ROUTINE_DBG_MSP00 ("initloader");  /* generated by Glue */
    PyObject* module;
    PyObject* dict;

    module = Py_InitModule4 ((char *) moduleName, loaderModuleMethods, "Interface to the MaxDB Loader", NULL, PYTHON_API_VERSION);
    if (module == NULL) {
        return;
    }
    dict = PyModule_GetDict (module);
    CommunicationErrorType = createExceptionKind ("loader.CommunicationError", CommunicationErrorCodeC);
    PyDict_SetItemString (dict, LIT("CommunicationError"), CommunicationErrorType);
    LoaderErrorType = createExceptionKind ("loader.LoaderError", LoaderErrorCodeC);
    PyDict_SetItemString (dict, LIT("LoaderError"), LoaderErrorType);
    LoaderType.ob_type = &PyType_Type;
    /* snippet Generic_initroutine */
    initializeRTE ();
    /* endsnippet Generic_initroutine */
    if (PyErr_Occurred ()) {
        Py_FatalError (LIT("can't initialize module loader"));
    }
}


externC GLUEEXPORT void
initloader ()
{
    initloaderInternal ("loader");
}
