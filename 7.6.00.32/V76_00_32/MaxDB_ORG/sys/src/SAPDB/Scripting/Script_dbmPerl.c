/*
===========================================
   responsible:    DanielD
   description:    Interface to DB Manager
   target:         Perl
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

#ifdef __cplusplus
#include <math.h>
#include <stdlib.h>
extern "C" {
#else
#define bool char
#define HAS_BOOL 1
#endif

#define PERL_GLUE 1
#define Glue_BEGIN_ALLOW_THREADS
#define Glue_END_ALLOW_THREADS

#include "patchlevel.h"/* no check */

#if defined (ACTIVE_PERL)
    #if PATCHLEVEL < 6
        #define PERL_OBJECT
    #endif
#endif

#include "EXTERN.h"/* no check */
#include "perl.h"/* no check */
#if defined (PERL_OBJECT)
#include "perlCAPI.h"/* no check */
#endif
#include "XSUB.h"/* no check */
#undef free
#undef malloc
#undef realloc
#include <string.h>
#if defined (bool)
#undef bool
#endif
#if defined (PERL_OBJECT)
    #if !defined (PL_sv_undef)
        #define PL_sv_undef sv_undef
    #endif
    #if !defined (PL_sv_yes)
        #define PL_sv_yes sv_yes
    #endif
    #if !defined (PL_errgv)
        #define PL_errgv errgv
    #endif
#endif
#ifdef __cplusplus
}
#endif

#if defined (_WIN32)
#define GLUEEXPORT __declspec(dllexport)
#else
#define GLUEEXPORT
#endif

#define LIT(c) CONST_CAST(char *, c)
#define CROAKBUF_SIZE 200

#define MF__
#if defined (__cplusplus)
#define externC extern "C"
#else
#define externC
#endif

#define SL_None &PL_sv_undef
#define SL_isNone(val) (!SvOK (val))
#define SL_isTrue(val) SvTRUE (val)

static const char * invalidArgCount_C = "Invalid number of arguments";

static void *
getGluePointer (SV * sv, const char * className, char * croakBuf)
{
    IV   tmp;

    if (SvGMAGICAL(sv)) {
        mg_get(sv);
    }

    /* Check to see if this is an object */
    if (sv_isobject(sv)) {
        SV *tsv = (SV*) SvRV(sv);
        if ((SvTYPE(tsv) == SVt_PVHV)) {
            MAGIC *mg;
            if (SvMAGICAL(tsv)) {
                mg = mg_find(tsv,'P');
                if (mg) {
                    SV *rsv = mg->mg_obj;
                    if (sv_isobject(rsv)) {
                        tmp = SvIV((SV*)SvRV(rsv));
                    }
                }
            }
            else {
                strcpy (croakBuf, "Not a valid pointer value");
                return NULL;
            }
        }
        else {
            tmp = SvIV((SV*)SvRV(sv));
        }
    }
    else if (SL_isNone (sv)) {            /* Check for undef */
        strcpy (croakBuf, "Undef object not allowed");
        return NULL;
    }
    else if (SvTYPE(sv) == SVt_RV) {       /* Check for NULL pointer */
        if (!SvROK(sv)) {
            strcpy (croakBuf, "NULL object not allowed");
        }
        else {
            strcpy (croakBuf, "NULL object not allowed");
        }
        return NULL;
    }
    else {                                 /* Dont know what it is */
        strcpy (croakBuf, "Not a valid pointer value");
        return NULL;
    }
    /* Now see if the types match */

    if (!sv_isa (sv, (char *) className)) {
        sprintf (croakBuf, "Object is of class %s (should be %s)",
                HvNAME(SvSTASH(SvRV(sv))), className);
        return NULL;
    }
    return (void*) tmp;
}


/*
 * Exceptions
 */

static const char * CommunicationError_dbm = "CommunicationError";
static const char * DBMServError_dbm = "DBMServError";

