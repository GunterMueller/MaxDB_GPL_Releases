/*
===========================================
   responsible:    DanielD
   description:    Interface to MaxDB
   target:         Python
   last changed:   07.06.2005

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end
===========================================*/


#include <stddef.h>

#if defined (HPUX) || defined (AIX) || defined (SUN) || defined (SOLARIS) || defined (OSF1) || defined (LINUX) || defined (FREEBSD)
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
    "    return \"sql.CommunicationError: [%d] %s\" % (self.errorCode, self.message)\n"
    "\n";
static PyObject *SQLErrorType;
static const char * SQLErrorCodeC =
    "\n"
    "def __str__ (self):\n"
    "    result = \"sql.SQLError: [%d] (at %d) %s\" % (self.errorCode, self.errorPos, self.message)\n"
    "    return result\n"
    "\n";
/* snippet Generic_precode */
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#if defined (PERL_GLUE)
    /* conflict with gsp00 enum */
    #undef na
    /* conflict with malloc.h realloc included later */
    #undef realloc
#endif
#if defined (__cplusplus)
#include "heo02.h"
#else
#include "heo102.h"
#endif
#include "hsp02.h"
#include "hsp30.h"
#include "hsp40.h"
#include "hsp41.h"
#include "hsp42.h"
#include "hsp77.h"
#include "hsp78_0.h"
#include "hsp100.h"
#include "Scripting/Script_SQLPacket.h"

#if defined (PERL_GLUE)
    #include "patchlevel.h"/* no check */
    #if PATCHLEVEL >= 5
        #define na PL_na
    #else
        #define na Perl_na
    #endif
    #undef read
#endif

#if defined (PYTHON_GLUE)
typedef PyObject SL_ValueT;
#elif defined (PERL_GLUE)
typedef SV SL_ValueT;
#else
    #error Invalid Glue Environment
#endif

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

/*
 * tracing
 */

#define tracelog stdout

static void
logformat (
    const char * format,
    ...)
{
    va_list args;

    va_start (args, format);
    vfprintf (tracelog, format, args);
    va_end (args);
}

static void
loghere (
    const char * msg,
    int line)
{
    logformat ("%s @ %d\n", msg, line);
}

static void hexdump (
    const void * buf,
    unsigned int startoffset,
    unsigned int length)
{
    const unsigned char * current = ((const unsigned char *) buf) + startoffset;
    const unsigned char * eoDump = current + length;
    unsigned int lineoffset = startoffset;
    char line [120];
    static const int chunkSizeC = 16;

    while (current < eoDump) {
        char * writePtr = line;
        int i;

        writePtr += sprintf (line, "%04x ", lineoffset);
        for (i = 0; i < chunkSizeC; ++i) {
            writePtr += sprintf (writePtr, "%02x ", current [i]);
        }
        writePtr += sprintf (writePtr, "  |");
        for (i = 0; i < chunkSizeC; ++i) {
            int asChar = current [i];
            if (asChar < 32) {
                asChar = '.';
            }
            writePtr += sprintf (writePtr, "%c", asChar);
        }
        fprintf (tracelog, "%s|\n", line);
        current += chunkSizeC;
        lineoffset += chunkSizeC;
    }
}

#if 0
#define LOGHERE(msg)   loghere (msg, __LINE__)
#define LOGF            logformat
#define HEXDUMP(buf, startoffs, length) hexdump (buf, startoffs, length)
#else
#define LOGHERE(msg)
#define LOGF
#define HEXDUMP(buf, startoffs, length)
#endif

static void * tracingMalloc (size_t len)
{
    void * result = malloc (len);
    printf("malloc: 0x%p\n", result);
    return result;
}

static void tracingFree (void * ptr)
{
    printf("free: 0x%p\n", ptr);
    free (ptr);
}

/*
#define malloc(len) tracingMalloc(len)
#define free(ptr)   tracingFree(ptr)
*/

#if defined (__cplusplus) && (COMPILEMODE_MSP00 >= SLOW_MSP00)
    /*!
      Class: ScriptingTracer
     */
    class ScriptingTracer
    {
    public:
        ScriptingTracer (const char * name);
        ~ScriptingTracer ();
        void trace (int layer, const char *format, ...);
        static void switchTrace (bool on);
        static void initTrace ();
    private:
        const char * procName;
        static bool traceIsOn;
    };

    ScriptingTracer::ScriptingTracer (const char * name)
    {
        if (traceIsOn) {
            printf("<%s>\n", name);
        }
        this->procName = name;
    }

    ScriptingTracer::~ScriptingTracer ()
    {
        if (traceIsOn) {
            printf("</%s>\n", this->procName);
        }
    }

    void ScriptingTracer::trace (int layer, const char *format, ...)
    {
        if (traceIsOn) {
            va_list         ap;

            va_start (ap, format);
            vprintf(format, ap);
            va_end (ap);
        }
    }

    bool ScriptingTracer::traceIsOn = false;

    void ScriptingTracer::switchTrace (bool on)
    {
        traceIsOn = on;
    }

    void ScriptingTracer::initTrace ()
    {
        const char * sapdbTrace = getenv ("SAPDBTRACE");
        if (sapdbTrace == NULL) {
            traceIsOn = false;
        }
        else {
            traceIsOn = true;
        }
    }
    /*! EndClass: ScriptingTracer */

    #undef TRACER_CLASS_MSP00
    #define TRACER_CLASS_MSP00 ScriptingTracer

#endif

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
#endif

char * buildInfo ()
{
    static tsp00_Versionc versionString;
    static bool isInitialized;
#if defined (PYTHON_GLUE)
    tsp100_CompName compName = "sqlpy    ";
#elif defined (PERL_GLUE)
    tsp100_CompName compName = "sqlpy    ";
#else
    #error must be either Python or Perl
#endif

    if (!isInitialized) {
        sp100_GetVersionString (compName, s100buildnumber, versionString);
    }
    return versionString;
}


static const int internalErrorC = -16506;
static const int conversionErrorC = -817;

typedef unsigned char byte_t;

typedef struct ParseInfoT {
    Script_SqlParseid     pid;
    int               paramCount;
    int               inputParamCount;
    int               outputParamCount;
    int               inLongCount;
    int               outLongCount;
    tsp1_param_info * paramInfo;
    char            * sqlString;
} ParseInfoT;

typedef struct LongReaderT LongReaderT;

static void
SL_getRawString (
    PyObject* value,
    const char ** chars,
    int * len);

static SL_ValueT *
SL_fromString (
    const char * value,
    int len);

static SL_ValueT *
SL_fromString (
    const char * value,
    int len);

static SL_ValueT *
SL_fromUCS2 (
    const void * value,
    int charLen);

static int
SL_readCall (
    SL_ValueT * value,
    const tsp77encoding * targetEncoding,
    byte_t * streamBuf,
    int requestedLen);

static SL_ValueT *
readAsciiLong (
    LongReaderT * nself,
    int requested);

static SL_ValueT *
readUCS2Long (
    LongReaderT * nself,
    int requested);

static SL_ValueT *
readLong (
    LongReaderT * nself,
    int requested);

static bool
fillParamInfos (
    Script_SessionT   * session,
    ParseInfoT * parseInfo,
    bool initial);

static void
increaseRef (
    void * obj);

static void
decreaseRef (
    void * obj);

static void
setAutocommit (
    Script_SessionT   * session,
    bool autocommit);

static bool
getAutocommit (
    Script_SessionT   * session);

static bool
lastWasEOT (
    Script_SessionT   * session);

static void
enableCursorNames (
    Script_SessionT   * session,
    bool enable);

static void
enableGenericResults (
    Script_SessionT   * session,
    bool enable);

static bool
isGenericResultsEnabled (
    Script_SessionT   * session);

static bool
isProcServerSession (
    Script_SessionT   * session);

static int
nextCursorCount (
    Script_SessionT   * session);

static void
addCursorName (
    Script_SessionT   * session);

static void
clearEOTFlag (
    Script_SessionT   * session);

static void
setEOTFlag (
    Script_SessionT   * session);



/*----------------------------------------*/

static void
clearParseInfo (
    Script_SessionT   * session,
    ParseInfoT * parseInfo)
{
    ROUTINE_DBG_MSP00 ("clearParseInfo");
    if ((session != NULL) && session->is_connected) {
        if (parseInfo->paramCount != -1) {
            i28droppid (session, parseInfo->pid);
        }
    }
    if (parseInfo->paramInfo != NULL) {
        free (parseInfo->paramInfo);
        parseInfo->paramInfo = NULL;
    }
    parseInfo->paramCount = -1;
    if (parseInfo->sqlString != NULL) {
        free (parseInfo->sqlString);
        parseInfo->sqlString = NULL;
    }
}

/*----------------------------------------*/

typedef struct PreparedT {
    Script_SessionT   * session;
    ParseInfoT        parseInfo;
} PreparedT;

typedef enum fetchKindsT {
    nextFetchE, relativeFetchE, absoluteFetchE,
    firstFetchE, lastFetchE,
    fetchKindsE
} fetchKindsT;

static const char * fetchCommandsC [] = {
    " next ", " relative ? ", " pos (?) ", " first ", " last "
};

typedef struct ResultSetT {
    Script_SessionT   * session;
    tsp00_KnlIdentifierc cursorName;
    ParseInfoT        fetchInfo [fetchKindsE];
    byte_t            * dataBuf;
    int               dataSize;
    int               dataLen;
    byte_t            * currentRow;
    byte_t            * eoRecords;
    int               rowSize;
    int               maxRecordsPerFetch;
    const char      * colNames;
    tsp1_param_info * colInfo;
    int               colCount;
    int               currentPos;
} ResultSetT;

/*----------------------------------------*/

static int dbgSizes [] = {36, 256, 1111};

static void
checkMem ()
{
    ROUTINE_DBG_MSP00 ("checkMem");
    int i;
    for (i = 0; i < 3; ++i) {
        void * p  = malloc (dbgSizes [i]);
        free (p);
    }
}

/*----------------------------------------*/

static const int atEndStreamC = -2;
static const int errorStreamC = -1;

struct InputStreamVMT;

typedef struct InputStreamVMT* InputStreamT;

typedef int readFunctionT (InputStreamT * instream, byte_t * buf, int requestedLen);

typedef struct InputStreamVMT {
    readFunctionT * read;
} InputStreamVMT;

/*----------------------------------------*/

typedef struct StringInputStreamT {
    const InputStreamVMT * vmt;
    const tsp77encoding * sourceEncoding;
    const tsp77encoding * targetEncoding;
    const char * data;
    const char * eoData;
    const char * pos;
} StringInputStreamT;

/*----------------------------------------*/

static int
stringInputRead (
    StringInputStreamT * self,
    byte_t * buf,
    int requestedLen)
{
    ROUTINE_DBG_MSP00 ("stringInputRead");
    int remaining = self->eoData - self->pos;
    tsp78ConversionResult convResult;
    tsp00_Uint4 bytesWritten;
    tsp00_Uint4 bytesRead;
    int result;

    if (requestedLen <= 0) {
        return errorStreamC;
    }
    if (remaining <= 0) {
        return atEndStreamC;
    }
    convResult = sp78convertString (
        self->targetEncoding, buf, requestedLen, &bytesWritten, false,
        self->sourceEncoding, self->pos, remaining, &bytesRead);
    switch (convResult) {
        case sp78_Ok:
        case sp78_SourceExhausted:
        case sp78_TargetExhausted:
            self->pos += bytesRead;
            result = bytesWritten;
            break;
        default:
            result = errorStreamC;
    }
    return result;
}

/*----------------------------------------*/

static const InputStreamVMT StringInputStreamVMT_Imp = {
    (readFunctionT*) stringInputRead,
};

/*----------------------------------------*/

static InputStreamT *
initStringInputStream (
    StringInputStreamT * self,
    const tsp77encoding * sourceEncoding,
    const tsp77encoding * targetEncoding,
    const char * data,
    int len)
{
    ROUTINE_DBG_MSP00 ("initStringInputStream");
    self->vmt = &StringInputStreamVMT_Imp;
    self->sourceEncoding = sourceEncoding;
    self->targetEncoding = targetEncoding;
    self->data = data;
    self->pos = data;
    self->eoData = data + len;
    return(InputStreamT*) self;
}

typedef struct CallableInputStreamT {
    const InputStreamVMT * vmt;
    const tsp77encoding * targetEncoding;
    SL_ValueT * callable;
    bool atEnd;
} CallableInputStreamT;

/*----------------------------------------*/

static int
callableInputRead (
    CallableInputStreamT * self,
    byte_t * buf,
    int requestedLen)
{
    ROUTINE_DBG_MSP00 ("callableInputRead");
    if (self->atEnd) {
        return atEndStreamC;
    }
    return SL_readCall (self->callable, self->targetEncoding, buf, requestedLen);
}

/*----------------------------------------*/

static const InputStreamVMT CallableInputStreamVMT_Imp = {
    (readFunctionT*) callableInputRead,
};

/*----------------------------------------*/

static InputStreamT *
initCallableInputStream (
    CallableInputStreamT * self,
    SL_ValueT * callable,
    const tsp77encoding * targetEncoding)
{
    ROUTINE_DBG_MSP00 ("initCallableInputStream");
    self->vmt = &CallableInputStreamVMT_Imp;
    self->callable = callable;
    self->targetEncoding = targetEncoding;
    self->atEnd = false;
    return(InputStreamT*) self;
}

/*----------------------------------------*/

struct LongReaderT {
    Script_SessionT    * session;
    tsp00_LongDescriptor descriptor;
    SL_ValueT          * (*read) (LongReaderT * nself, int requested);
    byte_t             * dataBuf;
    int                  dataSize;
    int                  dataPos;
    int                  dataLen;
    long                 bytesRemaining;
    bool                 lastGetvalFound;
};

/*----------------------------------------*/

bool keywordMatch (
    const char * kwArg,
    int kwArgLen,
    const char * token,
    int tokenLen)
{
    ROUTINE_DBG_MSP00 ("keywordMatch");

    if (kwArgLen != tokenLen) {
        return false;
    }
    if (memcmp (kwArg, token, kwArgLen) == 0) {
        return true;
    }
    else {
        return false;
    }
}

/*----------------------------------------*/

void setStringcConfig (
    char * configField,
    int fieldLen,
    const char * value,
    int valueLen)
{
    ROUTINE_DBG_MSP00 ("setStringcConfig");

    int maxfield = fieldLen - 1;
    if (valueLen == -1) {
        valueLen = strlen (value);
    }
    if (maxfield < valueLen) {
        valueLen = maxfield;
    }
    memcpy (configField, value, valueLen);
    configField [valueLen] = '\0';
}

/*----------------------------------------*/

void setStringConfig (
    char * configField,
    int fieldLen,
    const char * value,
    int valueLen)
{
    ROUTINE_DBG_MSP00 ("setStringConfig");

    if (fieldLen <= valueLen) {
        memcpy (configField, value, fieldLen);
    }
    else {
        memcpy (configField, value, valueLen);
        memset (configField + valueLen, ' ', fieldLen - valueLen);
    }
}

/*----------------------------------------*/

int parseInt (
    const char * token,
    int tokenLen)
{
    ROUTINE_DBG_MSP00 ("parseInt");
    int result = 0;
    int i;

    for (i = 0; i < tokenLen; ++i) {
        result *= 10;
        result += token [i] - '0';
    }
    return result;
}

/*----------------------------------------*/

bool parseBool (
    const char * token,
    int tokenLen,
    bool defaultValue)
{
    ROUTINE_DBG_MSP00 ("parseBool");
    bool result = defaultValue;

    if (memcmp (token, "true", tokenLen) == 0) {
        result = true;
    }
    else if (memcmp (token, "on", tokenLen) == 0) {
        result = true;
    }
    else if (memcmp (token, "1", tokenLen) == 0) {
        result = true;
    }
    else if (memcmp (token, "false", tokenLen) == 0) {
        result = false;
    }
    else if (memcmp (token, "off", tokenLen) == 0) {
        result = false;
    }
    else if (memcmp (token, "0", tokenLen) == 0) {
        result = false;
    }
    return result;
}

/*----------------------------------------*/

static void
handleConfigArgument (
    Script_SessionT   * session,
    const char * token,
    int tokenLen,
    const char * value,
    int valueLen)
{
    ROUTINE_DBG_MSP00 ("handleConfigArgument");

    if (keywordMatch ("version", 7, token, tokenLen)) {
        setStringConfig ((char *)session->senderid, sizeof (tsp00_C5),
            value, valueLen);
    }
    else if (keywordMatch ("component", 9, token, tokenLen)) {
        setStringConfig ((char *) session->senderid + sizeof (tsp00_C5), sizeof (tsp00_C3),
            value, valueLen);
    }
    else if (keywordMatch ("sqlmode", 7, token, tokenLen)) {
        setStringConfig ((char*)session->connectInfo.sqlmode, sizeof (session->connectInfo.sqlmode),
            value, valueLen);
    }
    else if (keywordMatch ("cachelimit", 10, token, tokenLen)) {
        session->connectInfo.cachelimit = parseInt (value, valueLen);
    }
    else if (keywordMatch ("timeout", 7, token, tokenLen)) {
        session->connectInfo.timeout = parseInt (value, valueLen);
    }
    else if (keywordMatch ("isolation", 9, token, tokenLen)) {
        session->connectInfo.isolation = parseInt (value, valueLen);
    }
    else if (keywordMatch ("autocommit", 10, token, tokenLen)) {
        setAutocommit (session, parseBool (value, valueLen, false));
    }
    else if (keywordMatch ("cursornames", 11, token, tokenLen)) {
        enableCursorNames (session, parseBool (value, valueLen, true));
    }
    else if (keywordMatch ("genericResults", 14, token, tokenLen)) {
        enableGenericResults (session, parseBool (value, valueLen, true));
    }
    else if (keywordMatch ("spaceOption", 11, token, tokenLen)) {
        session->space_option = parseBool (value, valueLen, true);
    }
    else if (keywordMatch ("fixedChars", 10, token, tokenLen)) {
        session->variable_input = ! parseBool (value, valueLen, true);
    }
    else if (keywordMatch ("challengeResponse", 17, token, tokenLen)) {
        session->try_challenge_response = parseBool (value, valueLen, true);
    }
}

/*----------------------------------------*/

static void
parseSessionConfig (
    Script_SessionT   * session,
    const char * configString)
{
    ROUTINE_DBG_MSP00 ("parseSessionConfig");
    const char * argStart;
    const char * nextConfig;
    bool atEnd = (configString [0] == '\0');
    int tokenLen;
    int argLen;

    while (!atEnd) {
        argStart = strchr (configString, '=');
        if (argStart == NULL) {
            return;
        }
        tokenLen = argStart - configString;
        ++argStart;
        nextConfig = strchr (argStart, '&');
        if (nextConfig == NULL) {
            atEnd = true;
            nextConfig = argStart + strlen (argStart);
        }
        argLen = nextConfig - argStart;
        handleConfigArgument (session, configString, tokenLen, argStart, argLen);
        configString = nextConfig + 1;
    }

}

/*----------------------------------------*/

static void
addNull (
    Script_SessionT   * session,
    int inoutlen,
    int bufpos)
{
    if (session->variable_input) {
        i28addNullField (session);
    }
    else {
        i28pnull (session, inoutlen, bufpos);
    }
}

/*----------------------------------------*/

static void
addValue (
    Script_SessionT   * session,
    const char /* byte */ * data,
    int datalen,
    int inoutlen,
    int bufpos,
    char fillchar)
{
    if (session->variable_input) {
        i28addDataField (session, data, datalen);
    }
    else {
        i28parg (session, data, datalen, inoutlen, bufpos, fillchar);
    }
}

/* endsnippet Generic_precode */
/* snippet Python_precode */

static void
dumpObjectInfo (
    void * obj,
    const char * comment)
{
    ROUTINE_DBG_MSP00 ("dumpObjectInfo");
    PyObject * pyObj = REINTERPRET_CAST (PyObject*, obj);
    const char * rawdata;
    int          rawlen;
    SL_getRawString (pyObj, &rawdata, &rawlen);
    printf("%s0x%0p (%s, %d)\n", comment, pyObj, pyObj->ob_type->tp_name, pyObj->ob_refcnt);
    printf("    '%.*s'\n", rawlen, rawdata);
}

/*----------------------------------------*/

static void
increaseRef (
    void * obj)
{
    ROUTINE_DBG_MSP00 ("increaseRef");
    Py_INCREF (REINTERPRET_CAST (PyObject*, obj));
}

/*----------------------------------------*/

