/*!================================================================

 module:        nativeComm.c

 -------------------------------------------------------------------

 responsible:   MarcoP

 special area:  JDBC

 description:   Native Methods to speed up some JDBC operations

 -------------------------------------------------------------------

 Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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




 ===================================================================*/

#include "extern/jni/jni.h"

#include "gsp00.h"
#include "heo102.h"
#include "heo03.h"
#include "gsp09.h"

#if 0
#define TRACEIN(str) printf ("<%s>\n", str)
#define TRACEOUT()   printf ("</>\n")
#define TRACE(args)  printf args
#else
#define TRACEIN(str) (void) 0
#define TRACEOUT()   (void) 0
#define TRACE(args)  (void) 0
#endif

typedef struct ConnectionDataT {
    tsp00_Int4 handle;
    void     * requestPacket;
    tsp00_Int4 maxRequestLen;
} ConnectionDataT;

typedef struct JavaStringT {
    jfieldID id;
    jstring obj;
    jboolean isCopy;
    const char * chars;
} JavaStringT;

/*----------------------------------------*/

static void
loadStringField (
    JavaStringT * javaString,
    JNIEnv * env,
    jclass clazz,
    jobject self,
    const char * fieldName)
{
    if (clazz == NULL) {
        clazz = (*env)->GetObjectClass (env, self);
    }
    javaString->id = (*env)->GetFieldID (env, clazz, fieldName, "Ljava/lang/String;");
    javaString->obj = (*env)->GetObjectField (env, self, javaString->id);
    javaString->chars = (*env)->GetStringUTFChars (env, javaString->obj, &javaString->isCopy);
}

/*----------------------------------------*/

static void
releaseString (
    JavaStringT * javaString,
    JNIEnv * env)
{
    if (javaString->isCopy) {
        (*env)->ReleaseStringUTFChars (env, javaString->obj, javaString->chars);
    }
}

typedef struct NativeFieldT {
    jobject obj;
    jboolean isCopy;
    ConnectionDataT * data;
} NativeFieldT;

/*----------------------------------------*/

static bool
loadNativeField (
    NativeFieldT * field,
    JNIEnv * env,
    jobject self)
{
    bool isOK;
    jclass clazz = (*env)->GetObjectClass (env, self);
    jfieldID fieldID = (*env)->GetFieldID (env, clazz, "nativeStruct", "[B");

    field->obj = (*env)->GetObjectField (env, self, fieldID);
    if (field->obj != NULL) {
        field->data = (ConnectionDataT* ) (*env)->GetByteArrayElements (env, field->obj, &field->isCopy);
        if (field->data->handle == -1) {
            isOK = false;
        }
        else {
            isOK = true;
        }
    }
    else {
        field->data = NULL;
        isOK = false;
    }
    return isOK;
}

/*----------------------------------------*/

static void
releaseNativeField (
    NativeFieldT * field,
    JNIEnv * env)
{
    if (field->isCopy) {
        (*env)->ReleaseByteArrayElements (env, field->obj, (jbyte*) field->data, 0);
    }
}

/*----------------------------------------*/

static void
throwRteException (
    JNIEnv * env,
    tsp00_ErrTextc errtext,
    tsp00_Int4 errcode,
    jboolean trim)
{
    jclass clazz;
    jmethodID mid;
    jint ival;
    jstring sval;
    jobject obj;

    if (trim) {
        char * eotext = errtext + sizeof (tsp00_ErrText) - 1;
        while ((*eotext <= ' ') && (eotext > errtext)) {
            --eotext;
        }
        eotext [1] = '\0';
    }
    clazz = (*env)->FindClass (env, "com/sap/dbtech/rte/comm/RTEException");
    mid = (*env)->GetMethodID(env,
                             clazz,
                             "<init>",
                             "(Ljava/lang/String;I)V");
    ival = errcode;
    sval = (*env)->NewStringUTF(env,errtext);
    obj  = (*env)->NewObject(env,
                             clazz,
                             mid,
                             sval,
                             ival);
    (*env)->Throw (env, obj);
}

/*----------------------------------------*/