/* snippet Perl_precode */
#define CN72 1
#undef na
#include "hsp100.h"
#include "hcn14.h"
#include "hsp77.h"
#include "hsp78_0.h"

#include "patchlevel.h"/* no check */
#if PATCHLEVEL >= 5
    #define na PL_na
#else
    #define na Perl_na
#endif

typedef SV SL_ValueT;

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

static int
errorOccured (
    int rc,
    tsp00_ErrTextc VAR_ARRAY_REF msg,
    char * croakBuf)
{
    ROUTINE_DBG_MSP00 ("errorOccured");
    if (rc != 0) {
        sprintf (croakBuf, "%s (%d): %s", CommunicationError_dbm, rc, (char *) msg);
        return 1;
    }
    else {
        return 0;
    }
}

/*----------------------------------------*/

static int
commErrOccured (int rc, char * msg, char * croakBuf)
{
    ROUTINE_DBG_MSP00 ("commErrOccured");
    if (rc != 0) {
        sprintf (croakBuf, "%s (%d): %s", CommunicationError_dbm, rc, msg);
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
    ROUTINE_DBG_MSP00 ("dbmServErrOccured");
    const char  * rawData;
    int           errOccurred = 0;
    tsp00_Int4      serverError;
    const void  * payLoad;
    tsp00_Int4      payLoadLen;
    tsp00_Int4      specificError;
    tsp00_ErrTextc errtext;


    if (rc == DBMAPI_COMMERR_CN14) {
        sprintf (croakBuf, "Communication Error: %s", msg);
        return 1;
    }
    serverError = cn14analyzeDbmAnswer (nself, &payLoad, &payLoadLen,
            &specificError, errtext);
    trimPayload (payLoad, &payLoadLen);
    if (serverError != DBMAPI_OK_CN14) {
        errOccurred = 1;
        payLoadLen = MIN_EO00 (150, payLoadLen);
        sprintf (croakBuf, "Error DBM Server: %d %s %*s",
                 specificError, (char*) errtext, payLoadLen, payLoad);
    }
    return errOccurred;
}

/*----------------------------------------*/

static int
allocFailed (void * ptr, char * croakBuf)
{
    ROUTINE_DBG_MSP00 ("allocFailed");

    if (ptr == NULL) {
        strcpy (croakBuf, "Out of memory");
        return 1;
    }
    else {
        return 0;
    }
}

/* endsnippet Perl_precode */
static const char * DBMClassID = "DBM";

typedef struct DBMObjectT {
    const char * classID;
    void *nself;
/* no code for key DBM Perl_/Generic_ in ['Generic_precode']*/
} DBMObjectT;

#define isDBMObject(op) ((op)->classID == DBMClassID)

static const char * notClassDBM_C = "Object not of class DBM";

static DBMObjectT *
newDBM ()
{
    DBMObjectT * result;

    Newz (0, result, 1, DBMObjectT);
    return result;
}

static void
freeDBM (
    DBMObjectT * value)
{
    Safefree (value);
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

externC XS (DBM_cmd)
{
    ROUTINE_DBG_MSP00 ("DBM_cmd");
/* no code for key Perl_/Generic_code */
    int ok = 1;
    int argvi = 0;
    char croakBuf [CROAKBUF_SIZE];
    dXSARGS;
    DBMObjectT* self;
    void* nself;
    StringT cmd;
    StringT output;
    tsp00_ErrTextc msg;
    tsp00_Int4 result;

    memset (&cmd, '\0', sizeof (cmd));
    memset (&output, '\0', sizeof (output));
    
    cv = cv;
    if (items != 2) {
        croak (invalidArgCount_C);
    }
    self = (DBMObjectT*) getGluePointer (ST(0), DBMClassID, croakBuf);
    if ((self == NULL) || !isDBMObject (self)) {
        croak (croakBuf);
    }
    nself = self->nself;
    switch (items) {
        case 2: string2C (self, ST(1), &cmd);
    }
    if (!ok) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Perl_/Generic_nativeCall */
    result = cmdAndRead (nself, cmd, &output, msg);
    if (dbmServErrOccured (nself, result, msg, cmd.realObject, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    ST (argvi) = sv_newmortal ();
    string2Perl (self, &output, ST (argvi++));
  cleanup_label:
    stringDestructor (self, cmd);
    stringDestructor (self, output);
    if (!ok) {
        croak (croakBuf);
    }
    XSRETURN (argvi);
}

/*----------------------------------------*/

externC XS (DBM_rawCmd)
{
    ROUTINE_DBG_MSP00 ("DBM_rawCmd");
/* no code for key Perl_/Generic_code */
    int ok = 1;
    int argvi = 0;
    char croakBuf [CROAKBUF_SIZE];
    dXSARGS;
    DBMObjectT* self;
    void* nself;
    StringT cmd;
    StringT output;
    tsp00_ErrTextc msg;
    tsp00_Int4 result;

    memset (&cmd, '\0', sizeof (cmd));
    memset (&output, '\0', sizeof (output));
    
    cv = cv;
    if (items != 2) {
        croak (invalidArgCount_C);
    }
    self = (DBMObjectT*) getGluePointer (ST(0), DBMClassID, croakBuf);
    if ((self == NULL) || !isDBMObject (self)) {
        croak (croakBuf);
    }
    nself = self->nself;
    switch (items) {
        case 2: string2C (self, ST(1), &cmd);
    }
    if (!ok) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Perl_/Generic_nativeCall */
    result = rawCommand (nself, cmd, &output, msg);
    if (commErrOccured (result, msg, croakBuf)) {
        ok = 0;
        goto cleanup_label;
    }
    /* outargs */
    ST (argvi) = sv_newmortal ();
    string2Perl (self, &output, ST (argvi++));
  cleanup_label:
    stringDestructor (self, cmd);
    stringDestructor (self, output);
    if (!ok) {
        croak (croakBuf);
    }
    XSRETURN (argvi);
}

/*----------------------------------------*/

externC XS (DBM_cancelCmd)
{
    ROUTINE_DBG_MSP00 ("DBM_cancelCmd");
/* no code for key Perl_/Generic_code */
    int ok = 1;
    int argvi = 0;
    char croakBuf [CROAKBUF_SIZE];
    dXSARGS;
    DBMObjectT* self;
    void* nself;
    int result;

    
    cv = cv;
    if (items != 1) {
        croak (invalidArgCount_C);
    }
    self = (DBMObjectT*) getGluePointer (ST(0), DBMClassID, croakBuf);
    if ((self == NULL) || !isDBMObject (self)) {
        croak (croakBuf);
    }
    nself = self->nself;
/* no code for key Perl_/Generic_nativeCall */
    result = cancelCmd (nself);
    /* outargs */
    XSRETURN (argvi);
}

/*----------------------------------------*/

externC XS (DBM_release)
{
    ROUTINE_DBG_MSP00 ("DBM_release");
/* no code for key release Perl_/Generic_ in ['Generic_nativeCall']*/
    int ok = 1;
    int argvi = 0;
    char croakBuf [CROAKBUF_SIZE];
    dXSARGS;
    DBMObjectT* self;
    void* nself;

    
    cv = cv;
    if (items != 1) {
        croak (invalidArgCount_C);
    }
    self = (DBMObjectT*) getGluePointer (ST(0), DBMClassID, croakBuf);
    if ((self == NULL) || !isDBMObject (self)) {
        croak (croakBuf);
    }
    nself = self->nself;
/* snippet release Generic_nativeCall */
    localRelease (self->nself);
    self->nself = NULL;
    /* endsnippet release Generic_nativeCall */
    /* outargs */
    XSRETURN (argvi);
}


/*----------------------------------------*/

externC XS (DBM_destructor)
{
    ROUTINE_DBG_MSP00 ("DBM_destructor");
    char croakBuf [CROAKBUF_SIZE];
    dXSARGS;
    DBMObjectT* self;
    void* nself;

    cv = cv;
    if (items != 1) {
        croak (invalidArgCount_C);
    }
    self = (DBMObjectT*) getGluePointer (ST(0), DBMClassID, croakBuf);
    if ((self == NULL) || !isDBMObject (self)) {
        croak (croakBuf);
    }
    nself = self->nself;
    localRelease (nself);
    freeDBM (self);
    XSRETURN (0);
}

/*----------------------------------------*/

externC XS (dbm_DBM)
{
    ROUTINE_DBG_MSP00 ("dbm_DBM");
/* no code for key constructor Perl_/Generic_ in ['Generic_nativeCall']*/
    DBMObjectT * newObj = NULL;
    int ok = 1;
    int argvi = 0;
    char croakBuf [CROAKBUF_SIZE];
    dXSARGS;
    char * servernode = LIT( "");
    char * dbname = LIT( "");
    char * dbroot = LIT( "");
    char * userpwd =  NULL;
    void * session;
    tsp00_ErrTextc msg;
    tsp00_Int4 result;

    
    cv = cv;
    if ((items < 0) || (items > 4)) {
        croak (invalidArgCount_C);
    }
    switch (items) {
        case 4: userpwd = (char *) SvPV (ST(3), na);
        case 3: dbroot = (char *) SvPV (ST(2), na);
        case 2: dbname = (char *) SvPV (ST(1), na);
        case 1: servernode = (char *) SvPV (ST(0), na);
    }
    if (!ok) {
        ok = 0;
        goto cleanup_label;
    }
    newObj = newDBM ();
    if (allocFailed (newObj, croakBuf)) {
        ok = 0;
        newObj = NULL;
        goto cleanup_label;
    }
    newObj->classID = DBMClassID;
/* snippet constructor Generic_nativeCall */
    result = doConnect (servernode, dbname, dbroot, userpwd, &session, msg);
    newObj->nself = session;
    ok = ! errorOccured (result, msg, croakBuf);
    /* endsnippet constructor Generic_nativeCall */
    /* outargs */
    ST (argvi) = sv_newmortal ();
    sv_setref_pv (ST (argvi++), (char *)DBMClassID, (void *) newObj);
  cleanup_label:
    if (!ok) {
        if (newObj != NULL) {
            free (newObj);
        }
        croak (croakBuf);
    }
    XSRETURN (argvi);
}

/*----------------------------------------*/

externC XS (dbm_saveUser)
{
    ROUTINE_DBG_MSP00 ("dbm_saveUser");
/* no code for key Perl_/Generic_code */
    int ok = 1;
    int argvi = 0;
    char croakBuf [CROAKBUF_SIZE];
    dXSARGS;
    char * dbname;
    char * servernode;
    char * userpwd;

    
    cv = cv;
    if (items != 3) {
        croak (invalidArgCount_C);
    }
    switch (items) {
        case 3: userpwd = (char *) SvPV (ST(2), na);
        case 2: servernode = (char *) SvPV (ST(1), na);
        case 1: dbname = (char *) SvPV (ST(0), na);
    }
    if (!ok) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Perl_/Generic_nativeCall */
    doSaveUser (dbname, servernode, userpwd);
    /* outargs */
  cleanup_label:
    if (!ok) {
        croak (croakBuf);
    }
    XSRETURN (argvi);
}

/*----------------------------------------*/

externC XS (dbm_checkUser)
{
    ROUTINE_DBG_MSP00 ("dbm_checkUser");
/* no code for key Perl_/Generic_code */
    int ok = 1;
    int argvi = 0;
    char croakBuf [CROAKBUF_SIZE];
    dXSARGS;
    char * dbname;
    char * servernode;
    char * userpwd;
    tsp00_Int4 result;

    
    cv = cv;
    if (items != 3) {
        croak (invalidArgCount_C);
    }
    switch (items) {
        case 3: userpwd = (char *) SvPV (ST(2), na);
        case 2: servernode = (char *) SvPV (ST(1), na);
        case 1: dbname = (char *) SvPV (ST(0), na);
    }
    if (!ok) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Perl_/Generic_nativeCall */
    result = doCheckUser (dbname, servernode, userpwd);
    /* outargs */
    ST (argvi) = sv_newmortal ();
    sv_setiv (ST (argvi++), (IV) result);
  cleanup_label:
    if (!ok) {
        croak (croakBuf);
    }
    XSRETURN (argvi);
}

/*----------------------------------------*/

externC XS (dbm__buildInfo)
{
    ROUTINE_DBG_MSP00 ("dbm__buildInfo");
/* no code for key Perl_/Generic_code */
    int ok = 1;
    int argvi = 0;
    char croakBuf [CROAKBUF_SIZE];
    dXSARGS;
    char * result;

    
    cv = cv;
    if (items != 0) {
        croak (invalidArgCount_C);
    }
/* no code for key Perl_/Generic_nativeCall */
    result = buildInfo ();
    /* outargs */
    ST (argvi) = sv_newmortal ();
    sv_setpv (ST (argvi++), (char *) result);
    XSRETURN (argvi);
}

/*----------------------------------------*/

externC XS (dbm_deleteUser)
{
    ROUTINE_DBG_MSP00 ("dbm_deleteUser");
/* no code for key Perl_/Generic_code */
    int ok = 1;
    int argvi = 0;
    char croakBuf [CROAKBUF_SIZE];
    dXSARGS;
    char * dbname;
    char * servernode;

    
    cv = cv;
    if (items != 2) {
        croak (invalidArgCount_C);
    }
    switch (items) {
        case 2: servernode = (char *) SvPV (ST(1), na);
        case 1: dbname = (char *) SvPV (ST(0), na);
    }
    if (!ok) {
        ok = 0;
        goto cleanup_label;
    }
/* no code for key Perl_/Generic_nativeCall */
    doDeleteUser (dbname, servernode);
    /* outargs */
  cleanup_label:
    if (!ok) {
        croak (croakBuf);
    }
    XSRETURN (argvi);
}


#if defined (ACTIVE_PERL)
#define boot_SAP__DBTech__dbm boot_SAP__DBTech__dbmaperl
#else
#define boot_SAP__DBTech__dbm boot_SAP__DBTech__dbmcperl
#endif

externC GLUEEXPORT XS(boot_SAP__DBTech__dbm)
{
    dXSARGS;
    char * file = CONST_CAST (char *, __FILE__);
    cv = cv;
    items = items;
    newXS (LIT ("SAP::DBTech::dbmc::dbm_DBM"), dbm_DBM, file);
    newXS (LIT ("SAP::DBTech::dbmc::dbm_saveUser"), dbm_saveUser, file);
    newXS (LIT ("SAP::DBTech::dbmc::dbm_checkUser"), dbm_checkUser, file);
    newXS (LIT ("SAP::DBTech::dbmc::dbm__buildInfo"), dbm__buildInfo, file);
    newXS (LIT ("SAP::DBTech::dbmc::dbm_deleteUser"), dbm_deleteUser, file);
    newXS (LIT ("SAP::DBTech::dbmc::DBM_cmd"), DBM_cmd, file);
    newXS (LIT ("SAP::DBTech::dbmc::DBM_rawCmd"), DBM_rawCmd, file);
    newXS (LIT ("SAP::DBTech::dbmc::DBM_cancelCmd"), DBM_cancelCmd, file);
    newXS (LIT ("SAP::DBTech::dbmc::DBM_release"), DBM_release, file);
    newXS (LIT("SAP::DBTech::dbmc::DBM_destructor"), DBM_destructor, file);
/* no code for key Perl_/Generic_ in ['Python_precode', 'Perl_precode', 'DBM Generic_precode', 'constructor Generic_nativeCall', 'release Generic_nativeCall']*/
    ST(0) = &PL_sv_yes;
    XSRETURN(1);
}