static void
decreaseRef (
    void * obj)
{
    ROUTINE_DBG_MSP00 ("decreaseRef");
    Py_DECREF(REINTERPRET_CAST (PyObject*, obj));
}

/*----------------------------------------*/

static bool
    SL_isString (
    PyObject* value)
{
    ROUTINE_DBG_MSP00 ("SL_isString");
    return PyString_Check (value);
}

/*----------------------------------------*/

static bool
SL_isCallable (
    PyObject* value)
{
    ROUTINE_DBG_MSP00 ("SL_isCallable");
    bool result;

    if (PyCallable_Check (value)) {
        result = true;
    }
    else {
        result = false;
    }
    return result;
}

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
    PyObject* value,
    const char ** chars,
    int * len,
    const tsp77encoding ** srcEncoding)
{
#if (PY_MAJOR_VERSION >= 2)
#if ! defined (Py_UNICODE_SIZE)
#define Py_UNICODE_SIZE 2
#endif
    if (PyUnicode_Check (value)) {
        *chars = PyUnicode_AS_DATA (value);
        *len = PyUnicode_GET_DATA_SIZE (value);
        if (Py_UNICODE_SIZE == 2) {
            *srcEncoding = sp77encodingUCS2Native;
        }
        else {
            *srcEncoding = sp77encodingUCS4Native;
        }
    }
    else if (PyString_Check (value)) {
        *chars = PyString_AsString (value);
        *len = PyString_Size (value);
        *srcEncoding = sp77encodingAscii;
    }
    else {
        PyObject * str = PyObject_Str (value);
        *chars = PyString_AsString (str);
        *len = PyString_Size (str);
        *srcEncoding = sp77encodingAscii;
        Py_DECREF (str);
    }
#else
    SL_getRawString (value, chars, len);
    *srcEncoding = sp77encodingAscii;
#endif
}

/*----------------------------------------*/

static int
SL_getNumber (
    PyObject * value,
    tsp1_param_info * paramInfo,
    tsp00_Number * number)
{
    ROUTINE_DBG_MSP00 ("SL_getNumber");
    enum {
        isInt, isFloat
    };
    long intValue;
    double floatValue;
    int valueKind;
    tsp00_NumError numrc;

    if (PyInt_Check (value)) {
        intValue = PyInt_AS_LONG (value);
        valueKind = isInt;
    }
    else if (PyLong_Check (value)) {
        floatValue = PyLong_AsDouble (value);
        valueKind = isFloat;
    }
    else if (PyFloat_Check (value)) {
        floatValue = PyFloat_AsDouble (value);
        valueKind = isFloat;
    }
    else {
        PyObject * floatObject = PyNumber_Float (value);
        if (floatObject == NULL) {
            return false;
        }
        floatValue = PyFloat_AsDouble (value);
        valueKind = isFloat;
        Py_DECREF (floatObject);
    }
    if (valueKind == isInt) {
        s41plint (number, 1, csp_fixed, paramInfo->sp1i_frac, intValue, &numrc);
    }
    else {
        int frac;
        if ((paramInfo->sp1i_data_type == dfloat) || (paramInfo->sp1i_data_type == dfloat)) {
            frac = csp_float_frac;
        }
        else {
            frac = paramInfo->sp1i_frac;
        }
        s41plrel (number, 1, csp_fixed, frac, floatValue, &numrc);
    }
    return true;
}

/*----------------------------------------*/

static SL_ValueT *
SL_fromTrue ()
{
    ROUTINE_DBG_MSP00 ("SL_fromTrue");

    Py_INCREF(Py_True);
    return Py_True;
}

/*----------------------------------------*/

static SL_ValueT *
SL_fromFalse ()
{
    ROUTINE_DBG_MSP00 ("SL_fromFalse");

    Py_INCREF(Py_False);
    return Py_False;
}

/*----------------------------------------*/

static SL_ValueT *
SL_fromNone ()
{
    ROUTINE_DBG_MSP00 ("SL_fromNone");

    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static SL_ValueT *
SL_fromInt (
    long value)
{
    ROUTINE_DBG_MSP00 ("SL_fromInt");

    return PyInt_FromLong (value);
}

/*----------------------------------------*/

static SL_ValueT *
SL_fromDouble (
    double value)
{
    ROUTINE_DBG_MSP00 ("SL_fromDouble");

    return PyFloat_FromDouble (value);
}

/*----------------------------------------*/

static SL_ValueT *
SL_fromString (
    const char * value,
    int len)
{
    ROUTINE_DBG_MSP00 ("SL_fromString");

    if (len == -1) {
        len = strlen (value);
    }
    return PyString_FromStringAndSize (value, len);
}

/*----------------------------------------*/

static SL_ValueT *
SL_fromUCS2 (
    const void * value,
    int charLen)
{
    ROUTINE_DBG_MSP00 ("SL_fromUCS2");
    SL_ValueT * result;

#if (PY_MAJOR_VERSION >= 2) && (Py_UNICODE_SIZE == 2)
    result = PyUnicode_FromUnicode (value, charLen);
#else
    tsp78ConversionResult convResult;
    tsp00_Uint4 destBytesWritten;
    tsp00_Uint4 srcBytesParsed;

    /* alloc object */
#if (PY_MAJOR_VERSION >= 2) && (Py_UNICODE_SIZE == 4)
    result = PyUnicode_FromUnicode (NULL, charLen);
    if (result == NULL) {
        return result;
    }
    /* try conversion */
    convResult = sp78convertString (
        sp77encodingUCS4Native, PyUnicode_AS_DATA (result), charLen * 4, &destBytesWritten, false,
        sp77encodingUCS2Native, value, charLen * 2, &srcBytesParsed);
#else
    result = PyString_FromStringAndSize (NULL, charLen);
    if (result == NULL) {
        return result;
    }
    /* try conversion */
    convResult = sp78convertString (
        sp77encodingAscii, PyString_AsString (result), charLen, &destBytesWritten, false,
        sp77encodingUCS2Native, value, charLen * 2, &srcBytesParsed);
#endif
    /* fail: delete object */
    if (convResult != sp78_Ok) {
        Py_DECREF (result);
        result = SL_fromNone ();
    }
#endif
    return result;
}

/*----------------------------------------*/

static int
SL_readCall (
    PyObject * value,
    const tsp77encoding * targetEncoding,
    byte_t * streamBuf,
    int requestedLen)
{
    ROUTINE_DBG_MSP00 ("SL_readCall");
    int requestedChars;
    PyObject   * callResult;
    const char * resultData;
    int          resultLen;
    const tsp77encoding * sourceEncoding;
    int          readResult;

    /* argTuple = Py_BuildValue ("i", requestedLen); */
    if (targetEncoding == sp77encodingUCS2Native) {
        requestedChars = requestedLen / 2;
    }
    else {
        requestedChars = requestedLen;
    }
    callResult = PyObject_CallFunction (value, LIT("(i)"), requestedLen);
    if (callResult == NULL) {
        readResult = errorStreamC;
    }
    else {
        SL_getEncodedString (callResult, &resultData, &resultLen,
            &sourceEncoding);
        if (resultLen == 0) {
            readResult = atEndStreamC;
        }
        else if (resultLen < 0) {
            readResult = errorStreamC;
        }
        else {
            tsp78ConversionResult convResult;
            tsp00_Uint4 bytesWritten;
            tsp00_Uint4 bytesRead;

            convResult = sp78convertString (
                targetEncoding, streamBuf, requestedLen, &bytesWritten, false,
                sourceEncoding, resultData, resultLen, &bytesRead);
            switch (convResult) {
                case sp78_Ok:
                case sp78_SourceExhausted:
                case sp78_TargetExhausted:
                    readResult = bytesWritten;
                    break;
                default:
                    readResult = errorStreamC;
            }
        }
        Py_XDECREF (callResult);
    }
    return readResult;
}

/*----------------------------------------*/

static void
    SL_forget (
    PyObject * value)
{
    Py_XDECREF (value);
}

/*----------------------------------------*/

typedef struct SequenceT {
    PyObject * pySeq;
} SequenceT;

static SequenceT EmptySequenceC = {NULL};

static int
sequencePy2C (
    void * dummy,
    PyObject * pySeq,
    SequenceT * cSeq)
{
    ROUTINE_DBG_MSP00 ("sequencePy2C");
    cSeq->pySeq = pySeq;
    return 1;
}

/*----------------------------------------*/

static int
sequenceLength (
    SequenceT * cSeq)
{
    ROUTINE_DBG_MSP00 ("sequenceLength");
    int result;

    if (cSeq->pySeq == NULL) {
        result = 0;
    }
    else {
        result = PyObject_Length (cSeq->pySeq);
    }
    return result;
}

/*----------------------------------------*/

static PyObject *
getSequenceElement (
    SequenceT * cSeq,
    int index)
{
    ROUTINE_DBG_MSP00 ("getSequenceElement");
    PyObject * result;

    PyObject * key = PyInt_FromLong (index);
    result = PyObject_GetItem (cSeq->pySeq, key);
    Py_DECREF (key);
    return result;
}

/*----------------------------------------*/

const int hasResultSetC        = 1;
const int hasOutputParametersC = 2;
const int hasRowsAffectedC     = 4;
const int hasSerialC           = 8;

typedef struct SQLResultT {
    PyObject * resultObj;
    int isTuple;
    int isOK;
    int additionalValues;
    bool       genericResults;
    int        itemsSet;
    PyObject * resultSet;
    PyObject * outputParameters;
    long       rowsAffected;
    PyObject * lastSerial;
    PyObject * firstSerial;
} SQLResultT;

/*----------------------------------------*/

static int
sqlResultC2Py (
    void * dummy,
    SQLResultT result,
    PyObject ** pyResult);

/*----------------------------------------*/

static SL_ValueT *
sqlResultC2SL_Value (
    SQLResultT * result)
{
    ROUTINE_DBG_MSP00 ("sqlResultC2SL_Value");
    return result->resultObj;
}

/*----------------------------------------*/

static void
initResult (
    SQLResultT * result,
    int additional)
{
    ROUTINE_DBG_MSP00 ("initResult");
    result->resultObj = NULL;
    result->isTuple = 0;
    result->isOK = true;
    result->additionalValues = additional;
    result->genericResults = false;
    result->itemsSet = 0;
    result->resultSet = SL_fromNone ();
    result->outputParameters = SL_fromNone ();
    result->rowsAffected = 0;
    result->lastSerial = SL_fromNone ();
    result->firstSerial = SL_fromNone ();
}

/*----------------------------------------*/

static bool
allocResult (
    SQLResultT * result,
    int elements)
{
    ROUTINE_DBG_MSP00 ("allocResult");
    bool allocOK = true;

    /* ASSERT_DBG_MSP00 (result->resultObj == NULL); */
    result->resultObj = PyTuple_New (elements + result->additionalValues);
    result->isTuple = 1;
    allocOK = (result->resultObj != 0);
    return allocOK;
}

/*----------------------------------------*/

static bool
setResult (
    SQLResultT * result,
    PyObject * newElement)
{
    ROUTINE_DBG_MSP00 ("setResult");

    /* ASSERT_DBG_MSP00 (result->resultObj == NULL); */
    result->isTuple = false;
    if (newElement == NULL) {
        Py_INCREF (Py_None);
        newElement = Py_None;
    }
    result->resultObj = newElement;
    return true;
}

/*----------------------------------------*/

static bool
setResultElement (
    SQLResultT * result,
    int index,
    PyObject * newElement)
{
    ROUTINE_DBG_MSP00 ("setElement");
    bool assignOK = true;

    if (result->isTuple) {
        if (index < 0) {
            index = PyObject_Length (result->resultObj) - index;
        }
        assignOK = !PyTuple_SetItem (result->resultObj, index, newElement);
    }
    else {
        if (index == 0) {
            result->resultObj = newElement;
        }
        else {
            Py_XDECREF(newElement);
            PyErr_SetString(PyExc_IndexError,
                "tuple assignment index out of range");
            assignOK = false;
        }
    }
    return assignOK;
}

/*----------------------------------------*/

static int
sqlResultOK (
    SQLResultT * result)
{
    ROUTINE_DBG_MSP00 ("sqlResultOK");
    return(result->isOK);
}

/*----------------------------------------*/

static SL_ValueT *
getResultSet (
    SQLResultT * resultObj)
{
    SL_ValueT * result = resultObj->resultSet;
    increaseRef (result);
    return result;
}

/*----------------------------------------*/

static SL_ValueT *
getOutputParameters (
    SQLResultT * resultObj)
{
    SL_ValueT * result = resultObj->outputParameters;
    increaseRef (result);
    return result;
}

/*----------------------------------------*/

static long
getRowsAffected (
    SQLResultT * resultObj)
{
    return resultObj->rowsAffected;
}

/*----------------------------------------*/

static SL_ValueT *
mem2int (
    const char * number,
    int length)
{
    int         intVal;
    tsp00_NumError  numErr;
    SL_ValueT  * result = NULL;

    s40glint (REINTERPRET_CAST (tsp00_Number*, number),
        1, length, REFPARM (intVal), REFPARM (numErr));
    if (numErr == num_ok) {
        result  = SL_fromInt (intVal);
    }
    return result;
}


/*----------------------------------------*/

static void
setSerialEntries (
    Script_SessionT   * session,
    SQLResultT        * result)
{
    char * firstData = i28getpart (session) + 1;
    char * lastData = i28getpart (session) + 22;
    result->firstSerial = mem2int (firstData, 38);
    if (memcmp (firstData, lastData, 20) == 0) {
        result->lastSerial = result->firstSerial;
        if (result->lastSerial != NULL) {
            increaseRef (result->lastSerial);
        }
    }
    else {
        result->lastSerial = mem2int (lastData, 38);
    }
    result->itemsSet |= hasSerialC;
}

/*----------------------------------------*/

static SL_ValueT *
getLastSerial (
    SQLResultT * resultObj)
{
    SL_ValueT * result = resultObj->lastSerial;
    increaseRef (result);
    return result;
}

/*----------------------------------------*/

static SL_ValueT *
getFirstSerial (
    SQLResultT * resultObj)
{
    SL_ValueT * result = resultObj->firstSerial;
    return result;
}

/*----------------------------------------*/


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
        PyObject_SetAttrString (exception, LIT("errorCode"), pycode) ;
        PyObject_SetAttrString (exception, LIT("message"), pymsg);
    }
    else {
        exception = Py_BuildValue (LIT("NN"), pycode, pymsg);
    }
    PyErr_SetObject (CommunicationErrorType, exception);
    Py_XDECREF (pycode);
    Py_XDECREF (pymsg);
    Py_DECREF(exception);
}

/*----------------------------------------*/

static void
raiseSQLError (
    int code,
    int pos,
    tsp00_C5  sqlStateStr,
    const char * msg,
    const char * traceback,
    int          tracebackLen)
{
    ROUTINE_DBG_MSP00 ("raiseSQLError");
    PyObject * exception = PyInstance_New (SQLErrorType, NULL, NULL);
    PyObject * errorCode = NULL;
    PyObject * message = NULL;
    PyObject * errorPos = NULL;
    PyObject * sqlState = NULL;
    PyObject * udeTraceback = NULL;

    errorCode = PyInt_FromLong  (code);
    message = PyString_FromString  (msg);
    errorPos = PyInt_FromLong  (pos);
    sqlState = PyString_FromStringAndSize  ((char *)sqlStateStr, sizeof (tsp00_C5));
    if (traceback != NULL) {
        udeTraceback = PyString_FromStringAndSize (traceback, tracebackLen);
    }
    else {
        udeTraceback = SL_fromNone ();
    }
    if (exception != NULL) {
        PyObject_SetAttrString (exception, LIT("errorCode"), errorCode) ;
        PyObject_SetAttrString (exception, LIT("message"), message);
        PyObject_SetAttrString (exception, LIT("errorPos"), errorPos);
        PyObject_SetAttrString (exception, LIT("sqlState"), sqlState);
        PyObject_SetAttrString (exception, LIT("udeTraceback"), udeTraceback);
    }
    else {
        exception = Py_BuildValue (LIT("NNNNN"), errorCode, message,
            errorPos, sqlState, udeTraceback);
    }
    PyErr_SetObject (SQLErrorType, exception);
    Py_DECREF(errorCode);
    Py_DECREF(message);
    Py_DECREF(errorPos);
    Py_DECREF(sqlState);
    Py_DECREF(udeTraceback);
    Py_DECREF(exception);
}

/*----------------------------------------*/

static void
setConversionError (
    Script_SessionT * nself,
    int parameterIndex)
{
    static const char * convErrMsg = "Incompatible data types";
    Script_SqlResultT * sqlresult = &nself->lasterr.sqlresult;
    int msglen = strlen (convErrMsg);

    sqlresult->returnCode = conversionErrorC;
    sqlresult->errorPos = parameterIndex;
    memcpy (sqlresult->sqlstate, "S0817", 5);
    memcpy (sqlresult->sqlmsg, convErrMsg, msglen + 1);
    sqlresult->messagelistData = NULL;
    sqlresult->messagelistDataLen = 0;
}

/*----------------------------------------*/

static void
raiseParameterError (
    int parameterIndex,
    const char * errMsg)
{
    tsp00_C5 sqlState;
    static const char * convErrMsg = "Incompatible data types";
    memcpy (sqlState, "S0101", 5);
    raiseSQLError (-101, parameterIndex, sqlState, errMsg, NULL, -1);
}

/*----------------------------------------*/

static boolean
commErrOccured (int commErr, const char * msg, char * croakBuf)
{
    ROUTINE_DBG_MSP00 ("commErrOccurred");
    boolean errOccurred = false;

    if (commErr != va_ok) {
        raiseCommunicationError (commErr, msg);
        errOccurred = true;
    }
    return errOccurred;
}

/*----------------------------------------*/

static boolean
sqlErrOccured (Script_SessionT   * nself, char * croakBuf)
{
    ROUTINE_DBG_MSP00 ("sqlErrOccurred");
    boolean errOccurred = false;
    Script_SqlResultT * sqlresult;

    if (nself->lasterr.sqlresult.returnCode != 0) {
        /*
         * create Exception object
         */
        sqlresult = &nself->lasterr.sqlresult;
        raiseSQLError (sqlresult->returnCode, sqlresult->errorPos,
            sqlresult->sqlstate, (const char *) sqlresult->sqlmsg,
            sqlresult->messagelistData, sqlresult->messagelistDataLen);
        if (sqlresult->messagelistData != NULL) {
            free (sqlresult->messagelistData);
            sqlresult->messagelistData = NULL;
        }
        errOccurred = true;
    }
    else if (nself->lasterr.sp_rc != 0) {
        raiseCommunicationError (nself->lasterr.sp_rc, (const char *) nself->lasterr.errtext);
        errOccurred = true;
    }
    return errOccurred;
}

/*----------------------------------------*/

static void
clearLastPacketError (
    Script_SessionT   * nself)
{
    ROUTINE_DBG_MSP00 ("clearLastpacketError");
    nself->lasterr.sqlresult.returnCode = 0;
    nself->lasterr.sp_rc = 0;
}

/*----------------------------------------*/

static boolean
isSessionConnected (Script_SessionT   * session, char * croakBuf)
{
    ROUTINE_DBG_MSP00 ("isSessionConnected");

    if ((session != NULL) && session->is_connected) {
        return true;
    }
    raiseCommunicationError (1, "Invalid Session");
    return false;
}

/*----------------------------------------*/

static int
requestReceive (
    Script_SessionT   * session,
    bool withInfo,
    bool allowRetry)
{
    ROUTINE_DBG_MSP00 ("requestReceive");
    int rc;

#if defined (SAPDB_GARBAGE)
    Py_BEGIN_ALLOW_THREADS
    rc = i28sql (session, NULL);
    Py_END_ALLOW_THREADS
#else
    rc = i28requestForPython (session, NULL);
    /* printf("<session %p>\n", session); */
    if (rc == Script_db_ok) {
        Py_BEGIN_ALLOW_THREADS
        rc = i28receiveForPython (session, NULL);
        /* printf("</session %p>\n", session); */
        Py_END_ALLOW_THREADS
    }
#endif
    if (rc == Script_db_not_accessible) {
        Script_SqlResultT * sqlresult = &(session->lasterr.sqlresult);
        sqlresult->sqlmsg [0] = '\0';

        if (isProcServerSession (session)) {
            sqlresult->returnCode = rc;
        }
        else {
            Py_BEGIN_ALLOW_THREADS
            rc = i28connect (session);
            Py_END_ALLOW_THREADS
            if (rc == Script_db_ok) {
                if (lastWasEOT (session) && allowRetry) {
                    rc = Script_db_parse_again;
                }
                else {
                    rc = Script_db_cmd_timeout;
                    strcpy ((char *) sqlresult->sqlmsg,
                        "Session inactivity timeout (work rolled back)");
                }
                sqlresult->returnCode = rc;
            }
        }
    }
    else if (isProcServerSession (session)) {
        /*
         * check for proc call
         */
        /*
         * execute proc call
         */
    }
    clearEOTFlag (session);
    return rc;
}