static jobject
createStructuredBytes (
    JNIEnv * env,
    jobject byteArray)
{
    jclass clazz;
    jmethodID constructor;
    jobject result;

    clazz = (*env)->FindClass (env, "com/sap/dbtech/util/StructuredBytes");
    if ((*env)->ExceptionOccurred (env)) {
        return NULL;
    }
    constructor = (*env)->GetMethodID (env, clazz, "<init>", "([B)V");
    if ((*env)->ExceptionOccurred (env)) {
        return NULL;
    }
    result = (*env)->NewObject (env, clazz, constructor, byteArray);
    if ((*env)->ExceptionOccurred (env)) {
        return NULL;
    }
    return result;
}

/*----------------------------------------*/

static void
monitorEnter (
    JNIEnv * env,
    jclass clazz)
{
    /*printf("locking %p\n", clazz);*/
    (*env)->MonitorEnter (env, clazz);
}

/*----------------------------------------*/

static void
monitorExit (
    JNIEnv * env,
    jclass clazz)
{
    (*env)->MonitorExit (env, clazz);
    /*printf("unlocked %p\n", clazz);*/
}

/*----------------------------------------*/

static tsp00_Int4
mapErrorCode (tsp01_CommErr_Enum comm_Err)
{
    switch (comm_Err) {
      case (commErrOk_esp01):{
        return -1;
      }
      case (commErrNotOk_esp01):{
        return -807;
      }
      case (commErrTasklimit_esp01):{
        return -9807;
      }
      case (commErrTimeout_esp01):{
        return 700;
      }
      case (commErrCrash_esp01):{
        return 800;
      }
      case (commErrStartRequired_esp01):{
        return -813;
      }
      case (commErrShutdown_esp01):{
        return -708;
      }
      case (commErrSendLineDown_esp01):{
        return -708;
      }
      case (commErrReceiveLineDown_esp01):{
        return -708;
      }
      case (commErrPacketLimit_esp01):{
        return -1;
      }
      case (commErrReleased_esp01):{
        return -807;
      }
      case (commErrWouldBlock_esp01):{
        return -1;
      }
      case (commErrUnknownRequest_esp01):{
        return -1;
      }
      case (commErrServerOrDBUnknown_esp01):{
        return -709;
      }
      default:
        return -1;
    }
}

/*----------------------------------------*/

/*
 * Class:     com_sap_dbtech_rte_comm_NativeComm
 * Method:    connect
 * Signature: ()V
 */
externC JNIEXPORT void JNICALL Java_com_sap_dbtech_rte_comm_NativeComm_connect (
    JNIEnv * env,
    jobject self)
{
    static jboolean isInitialized = JNI_FALSE;
    static tsp00_CompName  sapdbcComponentC =
        "R sapdbc                                                        ";
    jclass          clazz;
    JavaStringT     host;
    JavaStringT     dbname;
    tsp01_CommErr_Enum commErr;
    tsp00_ErrTextc  errtext;
    tsp00_Int4      reference;
    tsp00_Int4      packetSize;
    void          * packetList [1];
    jbyteArray      connectionDataObject;
    jbyte         * rawArray;
    jboolean        isCopy;
    ConnectionDataT * connectionData;
    jfieldID        nativeStructID;

    TRACEIN ("connect");
    clazz = (*env)->GetObjectClass (env, self);
    monitorEnter (env, clazz);
    if (isInitialized == JNI_FALSE) {
        sqlinit (sapdbcComponentC, NULL);
        isInitialized = JNI_TRUE;
    }
    loadStringField (&host, env, clazz, self, "host");
    loadStringField (&dbname, env, clazz, self, "dbname");
    commErr = SqlAConnect ( (char *) host.chars,
        (char *)dbname.chars, srvUser_esp01, 1, &reference,
        &packetSize, packetList, errtext);
    if (commErr == commErrOk_esp01) {
        /*
         * fill instance variables
         */
        connectionDataObject = (*env)->NewByteArray (env, sizeof (ConnectionDataT));
        rawArray = (*env)->GetByteArrayElements (env, connectionDataObject, &isCopy);
        connectionData = (ConnectionDataT *) rawArray;
        connectionData->handle = reference;
        connectionData->requestPacket = packetList [0];
        connectionData->maxRequestLen = packetSize;
        if (isCopy == JNI_TRUE) {
            (*env)->ReleaseByteArrayElements (env, connectionDataObject, rawArray, 0);
        }
        nativeStructID = (*env)->GetFieldID (env, clazz, "nativeStruct", "[B");
        (*env)->SetObjectField (env, self, nativeStructID, connectionDataObject);
    }
    else {
        /*
         * create exception
         */
        throwRteException (env, errtext, mapErrorCode(commErr), JNI_FALSE);
    }
    releaseString (&host, env);
    releaseString (&dbname, env);
    monitorExit (env, clazz);
    TRACEOUT();
}