/*----------------------------------------*/

typedef struct CommErrorT {
    int             code;
    tsp00_ErrTextc  msg;
} CommErrorT;

/*----------------------------------------*/

typedef struct SQLErrorT {
    int             code;
    int             pos;
    tsp00_C5c       sqlState;
    tsp00_ErrTextc msg;
} SQLErrorT;


#define croakBuf NULL

/* endsnippet Python_precode */
typedef struct SapDB_LongReaderObjectT {
    PyObject_HEAD
    LongReaderT nself;
/* no code for key SapDB_LongReader Python_/Generic_ in ['Generic_precode', 'Python_precode']*/
} SapDB_LongReaderObjectT;

staticforward PyTypeObject SapDB_LongReaderType;
#define isSapDB_LongReaderObject(op) ((op)->ob_type == &SapDB_LongReaderType)


static SapDB_LongReaderObjectT*
newSapDB_LongReader ()
{
    SapDB_LongReaderObjectT* result;

    result = PyObject_NEW (SapDB_LongReaderObjectT, &SapDB_LongReaderType);
    if (result != NULL) {
        memset (&result->nself, '\0', sizeof (SapDB_LongReaderObjectT) - offsetof (SapDB_LongReaderObjectT, nself));
    }
    return result;
}

static void
freeSapDB_LongReader (
    SapDB_LongReaderObjectT * value)
{
    Py_DECREF (value);
}
/* snippet SapDB_LongReader Generic_precode */

#define DESCFIELD(descriptor, name) (descriptor.variant.C_false.name##_F)
#if defined (__cplusplus)
#define BECOMES(lvalue, expr)       lvalue.becomes (expr)
#else
#define BECOMES(lvalue, expr)       lvalue = expr
#endif
/*----------------------------------------*/

static void
setReaderState (
    LongReaderT * nself,
    byte_t * longBuf)
{
    ROUTINE_DBG_MSP00 ("setReaderState");
    tsp00_ValMode_Enum valMode = DESCFIELD (nself->descriptor, ld_valmode);
    int valLen = DESCFIELD (nself->descriptor, ld_vallen);
    const byte_t * longData;

    if ((valMode == vm_datapart)
            || (valMode == vm_alldata)
            || (valMode == vm_lastdata)) {
        if (valLen > nself->dataSize) {
            if (nself->dataBuf == NULL) {
                nself->dataBuf = REINTERPRET_CAST (byte_t*, malloc (valLen));
            }
            else {
                nself->dataBuf = REINTERPRET_CAST (byte_t*, realloc (nself->dataBuf, valLen));
            }
            memset (nself->dataBuf, '%', valLen);
            nself->dataSize = valLen;
        }
        longData = longBuf + DESCFIELD (nself->descriptor, ld_valpos) - 1;
        memcpy (nself->dataBuf, longData, valLen);
        nself->dataPos = 0;
        nself->dataLen = valLen;
    }
    if ((valMode == vm_alldata) || (valMode == vm_lastdata)) {
        nself->lastGetvalFound = true;
    }
}

/*----------------------------------------*/

static void
initLongReader (
    LongReaderT * nself,
    Script_SessionT   * session,
    tsp00_LongDescriptor * descriptor,
    byte_t * basePointer,
    bool isAsciiCol)
{
    ROUTINE_DBG_MSP00 ("initLongReader");

    nself->session = session;
    if (isAsciiCol) {
        nself->read = readAsciiLong;
    }
    else {
        nself->read = readUCS2Long;
    }
    increaseRef (session->more_data);
    memcpy (&nself->descriptor, descriptor, sizeof (*descriptor));
    if (DESCFIELD (nself->descriptor, ld_valpos) > 0) {
        nself->bytesRemaining = DESCFIELD (nself->descriptor, ld_maxlen);
    }
    else {
        nself->bytesRemaining = -1;
    }
    setReaderState (nself, basePointer);
}

/*----------------------------------------*/

#if defined (PYTHON_GLUE)
static PyObject *
createLongReader (
    Script_SessionT   * session,
    tsp00_LongDescriptor * descriptor,
    byte_t * basePointer,
    bool isAsciiCol)
{
    ROUTINE_DBG_MSP00 ("createLongReader");
    SapDB_LongReaderObjectT * reader = newSapDB_LongReader ();

    if (reader == NULL) {
        return NULL;
    }
    initLongReader (&reader->nself, session, descriptor, basePointer, isAsciiCol);
    return REINTERPRET_CAST (PyObject *, reader);
}

/*----------------------------------------*/

#elif defined (PERL_GLUE)
static SV *
createLongReader (
    Script_SessionT   * session,
    tsp00_LongDescriptor * descriptor,
    bool isAsciiCol)
{
    ROUTINE_DBG_MSP00 ("createLongReader");
    char croakBuf [200];
    int resultCount;
    SV * perlObject = NULL;
    SapDB_LongReaderObjectT * reader = NULL;
    dSP;

    /*
     * create resultset object
     */
    /* ENTER; */
    /* SAVETMPS; */
    PUSHMARK (sp);
    PUTBACK;
    resultCount = perl_call_pv (LIT("sql::createReaderZombie"),
                      G_SCALAR | G_EVAL);
    SPAGAIN;
    if (SvTRUE (GvSV (PL_errgv))) {
        POPs;
    }
    else {
        perlObject = POPs;
        SvREFCNT_inc (perlObject);  /* increase refcount to prevent delete */
    }
    PUTBACK;
    /* FREETMPS; */
    /* LEAVE; */
    /*
     * retrieve actual pointer from this mess
     */
    if (resultCount != 1) {
        return NULL;
    }
    reader = REINTERPRET_CAST (SapDB_LongReaderObjectT*,
         getGluePointer (perlObject, SapDB_LongReaderClassID, croakBuf));
    if ((reader == NULL) || !isSapDB_LongReaderObject (reader)) {
        sv_2mortal (perlObject);
        return NULL;
    }
    /*
     * fill reader object with soul
     */
    initLongReader (&reader->nself, session, descriptor, isAsciiCol);
    return perlObject;
}
#endif
/*----------------------------------------*/

static void
closeReader (
    LongReaderT * nself)
{
    ROUTINE_DBG_MSP00 ("closeReader");
    if (!nself->lastGetvalFound) {
        Script_SessionT   * session = nself->session;
        int rc;

        i28initspecial (session, sp1m_getval);
        i28newpart (session, sp1pk_longdata);
        DESCFIELD (nself->descriptor, ld_valmode) = vm_close;
        i28parg (session, REINTERPRET_CAST (const char *, &nself->descriptor),
            sizeof (tsp00_LongDescriptor), sizeof (tsp00_LongDescriptor) + 1,
            1, '\0');
        rc = requestReceive (session, false, false);
        nself->lastGetvalFound = true;
    }
    if (nself->dataBuf != NULL) {
        free (nself->dataBuf);
    }
    decreaseRef (nself->session->more_data);
}

/*----------------------------------------*/

#if defined (PYTHON_GLUE)
static void
destructor_SapDB_LongReader (
    PyObject * pyself)
{
    ROUTINE_DBG_MSP00 ("destructor_SapDB_LongReader");
    SapDB_LongReaderObjectT* self = REINTERPRET_CAST (SapDB_LongReaderObjectT*, pyself);
    LongReaderT * nself = &self->nself;
    closeReader (nself);
    PyObject_Del (pyself);
}
#endif

/*----------------------------------------*/

static int
doGetval (
    LongReaderT * nself)
{
    ROUTINE_DBG_MSP00 ("doGetval");
    Script_SessionT   * session = nself->session;
    int rc;
    byte_t * resultBuf;

    i28initspecial (session, sp1m_getval);
    i28newpart (session, sp1pk_longdata);
    DESCFIELD (nself->descriptor, ld_vallen) = -1;
    i28parg (session, REINTERPRET_CAST (const char *, &nself->descriptor),
        sizeof (tsp00_LongDescriptor), sizeof (tsp00_LongDescriptor) + 1,
        1, '\0');
    rc = requestReceive (session, false, false);
    if (rc != Script_db_ok) {
        nself->lastGetvalFound = true;
        nself->dataLen = 0;
        nself->dataPos = 0;
        return rc;
    }
    i28findpart (session, sp1pk_longdata);
    resultBuf = (byte_t *) i28argsbuf (session);
    memcpy (&nself->descriptor, resultBuf + 1, sizeof (tsp00_LongDescriptor));
    setReaderState (nself, resultBuf);
    return 0;
}

/*----------------------------------------*/

static SL_ValueT *
readAsciiLong (
    LongReaderT * nself,
    int requested)
{
    ROUTINE_DBG_MSP00 ("readAsciiLong");
    SL_ValueT * result = NULL;
    char * resultBuf;
    long   resultLen = 0;
    int remaining = nself->dataLen - nself->dataPos;

    if (requested < 0) {
        if (nself->bytesRemaining < 0) {
            /* read first chunk to get size */
            doGetval (nself);
            nself->bytesRemaining = DESCFIELD (nself->descriptor, ld_maxlen);
            remaining = nself->dataLen - nself->dataPos;
        }
        requested = nself->bytesRemaining;
    }
    resultBuf = REINTERPRET_CAST (char*, malloc (requested));
    while (requested > 0) {
        if (remaining >= requested) {
            /* copy requested */
            memcpy (resultBuf + resultLen, nself->dataBuf + nself->dataPos, requested);
            nself->dataPos += requested;
            nself->bytesRemaining -= requested;
            resultLen += requested;
            requested = 0;
        }
        else {
            /* copy remaining */
            if (remaining > 0) {
                memcpy (resultBuf + resultLen, nself->dataBuf + nself->dataPos, remaining);
                requested -= remaining;
                resultLen += remaining;
                nself->dataPos += remaining;
                nself->bytesRemaining -= remaining;
            }
            if (nself->lastGetvalFound) {
                break;
            }
            doGetval (nself);
            remaining = nself->dataLen - nself->dataPos;
        }
    }
    result = SL_fromString (resultBuf, resultLen);
    free (resultBuf);
    return result;
}

/*----------------------------------------*/

static SL_ValueT *
readUCS2Long (
    LongReaderT * nself,
    int requested)
{
    ROUTINE_DBG_MSP00 ("readUnicodeLong");
    SL_ValueT * result = NULL;
    char * resultBuf;
    long   resultLen = 0;
    int remaining = (nself->dataLen - nself->dataPos) / 2; /* in chars */

    if (requested < 0) {
        if (nself->bytesRemaining < 0) {
            /* read first chunk to get size */
            doGetval (nself);
            nself->bytesRemaining = DESCFIELD (nself->descriptor, ld_maxlen);
            remaining = (nself->dataLen - nself->dataPos) / 2;
        }
        requested = nself->bytesRemaining / 2;
    }
    resultBuf = REINTERPRET_CAST (char*, malloc (requested * 2));
    while (requested > 0) {
        if (remaining >= requested) {
            /* copy requested */
            memcpy (resultBuf + (resultLen * 2), nself->dataBuf + nself->dataPos,
                requested * 2);
            nself->dataPos += requested * 2;
            nself->bytesRemaining -= requested * 2;
            resultLen += requested;
            requested = 0;
        }
        else {
            /* copy remaining */
            if (remaining > 0) {
                memcpy (resultBuf + (resultLen * 2), nself->dataBuf + nself->dataPos,
                    remaining * 2);
                requested -= remaining;
                resultLen += remaining;
                nself->dataPos += remaining * 2;
                nself->bytesRemaining -= remaining * 2;
            }
            if (nself->lastGetvalFound) {
                break;
            }
            doGetval (nself);
            remaining = (nself->dataLen - nself->dataPos) / 2;
        }
    }
    result = SL_fromUCS2 (resultBuf, resultLen);
    free (resultBuf);
    return result;
}

/*----------------------------------------*/

static SL_ValueT *
readLong (
    LongReaderT * nself,
    int requested)
{
    ROUTINE_DBG_MSP00 ("readLong");
    return nself->read (nself, requested);
}

/*----------------------------------------*/

static long
remainingLength (
    LongReaderT * nself)
{
    ROUTINE_DBG_MSP00 ("remainingLength");
    if (nself->bytesRemaining < 0) {
        /* read first chunk to get size */
        doGetval (nself);
        nself->bytesRemaining = DESCFIELD (nself->descriptor, ld_maxlen);
    }
    if (nself->read == readUCS2Long) {
        return nself->bytesRemaining / 2;
    }
    else {
        return nself->bytesRemaining;
    }
}

/* endsnippet SapDB_LongReader Generic_precode */
/* snippet SapDB_LongReader Python_precode */

static PyObject *
read_SapDB_LongReader (PyObject *pyself, PyObject* args, PyObject* keywArgs);

static PyObject *
callableRead (PyObject *pyself, PyObject* args, PyObject* keywArgs)
{
    ROUTINE_DBG_MSP00 ("callableRead");
    return read_SapDB_LongReader (pyself, args, keywArgs);
}

static PyObject *
__call___SapDB_LongReader (PyObject *pyself, PyObject* args, PyObject* keywArgs)
{
    return read_SapDB_LongReader (pyself, args, keywArgs);
}

/* endsnippet SapDB_LongReader Python_precode */
/*----------------------------------------*/

static PyObject *
read_SapDB_LongReader (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("read_SapDB_LongReader");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_LongReaderObjectT* self = REINTERPRET_CAST (SapDB_LongReaderObjectT*, pyself);
    LongReaderT* nself = &self->nself;
    static const char * kwlist [] = {
        LIT("count"), NULL};
    int count =  -1;
    SL_ValueT * result;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("|i:SapDB_LongReader.read"), (char **) kwlist, &count)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = readLong (nself, count);
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return result;
}

/*----------------------------------------*/

static PyObject *
remainingLength_SapDB_LongReader (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("remainingLength_SapDB_LongReader");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_LongReaderObjectT* self = REINTERPRET_CAST (SapDB_LongReaderObjectT*, pyself);
    LongReaderT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    int result;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = remainingLength (nself);
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return Py_BuildValue (LIT("i"), result);
}

static PyMethodDef SapDB_LongReaderClassMethods [] = {
    {LIT("read"), (PyCFunction)read_SapDB_LongReader, METH_VARARGS|METH_KEYWORDS,
        "returns the subsequent <i>count</i> characters.\n\n                    Returns an empty String when the end of the LONG\n                    is reached.\n                    "},
    {LIT("__call__"), (PyCFunction)__call___SapDB_LongReader, METH_VARARGS|METH_KEYWORDS, NULL},
    {LIT("remainingLength"), (PyCFunction)remainingLength_SapDB_LongReader, METH_VARARGS|METH_KEYWORDS, NULL},
    {NULL, NULL}
};


static PyObject*
SapDB_LongReaderGetattr (PyObject* self, char* name)
{
    return Py_FindMethod (SapDB_LongReaderClassMethods, self, name);
}

statichere PyTypeObject SapDB_LongReaderType = {
    PyObject_HEAD_INIT (NULL)
    0,
    LIT("SapDB_LongReader"),    /* tp_name */
    sizeof (SapDB_LongReaderObjectT),    /* tp_basicsize */
    0,    /* tp_itemsize */
    REINTERPRET_CAST (destructor, destructor_SapDB_LongReader),    /* tp_dealloc */
    0,    /* tp_print */
    REINTERPRET_CAST (getattrfunc, SapDB_LongReaderGetattr),    /* tp_getattr */
    0,    /* tp_setattr */
    0,    /* tp_compare */
    REINTERPRET_CAST (reprfunc, 0),    /* tp_repr */
    0,    /* tp_as_number */
    0,    /* tp_as_sequence */
    0,    /* tp_as_mapping */
    REINTERPRET_CAST (hashfunc, 0),    /* tp_hash */
    REINTERPRET_CAST (ternaryfunc, callableRead),    /* tp_call */
    REINTERPRET_CAST (reprfunc, 0),    /* tp_str */
    0,    /* tp_getattro */
    0,    /* tp_setattro */
    0,    /* *tp_as_buffer */
    0,    /* tp_xxx4 */
    "allows retrieval of LONGs.\n\n        Python: This is a 'callable' object. Calling it with an\n        integer argument will return the next bytes of the LONG\n        (similar to calling the <i>read</i> method).",    /* tp_doc */
};

typedef struct SapDB_ResultSetObjectT {
    PyObject_HEAD
    ResultSetT nself;
/* no code for key SapDB_ResultSet Python_/Generic_ in ['Generic_precode']*/
} SapDB_ResultSetObjectT;

staticforward PyTypeObject SapDB_ResultSetType;
#define isSapDB_ResultSetObject(op) ((op)->ob_type == &SapDB_ResultSetType)


static SapDB_ResultSetObjectT*
newSapDB_ResultSet ()
{
    SapDB_ResultSetObjectT* result;

    result = PyObject_NEW (SapDB_ResultSetObjectT, &SapDB_ResultSetType);
    if (result != NULL) {
        memset (&result->nself, '\0', sizeof (SapDB_ResultSetObjectT) - offsetof (SapDB_ResultSetObjectT, nself));
    }
    return result;
}

static void
freeSapDB_ResultSet (
    SapDB_ResultSetObjectT * value)
{
    Py_DECREF (value);
}
/* snippet SapDB_ResultSet Generic_precode */
static int
openCursor (
    Script_SessionT   * session,
    ResultSetT * resultSet);

/*----------------------------------------*/
#if defined (PYTHON_GLUE)
static void
createResultSetFromReply (
    Script_SessionT   * session,
    SQLResultT * result)
{
    ROUTINE_DBG_MSP00 ("createResultSetFromReply");
    SapDB_ResultSetObjectT * newObj = newSapDB_ResultSet ();
    int rc;

    if (newObj == NULL) {
        return;
    }
    rc = openCursor (session, &newObj->nself);
    if (rc != 0) {
        sqlErrOccured (session, NULL);
        Py_DECREF (newObj);
        newObj = NULL;
    }
    result->resultSet =  REINTERPRET_CAST(PyObject *, newObj);
    result->itemsSet |= hasResultSetC;
}
#elif defined (PERL_GLUE)
static SV *
createResultSetFromReply (
    Script_SessionT   * session)
{
    ROUTINE_DBG_MSP00 ("createResultSetFromReply");
    char croakBuf [200];
    int resultCount;
    SV * perlObject = NULL;
    SapDB_ResultSetObjectT * resultSet = NULL;
    int rc;
    dSP;

    /*
     * create resultset object
     */
    /* ENTER; */
    /* SAVETMPS; */
    PUSHMARK (sp);
    PUTBACK;
    resultCount = perl_call_pv (LIT("sql::createCursorZombie"),
                      G_SCALAR | G_EVAL);
    SPAGAIN;
    if (SvTRUE (GvSV (PL_errgv))) {
        POPs;
    }
    else {
        perlObject = POPs;
    }
    PUTBACK;
    /* FREETMPS; */
    /* LEAVE; */
    /*
     * retrieve actual pointer from this mess
     */
    if (resultCount != 1) {
        return NULL;
    }
    resultSet = REINTERPRET_CAST (SapDB_ResultSetObjectT*,
         getGluePointer (perlObject, SapDB_ResultSetClassID, croakBuf));
    if ((resultSet == NULL) || !isSapDB_ResultSetObject (resultSet)) {
        sv_2mortal (perlObject);
        return NULL;
    }
    /*
     * fill resultset object with soul
     */
    rc = openCursor (session, &resultSet->nself);
    if (rc != 0) {
        sv_2mortal (perlObject);
        return NULL;
    }
    return perlObject;
}
#endif
/*----------------------------------------*/

static void *
argAddr (
    byte_t * dataBuf,
    int bufPos)
{
    ROUTINE_DBG_MSP00 ("argAddr");
    byte_t    *bufaddr;
    byte_t        *result;

    bufaddr = dataBuf + bufPos;
    if (bufaddr [-1] == (unsigned char) csp_undef_byte)
        result = NULL;
    else
        result = bufaddr;
    return result;
}

/*----------------------------------------*/

static void
getOutputParameter2 (
    Script_SessionT   * session,
    byte_t * basePointer,
    byte_t * dataBuf,
    ParseInfoT * parseInfo,
    SQLResultT * result)
{
    ROUTINE_DBG_MSP00 ("getOutputParameter2");
    int         outputIndex = -1;
    int         i;
    tsp1_param_info * paramInfo = parseInfo->paramInfo;
    SL_ValueT * value;
    void      * addr;
    int         intVal;
    double      floatVal;
    tsp00_NumError  numErr;
    int         valLen;

    allocResult (result, parseInfo->outputParamCount);
    for (i = 0; i < parseInfo->paramCount; ++i, ++paramInfo) {
        if (paramInfo->sp1i_io_type != sp1io_input) {
            ++outputIndex;
            addr = argAddr (dataBuf, paramInfo->variant.C_true.sp1i_bufpos_F);
            if (addr == NULL) {
                value = SL_fromNone ();
            }
            else {
                switch (paramInfo->sp1i_data_type) {
                    case dfixed:
                        if (paramInfo->sp1i_frac == 0) {
                            s40glint (REINTERPRET_CAST (tsp00_Number*, addr),
                                1, paramInfo->sp1i_length,
                                REFPARM (intVal), REFPARM (numErr));
                            if (numErr == num_ok) {
                                value = SL_fromInt (intVal);
                            }
                        }
                        else {
                            s40glrel (REINTERPRET_CAST (tsp00_Number*, addr),
                                1, paramInfo->sp1i_length,
                                REFPARM (floatVal), REFPARM (numErr));
                            if (numErr == num_ok) {
                                value = SL_fromDouble (floatVal);
                            }
                        }
                        if (numErr != num_ok) {
                            char strbuf [100];
                            int outlen;
                            s42gchr (addr, 1, paramInfo->sp1i_length,
                                paramInfo->sp1i_frac, paramInfo->sp1i_length,
                                strbuf, 1, REFPARM (outlen), REFPARM (numErr));
                            value = SL_fromString (strbuf, outlen);
                        }
                        break;
                    case dfloat:
                    case dvfloat:
                        s40glrel (REINTERPRET_CAST (tsp00_Number*, addr),
                            1, paramInfo->sp1i_length,
                            REFPARM (floatVal), REFPARM (numErr));
                        value = SL_fromDouble (floatVal);
                        break;
                    case dsmallint:
                    case dinteger:
                        s40glint (REINTERPRET_CAST (tsp00_Number*, addr),
                            1, paramInfo->sp1i_length,
                            REFPARM (intVal), REFPARM (numErr));
                        value = SL_fromInt (intVal);
                        break;
                    case dcha:
                    case dche:
                    case dvarchara:
                    case dvarchare:
                    case ddate:
                    case dtime:
                    case dtimestamp:
                        valLen = s30klen (addr, ' ', paramInfo->sp1i_length);
                        if ((valLen == 0) && (session->space_option)) {
                            valLen = 1;
                        }
                        value = SL_fromString (REINTERPRET_CAST (const char *, addr),
                            valLen);
                        break;
                    case dchb:
                    case dvarcharb:
                        valLen = s30klen (addr, '\0', paramInfo->sp1i_length);
                        value = SL_fromString (REINTERPRET_CAST (const char *, addr),
                            valLen);
                        break;
                    case dunicode:
                    case dvarcharuni:
                        valLen = paramInfo->sp1i_length
                            - sp77encodingUCS2Native->countPadChars (addr, paramInfo->sp1i_length * 2, ' ');
                        value = SL_fromUCS2 (addr, valLen);
                        break;
                    case dboolean:
                        if (((byte_t*)addr)[0] == '\0') {
                            value = SL_fromFalse ();
                        }
                        else {
                            value = SL_fromTrue ();
                        }
                        break;
                    case dstra:
                    case dstre:
                    case dstrb:
                    case dlonga:
                    case dlonge:
                    case dlongb:
                        value = createLongReader (session,
                            REINTERPRET_CAST (tsp00_LongDescriptor*, addr),
                            basePointer,
                            true);
                        break;
                    case dstruni:
                    case dlonguni:
                        value = createLongReader (session,
                            REINTERPRET_CAST (tsp00_LongDescriptor*, addr),
                            basePointer,
                            false);
                        break;
                    default:
                        value = SL_fromNone ();
                        break;
                }
            }
            if (value != NULL) {
                setResultElement (result, outputIndex, value);
            }
        }
    }
}

/*----------------------------------------*/

static int
doDescribe (
    Script_SessionT   * session,
    const char * cursorName)
{
    ROUTINE_DBG_MSP00 ("doDescribe");
    int rc;

    if (cursorName [0] != '\0') {
        rc = i28adbsf (session, "DESCRIBE \"%s\"", cursorName);
    }
    else {
        rc = i28adbs_noinfo  (session, "DESCRIBE", false);
    }
    return rc;
}

/*----------------------------------------*/

static int
openCursor (
    Script_SessionT   * session,
    ResultSetT * resultSet)
{
    ROUTINE_DBG_MSP00 ("openCursor");
    int nameLen = -1;
    int rc = 0;
    int infoSize;
    int i;

    resultSet->session = session;
    increaseRef (session->more_data);
    for (i = 0; i < fetchKindsE; ++i) {
        resultSet->fetchInfo [i].paramCount = -1;
    }
    memset (resultSet->cursorName, '\0', sizeof (resultSet->cursorName));
    i28gresulttablename (session, &nameLen, resultSet->cursorName);
    if (i28findpart (session, sp1pk_columnnames)) {
        nameLen = i28nameslen (session);
    }
    else {
        nameLen = -1;
    }
    if (nameLen == -1) {
        rc = doDescribe (session, resultSet->cursorName);
        if (rc == 0) {
            nameLen = i28nameslen (session);
        }
    }
    if (rc == 0) {
        resultSet->colNames = REINTERPRET_CAST (const char *, malloc (nameLen));
        memcpy ((void *) resultSet->colNames , i28colnames (session), nameLen);
        resultSet->colCount = i28paramcount (session);
        infoSize = sizeof (tsp1_param_info) * resultSet->colCount;
        resultSet->colInfo = REINTERPRET_CAST (tsp1_param_info *, malloc (infoSize));
        memcpy (resultSet->colInfo, i28gparaminfo (session, 0), infoSize);
        resultSet->currentPos = 0;
    }
    return rc;
}

/*----------------------------------------*/

static int
getFetchBufPos (
    ResultSetT * resultSet)
{
    ROUTINE_DBG_MSP00 ("getFetchBufPos");
    return(resultSet->currentRow - resultSet->dataBuf)
    / resultSet->rowSize;
}

/*----------------------------------------*/

static int
getFetchBufRecords (
    ResultSetT * resultSet)
{
    ROUTINE_DBG_MSP00 ("getFetchBufRecords");
    return(resultSet->currentRow - resultSet->eoRecords)
    / resultSet->rowSize;
}

/*----------------------------------------*/

static boolean
moveFetchBufPos (
    ResultSetT * resultSet,
    int relative)
{
    ROUTINE_DBG_MSP00 ("moveFetchBufPos");
    boolean result;
    byte_t * newPos = resultSet->currentRow + (resultSet->rowSize * relative);
    if ((resultSet->dataBuf <= newPos) && (newPos < resultSet->eoRecords)) {
        result = true;
        resultSet->currentRow = newPos;
    }
    else {
        result = false;
    }
    return result;
}

/*----------------------------------------*/

static int
calculateFetchRecord (
    ParseInfoT * parseInfo)
{
    int result = 0;
    int i;

    for (i = 0; i < parseInfo->paramCount; ++i) {
        tsp1_param_info * paramInfo = &parseInfo->paramInfo [i];
        if (paramInfo->sp1i_io_type == sp1io_output) {
            result += paramInfo->sp1i_in_out_len;
        }
    }
    return result;
}

/*----------------------------------------*/

static int
parseFetch (
    ResultSetT * resultSet,
    fetchKindsT fetchKind)
{
    ROUTINE_DBG_MSP00 ("parseFetch");
    Script_SessionT   * session = resultSet->session;
    ParseInfoT * parseInfo;
    int rc;
    const char * fmt;
    int i;

    i28initparse (session);
    if (strlen (resultSet->cursorName) > 0) {
        fmt = "Fetch %s \"%s\"";
    }
    else {
        fmt = "Fetch %s ";
    }
    i28pcmdf (session, fmt,
        fetchCommandsC [fetchKind], STATIC_CAST (char *, resultSet->cursorName));
    i28pcmd (session, "into ?", -1);
    for (i = 1; i < resultSet->colCount; ++i) {
        i28pcmd (session, ", ?", -1);
    }
#if defined (__cplusplus)
    session->segment->sp1c_sqlmode ()
        = tsp1_sqlmode::fromInt (sp1sm_internal);
#else
    session->segment->variant.C_3.sp1c_sqlmode_F = sp1sm_internal;
#endif
    session->segment->variant.C_3.sp1c_mass_cmd_F = true;
    rc = requestReceive (session, false, false);
    if (rc != Script_db_ok) {
        return rc;
    }
    parseInfo = &resultSet->fetchInfo [fetchKind];
    if (!fillParamInfos (session, parseInfo, true)) {
        return internalErrorC;
    }
    if (resultSet->rowSize == 0) {
        resultSet->rowSize = calculateFetchRecord (parseInfo);
    }
    return Script_db_ok;
}

/*----------------------------------------*/

static int
copyFetchBuffer (
    ResultSetT * resultSet)
{
    ROUTINE_DBG_MSP00 ("copyFetchBuffer");
    int partLen;
    byte_t * newBuf;
    int recordsInFetch;

    if (!i28findpart (resultSet->session, sp1pk_data)) {
        return Script_db_row_not_found;
    }
    if (resultSet->session->part->variant.C_2.sp1p_arg_count_F == 0) {
        /* workaround: current bug in fetch relative */
        return Script_db_row_not_found;
    }
    /*
     * make sure buffer is large enough
     */
    partLen = i28partlen (resultSet->session);
    if (resultSet->dataSize < partLen) {
        newBuf = REINTERPRET_CAST (byte_t*, realloc (resultSet->dataBuf, partLen));
        if (newBuf == NULL) {
            return internalErrorC;
        }
        else {
            resultSet->dataBuf = newBuf;
            resultSet->dataSize = partLen;
        }
    }
    /*
     * copy data
     */
    resultSet->dataLen = partLen;
    memcpy (resultSet->dataBuf, i28argsbuf (resultSet->session), partLen);
    /*
     * set currentRow and eoRecords
     */
    recordsInFetch = resultSet->session->part->variant.C_2.sp1p_arg_count_F;
    if (recordsInFetch > resultSet->maxRecordsPerFetch) {
        resultSet->maxRecordsPerFetch = recordsInFetch;
    }
    resultSet->currentRow = resultSet->dataBuf;
    resultSet->eoRecords = resultSet->dataBuf
                           + (recordsInFetch * resultSet->rowSize);
    return Script_db_ok;
}

/*----------------------------------------*/

static int
doFetch (
    ResultSetT * resultSet,
    fetchKindsT fetchKind,
    int parameter)
{
    ROUTINE_DBG_MSP00 ("doFetch");
    Script_SessionT   * session = resultSet->session;
    ParseInfoT     * parseInfo;
    int          rc;
    tsp00_Number     num;
    tsp00_NumError   res;
    tsp1_param_info * paramInfo;
    int              fetchCount;

    if (resultSet->maxRecordsPerFetch > 0) {
        fetchCount = resultSet->maxRecordsPerFetch;
    }
    else {
        fetchCount = 30000;
    }
    parseInfo = &resultSet->fetchInfo [fetchKind];
    if (parseInfo->paramCount == -1) {
        rc = parseFetch (resultSet, fetchKind);
        if (rc != Script_db_ok) {
            return rc;
        }
    }
    i28initexecute (session, parseInfo->pid);
    switch (fetchKind) {
        case relativeFetchE:
            if (parameter < 0) {
                if (resultSet->maxRecordsPerFetch > 0) {
                    fetchCount = resultSet->maxRecordsPerFetch;
                }
                else {
                    fetchCount = 1;
                }
            }
            /*FALLTHROUGH*/
        case absoluteFetchE:
            /*
             * add parameter
             */
            i28newDataPart (session, 1);
            s41plint (&num, 1, csp_fixed, 0, parameter, REFPARM (res));
            paramInfo = &parseInfo->paramInfo [0];
            addValue (session, (const char /* byte */ *) num, sizeof (num),
                paramInfo->sp1i_in_out_len, paramInfo->variant.C_true.sp1i_bufpos_F,
                csp_defined_byte);
            break;
    }
    /*
     * add result count
     */
    i28newpart (session, sp1pk_resultcount);
    s41plint (&num, 1, csp_fixed, 0, fetchCount, REFPARM (res));
    i28parg (session, (const char /* byte */ *) num, sizeof (num),
        6 /* fixed(10)*/ + 1, 1, csp_defined_byte);
    /*
     * execute
     */
    rc = requestReceive (session, false, false);
    if (rc == Script_db_ok) {
        rc = copyFetchBuffer (resultSet);
        LOGF ("FETCH new data: buf %p\n", resultSet->dataBuf);
        HEXDUMP (resultSet->dataBuf, 0, 64);
    }
    return rc;
}

/*----------------------------------------*/

static void
getFetchOutput (
    SQLResultT * result,
    ResultSetT * resultSet,
    int          fetchKind,
    int          rc)
{
    ROUTINE_DBG_MSP00 ("getFetchOutput");
    switch (rc) {
        case Script_db_ok:
            LOGF ("get fetch: buf %p, row %p\n", resultSet->dataBuf, resultSet->currentRow);
            HEXDUMP (resultSet->currentRow, 0, 16);
            if ((resultSet->dataBuf <= resultSet->currentRow)
                && (resultSet->currentRow < resultSet->eoRecords))
            {
                getOutputParameter2 (resultSet->session,
                    resultSet->dataBuf,
                    resultSet->currentRow,
                    &resultSet->fetchInfo [fetchKind],
                    result);
            }
            else {
                setResult (result, SL_fromNone ());
            }
            break;
        case Script_db_row_not_found:
            setResult (result, SL_fromNone ());
            break;
        default:
            result->isOK = false;
            break;
    }
}

/*----------------------------------------*/

static SQLResultT
relativeRow (
    ResultSetT * resultSet,
    int count)
{
    ROUTINE_DBG_MSP00 ("relativeRow");
    int rc = Script_db_ok;
    SQLResultT result;

    initResult (&result, 0);
    /*
     * quick fix: absolute passes to relative, but
     *    FETCH RELATIVE hasn't been parsed yet
     */
    if (resultSet->fetchInfo [relativeFetchE].paramCount == -1) {
        rc = parseFetch (resultSet, relativeFetchE);
        if (rc != Script_db_ok) {
            getFetchOutput (&result, resultSet, 0, rc);
            return result;
        }
    }
    /* end quick fix */
    if (resultSet->dataBuf == NULL) {
        /*
         * no result yet requires FETCH
         */
        rc = doFetch (resultSet, relativeFetchE, count);
        if (rc == Script_db_ok) {
            resultSet->currentRow= resultSet->dataBuf;
            resultSet->currentPos += count;
        }
        else {
            getFetchOutput (&result, resultSet, 0, rc);
            return result;
        }
    }
    else if (moveFetchBufPos (resultSet, count)) {
        resultSet->currentPos += count;
    }
    else {
        /*
         * moving outside of buffer requires FETCH
         */
        rc = doFetch (resultSet, relativeFetchE, count);
        switch (rc) {
            case 0:
                resultSet->currentPos += count;
                break;
            case 100:
                if (count > 0) {
                    resultSet->currentRow = resultSet->eoRecords;
                }
                else {
                    resultSet->currentRow = resultSet->dataBuf - resultSet->rowSize;
                }
                break;
            default:
                result.isOK = false;
                return result;
        }
    }
    getFetchOutput (&result, resultSet, relativeFetchE, rc);
    return result;
}

/*----------------------------------------*/

static SQLResultT
absoluteRow (
    ResultSetT * resultSet,
    int count)
{
    ROUTINE_DBG_MSP00 ("absoluteRow");
    int rc = Script_db_ok;
    SQLResultT result;

    if (count == (resultSet->currentPos + 1)) {
        result = relativeRow(resultSet, 1);
    }
    else {
        initResult (&result, 0);
        rc = doFetch (resultSet, absoluteFetchE, count);
        if (rc != Script_db_ok) {
            getFetchOutput (&result, resultSet, 0, rc);
            return result;
        }
        if (count == -1) {
            /*
             * special case: optimized for FETCH PREV
             */
            resultSet->currentRow = resultSet->eoRecords - resultSet->rowSize;
        }
        else {
            resultSet->currentRow = resultSet->dataBuf;
        }
        getFetchOutput (&result, resultSet, absoluteFetchE, rc);
        resultSet->currentPos = count;
    }
    return result;
}

/*----------------------------------------*/

#if defined (PYTHON_GLUE)
static PyObject *
absoluteRowBased0 (
    PyObject * pyself,
    int count)
{
    ROUTINE_DBG_MSP00 ("absoluteRowBased0");
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    SQLResultT result;
    PyObject * resultObj;

    result = absoluteRow (nself, count + 1);
    if (!result.isOK && sqlErrOccured (nself->session, NULL)) {
        return NULL;
    }
    if (!sqlResultC2Py (self, result, &resultObj)) {
        return NULL;
    }
    if (SL_isNone (resultObj)) {
        PyErr_SetNone (PyExc_IndexError);
        return NULL;
    }
    return resultObj;
}

static PyObject *
__getitem___SapDB_ResultSet (
    PyObject * pyself,
    PyObject* args,
    PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("absoluteRowBased0");
    int index;
    static const char * kwlist [] = {
        LIT("index"), NULL};

    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("i:SapDB_ResultSet.__getitem__"), (char **) kwlist, &index)) {
        return NULL;
    }
    return absoluteRowBased0 (pyself, index);
}

#endif

/*----------------------------------------*/

static SQLResultT
firstRow (
    ResultSetT * resultSet)
{
    ROUTINE_DBG_MSP00 ("first");
    int rc = Script_db_ok;
    SQLResultT result;

    initResult (&result, 0);
    rc = doFetch (resultSet, firstFetchE, -1);
    resultSet->currentRow = resultSet->dataBuf;
    getFetchOutput (&result, resultSet, firstFetchE, rc);
    resultSet->currentPos = 1;
    return result;
}

/*----------------------------------------*/

static SQLResultT
lastRow (
    ResultSetT * resultSet)
{
    ROUTINE_DBG_MSP00 ("lastRow");
    int rc;
    SQLResultT result;

    initResult (&result, 0);
    rc = doFetch (resultSet, lastFetchE, -1);
    resultSet->currentRow = resultSet->eoRecords
                            - resultSet->rowSize;
    getFetchOutput (&result, resultSet, lastFetchE, rc);
    resultSet->currentPos = -1;
    return result;
}

/*----------------------------------------*/

static SQLResultT
currentRow (
    ResultSetT * resultSet)
{
    ROUTINE_DBG_MSP00 ("currentRow");
    SQLResultT result;
    int i;

    initResult (&result, 0);
    initResult (&result, 0);
    for (i = 0; i < fetchKindsE; ++i) {
        if (resultSet->fetchInfo [i].paramCount != -1) {
            LOGF ("currentRow: databuf %p, currentRow %p, eoRecords %p\n",
                resultSet->dataBuf, resultSet->currentRow, resultSet->eoRecords);
            if ((resultSet->dataBuf <= resultSet->currentRow)
                && (resultSet->currentRow < resultSet->eoRecords))
            {
                getOutputParameter2 (resultSet->session,
                    resultSet->dataBuf,
                    resultSet->currentRow,
                    &resultSet->fetchInfo [i],
                    &result);
            }
            else {
                setResult (&result, SL_fromNone ());
            }
            return result;
        }
    }
    result.isOK = false;
    return result;
}

/*----------------------------------------*/

static SQLResultT
nextRow (
    ResultSetT * resultSet)
{
    ROUTINE_DBG_MSP00 ("nextRow");
    return relativeRow (resultSet, 1);
}

/*----------------------------------------*/

static SQLResultT
previousRow (
    ResultSetT * resultSet)
{
    ROUTINE_DBG_MSP00 ("previousRow");
    return relativeRow (resultSet, -1);
}

/*----------------------------------------*/

static void
setFetchSize (
    ResultSetT * resultSet,
    int fetchSize)
{
    if (fetchSize > 0) {
        resultSet->maxRecordsPerFetch = fetchSize;
    }
    else {
        fetchSize = -1;
    }
}