/*
 * Class:     com_sap_dbtech_rte_comm_NativeComm
 * Method:    connectX
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sap_dbtech_rte_comm_NativeComm_connectX (
    JNIEnv * env,
    jobject self)
{
    static jboolean isInitialized = JNI_FALSE;
    static tsp00_CompName  sapdbcComponentC =
        "R sapdbc                                                        ";
    jclass          clazz;
    JavaStringT     host;
    JavaStringT     dbname;
    JavaStringT     dbroot;
    JavaStringT     pgm;
    tsp01_CommErr   commErr;
    tsp00_ErrTextc  errtext;
    tsp00_Int4      reference;
    tsp00_Int4      packetSize;
    void          * packetList [1];
    jbyteArray      connectionDataObject;
    jbyte         * rawArray;
    jboolean        isCopy;
    ConnectionDataT * connectionData;
    jfieldID        nativeStructID;
    jobject         localRef;

    TRACEIN ("connect");
    clazz = (*env)->GetObjectClass (env, self);
    monitorEnter (env, clazz);
    if (isInitialized == JNI_FALSE) {
        sqlinit (sapdbcComponentC, NULL);
        isInitialized = JNI_TRUE;
    }
    loadStringField (&host, env, clazz, self, "host");
    loadStringField (&dbname, env, clazz, self, "dbname");
    loadStringField (&dbroot, env, clazz, self, "dbroot");
    loadStringField (&pgm, env, clazz, self, "pgm");
    sqlx2connectc (0,
                (char *) host.chars,
                (char *) dbname.chars,
                (char *) dbroot.chars,
                (char *) pgm.chars,
                &reference,
                &packetSize,
                packetList,
                errtext,
                &commErr);
    if (commErr == commErrOk_esp01) {
        /*
         * fill instance variables
         */
        connectionDataObject = (*env)->NewByteArray (env, sizeof (ConnectionDataT));
        rawArray = (*env)->GetByteArrayElements (env, connectionDataObject, &isCopy);
        connectionData = (ConnectionDataT *) rawArray;
        connectionData->handle = reference;
        connectionData->requestPacket = packetList [0];
        connectionData->maxRequestLen = packetSize;
        if (isCopy == JNI_TRUE) {
            (*env)->ReleaseByteArrayElements (env, connectionDataObject, rawArray, 0);
        }
        nativeStructID = (*env)->GetFieldID (env, clazz, "nativeStruct", "[B");
        (*env)->SetObjectField (env, self, nativeStructID, connectionDataObject);
    }
    else {
        /*
         * create exception
         */
        throwRteException (env, errtext, mapErrorCode(commErr), JNI_FALSE);
    }
    releaseString (&host, env);
    releaseString (&dbname, env);
    releaseString (&dbroot, env);
    releaseString (&pgm, env);
    monitorExit (env, clazz);
    TRACEOUT();
}


/*
 * Class:     com_sap_dbtech_rte_comm_NativeComm
 * Method:    cancel
 * Signature: ()V
 */
externC JNIEXPORT void JNICALL Java_com_sap_dbtech_rte_comm_NativeComm_cancel (
    JNIEnv * env,
    jobject self)
{
    NativeFieldT field;

    TRACEIN ("cancel");
    if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", mapErrorCode(commErrNotOk_esp01), JNI_FALSE);
        TRACEOUT();
        return;
    }
    sqlacancel (field.data->handle);
    releaseNativeField (&field, env);
    TRACEOUT();
}

/*
 * Class:     com_sap_dbtech_rte_comm_NativeComm
 * Method:    getRequestPacket
 * Signature: ()Lcom/sap/dbtech/util/StructuredMem;
 */