/*----------------------------------------*/

static int
columnCount (
    ResultSetT * resultSet)
{
    return resultSet->colCount;
}

/*----------------------------------------*/

static const char * sqlTypeNamesC [] = {
    "Fixed",
    "Float",
    "Char",
    "Char EBCDIC",
    "Char Binary",
    "Rowid",
    "Long",
    "Long EBCDIC",
    "Long Binary",
    "Long Doublebyte",
    "Date",
    "Time",
    "Float",
    "Timestamp",
    "Unknown",
    "Number",
    "Nonumber",
    "Duration",
    "Doublebyte EBCDIC",
    "Long",
    "Long EBCDIC",
    "Long Binary",
    "Long Doublebyte",
    "Boolean",
    "Unicode",
    "filler1",
    "filler2",
    "filler3",
    "filler4",
    "Smallint",
    "Integer",
    "Varchar",
    "Varchar EBCDIC",
    "Varchar Binary",
    "Long Unicode",
    "Long Unicode",
    "Varchar Unicode",
    "UDT",
    "ABAP Table",
};

static int sqlTypeCodesC [] = {
    3,
    6,
    1,
    1,
    -1,
    -2,
    -1,
    -1,
    -4,
    1111,
    91,
    92,
    6,
    93,
    1111,
    6,
    1111,
    1111,
    1,
    -1,
    -1,
    -4,
    -1,
    -7,
    1,
    1111,
    1111,
    1111,
    1111,
    5,
    4,
    12,
    12,
    -3,
    -12,
    -1,
    12,
    1111,
    1111,
};

/*----------------------------------------*/

static SQLResultT
columnInfo (
    ResultSetT * resultSet,
    int colIndex)
{
    ROUTINE_DBG_MSP00 ("columnInfo");
    SQLResultT result;
    const char * namePos;
    tsp1_param_info * paramInfo;
    int i;

    initResult (&result, 0);
    if ((colIndex < 0) || (resultSet->colCount <= colIndex)) {
        result.isOK = false;
        return result;
    }
    allocResult (&result, 4);
    /*
     * get name
     */
    namePos = resultSet->colNames;
    for (i = 0; i < resultSet->colCount; ++i) {
        if (i == colIndex) {
            setResultElement (&result, 0, SL_fromString (namePos + 1, namePos [0]));
            break;
        }
        namePos += namePos [0] + 1;
    }
    /*
     * get type, len and frac
     */
    paramInfo = &resultSet->colInfo [i];
    setResultElement (&result, 1, SL_fromString (sqlTypeNamesC [paramInfo->sp1i_data_type], -1));
    setResultElement (&result, 2, SL_fromInt (paramInfo->sp1i_length));
    setResultElement (&result, 3, SL_fromInt (paramInfo->sp1i_frac));
    return result;
}

/*----------------------------------------*/

#if defined (__cplusplus)
#define ISMEMBER_M(set, element)    set.includes (element)
#else
#define ISMEMBER_M(set, element)    (set [0] & (1 << element))
#endif

/*----------------------------------------*/

static SL_ValueT *
paramMode2maybeNull (
    tsp1_param_opt_set paramMode)
{
    SL_ValueT * result;

    if (ISMEMBER_M (paramMode, sp1ot_mandatory)) {
        /* false */
        result = SL_fromNone ();
    }
    else {
        /* true */
        result = SL_fromInt (1);
    }
    return result;
}


/*----------------------------------------*/

static SL_ValueT *
ioType2string (
    tsp1_param_io_type ioType)
{
    const char * str;
    int slen;

    switch (ioType) {
        case sp1io_input:
            str = "IN";
            slen = 2;
            break;
        case sp1io_output:
            str = "OUT";
            slen = 3;
            break;
        case sp1io_inout:
            str = "IN/OUT";
            slen = 6;
            break;
        default:
            str = "UNKNOWN";
            slen = 7;
            break;
    }
    return SL_fromString (str, slen);
}

/*----------------------------------------*/

static SQLResultT
getDescription (
    int entryCount,
    const char * nameBuf,
    tsp1_param_info * paramInfos)
{
    ROUTINE_DBG_MSP00 ("getDescription");
    SQLResultT result;
    SQLResultT tuple;
    const char * namePos;
    tsp1_param_info * paramInfo;
    int i;

    initResult (&result, 0);
    allocResult (&result, entryCount);
    namePos = nameBuf;
    for (i = 0; i < entryCount; ++i) {
        initResult (&tuple, 0);
        allocResult (&tuple, 7);
        /*
         * get name
         */
        if (namePos != NULL) {
            setResultElement (&tuple, 0, SL_fromString (namePos + 1, namePos [0]));
            namePos += namePos [0] + 1;
        }
        else {
            setResultElement (&tuple, 0, SL_fromString ("", 0));
        }
        /*
         * get type, len and frac
         */
        paramInfo = &paramInfos [i];
        setResultElement (&tuple, 1, SL_fromString (sqlTypeNamesC [paramInfo->sp1i_data_type], -1));
        setResultElement (&tuple, 2, SL_fromInt (sqlTypeCodesC [paramInfo->sp1i_data_type]));
        setResultElement (&tuple, 3, SL_fromInt (paramInfo->sp1i_length));
        setResultElement (&tuple, 4, SL_fromInt (paramInfo->sp1i_frac));
        setResultElement (&tuple, 5, paramMode2maybeNull (paramInfo->sp1i_mode));
        setResultElement (&tuple, 6, ioType2string (paramInfo->sp1i_io_type));
        /*
         * set result from tuple
         */
        setResultElement (&result, i, sqlResultC2SL_Value(&tuple));
    }
    return result;
}

/*----------------------------------------*/

static SQLResultT
getCursorDescription (
    ResultSetT * resultSet)
{
    ROUTINE_DBG_MSP00 ("getCursorDescription");
    return getDescription (resultSet->colCount,
        resultSet->colNames, resultSet->colInfo);
}

/*----------------------------------------*/

static char *
cursorName (
    ResultSetT * nself)
{
    ROUTINE_DBG_MSP00 ("cursorName");
    return nself->cursorName;
}

/*----------------------------------------*/

static void
closeResultSet (
    ResultSetT * nself)
{
    ROUTINE_DBG_MSP00 ("closeResultSet");
    int i;

    for (i = 0; i < fetchKindsE; ++i) {
        clearParseInfo (nself->session, &nself->fetchInfo [i]);
    }
    if (strlen (nself->cursorName) > 0) {
        if (nself->session->is_connected) {
            i28adbsf (nself->session, "CLOSE \"%s\"", STATIC_CAST (char*, nself->cursorName));
        }
    }
    decreaseRef (nself->session->more_data);
    if (nself->dataBuf != NULL) {
        free (nself->dataBuf);
        nself->dataBuf = NULL;
    }
    if (nself->colNames != NULL) {
        const void * tmpVoid = nself->colNames;
        free (CONST_CAST (void *, tmpVoid));
        nself->colNames = NULL;
    }
    if (nself->colInfo != NULL) {
        free (nself->colInfo);
        nself->colInfo = NULL;
    }
    nself->colCount = 0;
}

/*----------------------------------------*/

#if defined (PYTHON_GLUE)
static void
destructor_SapDB_ResultSet (
    PyObject * pyself)
{
    ROUTINE_DBG_MSP00 ("destructor_SapDB_ResultSet");
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);

    closeResultSet (&self->nself);
    PyObject_Del (pyself);
}
#endif

/*----------------------------------------*/
/* endsnippet SapDB_ResultSet Generic_precode */
/*----------------------------------------*/

static PyObject *
next_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("next_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    SQLResultT result;
    PyObject * resultObj;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = nextRow (nself);
    if (!sqlResultOK (&result) && sqlErrOccured (nself->session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, result, &resultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return resultObj;
}

/*----------------------------------------*/

static PyObject *
previous_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("previous_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    SQLResultT result;
    PyObject * resultObj;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = previousRow (nself);
    if (!sqlResultOK (&result) && sqlErrOccured (nself->session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, result, &resultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return resultObj;
}

/*----------------------------------------*/

static PyObject *
relative_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("relative_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        LIT("count"), NULL};
    int count;
    SQLResultT result;
    PyObject * resultObj;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("i:SapDB_ResultSet.relative"), (char **) kwlist, &count)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = relativeRow (nself, count);
    if (!sqlResultOK (&result) && sqlErrOccured (nself->session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, result, &resultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return resultObj;
}

/*----------------------------------------*/

static PyObject *
absolute_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("absolute_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        LIT("i"), NULL};
    int i;
    SQLResultT result;
    PyObject * resultObj;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("i:SapDB_ResultSet.absolute"), (char **) kwlist, &i)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = absoluteRow (nself, i);
    if (!sqlResultOK (&result) && sqlErrOccured (nself->session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, result, &resultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return resultObj;
}

/*----------------------------------------*/

static PyObject *
first_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("first_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    SQLResultT result;
    PyObject * resultObj;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = firstRow (nself);
    if (!sqlResultOK (&result) && sqlErrOccured (nself->session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, result, &resultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return resultObj;
}

/*----------------------------------------*/

static PyObject *
last_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("last_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    SQLResultT result;
    PyObject * resultObj;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = lastRow (nself);
    if (!sqlResultOK (&result) && sqlErrOccured (nself->session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, result, &resultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return resultObj;
}

/*----------------------------------------*/

static PyObject *
current_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("current_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    SQLResultT result;
    PyObject * resultObj;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = currentRow (nself);
    if (!sqlResultOK (&result) && sqlErrOccured (nself->session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, result, &resultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return resultObj;
}

/*----------------------------------------*/

static PyObject *
setFetchSize_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("setFetchSize_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        LIT("fetchSize"), NULL};
    int fetchSize;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("i:SapDB_ResultSet.setFetchSize"), (char **) kwlist, &fetchSize)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    setFetchSize (nself, fetchSize);
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static PyObject *
cursorName_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("cursorName_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    char * result;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = cursorName (nself);
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return Py_BuildValue (LIT("s"), result);
}

/*----------------------------------------*/

static PyObject *
columnCount_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("columnCount_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    int result;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = columnCount (nself);
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return Py_BuildValue (LIT("i"), result);
}

/*----------------------------------------*/

static PyObject *
columnInfo_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("columnInfo_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        LIT("i"), NULL};
    int i;
    SQLResultT result;
    PyObject * resultObj;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("i:SapDB_ResultSet.columnInfo"), (char **) kwlist, &i)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = columnInfo (nself, i);
    if (!sqlResultOK (&result) && sqlErrOccured (nself->session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, result, &resultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return resultObj;
}

/*----------------------------------------*/

static PyObject *
getDescription_SapDB_ResultSet (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("getDescription_SapDB_ResultSet");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultSetObjectT* self = REINTERPRET_CAST (SapDB_ResultSetObjectT*, pyself);
    ResultSetT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    SQLResultT result;
    PyObject * resultObj;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = getCursorDescription (nself);
    if (!sqlResultOK (&result) && sqlErrOccured (nself->session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, result, &resultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return resultObj;
}

static PyMethodDef SapDB_ResultSetClassMethods [] = {
    {LIT("next"), (PyCFunction)next_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "gets the next row.\n\n                    Output values are returned as a list.<BR>\n                    The end of the result set is signaled by an\n                    undefined value (<b>Python</b>: None).\n                    "},
    {LIT("previous"), (PyCFunction)previous_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "gets the previous row.\n\n                    Output values are returned as a list.<BR>\n                    The end of the result set is signaled by an\n                    undefined value (<b>Python</b>: None).\n                    "},
    {LIT("relative"), (PyCFunction)relative_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "moves relative to the current row.\n\n                    A negative value of <i>count</i> will move backwards\n                    in the ResultSet.<BR>\n                    Output values are returned as a list.<BR>\n                    The end of the result set is signaled by an\n                    undefined value (<b>Python</b>: None).\n                    "},
    {LIT("absolute"), (PyCFunction)absolute_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "gets the <i>i</i>th row.\n\n                    Output values are returned as a list.<BR>\n                    The first row has the index 1.<BR>\n                    The end of the result set is signaled by an\n                    undefined value (<b>Python</b>: None).\n                    "},
    {LIT("__getitem__"), (PyCFunction)__getitem___SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS, NULL},
    {LIT("first"), (PyCFunction)first_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "gets the first row.\n\n                    Output values are returned as a list.<BR>\n                    The end of the result set is signaled by an\n                    undefined value (<b>Python</b>: None).\n                    "},
    {LIT("last"), (PyCFunction)last_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "gets the last row.\n\n                    Output values are returned as a list.<BR>\n                    The end of the result set is signaled by an\n                    undefined value (<b>Python</b>: None).\n                    "},
    {LIT("current"), (PyCFunction)current_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "returns the current row.\n\n                    Output values are returned as a list.\n                    "},
    {LIT("setFetchSize"), (PyCFunction)setFetchSize_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "sets the number of rows to read per database communication"},
    {LIT("cursorName"), (PyCFunction)cursorName_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "returns the name of the result set."},
    {LIT("columnCount"), (PyCFunction)columnCount_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "returns the number of columns."},
    {LIT("columnInfo"), (PyCFunction)columnInfo_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "returns information about a column.\n\n                    A list of four values is returned:\n                    <OL start=\"0\">\n                    <LI>the column name\n                    <LI>the column type\n                    <LI>the length of the column\n                    <LI>the number of fractional digits\n                    </OL>\n                    The first column has the index 0;\n                    "},
    {LIT("getDescription"), (PyCFunction)getDescription_SapDB_ResultSet, METH_VARARGS|METH_KEYWORDS,
        "returns a description of the result set\n\n                    A list of tuples is returned, with the following fields\n                    <OL start=\"0\">\n                    <LI>the column name\n                    <LI>the column type as a string\n                    <LI>the column type as an ANSI/ODBC type integer\n                    <LI>the length of the column\n                    <LI>the number of fractional digits\n                    <LI>whether the column may contain NULL\n                    <LI>whether the column is IN, OUT or IN/OUT\n                    </OL>\n                    "},
    {NULL, NULL}
};


static PySequenceMethods SapDB_ResultSetSequenceVMT = {
    0,    /* sq_length */
    0,    /* sq_concat */
    0,    /* sq_repeat */
    REINTERPRET_CAST (intargfunc, absoluteRowBased0),    /* sq_item */
    0,    /* sq_slice */
    0,    /* sq_ass_item */
    0,    /* sq_ass_slice */
};

static PyObject*
SapDB_ResultSetGetattr (PyObject* self, char* name)
{
    return Py_FindMethod (SapDB_ResultSetClassMethods, self, name);
}

statichere PyTypeObject SapDB_ResultSetType = {
    PyObject_HEAD_INIT (NULL)
    0,
    LIT("SapDB_ResultSet"),    /* tp_name */
    sizeof (SapDB_ResultSetObjectT),    /* tp_basicsize */
    0,    /* tp_itemsize */
    REINTERPRET_CAST (destructor, destructor_SapDB_ResultSet),    /* tp_dealloc */
    0,    /* tp_print */
    REINTERPRET_CAST (getattrfunc, SapDB_ResultSetGetattr),    /* tp_getattr */
    0,    /* tp_setattr */
    0,    /* tp_compare */
    REINTERPRET_CAST (reprfunc, 0),    /* tp_repr */
    0,    /* tp_as_number */
    &SapDB_ResultSetSequenceVMT,    /* tp_as_sequence */
    0,    /* tp_as_mapping */
    REINTERPRET_CAST (hashfunc, 0),    /* tp_hash */
    REINTERPRET_CAST (ternaryfunc, 0),    /* tp_call */
    REINTERPRET_CAST (reprfunc, 0),    /* tp_str */
    0,    /* tp_getattro */
    0,    /* tp_setattro */
    0,    /* *tp_as_buffer */
    0,    /* tp_xxx4 */
    "allows navigation and retrieval of a result set.\n\n        Objects are created by all sql commands which create a result set\n        (SapDB_Session::sql, SapDB_Prepared::execute).\n        Python: a SapDB_ResultSet can be used as an iterator:\n        <PRE>\n        <b>for</b> row <b>in</b> resultSet:\n            ... process row ...\n        </PRE>\n        Currently, this will iterate over the result set from the beginning,\n        even if some rows have been fetched.\n        ",    /* tp_doc */
};

typedef struct SapDB_ResultObjectT {
    PyObject_HEAD
    SQLResultT nself;
/* no code for key SapDB_Result Python_/Generic_ in ['Python_precode', 'Set Generic_precode']*/
} SapDB_ResultObjectT;

staticforward PyTypeObject SapDB_ResultType;
#define isSapDB_ResultObject(op) ((op)->ob_type == &SapDB_ResultType)


static SapDB_ResultObjectT*
newSapDB_Result ()
{
    SapDB_ResultObjectT* result;

    result = PyObject_NEW (SapDB_ResultObjectT, &SapDB_ResultType);
    if (result != NULL) {
        memset (&result->nself, '\0', sizeof (SapDB_ResultObjectT) - offsetof (SapDB_ResultObjectT, nself));
    }
    return result;
}

static void
freeSapDB_Result (
    SapDB_ResultObjectT * value)
{
    Py_DECREF (value);
}
/* snippet SapDB_Result Python_precode */

/*----------------------------------------*/

static PyObject *
sqlResult2py (
    SQLResultT * result)
{
    SapDB_ResultObjectT * resultObj;
    resultObj = PyObject_NEW (SapDB_ResultObjectT, &SapDB_ResultType);
    if (resultObj != NULL) {
        memcpy (&resultObj->nself, result, sizeof (SQLResultT));
        result->resultSet = NULL;
        result->outputParameters = NULL;
        result->lastSerial = NULL;
        result->firstSerial = NULL;
    }
    return (PyObject *) resultObj;
}

/*----------------------------------------*/

static void
closeResult (
    SQLResultT * resultObj)
{
    if (resultObj->resultSet != NULL) {
        decreaseRef (resultObj->resultSet);
    }
    if (resultObj->outputParameters != NULL) {
        decreaseRef (resultObj->outputParameters);
    }
    if (resultObj->lastSerial != NULL) {
        decreaseRef (resultObj->lastSerial);
    }
    if (resultObj->firstSerial != NULL) {
        decreaseRef (resultObj->firstSerial);
    }
}

/*----------------------------------------*/

static int
sqlResultC2Py (
    void * dummy,
    SQLResultT result,
    PyObject ** pyResult)
{
    ROUTINE_DBG_MSP00 ("sqlResultC2Py");
    if (result.resultObj) {
        *pyResult = result.resultObj;
    }
    else if (result.genericResults) {
        *pyResult = sqlResult2py (&result);
    }
    else if (result.itemsSet & hasResultSetC) {
        if (result.itemsSet & hasOutputParametersC) {
            *pyResult = sqlResult2py (&result);
        }
        else {
            *pyResult = getResultSet (&result);
        }
    }
    else if (result.itemsSet & hasOutputParametersC) {
        if (result.itemsSet & hasSerialC) {
            *pyResult = sqlResult2py (&result);
        }
        else {
            *pyResult = getOutputParameters (&result);
        }
    }
    else if (result.itemsSet & hasSerialC) {
        *pyResult = sqlResult2py (&result);
    }
    else if (result.itemsSet & hasRowsAffectedC) {
        *pyResult = PyInt_FromLong  (result.rowsAffected);
    }
    else {
        *pyResult = SL_fromNone ();
    }

    closeResult (&result);
    return (*pyResult != NULL);
}

/*----------------------------------------*/

#if defined (PYTHON_GLUE)
static void
destructor_SapDB_Result (
    PyObject * pyself)
{
    ROUTINE_DBG_MSP00 ("destructor_SapDB_Result");
    SapDB_ResultObjectT* self = REINTERPRET_CAST (SapDB_ResultObjectT*, pyself);

    closeResult (&self->nself);
    PyObject_Del (pyself);
}

/*----------------------------------------*/
#endif
/* endsnippet SapDB_Result Python_precode */
/*----------------------------------------*/

static PyObject *
getResultSet_SapDB_Result (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("getResultSet_SapDB_Result");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultObjectT* self = REINTERPRET_CAST (SapDB_ResultObjectT*, pyself);
    SQLResultT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    SL_ValueT * result;

/* no code for key Python_/Generic_nativeCall */
    result = getResultSet (nself);
    /* outargs */
    return result;
}

/*----------------------------------------*/

static PyObject *
getOutputParameters_SapDB_Result (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("getOutputParameters_SapDB_Result");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultObjectT* self = REINTERPRET_CAST (SapDB_ResultObjectT*, pyself);
    SQLResultT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    SL_ValueT * result;

/* no code for key Python_/Generic_nativeCall */
    result = getOutputParameters (nself);
    /* outargs */
    return result;
}

/*----------------------------------------*/

static PyObject *
getRowsAffected_SapDB_Result (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("getRowsAffected_SapDB_Result");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultObjectT* self = REINTERPRET_CAST (SapDB_ResultObjectT*, pyself);
    SQLResultT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    long result;

/* no code for key Python_/Generic_nativeCall */
    result = getRowsAffected (nself);
    /* outargs */
    return Py_BuildValue (LIT("l"), result);
}

/*----------------------------------------*/

static PyObject *
getLastSerial_SapDB_Result (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("getLastSerial_SapDB_Result");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultObjectT* self = REINTERPRET_CAST (SapDB_ResultObjectT*, pyself);
    SQLResultT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    SL_ValueT * result;

/* no code for key Python_/Generic_nativeCall */
    result = getLastSerial (nself);
    /* outargs */
    return result;
}

/*----------------------------------------*/

static PyObject *
getFirstSerial_SapDB_Result (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("getFirstSerial_SapDB_Result");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_ResultObjectT* self = REINTERPRET_CAST (SapDB_ResultObjectT*, pyself);
    SQLResultT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    SL_ValueT * result;

/* no code for key Python_/Generic_nativeCall */
    result = getFirstSerial (nself);
    /* outargs */
    return result;
}

static PyMethodDef SapDB_ResultClassMethods [] = {
    {LIT("getResultSet"), (PyCFunction)getResultSet_SapDB_Result, METH_VARARGS|METH_KEYWORDS,
        "returns a SapDB_ResultSet if one is available, None otherwise"},
    {LIT("getOutputParameters"), (PyCFunction)getOutputParameters_SapDB_Result, METH_VARARGS|METH_KEYWORDS,
        "returns a tuple of output parameters if available, None otherwise"},
    {LIT("getRowsAffected"), (PyCFunction)getRowsAffected_SapDB_Result, METH_VARARGS|METH_KEYWORDS,
        "returns the number of rows affected by an INSERT, UPDATE or DELETE"},
    {LIT("getLastSerial"), (PyCFunction)getLastSerial_SapDB_Result, METH_VARARGS|METH_KEYWORDS,
        "returns the last serial value created by the command, None otherwise"},
    {LIT("getFirstSerial"), (PyCFunction)getFirstSerial_SapDB_Result, METH_VARARGS|METH_KEYWORDS,
        "returns the first serial value created by the command, None otherwise"},
    {NULL, NULL}
};


static PyObject*
SapDB_ResultGetattr (PyObject* self, char* name)
{
    return Py_FindMethod (SapDB_ResultClassMethods, self, name);
}

statichere PyTypeObject SapDB_ResultType = {
    PyObject_HEAD_INIT (NULL)
    0,
    LIT("SapDB_Result"),    /* tp_name */
    sizeof (SapDB_ResultObjectT),    /* tp_basicsize */
    0,    /* tp_itemsize */
    REINTERPRET_CAST (destructor, destructor_SapDB_Result),    /* tp_dealloc */
    0,    /* tp_print */
    REINTERPRET_CAST (getattrfunc, SapDB_ResultGetattr),    /* tp_getattr */
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
    "result of a SQl command\n\n        May contain result set, result count,\n        output parameters, last serial value.\n        ",    /* tp_doc */
};

typedef struct SapDB_PreparedObjectT {
    PyObject_HEAD
    PreparedT nself;
/* no code for key SapDB_Prepared Python_/Generic_ in ['Generic_precode', '::select Generic_nativeCall vars']*/
} SapDB_PreparedObjectT;

staticforward PyTypeObject SapDB_PreparedType;
#define isSapDB_PreparedObject(op) ((op)->ob_type == &SapDB_PreparedType)


static SapDB_PreparedObjectT*
newSapDB_Prepared ()
{
    SapDB_PreparedObjectT* result;

    result = PyObject_NEW (SapDB_PreparedObjectT, &SapDB_PreparedType);
    if (result != NULL) {
        memset (&result->nself, '\0', sizeof (SapDB_PreparedObjectT) - offsetof (SapDB_PreparedObjectT, nself));
    }
    return result;
}

static void
freeSapDB_Prepared (
    SapDB_PreparedObjectT * value)
{
    Py_DECREF (value);
}
/* snippet SapDB_Prepared Generic_precode */

typedef struct LongInputT {
    bool readStarted;
    bool hasData;
    unsigned short colIndex;
    tsp00_LongDescriptor descriptor;
    InputStreamT * stream;
    union {
        StringInputStreamT stringStream;
        CallableInputStreamT callableStream;
    } obj;
} LongInputT;

/*----------------------------------------*/

static void
putLongvals (
    Script_SessionT   * session,
    ParseInfoT * parseInfo,
    LongInputT * longs,
    bool forPutval)
{
    int    descriptorLen;
    int    descriptorPos;
    byte_t * dataPtr;
#if defined (PUTVAL_REQUIRES_TWO_BYTE_COUNT)
    byte_t * startPtr;
#endif
    int    bufPos;
    int    freeLen;
    int    bytesRead;
    tsp00_Int2 transferCount = 0;
    LongInputT * currentLong;
    InputStreamT * instream;
    tsp1_param_info * paramInfo;
    int i;

    if (forPutval) {
        descriptorLen = sizeof (tsp00_LongDescriptor) + 1;
    }
    else {
        descriptorLen = 0;
    }
    i28getFreePartInfo (session, REINTERPRET_CAST (void**, &dataPtr), &bufPos, &freeLen);
    freeLen -= descriptorLen; /* reserve room for end-of-long marker */
#if defined (PUTVAL_REQUIRES_TWO_BYTE_COUNT)
    startPtr = dataPtr;
    if (forPutval) {
        dataPtr += 2;
        bufPos += 2;
    }
#endif
    for (i = 0; (freeLen > descriptorLen) && (i < parseInfo->inLongCount); ++i) {
        currentLong = &longs [i];
        instream = currentLong->stream;
        paramInfo = &parseInfo->paramInfo [currentLong->colIndex];
        if (currentLong->hasData) {
            bytesRead = (*instream)->read (instream,
                            dataPtr + descriptorLen, freeLen - descriptorLen);
            if (bytesRead > 0) {
                if (bytesRead < (freeLen - descriptorLen)) {
                    if (currentLong->readStarted) {
                        BECOMES (DESCFIELD (currentLong->descriptor, ld_valmode), vm_lastdata);
                    }
                    else {
                        BECOMES (DESCFIELD (currentLong->descriptor, ld_valmode), vm_alldata);
                    }
                    currentLong->hasData = false;
                }
                else {
                    BECOMES (DESCFIELD (currentLong->descriptor, ld_valmode), vm_datapart);
                    currentLong->readStarted = true;
                }
                DESCFIELD (currentLong->descriptor, ld_valpos) = bufPos + descriptorLen;
                DESCFIELD (currentLong->descriptor, ld_vallen) = bytesRead;
                if (forPutval) {
                    descriptorPos = bufPos;
                }
                else {
                    descriptorPos = paramInfo->variant.C_true.sp1i_bufpos_F;
                }
                i28parg (session, REINTERPRET_CAST (const char*, &currentLong->descriptor),
                    sizeof (currentLong->descriptor),
                    sizeof (currentLong->descriptor) + 1,
                    descriptorPos, '\0');
                ++transferCount;
                i28extendpart (session, bytesRead);
                dataPtr += bytesRead + descriptorLen;
                bufPos += bytesRead + descriptorLen;
                freeLen -= bytesRead + descriptorLen;
            }
            else {
                currentLong->hasData = false;
            }
        }
    }
    if (forPutval && !(longs [parseInfo->inLongCount - 1].hasData)) {
        /* at end of all longs */
        currentLong = &longs [parseInfo->inLongCount - 1];
        BECOMES (DESCFIELD (currentLong->descriptor, ld_valmode), vm_last_putval);
        i28parg (session, REINTERPRET_CAST (const char*, &currentLong->descriptor),
            sizeof (currentLong->descriptor),
            sizeof (currentLong->descriptor) + 1,
            bufPos, '\0');
        /* ++transferCount; */
    }
#if defined (PUTVAL_REQUIRES_TWO_BYTE_COUNT)
    if (forPutval) {
        memcpy (startPtr, &transferCount, sizeof (transferCount));
    }
#endif
    session->part->variant.C_2.sp1p_arg_count_F += transferCount;
}

/*----------------------------------------*/

static int
doPutval (
    Script_SessionT   * session,
    ParseInfoT * parseInfo,
    LongInputT * longs)
{
    ROUTINE_DBG_MSP00 ("doPutval");
    tsp00_LongDescriptor descriptor;
    int descriptorsReturned;
    byte_t * bufPos;
    int rc;
    int i;

    /*
     *  get descriptors
     */
    i28findpart (session, sp1pk_longdata);
    descriptorsReturned = session->part->variant.C_2.sp1p_arg_count_F;
#if defined (__cplusplus)
    bufPos = (byte_t *) session->part->sp1p_buf ().asCharp ();
#else
    bufPos = session->part->variant.C_1.sp1p_buf_F;
#endif
    for (i = 0; i < descriptorsReturned; ++i) {
        memcpy (&descriptor, bufPos + 1, sizeof (descriptor));
        memcpy (&longs [DESCFIELD (descriptor, ld_valind)].descriptor,
            &descriptor, sizeof (descriptor));
    }
    /*
     * send putval
     */
    i28initspecial (session, sp1m_putval);
    i28newpart (session, sp1pk_longdata);
    putLongvals (session, parseInfo, longs, true);
    rc = requestReceive (session, false, false);
    return rc;
}

/*----------------------------------------*/

static int
setInputParameter (
    Script_SessionT   * session,
    ParseInfoT * parseInfo,
    SequenceT    args,
    LongInputT * longs)
{
    ROUTINE_DBG_MSP00 ("setInputParameter");
    tsp1_param_info * paramInfo = parseInfo->paramInfo;
    SL_ValueT  * value;
    int argCount;
    int inputIndex = -1;
    char fillChar = ' ';
    const char * addr;
    int dataLen;
    int i;
    tsp00_Number number;
    char boolBuf [1];
    int longIndex = 0;
    LongInputT * currentLong;
    int transferRC;

    if (parseInfo->inputParamCount == 0) {
        return Script_db_ok;
    }
    argCount = sequenceLength (&args);
    i28newDataPart (session, parseInfo->paramCount);
    for (i = 0; i < parseInfo->paramCount; ++i, ++paramInfo) {
        if (paramInfo->sp1i_io_type != sp1io_output) {
            ++inputIndex;
            if (inputIndex < argCount) {
                value = getSequenceElement (&args, inputIndex);
            }
            else {
                value = SL_fromNone ();
            }
            if (value == NULL) {
                /* set exception and exit */
            }
            else {
            fillChar = ' ';
            switch (paramInfo->sp1i_data_type) {
                case dfixed:
                case dfloat:
                case dvfloat:
                case dsmallint:
                case dinteger:
                    if (SL_isNone (value)) {
                        addNull (session,
                            paramInfo->sp1i_in_out_len,
                            paramInfo->variant.C_true.sp1i_bufpos_F);
                    }
                    else {
                        if (SL_getNumber (value, paramInfo, &number)) {
                            addValue (session, (char /* byte */ *) number,
                                sizeof (number),
                                paramInfo->sp1i_in_out_len,
                                paramInfo->variant.C_true.sp1i_bufpos_F, '\0');
                        }
                        else {
                            setConversionError (session, inputIndex);
                            return conversionErrorC;
                        }
                    }
                    break;
                case dchb:
                case dvarcharb:
                    fillChar = '\0';
                    /*FALLTHROUGH*/
                case dcha:
                case dche:
                case dvarchara:
                case dvarchare:
                case ddate:
                case dtime:
                case dtimestamp:
                    if (SL_isNone (value)) {
                        addNull (session,
                            paramInfo->sp1i_in_out_len,
                            paramInfo->variant.C_true.sp1i_bufpos_F);
                    }
                    else {
                        SL_getRawString (value, &addr, &dataLen);
                        addValue (session, addr, dataLen,
                            paramInfo->sp1i_in_out_len,
                            paramInfo->variant.C_true.sp1i_bufpos_F, fillChar);
                    }
                    break;
                case dunicode:
                case dvarcharuni:
                    if (SL_isNone (value)) {
                        addNull (session,
                            paramInfo->sp1i_in_out_len,
                            paramInfo->variant.C_true.sp1i_bufpos_F);
                    }
                    else {
                        const tsp77encoding * srcEncoding;
                        SL_getEncodedString (value, &addr, &dataLen,
                            &srcEncoding);
                        transferRC = i28putUCS2 (session, addr,
                            srcEncoding, dataLen,
                            paramInfo->sp1i_in_out_len,
                            paramInfo->variant.C_true.sp1i_bufpos_F);
                        if (transferRC != Script_db_ok) {
                            setConversionError (session, inputIndex);
                            return conversionErrorC;
                        }
                    }
                    break;
                case dboolean:
                    if (SL_isNone (value)) {
                        addNull (session,
                            paramInfo->sp1i_in_out_len,
                            paramInfo->variant.C_true.sp1i_bufpos_F);
                    }
                    else {
                        if (SL_isTrue (value)) {
                            boolBuf [0] = '\1';
                        }
                        else {
                            boolBuf [0] = '\0';
                        }
                        addValue (session, boolBuf, sizeof (boolBuf),
                            paramInfo->sp1i_in_out_len,
                            paramInfo->variant.C_true.sp1i_bufpos_F, '\0');
                    }
                    break;
                case dstra:
                case dstre:
                case dstrb:
                case dstruni:
                case dlonga:
                case dlonge:
                case dlongb:
                case dlonguni:
                    if (SL_isNone (value)) {
                        addNull (session,
                            paramInfo->sp1i_in_out_len,
                            paramInfo->variant.C_true.sp1i_bufpos_F);
                    }
                    else {
                        const tsp77encoding * targetEncoding;
                        switch (paramInfo->sp1i_data_type) {
                            case dstruni:
                            case dlonguni:
                                targetEncoding = sp77encodingUCS2Native;
                                break;
                            default:
                                targetEncoding = sp77encodingAscii;
                                break;
                        }
                        /*
                         * put descriptor
                         */
                        currentLong = &longs [longIndex];
                        currentLong->colIndex = i;
                        DESCFIELD (currentLong->descriptor, ld_valind) = longIndex;
                        BECOMES (DESCFIELD (currentLong->descriptor, ld_valmode), vm_nodata);
                        addValue (session, REINTERPRET_CAST (const char*, &currentLong->descriptor),
                            sizeof (currentLong->descriptor),
                            paramInfo->sp1i_in_out_len,
                            paramInfo->variant.C_true.sp1i_bufpos_F, '\0');
                        /*
                         * convert value to stream
                         */
                        if (SL_isCallable (value)) {
                            currentLong->stream = initCallableInputStream (
                                &currentLong->obj.callableStream,
                                value, targetEncoding);
                            currentLong->hasData = true;
                        }
                        else {
                            const tsp77encoding * srcEncoding;
                            SL_getEncodedString (value, &addr, &dataLen,
                                &srcEncoding);
                            currentLong->stream = initStringInputStream (
                                &currentLong->obj.stringStream,
                                srcEncoding, targetEncoding,
                                addr, dataLen);
                            currentLong->hasData = true;
                        }
                        /*
                         *
                         */
                        ++longIndex;
                    }
                    break;
            }
            decreaseRef (value);
        }
    }
    }
    if (longs != NULL) {
        putLongvals (session, parseInfo, longs, false);
    }
    return Script_db_ok;
}

/*----------------------------------------*/

static bool
fillParamInfos (
    Script_SessionT   * session,
    ParseInfoT * parseInfo,
    bool initial)
{
    ROUTINE_DBG_MSP00 ("fillParamInfos");
    bool ok = true;
    int infoSize;
    int i;

    i28gparseid (session, parseInfo->pid);
    parseInfo->paramCount = i28paramcount (session);
    if (initial) {
        parseInfo->paramInfo = NULL;
    }
    if (parseInfo->paramCount > 0) {
        infoSize = parseInfo->paramCount * sizeof (tsp1_param_info);
        if (initial) {
            parseInfo->paramInfo = REINTERPRET_CAST (tsp1_param_info*, malloc (infoSize));
        }
        if (parseInfo->paramInfo == NULL) {
            ok = false;
        }
        else {
            memcpy (parseInfo->paramInfo, i28gparaminfo (session, 0), infoSize);
        }
    }
    parseInfo->inputParamCount = 0;
    parseInfo->outputParamCount = 0;
    parseInfo->inLongCount = 0;
    parseInfo->outLongCount = 0;
    for (i = 0; i < parseInfo->paramCount; ++i) {
        tsp1_param_info * paramInfo = &parseInfo->paramInfo [i];
        switch (paramInfo->sp1i_io_type) {
            case sp1io_input:
                ++parseInfo->inputParamCount;
                break;
            case sp1io_output:
                ++parseInfo->outputParamCount;
                break;
            case sp1io_inout:
                ++parseInfo->inputParamCount;
                ++parseInfo->outputParamCount;
                break;
            default:
                break;
        }
        switch (paramInfo->sp1i_data_type) {
            case dstra:
            case dstre:
            case dstrb:
            case dstruni:
            case dlonga:
            case dlonge:
            case dlongb:
            case dlonguni:
                switch (paramInfo->sp1i_io_type) {
                    case sp1io_input:
                        ++parseInfo->inLongCount;
                        break;
                    case sp1io_output:
                        ++parseInfo->outLongCount;
                        break;
                    case sp1io_inout:
                        ++parseInfo->inLongCount;
                        ++parseInfo->outLongCount;
                        break;
                }
                break;
        }
    }
    return ok;
}

/*----------------------------------------*/

static int
prepare (
    Script_SessionT   * session,
    PreparedT * prepared,
    const char * cmd,
    bool initial)
{
    ROUTINE_DBG_MSP00 ("prepare");
    int rc = 0;
    int cmdLen = strlen (cmd);
    int retryCount = 3;

    prepared->session = session;
    increaseRef (session->more_data);
    prepared->parseInfo.paramCount = -1;
    do {
        i28initparse (prepared->session);
        i28pcmd (prepared->session, cmd, cmdLen);
        if (getAutocommit (session)) {
            session->segment->variant.C_3.sp1c_commit_immediately_F = true;
        }
        rc = requestReceive (prepared->session, true, true);
        --retryCount;
    } while ((rc == Script_db_parse_again) && (retryCount > 0));
    if (rc == Script_db_ok) {
        fillParamInfos (session, &prepared->parseInfo, initial);
        if (initial) {
            prepared->parseInfo.sqlString = (char *) malloc (cmdLen + 1);
            if (prepared->parseInfo.sqlString != NULL) {
                memcpy (prepared->parseInfo.sqlString, cmd, cmdLen + 1);
            }
        }
    }
    return rc;
}

/*----------------------------------------*/

static int
prepareFetch (
    Script_SessionT   * session,
    PreparedT * prepared)
{
    ROUTINE_DBG_MSP00 ("prepareFetch");
    char fetchCmd [100];
    int nameLen;
    int pos;

    strcpy (fetchCmd, "FETCH  ");
    pos = strlen ( fetchCmd);
    nameLen = sizeof (fetchCmd) - pos;
    if (i28gresulttablename (session, &nameLen, fetchCmd + pos)) {
        fetchCmd [pos - 1] = '"';
        fetchCmd [pos + nameLen] = '"';
        fetchCmd [pos + nameLen + 1] = '\0';
    }
    return prepare (session, prepared, fetchCmd, true);
}

/*----------------------------------------*/

static bool
packet2result (
    Script_SessionT   * session,
    int rc,
    ParseInfoT * parseInfo,
    SQLResultT * result)
{
    ROUTINE_DBG_MSP00 ("packet2result");

    if ((rc != 0) && (rc != 100)) {
        result->isOK = false;
        return false;
    }
    if (rc == 100) {
        session->lasterr.sqlresult.returnCode = 0;
    }
    if (i28findpart (session, sp1pk_resultcount)) {
        result->rowsAffected = i28resultcount (session);
        result->itemsSet |= hasRowsAffectedC;
    }
    if (i28findpart (session, sp1pk_serial)) {
        setSerialEntries (session, result);
    }
    if (i28findpart (session, sp1pk_resulttablename)
        && (session->segment->variant.C_4.sp1r_function_code_F != csp1_create_procedure_fc))
    {
        createResultSetFromReply (session, result);
    }
    if (i28findpart (session, sp1pk_data)) {
        ParseInfoT localInfo;
        bool requireDeallocation = false;
        if (parseInfo == NULL) {
            parseInfo = &localInfo;
            if (fillParamInfos (session, parseInfo, true)) {
                requireDeallocation = true;
            }
            else {
                parseInfo->paramInfo = NULL;
            }

        }
        i28findpart (session, sp1pk_data);
        if (parseInfo->paramInfo != NULL) {
            byte_t * argsbuf = (byte_t *) i28argsbuf (session);
            getOutputParameter2 (session, argsbuf, argsbuf, parseInfo, result);
            if (requireDeallocation) {
                free (localInfo.paramInfo);
            }
            result->outputParameters = result->resultObj;
            result->resultObj = NULL;
            result->itemsSet |= hasOutputParametersC;
        }
        else {
            int partLen = i28partlen (session);
            char * partPtr = i28getpart (session);
            SL_ValueT * resultValue = SL_fromString (partPtr, partLen);
            setResult (result, resultValue);
        }
    }
    return true;
}

/*----------------------------------------*/

static SQLResultT
executePrepared (
    PreparedT * prepared,
    SequenceT   args)
{
    ROUTINE_DBG_MSP00 ("executePrepared");
    SQLResultT   result;
    LongInputT * longs = NULL;
    int          rc = Script_db_ok;
    ParseInfoT * parseInfo = &prepared->parseInfo;
    int          retryCount = 3;

    initResult (&result, 0);
    result.genericResults = isGenericResultsEnabled (prepared->session);
    if (parseInfo->inLongCount > 0) {
        longs = REINTERPRET_CAST (LongInputT*, calloc (parseInfo->inLongCount,
                    sizeof (LongInputT)));
    }
    do {
        if (rc == Script_db_parse_again) {
            --retryCount;
            rc = prepare (prepared->session, prepared,
                prepared->parseInfo.sqlString, false);
            if (rc != Script_db_ok) {
                break;
            }
        }
        clearLastPacketError (prepared->session);
        i28initexecute (prepared->session, parseInfo->pid);
        prepared->session->segment->variant.C_3.sp1c_with_info_F = true;
        prepared->session->segment->variant.C_3.sp1c_commit_immediately_F
            = getAutocommit (prepared->session);
        addCursorName (prepared->session);
        rc = setInputParameter (prepared->session, parseInfo, args, longs);
        if (rc != Script_db_ok) {
            result.isOK = false;
            return result;
        }
        rc = requestReceive (prepared->session, true, true);
    } while ((rc == Script_db_parse_again) && (retryCount > 0));
    packet2result (prepared->session, rc, parseInfo, &result);
    if (longs != NULL) {
        while (longs [parseInfo->inLongCount - 1].hasData
               && (rc == Script_db_ok)) {
            rc = doPutval (prepared->session, parseInfo, longs);
        }
    }
    if (longs != NULL) {
        free (longs);
    }
    return result;
}

/*----------------------------------------*/

static SQLResultT
nextPrepared (
    PreparedT * prepared)
{
    ROUTINE_DBG_MSP00 ("nextPrepared");
    SequenceT  args;
    args = EmptySequenceC;
    return executePrepared (prepared, args);
}

/*----------------------------------------*/

static SQLResultT
getPreparedDescription (
    PreparedT * prepared)
{
    ROUTINE_DBG_MSP00 ("getPreparedDescription");
    return getDescription (prepared->parseInfo.paramCount, NULL,
        prepared->parseInfo.paramInfo);
}

/*----------------------------------------*/

static void
closePrepared (
    PreparedT * nself)
{
    ROUTINE_DBG_MSP00 ("closePrepared");
    clearParseInfo (nself->session, &nself->parseInfo);
    decreaseRef (nself->session->more_data);
}

/*----------------------------------------*/

#if defined (PYTHON_GLUE)
static void
destructor_SapDB_Prepared (
    PyObject * pyself)
{
    ROUTINE_DBG_MSP00 ("destructor_SapDB_Prepared");
    SapDB_PreparedObjectT* self = REINTERPRET_CAST (SapDB_PreparedObjectT*, pyself);

    closePrepared (&self->nself);
    PyObject_Del (pyself);
}

/*----------------------------------------*/
#endif

/* endsnippet SapDB_Prepared Generic_precode */
/*----------------------------------------*/

static PyObject *
execute_SapDB_Prepared (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("execute_SapDB_Prepared");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_PreparedObjectT* self = REINTERPRET_CAST (SapDB_PreparedObjectT*, pyself);
    PreparedT* nself = &self->nself;
    static const char * kwlist [] = {
        LIT("sqlParms"), NULL};
    SequenceT sqlParms =  EmptySequenceC;
    PyObject * sqlParmsObj = NULL;
    SQLResultT result;
    PyObject * resultObj;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("|O:SapDB_Prepared.execute"), (char **) kwlist, &sqlParmsObj)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!sequencePy2C (self, sqlParmsObj, &sqlParms)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = executePrepared (nself, sqlParms);
    if (!sqlResultOK (&result) && sqlErrOccured (nself->session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, result, &resultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return resultObj;
}

/*----------------------------------------*/

static PyObject *
getDescription_SapDB_Prepared (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("getDescription_SapDB_Prepared");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_PreparedObjectT* self = REINTERPRET_CAST (SapDB_PreparedObjectT*, pyself);
    PreparedT* nself = &self->nself;
    static const char * kwlist [] = {
        NULL};
    SQLResultT result;
    PyObject * resultObj;

    if (!isSessionConnected (self->nself.session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = getPreparedDescription (nself);
    if (!sqlResultOK (&result) && sqlErrOccured (nself->session, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, result, &resultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return resultObj;
}

static PyMethodDef SapDB_PreparedClassMethods [] = {
    {LIT("execute"), (PyCFunction)execute_SapDB_Prepared, METH_VARARGS|METH_KEYWORDS,
        "execute statement with arguments.\n\n\n                    Output values are returned as a list.\n                    If the command creates a result set, then a\n                    ResultSet is returned.<BR>\n                    <B>Perl:</B> the arguments must be specified as\n                    a Reference to an array <i>[$a, $b, ...]</i>, not\n                    an array itself <i>($a, $b, ...)</i>.\n                    <B>Python:</B> the arguments must be specified\n                    as a list."},
    {LIT("getDescription"), (PyCFunction)getDescription_SapDB_Prepared, METH_VARARGS|METH_KEYWORDS,
        "returns a description of the parameters\n\n                    A list of tuples is returned, with the following fields\n                    <OL start=\"0\">\n                    <LI>the column name\n                    <LI>the column type as a string\n                    <LI>the column type as an ANSI/ODBC type integer\n                    <LI>the length of the column\n                    <LI>the number of fractional digits\n                    <LI>whether the parameter may contain NULL\n                    <LI>whether the parameter is IN, OUT or IN/OUT\n                    </OL>\n                    "},
    {NULL, NULL}
};


static PyObject*
SapDB_PreparedGetattr (PyObject* self, char* name)
{
    return Py_FindMethod (SapDB_PreparedClassMethods, self, name);
}

statichere PyTypeObject SapDB_PreparedType = {
    PyObject_HEAD_INIT (NULL)
    0,
    LIT("SapDB_Prepared"),    /* tp_name */
    sizeof (SapDB_PreparedObjectT),    /* tp_basicsize */
    0,    /* tp_itemsize */
    REINTERPRET_CAST (destructor, destructor_SapDB_Prepared),    /* tp_dealloc */
    0,    /* tp_print */
    REINTERPRET_CAST (getattrfunc, SapDB_PreparedGetattr),    /* tp_getattr */
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
    "allows execution of SQL command with parameters.\n\n        Objects are created with SapDB_Session::prepare.\n        ",    /* tp_doc */
};

typedef struct SapDB_SessionObjectT {
    PyObject_HEAD
    Script_SessionT *nself;
/* snippet SapDB_Session Generic_instance vars */
    Script_SessionT   session;
    bool              autocommit;
    int               cursorCount;
    bool              implicitCursorNames;
    bool              lastWasEOT;
    bool              genericResults;
    SL_ValueT       * procCallback;
/* endsnippet SapDB_Session Generic_instance vars */
} SapDB_SessionObjectT;

staticforward PyTypeObject SapDB_SessionType;
#define isSapDB_SessionObject(op) ((op)->ob_type == &SapDB_SessionType)


static SapDB_SessionObjectT*
newSapDB_Session ()
{
    SapDB_SessionObjectT* result;

    result = PyObject_NEW (SapDB_SessionObjectT, &SapDB_SessionType);
    if (result != NULL) {
        memset (&result->nself, '\0', sizeof (SapDB_SessionObjectT) - offsetof (SapDB_SessionObjectT, nself));
    }
    return result;
}

static void
freeSapDB_Session (
    SapDB_SessionObjectT * value)
{
    Py_DECREF (value);
}
/* snippet SapDB_Session Generic_precode */

static void
setAutocommit (
    Script_SessionT   * session,
    bool autocommit)
{
    ((SapDB_SessionObjectT*)session->more_data)->autocommit = autocommit;
}

/*----------------------------------------*/

static bool
getAutocommit (
    Script_SessionT   * session)
{
    if (((SapDB_SessionObjectT*)session->more_data)->autocommit) {
        return true;
    }
    else {
        return false;
    }
}

/*----------------------------------------*/

static bool
lastWasEOT (
    Script_SessionT   * session)
{
    if (((SapDB_SessionObjectT*)session->more_data)->autocommit) {
        return true;
    }
    if (((SapDB_SessionObjectT*)session->more_data)->lastWasEOT) {
        return true;
    }
    return false;
}

/*----------------------------------------*/

static void
enableCursorNames (
    Script_SessionT   * session,
    bool enable)
{
    ((SapDB_SessionObjectT*)session->more_data)->implicitCursorNames = enable;
}

/*----------------------------------------*/

static void
enableGenericResults (
    Script_SessionT   * session,
    bool enable)
{
    ((SapDB_SessionObjectT*)session->more_data)->genericResults = enable;
}

/*----------------------------------------*/

static bool
isGenericResultsEnabled (
    Script_SessionT   * session)
{
    return ((SapDB_SessionObjectT*)session->more_data)->genericResults;
}

/*----------------------------------------*/

static bool
isProcServerSession (
    Script_SessionT   * session)
{
    return ((SapDB_SessionObjectT*)session->more_data)->procCallback != NULL;
}

/*----------------------------------------*/

static int
nextCursorCount (
    Script_SessionT   * session)
{
    SapDB_SessionObjectT * self = (SapDB_SessionObjectT *) session->more_data;
    ++self->cursorCount;
    return self->cursorCount;
}

/*----------------------------------------*/

static void
addCursorName (
    Script_SessionT   * session)
{
    #define NAME_LEN_C 13
    char cursorName [NAME_LEN_C];
    int cursorCount = nextCursorCount (session);
    int index = NAME_LEN_C - 1;

    if (((SapDB_SessionObjectT *) session->more_data)->implicitCursorNames) {
        memcpy (cursorName, "PCURSOR_00000", NAME_LEN_C);
        while (cursorCount > 0) {
            cursorName [index] = (cursorCount % 10) + '0';
            cursorCount /= 10;
            --index;
        }
        i28newpart (session, sp1pk_resulttablename);
        i28addpartdata (session, cursorName, NAME_LEN_C);
    }
    #undef NAME_LEN_C
}

/*----------------------------------------*/

static void
clearEOTFlag (
    Script_SessionT   * session)
{
    ((SapDB_SessionObjectT*)session->more_data)->lastWasEOT = false;
}

/*----------------------------------------*/

static void
setEOTFlag (
    Script_SessionT   * session)
{
    ((SapDB_SessionObjectT*)session->more_data)->lastWasEOT = true;
}

/*----------------------------------------*/



static boolean
initializeRTE ()
{
    ROUTINE_DBG_MSP00 ("initializeRTE");
    tsp00_CompName compName;

    c2p (compName, sizeof (compName), "R SAP DB Scripting");
    sqlinit (compName, NULL);
    return true;
}

/*----------------------------------------*/

static void
fillConnectInfo (
    Script_ConnectInfoT * info,
    const char * user,
    const char * pwd,
    const char * dbname,
    const char * node)
{
    ROUTINE_DBG_MSP00 ("fillConnectInfo");
    tsp00_Name clearPwd;
    int i = 0;

    setStringcConfig (info->username, sizeof (info->username), user, -1);
    c2p (clearPwd, sizeof (clearPwd), pwd);
    for (i = 0; i < sizeof (clearPwd); ++i) {
        clearPwd [i] = toupper (clearPwd [i]);
    }
    s02applencrypt (clearPwd, info->password);
    setStringcConfig (info->dbname, sizeof (info->dbname), dbname, -1);
    setStringcConfig (info->node, sizeof (info->node), node, -1);
    setStringcConfig (info->sqlmode, sizeof (info->sqlmode), "INTERNAL", -1);
    info->cachelimit = -1;
    info->timeout = -1;
    info->isolation = -1;
}

/*----------------------------------------*/

static void
localRelease (
    Script_SessionT  * nself)
{
    ROUTINE_DBG_MSP00 ("localRelease");

    if ((nself != NULL) && (nself->is_connected)) {
        i28release (nself, 0);
    }
}

/*----------------------------------------*/


#if defined (PYTHON_GLUE)
static void
destructor_SapDB_Session (
    PyObject * pyself)
{
    ROUTINE_DBG_MSP00 ("destructor_SapDB_Session");
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);

    localRelease (self->nself);
    PyObject_Del (pyself);
}

/*----------------------------------------*/
#endif

static int
sql (
    Script_SessionT   * session,
    const char * cmd,
    SQLResultT * result)
{
    ROUTINE_DBG_MSP00 ("sql");
    int sqlrc;
    int retryCount = 3;

    initResult (result, 0);
    result->genericResults = isGenericResultsEnabled (session);
    do {
        i28initadbs (session);
        session->segment->variant.C_3.sp1c_with_info_F = true;
        i28pcmd (session, cmd, UNDEF_SP00);
        if (getAutocommit (session)) {
            session->segment->variant.C_3.sp1c_commit_immediately_F = true;
        }
        addCursorName (session);
        sqlrc = requestReceive (session, true, true);
        --retryCount;
    } while ((sqlrc == Script_db_parse_again) && (retryCount > 0));
    packet2result (session, sqlrc, NULL, result);
    return sqlrc;
}

/*----------------------------------------*/

static int
sqlX (
    Script_SessionT   * session,
    const char * cmd,
    SequenceT parms,
    SQLResultT * result)
{
    ROUTINE_DBG_MSP00 ("sqlX");
    int sqlrc;
    PreparedT prepared;

    memset (&prepared, '\0', sizeof (prepared));
    sqlrc = prepare (session, &prepared, cmd, true);
    if (sqlrc == 0) {
        *result = executePrepared (&prepared, parms);
        if (!sqlResultOK (result)) {
            sqlrc = session->lasterr.sqlresult.returnCode;
        }
        closePrepared (&prepared);
    }
    else {
        result->isOK = false;
    }
    return sqlrc;
}

/*----------------------------------------*/

static int
commit (
    Script_SessionT   * session)
{
    int result = i28adbs_noinfo (session, "COMMIT WORK", false);
    setEOTFlag (session);
    return result;
}

/*----------------------------------------*/

static int
rollback (
    Script_SessionT   * session)
{
    int result = i28adbs_noinfo (session, "ROLLBACK WORK", false);
    setEOTFlag (session);
    return result;
}

/*----------------------------------------*/

#if defined (PERL_GLUE)
static void
increaseRef (
    void * obj)
{
    ROUTINE_DBG_MSP00 ("increaseRef");
    SapDB_SessionObjectT * perlSession =
        REINTERPRET_CAST (SapDB_SessionObjectT*, obj);

    ++perlSession->refCount;
}

/*----------------------------------------*/

static void
decreaseRef (
    void * obj)
{
    ROUTINE_DBG_MSP00 ("decreaseRef");
    SapDB_SessionObjectT * perlSession =
        REINTERPRET_CAST (SapDB_SessionObjectT*, obj);

    --perlSession->refCount;
    if (perlSession->refCount < 0) {
        localRelease (perlSession->nself);
        freeSapDB_Session (perlSession);
    }
}

#endif
/*----------------------------------------*/

static SL_ValueT *
readIdField (
    tsp00_Byte ** current)
{
    tsp00_Byte * start = *current;
    SL_ValueT * result = NULL;
    int length = start [0] * 256 + start [1];
    result = SL_fromString ((char *) start + 2, length);
    *current += 2 + length;
    return result;
}

/*----------------------------------------*/

static SL_ValueT *
getProcId (
    Script_SessionT    * csession,
    bool               * ok)
{
    tsp00_Byte * rawdata;
    SL_ValueT  * pyname;
    SL_ValueT  * createDate;
    SL_ValueT  * tuple = NULL;
    if (!ok) {
        return NULL;
    }
    i28findpart (csession, sp1pk_procid);
    rawdata = (tsp00_Byte*) i28argsbuf (csession);
    pyname = readIdField (&rawdata);
    createDate = readIdField (&rawdata);
    if ((pyname != NULL) && (createDate != NULL)) {
        tuple = Py_BuildValue ("(OO)", pyname, createDate);
        if (tuple == NULL) {
            *ok = false;
        }
    }
    else {
        *ok = false;
    }
    SL_forget (pyname);
    SL_forget (createDate);
    return tuple;
}

/*----------------------------------------*/

static void
getProcInfo (
    Script_SessionT    * csession,
    ParseInfoT         * procInfo,
    bool               * ok)
{
    int i;
    tsp1_param_info * paramInfo;

    if (!ok) {
        return;
    }
    procInfo->paramInfo = NULL;
    procInfo->paramCount = i28paramcount (csession);
    if (procInfo->paramCount > 0) {
        procInfo->paramInfo = calloc (sizeof (tsp1_param_info), procInfo->paramCount);
        if (procInfo->paramInfo == NULL) {
            *ok = false;
            return;
        }
        memcpy (procInfo->paramInfo, i28argsbuf (csession),
            sizeof (tsp1_param_info) * procInfo->paramCount);
        procInfo->inputParamCount = 0;
        procInfo->outputParamCount = 0;
        for (i = 0, paramInfo = procInfo->paramInfo; i < procInfo->paramCount; ++i, ++paramInfo) {
            switch (paramInfo->sp1i_io_type) {
                case sp1io_input:
                    paramInfo->sp1i_io_type = sp1io_output;
                    ++procInfo->outputParamCount;
                    break;
                case sp1io_output:
                    paramInfo->sp1i_io_type = sp1io_input;
                    ++procInfo->inputParamCount;
                    break;
                case sp1io_inout:
                    ++procInfo->inputParamCount;
                    ++procInfo->outputParamCount;
                    break;
                default:
                    break;
            }
        }
    }
}

/*----------------------------------------*/

static SL_ValueT *
getProcCallArgs (
    Script_SessionT    * csession,
    ParseInfoT         * procInfo,
    bool               * ok)
{
    SQLResultT result;
    byte_t    * dataBuf;

    if (!ok) {
        return NULL;
    }
    i28findpart (csession, sp1pk_data);
    initResult (&result, 0);
    dataBuf = (byte_t *) i28argsbuf (csession);
    getOutputParameter2 (csession, dataBuf, dataBuf,
        procInfo, &result);
    if (result.resultObj == NULL) {
        *ok = false;
    }
    return result.resultObj;
}

/*----------------------------------------*/

static int
asErrorCode (
    SL_ValueT * value)
{
    if (value == NULL) {
        return 0;
    }
    if (SL_isNone (value)) {
        return 0;
    }
    if (PyInt_Check (value)) {
        return PyInt_AS_LONG (value);
    }
    return 0;
}

/*----------------------------------------*/

static void
buildCallErrorReply (
    Script_SessionT      * csession,
    int                    errorCode,
    const char           * message,
    int                    msglen)
{
    i28initprocreply (csession);
    i28seterrorinfo (csession, errorCode, message, msglen);
}

/*----------------------------------------*/

/*
 * pick the tuple (returnvalues, errorMode, errorMessage) apart
 *  and decide whether to return
 *   - the error given by the sproc
 *   - a general error because the interface wasn't followed
 *   - the actual values
 */
static void
buildCallArgumentReply (
    Script_SessionT      * csession,
    ParseInfoT           * procInfo,
    SL_ValueT            * resultTuple)
{
    SequenceT tuple;
    SL_ValueT * errorObject;
    int errorCode;
    SL_ValueT * returnValues;

    sequencePy2C (NULL, resultTuple, &tuple);
    /*
     * get error code item
     */
    errorObject = getSequenceElement (&tuple, 1);
    errorCode = asErrorCode (errorObject);
    SL_forget (errorObject);
    if (errorCode != 0) {
        const char * raw = "";
        int          len = 0;
        SL_ValueT  * messageObject = getSequenceElement (&tuple, 2);
        if (messageObject != NULL) {
            SL_getRawString (messageObject, &raw, &len);
        }
        buildCallErrorReply (csession, errorCode, raw, len);
        return;
    }
    /*
     * check output count
     */
    returnValues = getSequenceElement (&tuple, 0);
    if (returnValues == NULL) {
        buildCallErrorReply (csession, 1111, "invalid return values", -1);
        return;
    }
    sequencePy2C (NULL, returnValues, &tuple);
    if (sequenceLength (&tuple) > procInfo->inputParamCount) {
        buildCallErrorReply (csession, 1111, "too many return values", -1);
        return;
    }
    /*
     * copy args to packet
     */
    i28initprocreply (csession);
    setInputParameter (csession, procInfo, tuple, NULL);
}

/*----------------------------------------*/

/*
 * sqlareceive a proc call, call the callback
 *  and pass the result values back via sqlarequest
 */
static tsp01_CommErr
handleProcCall (
    Script_SessionT      * csession,
    SapDB_SessionObjectT * pysession,
    tsp00_ErrTextc         errtext,
    bool                 * inSession)
{
    tsp01_CommErr      connRC;
    tsp00_Int4         recvLength;
    SL_ValueT        * procid = NULL;
    ParseInfoT         procInfo;
    SL_ValueT        * callArgs = NULL;
    SL_ValueT        * resultTuple = NULL;
    bool               ok = true;
    char               message [128];

    strcpy (message, "call failure");
    /*
     * sqlareceive
     */
    sqlareceive (csession->reference, (void**)&csession->rec_packet,
        &recvLength, *(tsp00_ErrText *) &errtext, &connRC);
    if (connRC != va_ok) {
        return connRC;
    }
    if (recvLength == 0) {
        *inSession = false;
        return va_ok;
    }
    i28resetpackets (csession, false);
    /*
     * get id, sfields, input parameter
     */
    procid = getProcId (csession, &ok);
    getProcInfo (csession, &procInfo, &ok);
    callArgs = getProcCallArgs (csession, &procInfo, &ok);
    /*
     * call callback
     */
    if (ok) {
        resultTuple = PyObject_CallFunction (pysession->procCallback, LIT("(OOO)"),
            pysession, procid, callArgs);
        if (resultTuple == NULL) {
            SL_ValueT * exception = PyErr_Occurred ();

            if (exception) {
                const char * rawdata;
                int          rawlen;
                SL_getRawString (exception, &rawdata, &rawlen);
                sprintf (message, "%.127s", rawdata);
            }
            ok = false;
        }
    }
    /*
     * build reply packet
     */
    if (ok) {
        buildCallArgumentReply (csession, &procInfo, resultTuple);
    }
    else {
        /* TODO: look for exception */
        buildCallErrorReply (csession, 1111, message, -1);
    }
    /*
     * delete temporary objects
     */
    SL_forget (procid);
    if (procInfo.paramInfo != NULL) {
        free (procInfo.paramInfo);
    }
    SL_forget (callArgs);
    SL_forget (resultTuple);
    /*
     * send reply packet
     */
    i28closesend (csession);
    sqlarequest (csession->reference, csession->send_packet,
        i28packetlen (csession), *(tsp00_ErrText *) &errtext, &connRC);
    /*
     * return
     */
    return connRC;
}

/*----------------------------------------*/

/*
 *  connect to database session and enter a loop,
 *    calling callback (session, id, inparms) for each
 *    procedure call
 */
static tsp00_Int4
handleProcCalls (
    const char    * url,
    SL_ValueT     * callback,
    tsp00_ErrTextc  errtext)
{
    tsp00_Int4          rc = va_ok;
    Script_SessionT   * csession;
    tsp00_Int4          packetSize;
    void              * packetList[1];
    tsp01_CommErr       connRC;
    SapDB_SessionObjectT * pysession = NULL;
    bool                   inSession = true;

    /*
     * create session
     */
    /* python object */
    pysession = newSapDB_Session ();
    pysession->nself = &pysession->session;
    i28initsession (pysession->nself, NULL);
    pysession->session.lasterr.lasterr_on = true;
    pysession->nself->more_data = pysession;
    csession = &pysession->session;
    /* c session */
    connRC = SqlAConnect (
        "", (char *) url, srvUser_esp01, 1, &csession->reference,
        &packetSize, packetList, errtext);
    if (connRC != va_ok) {
        destructor_SapDB_Session ((SL_ValueT *) pysession);
        return connRC;
    }
    csession->send_packet = packetList [0];
    csession->is_connected = true;
    /*
     * store callback for recursive calls
     */
    pysession->procCallback = callback;
    /*
     * send initial packet to wait for first proc request
     */
    sqlarequest (csession->reference, csession->send_packet, 0,
        *(tsp00_ErrText *) &errtext, &connRC);
    /*
     * handle actual calls
     */
    while (inSession && (rc == 0)) {
        rc = handleProcCall (csession, pysession,
            errtext, &inSession);
    }
    /*
     * drop session
     */
    sqlarelease (csession->reference);
    csession->is_connected = false;
    destructor_SapDB_Session ((SL_ValueT *) pysession);
    /*
     * return
     */
    return rc;
}

/* endsnippet SapDB_Session Generic_precode */
/*----------------------------------------*/

static PyObject *
sql_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("sql_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        LIT("cmd"), NULL};
    char * cmd;
    SQLResultT sqlResult;
    PyObject * sqlResultObj;
    int result;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("s:SapDB_Session.sql"), (char **) kwlist, &cmd)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = sql (nself, cmd, &sqlResult);
    if ((result != 0) && sqlErrOccured (nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, sqlResult, &sqlResultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return sqlResultObj;
}

/*----------------------------------------*/

static PyObject *
sqlX_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("sqlX_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        LIT("cmd"), LIT("parms"), NULL};
    char * cmd;
    SequenceT parms =  EmptySequenceC;
    PyObject * parmsObj = NULL;
    SQLResultT sqlResult;
    PyObject * sqlResultObj;
    int result;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("s|O:SapDB_Session.sqlX"), (char **) kwlist, &cmd, &parmsObj)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!sequencePy2C (self, parmsObj, &parms)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = sqlX (nself, cmd, parms, &sqlResult);
    if ((result != 0) && sqlErrOccured (nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    if (!sqlResultC2Py (self, sqlResult, &sqlResultObj)) {
        ok = 0;
        goto cleanup_label;
    }
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return sqlResultObj;
}

/*----------------------------------------*/

static PyObject *
prepare_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("prepare_SapDB_Session");  /* generated by Glue */
/* no code for key SapDB_Session::prepare Python_/Generic_ in ['Generic_nativeCall vars']*/
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    SapDB_PreparedObjectT * newObj = NULL;
    static const char * kwlist [] = {
        LIT("cmd"), NULL};
    char * cmd;
    int result;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    newObj = newSapDB_Prepared ();
    if (newObj == NULL) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("s:SapDB_Session.prepare"), (char **) kwlist, &cmd)) {
        ok = 0;
        goto cleanup_label;
    }
/* snippet SapDB_Session::prepare Generic_nativeCall */
result = prepare (nself, &newObj->nself, cmd, true);
if ((result != 0) && sqlErrOccured (nself, croakBuf)) {
    ok = 0;
    goto cleanup_label;
}
/* endsnippet SapDB_Session::prepare Generic_nativeCall */
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        if (newObj != NULL) {
            Py_DECREF (newObj);
        };
        return NULL;
    }
    return REINTERPRET_CAST (PyObject *, newObj);
}

/*----------------------------------------*/

static PyObject *
release_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("release_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        LIT("withCommit"), NULL};
    int withCommit =  0;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("|i:SapDB_Session.release"), (char **) kwlist, &withCommit)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    i28release (nself, withCommit);
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static PyObject *
commit_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("commit_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        NULL};
    int result;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = commit (nself);
    if ((result != 0) && sqlErrOccured (nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static PyObject *
rollback_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("rollback_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        NULL};
    int result;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = rollback (nself);
    if ((result != 0) && sqlErrOccured (nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static PyObject *
setAutocommit_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("setAutocommit_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        LIT("autocommit"), NULL};
    int autocommit =  1;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("|i:SapDB_Session.setAutocommit"), (char **) kwlist, &autocommit)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    setAutocommit (nself, autocommit);
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static PyObject *
getAutocommit_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("getAutocommit_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        NULL};
    int result;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = getAutocommit (nself);
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    return Py_BuildValue (LIT("i"), result);
}

/*----------------------------------------*/

static PyObject *
switch_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("switch_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        LIT("trace"), LIT("test"), NULL};
    char * trace;
    char * test;
    int result;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("ss:SapDB_Session.switch"), (char **) kwlist, &trace, &test)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = i28switch (nself, trace, test);
    if ((result != 0) && sqlErrOccured (nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static PyObject *
switchlimit_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("switchlimit_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        LIT("trace"), LIT("test"), LIT("startroutine"), LIT("stoproutine"), LIT("buflimit"), NULL};
    char * trace;
    char * test;
    char * startroutine;
    char * stoproutine;
    int buflimit;
    int result;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("ssssi:SapDB_Session.switchlimit"), (char **) kwlist, &trace, &test, &startroutine, &stoproutine, &buflimit)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = i28switchlimit (nself, trace, test, startroutine, stoproutine, buflimit);
    if ((result != 0) && sqlErrOccured (nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static PyObject *
buflimit_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("buflimit_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        LIT("buflimit"), NULL};
    int buflimit;
    int result;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("i:SapDB_Session.buflimit"), (char **) kwlist, &buflimit)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = i28buflimit (nself, buflimit);
    if ((result != 0) && sqlErrOccured (nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static PyObject *
minbuf_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("minbuf_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        NULL};
    int result;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = i28minbuf (nself);
    if ((result != 0) && sqlErrOccured (nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

/*----------------------------------------*/

static PyObject *
maxbuf_SapDB_Session (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("maxbuf_SapDB_Session");  /* generated by Glue */
/* no code for key Python_/Generic_code */
    int ok = 1;
    SapDB_SessionObjectT* self = REINTERPRET_CAST (SapDB_SessionObjectT*, pyself);
    Script_SessionT* nself = self->nself;
    static const char * kwlist [] = {
        NULL};
    int result;

    if (!isSessionConnected (self->nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Python_/Generic_nativeCall */
    result = i28maxbuf (nself);
    if ((result != 0) && sqlErrOccured (nself, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef SapDB_SessionClassMethods [] = {
    {LIT("sql"), (PyCFunction)sql_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "exeute a SQL command\n\n                Returns data values on SELECT INTO, a ResultSet on SELECT\n                and an empty list otherwise.\n                "},
    {LIT("sqlX"), (PyCFunction)sqlX_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "exeute a SQL command with arguments\n\n                implemented using parse-execute.<BR>\n                Returns data values on SELECT INTO, a ResultSet on SELECT\n                and an empty list otherwise.\n                <B>Perl:</B> the arguments must be given as\n                    a Reference to an array <i>[$a, $b, ...]</i>, not\n                    an array itself <i>($a, $b, ...)</i>."},
    {LIT("prepare"), (PyCFunction)prepare_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "parse a SQL statement, resulting in a SapDB_Prepared object"},
    {LIT("release"), (PyCFunction)release_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "closes the connection\n\n                The default is to end the session without COMMIT"},
    {LIT("commit"), (PyCFunction)commit_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "commits the current transaction."},
    {LIT("rollback"), (PyCFunction)rollback_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "aborts the current transaction."},
    {LIT("setAutocommit"), (PyCFunction)setAutocommit_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "switches autocommit on or off."},
    {LIT("getAutocommit"), (PyCFunction)getAutocommit_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "returns the current autocommit state."},
    {LIT("switch"), (PyCFunction)switch_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "switches traces on and off"},
    {LIT("switchlimit"), (PyCFunction)switchlimit_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "switches kernel traces to start at routine <i>startroutine</I>\n                and end at routine <i>stoproutine</i>"},
    {LIT("buflimit"), (PyCFunction)buflimit_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "limits buffer output in kernel traces to <i>limit</i> bytes"},
    {LIT("minbuf"), (PyCFunction)minbuf_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "sets compact buffer output (hex + char)"},
    {LIT("maxbuf"), (PyCFunction)maxbuf_SapDB_Session, METH_VARARGS|METH_KEYWORDS,
        "sets full buffer output (hex, decimal, char  [+ EBCDIC])"},
    {NULL, NULL}
};


static PyObject*
SapDB_SessionGetattr (PyObject* self, char* name)
{
    return Py_FindMethod (SapDB_SessionClassMethods, self, name);
}

statichere PyTypeObject SapDB_SessionType = {
    PyObject_HEAD_INIT (NULL)
    0,
    LIT("SapDB_Session"),    /* tp_name */
    sizeof (SapDB_SessionObjectT),    /* tp_basicsize */
    0,    /* tp_itemsize */
    REINTERPRET_CAST (destructor, destructor_SapDB_Session),    /* tp_dealloc */
    0,    /* tp_print */
    REINTERPRET_CAST (getattrfunc, SapDB_SessionGetattr),    /* tp_getattr */
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
    "allows execution of sql commands.\n\n        When the last reference to the session is dropped, the database\n        connection is released with an implicit transaction rollback.\n        ",    /* tp_doc */
};

/*----------------------------------------*/

static PyObject *
connect_sql (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("connect_sql");  /* generated by Glue */
/* no code for key connect_sql Python_/Generic_ in ['Generic_nativeCall']*/
    int ok = 1;
    SapDB_SessionObjectT * newObj = NULL;
    static const char * kwlist [] = {
        LIT("user"), LIT("pwd"), LIT("dbname"), LIT("host"), LIT("config"), NULL};
    char * user;
    char * pwd;
    char * dbname;
    char * host = LIT( "");
    char * config = LIT( "");
    tsp00_Int4 result;

    newObj = newSapDB_Session ();
    if (newObj == NULL) {
        ok = 0;
        goto cleanup_label;
    }
    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("sss|ss:sql.connect"), (char **) kwlist, &user, &pwd, &dbname, &host, &config)) {
        ok = 0;
        goto cleanup_label;
    }
/* snippet connect_sql Generic_nativeCall */
newObj->nself = &newObj->session;
i28initsession (newObj->nself, NULL);
newObj->session.lasterr.lasterr_on = true;
newObj->autocommit = false;
newObj->cursorCount = 0;
newObj->implicitCursorNames = true;
newObj->genericResults = false;
newObj->lastWasEOT = true;
newObj->procCallback = NULL;
/* set special senderid */
setStringConfig ((char *) newObj->session.senderid + sizeof (tsp00_C5),
    sizeof (tsp00_C3), "DOM", 3);
fillConnectInfo (&newObj->nself->connectInfo, user, pwd, dbname, host);
newObj->nself->more_data = newObj;
newObj->nself->variable_input = true;
parseSessionConfig (newObj->nself, config);
result = i28sqlconnect (newObj->nself);
if ((result != 0) && sqlErrOccured (newObj->nself, croakBuf)) {
    ok = 0;
    goto cleanup_label;
}
/* endsnippet connect_sql Generic_nativeCall */
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        if (newObj != NULL) {
            Py_DECREF (newObj);
        };
        return NULL;
    }
    return REINTERPRET_CAST (PyObject *, newObj);
}

/*----------------------------------------*/

static PyObject *
_buildInfo_sql (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("_buildInfo_sql");  /* generated by Glue */
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
handleProcCalls_sql (PyObject *pyself, PyObject* args, PyObject* keywds)
{
    ROUTINE_DBG_MSP00 ("handleProcCalls_sql");  /* generated by Glue */
/* no code for key handleProcCalls_sql Python_/Generic_ in []*/
    int ok = 1;
    static const char * kwlist [] = {
        LIT("url"), LIT("callback"), NULL};
    char * url;
    SL_ValueT * callback;
    tsp00_ErrTextc msg;
    tsp00_Int4 result;

    if (!PyArg_ParseTupleAndKeywords (args, keywds, LIT("sO:sql.handleProcCalls"), (char **) kwlist, &url, &callback)) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key handleProcCalls_sql Python_/Generic_ in []*/
    result = handleProcCalls (url, callback, msg);
    if (commErrOccured (result, msg, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
  cleanup_label:
    if (!ok) {
        /* Cleanup */
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef sqlModuleMethods [] = {
    {LIT("connect"), (PyCFunction)connect_sql, METH_VARARGS|METH_KEYWORDS,
        "connect to database.\n\n                  The username is case sensitive. <code>config</code>\n                  is a string of the format:<BR>\n                  <i>keyword=value[&amp;keyword=value]...</i><BR>\n                  Allowed keywords are:\n                  <DL>\n                  <DT>version<DD>the assumed version of the client (e.g. '70200')\n                  <DT>component<DD>the component string (ODB, CPC, ...)\n                  <DT>sqlmode<DD>(internal, oracle)\n                  <DT>isolation<DD>the isolation level as an integer\n                  <DT>timeout<DD>the command timeout in seconds\n                  <DT>cachelimit<DD>\n                  </DL>"},
    {LIT("_buildInfo"), (PyCFunction)_buildInfo_sql, METH_VARARGS|METH_KEYWORDS,
        "returns a string containing the release and build number"},
    {LIT("handleProcCalls"), (PyCFunction)handleProcCalls_sql, METH_VARARGS|METH_KEYWORDS,
        "Calls callback for every call to a proc or trigger.\n\n                This method returns only after the session has been ended.\n                callback takes three arguments:\n                 * a SapDB_Session to execute SQL\n                 * a db proc id: (python name, timestamp)\n                 * the input parameters\n                the callback has to return a tuple with three values\n                 * output parameters (or None if error)\n                 * the error code\n                 * the error text"},
    {NULL, NULL}
};

externC GLUEEXPORT void
initsqlInternal (const char * moduleName)
{
    ROUTINE_DBG_MSP00 ("initsql");  /* generated by Glue */
    PyObject* module;
    PyObject* dict;

    module = Py_InitModule4 ((char *) moduleName, sqlModuleMethods, "Interface to MaxDB", NULL, PYTHON_API_VERSION);
    if (module == NULL) {
        return;
    }
    dict = PyModule_GetDict (module);
    CommunicationErrorType = createExceptionKind ("sql.CommunicationError", CommunicationErrorCodeC);
    PyDict_SetItemString (dict, LIT("CommunicationError"), CommunicationErrorType);
    SQLErrorType = createExceptionKind ("sql.SQLError", SQLErrorCodeC);
    PyDict_SetItemString (dict, LIT("SQLError"), SQLErrorType);
    SapDB_LongReaderType.ob_type = &PyType_Type;
    PyDict_SetItemString (dict, LIT("SapDB_LongReader"), REINTERPRET_CAST (PyObject*, &SapDB_LongReaderType));
    SapDB_ResultSetType.ob_type = &PyType_Type;
    PyDict_SetItemString (dict, LIT("SapDB_ResultSet"), REINTERPRET_CAST (PyObject*, &SapDB_ResultSetType));
    SapDB_ResultType.ob_type = &PyType_Type;
    PyDict_SetItemString (dict, LIT("SapDB_Result"), REINTERPRET_CAST (PyObject*, &SapDB_ResultType));
    SapDB_PreparedType.ob_type = &PyType_Type;
    PyDict_SetItemString (dict, LIT("SapDB_Prepared"), REINTERPRET_CAST (PyObject*, &SapDB_PreparedType));
    SapDB_SessionType.ob_type = &PyType_Type;
    PyDict_SetItemString (dict, LIT("SapDB_Session"), REINTERPRET_CAST (PyObject*, &SapDB_SessionType));
    /* snippet Generic_initroutine */
    IFSLOW_MEO00 (ScriptingTracer::initTrace ());
    initializeRTE ();
    /* endsnippet Generic_initroutine */
    if (PyErr_Occurred ()) {
        Py_FatalError (LIT("can't initialize module sql"));
    }
}


externC GLUEEXPORT void
initsql ()
{
    initsqlInternal ("sql");
}

externC GLUEEXPORT void
initsapdb ()
{
    initsqlInternal ("sapdb");
}

externC GLUEEXPORT void
initmaxdb ()
{
    initsqlInternal ("maxdb");
}