externC JNIEXPORT jobject JNICALL Java_com_sap_dbtech_rte_comm_NativeComm_getRequestPacket (
    JNIEnv * env,
    jobject self)
{
    NativeFieldT field;
    jobject byteArray;
    jobject result;

    TRACEIN ("getConnectPacket");
    if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", mapErrorCode(commErrNotOk_esp01), JNI_FALSE);
        TRACEOUT();
        return NULL;
    }
    byteArray = (*env)->NewByteArray (env, field.data->maxRequestLen);
    if ((*env)->ExceptionOccurred (env)) {
        TRACEOUT();
        return NULL;
    }
    result = createStructuredBytes (env, byteArray);
    if ((*env)->ExceptionOccurred (env)) {
        TRACEOUT();
        return NULL;
    }
    releaseNativeField (&field, env);
    TRACEOUT();
    return result;
}

/*
 * Class:     com_sap_dbtech_rte_comm_NativeComm
 * Method:    isConnected
 * Signature: ()Z
 */
externC JNIEXPORT jboolean JNICALL Java_com_sap_dbtech_rte_comm_NativeComm_isConnected (
    JNIEnv * env,
    jobject self)
{
    NativeFieldT field;
    jboolean result;

    TRACEIN ("isConnected");
    if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", mapErrorCode(commErrNotOk_esp01), JNI_FALSE);
        TRACEOUT();
        return JNI_FALSE;
    }
    result = field.data->handle != -1;
    releaseNativeField (&field, env);
    TRACEOUT();
    return result;
}

/*
 * Class:     com_sap_dbtech_rte_comm_NativeComm
 * Method:    receive
 * Signature: ()Lcom/sap/dbtech/util/StructuredMem;
 */
externC JNIEXPORT jobject JNICALL Java_com_sap_dbtech_rte_comm_NativeComm_receive (
    JNIEnv * env,
    jobject self)
{
    NativeFieldT    field;
    jobject         result = NULL;
    void          * replyPacket;
    tsp00_Int4      replyLen;
    tsp00_ErrTextc  errtext;
    tsp01_CommErr   commErr;
    jobject         byteArray;
    jbyte         * rawMem;
    jboolean        isCopy;

    TRACEIN ("receive");
    if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", mapErrorCode(commErrNotOk_esp01), JNI_FALSE);
        TRACEOUT();
        return NULL;
    }
    sqlareceive (field.data->handle, &replyPacket, &replyLen, (unsigned char*)errtext, &commErr);
    if (commErr == commErrOk_esp01) {
        byteArray = (*env)->NewByteArray (env, replyLen);
        rawMem = (*env)->GetByteArrayElements (env, byteArray, &isCopy);
        memcpy (rawMem, replyPacket, replyLen);
        if (isCopy == JNI_TRUE) {
            (*env)->ReleaseByteArrayElements (env, byteArray, rawMem, 0);
        }
        result = createStructuredBytes (env, byteArray);
    }
    else {
        /* trim message */
        throwRteException (env, errtext, mapErrorCode(commErr), JNI_TRUE);
    }
    releaseNativeField (&field, env);
    TRACEOUT();
    return result;
}

externC JNIEXPORT void JNICALL Java_com_sap_dbtech_rte_comm_NativeComm_release (
    JNIEnv * env,
    jobject self);

/*
 * Class:     com_sap_dbtech_rte_comm_NativeComm
 * Method:    reconnect
 * Signature: ()V
 */
externC JNIEXPORT void JNICALL Java_com_sap_dbtech_rte_comm_NativeComm_reconnect (
    JNIEnv * env,
    jobject self)
{
    TRACEIN ("reconnect");
    Java_com_sap_dbtech_rte_comm_NativeComm_release (env, self);
    Java_com_sap_dbtech_rte_comm_NativeComm_connect (env, self);
    TRACEOUT();
}

/*
 * Class:     com_sap_dbtech_rte_comm_NativeComm
 * Method:    release
 * Signature: ()V
 */
externC JNIEXPORT void JNICALL Java_com_sap_dbtech_rte_comm_NativeComm_release (
    JNIEnv * env,
    jobject self)
{
    NativeFieldT field;

    TRACEIN ("release");
    loadNativeField (&field, env, self);
    if ((field.data != NULL) && (field.data->handle != -1)) {
        sqlarelease (field.data->handle);
        field.data->handle = -1;
        releaseNativeField (&field, env);
    }
    TRACEOUT();
}

/*
 * Class:     com_sap_dbtech_rte_comm_NativeComm
 * Method:    rawRequest
 * Signature: (Lcom/sap/dbtech/util/StructuredMem;I)V
 */
externC JNIEXPORT void JNICALL Java_com_sap_dbtech_rte_comm_NativeComm_rawRequest (
    JNIEnv * env,
    jobject self,
    jobject data,
    jint len)
{
    NativeFieldT field;
    jbyte * rawMem;
    jboolean isCopy;
    tsp00_ErrTextc  errtext;
    tsp01_CommErr   commErr;

    TRACEIN ("request");
    if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", mapErrorCode(commErrNotOk_esp01), JNI_FALSE);
        TRACEOUT();
        return;
    }
    rawMem = (*env)->GetByteArrayElements (env, data, &isCopy);
    memcpy (field.data->requestPacket, rawMem, len);
    sqlarequest (field.data->handle, field.data->requestPacket, len, (unsigned char*)errtext, &commErr);
    if (commErr != commErrOk_esp01) {
        throwRteException (env, errtext, mapErrorCode(commErr), JNI_TRUE);
    }
    (*env)->ReleaseByteArrayElements (env, data, rawMem, 0);
    releaseNativeField (&field, env);
    TRACEOUT();
}

#define bufWindowSizeC 1024
#define dataFieldNameC "data"
#define dataFieldSignatureC "[B"

/*
 * Class:     com_sap_dbtech_util_StructuredBytesEx
 * Method:    getRealLen
 * Signature: (IIB)I
 */
JNIEXPORT jint JNICALL Java_com_sap_dbtech_util_StructuredBytesEx_getRealLen (
    JNIEnv * env,
    jobject self,
    jint offset,
    jint len,
    jbyte padchar)
{
    jbyte buf [bufWindowSizeC];
    jclass clazz;
    jfieldID fieldID;
    jobject byteArray;
    int windowSize;
    int windowOffset;
    jboolean found = JNI_FALSE;

    clazz = (*env)->GetObjectClass (env, self);
    fieldID = (*env)->GetFieldID (env, clazz, dataFieldNameC, dataFieldSignatureC);
    byteArray = (*env)->GetObjectField (env, self, fieldID);
    memset (buf, padchar, MIN_EO00 (sizeof (buf), len));
    windowOffset = MAX_EO00 (offset, offset + len - sizeof (buf));
    do {
        windowSize = MIN_EO00 (sizeof (buf), len);
        (*env)->GetByteArrayRegion (env, byteArray, windowOffset, windowSize, buf);
        while (windowSize > 0) {
            --windowSize;
            if (buf [windowSize] != padchar) {
                return windowOffset + windowSize - offset;
            }
        }
        windowOffset -= windowSize;
    } while (windowOffset > offset);
    return 0;
}

/*
 * Class:     com_sap_dbtech_util_StructuredBytesEx
 * Method:    fill
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_com_sap_dbtech_util_StructuredBytesEx_fill (
    JNIEnv * env,
    jobject self,
    jint offset,
    jint len,
    jint padchar)
{
    jbyte buf [bufWindowSizeC];
    jclass clazz;
    jfieldID fieldID;
    jobject byteArray;
    int windowSize;

    clazz = (*env)->GetObjectClass (env, self);
    fieldID = (*env)->GetFieldID (env, clazz, dataFieldNameC, dataFieldSignatureC);
    byteArray = (*env)->GetObjectField (env, self, fieldID);
    memset (buf, padchar, MIN_EO00 (sizeof (buf), len));
    while (len > 0) {
        windowSize = MIN_EO00 (sizeof (buf), len);
        (*env)->SetByteArrayRegion (env, byteArray, offset, windowSize, buf);
        len -= windowSize;
        offset += windowSize;
    }
}
/*
 * Class:     com_sap_dbtech_rte_comm_NativeComm
 * Method:    isChallengeResponseSupportedNative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_sap_dbtech_rte_comm_NativeComm_isChallengeResponseSupportedNative (JNIEnv * env , jobject self){
  NativeFieldT field;
  tsp00_ErrTextc  errtext;
  TRACEIN ("receive");
  if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", mapErrorCode(commErrNotOk_esp01), JNI_FALSE);
        TRACEOUT();
        return JNI_FALSE;
  }
  if( SqlBasicAuthenticationOnly ( field.data->handle, errtext)){
    return JNI_FALSE;
  }else{
    return JNI_TRUE;
  }
}