/*!================================================================

 module:        nativeComm.c

 -------------------------------------------------------------------

 responsible:   DanielD

 special area:  JDBC

 description:   Native Methods to access new style communication
                from UDE server

 -------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
#include "heo02.h"
#include "RunTime/ClientKernelCommunication/RTECKC_ClientCommunication.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

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
    RTECKC_ClientCommunication  connection;
    SAPDB_Byte                * requestPacket;
    SAPDB_UInt4                 maxRequestLen;
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
        clazz = env->GetObjectClass (self);
    }
    javaString->id = env->GetFieldID (clazz, fieldName, "Ljava/lang/String;");
    javaString->obj = (jstring) env->GetObjectField (self, javaString->id);
    javaString->chars = env->GetStringUTFChars (javaString->obj, &javaString->isCopy);
}

/*----------------------------------------*/

static void
releaseString (
    JavaStringT * javaString,
    JNIEnv * env)
{
    if (javaString->isCopy) {
        env->ReleaseStringUTFChars (javaString->obj, javaString->chars);
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
    jclass clazz = env->GetObjectClass (self);
    jfieldID fieldID = env->GetFieldID (clazz, "nativeStruct", "[B");

    field->obj = env->GetObjectField (self, fieldID);
    if (field->obj != NULL) {
        field->data = (ConnectionDataT* ) env->GetByteArrayElements ((jbyteArray) field->obj, &field->isCopy);
        isOK = true;
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
        env->ReleaseByteArrayElements ((jbyteArray) field->obj, (jbyte*) field->data, 0);
    }
}

/*----------------------------------------*/

static tsp00_Int4
mapErrorCode (RTECKC_ClientCommunication::CommRC comm_Err)
{
    switch (comm_Err) {
      case (RTECKC_ClientCommunication::NoError):{
        return -1;
      }
      case (RTECKC_ClientCommunication::Error):{
        return -807;
      }
      case (RTECKC_ClientCommunication::TaskLimit):{
        return -9807;
      }
      case (RTECKC_ClientCommunication::ClientConnectTimeout):{
        return 700;
      }
      case (RTECKC_ClientCommunication::ServerTimeout):{
        return 700;
      }
      case (RTECKC_ClientCommunication::ServerCrash):{
        return 800;
      }
      case (RTECKC_ClientCommunication::Offline):{
        return -813;
      }
      case (RTECKC_ClientCommunication::Shutdown):{
        return -708;
      }
      case (RTECKC_ClientCommunication::ResourceMissing):{
        return -709;
      }
      default:
        return -1;
    }
}

/*----------------------------------------*/

static void
throwRteException (
    JNIEnv     * env,
    const char * msg,
    RTECKC_ClientCommunication::CommRC commRC)
{
    jclass clazz;
    jmethodID mid;
    jint ival;
    jstring sval;
    jobject obj;

    //fprintf (stdout, "*** Exception: %s/%s\n", (char*) ToStr (commRC), msg);
    clazz = env->FindClass ("com/sap/dbtech/rte/comm/RTEException");
    mid = env->GetMethodID(
                             clazz,
                             "<init>",
                             "(Ljava/lang/String;I)V");
    ival = mapErrorCode (commRC);
    sval = env->NewStringUTF(msg);
    obj  = env->NewObject(clazz, mid, sval, ival);
    env->Throw ((jthrowable) obj);
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

    clazz = env->FindClass ("com/sap/dbtech/util/StructuredBytes");
    if (env->ExceptionOccurred ()) {
        return NULL;
    }
    constructor = env->GetMethodID (clazz, "<init>", "([B)V");
    if (env->ExceptionOccurred ()) {
        return NULL;
    }
    result = env->NewObject (clazz, constructor, byteArray);
    if (env->ExceptionOccurred ()) {
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
    env->MonitorEnter (clazz);
}

/*----------------------------------------*/

static void
monitorExit (
    JNIEnv * env,
    jclass clazz)
{
    env->MonitorExit (clazz);
    /*printf("unlocked %p\n", clazz);*/
}

/*----------------------------------------*/

/*
 * Class:     com_sap_dbtech_udeserver_UDEComm
 * Method:    connect
 * Signature: ()V
 */
externC JNIEXPORT void JNICALL Java_com_sap_dbtech_udeserver_UDEComm_connect (
    JNIEnv * env,
    jobject self)
{
    static jboolean isInitialized = JNI_FALSE;
    static tsp00_CompName  sapdbcComponentC;
    sapdbcComponentC.c2p ("R sapdbc");
    jclass          clazz;
    JavaStringT     uri;
    RTECKC_ClientCommunication::CommRC commErr;
    SAPDB_UInt4     packetSize;
    jbyteArray      connectionDataObject;
    jbyte         * rawArray;
    jboolean        isCopy;
    ConnectionDataT * connectionData;
    jfieldID        nativeStructID;

    TRACEIN ("connect");
    clazz = env->GetObjectClass (self);
    monitorEnter (env, clazz);
    if (isInitialized == JNI_FALSE) {
        sqlinit (sapdbcComponentC, NULL);
        isInitialized = JNI_TRUE;
    }
    loadStringField (&uri, env, clazz, self, "uri");

    SAPDBErr_MessageList msgList;
    connectionDataObject = env->NewByteArray (sizeof (ConnectionDataT));
    rawArray = env->GetByteArrayElements (connectionDataObject, &isCopy);
    connectionData = (ConnectionDataT *) rawArray;
    new (&connectionData->connection) RTECKC_ClientCommunication;
    commErr = connectionData->connection.Open (uri.chars, 1, packetSize, msgList);
    if (commErr == RTECKC_ClientCommunication::NoError) {
        /*
         * fill instance variables
         */
        connectionData->connection.GetPacket (0, connectionData->requestPacket, msgList);
        connectionData->maxRequestLen = packetSize;
        nativeStructID = env->GetFieldID (clazz, "nativeStruct", "[B");
        env->SetObjectField (self, nativeStructID, connectionDataObject);
    }
    else {
        /*
         * create exception
         */
        throwRteException (env, msgList.Message (), commErr);
    }
    if (isCopy == JNI_TRUE) {
        env->ReleaseByteArrayElements (connectionDataObject, rawArray, 0);
    }
    releaseString (&uri, env);
    monitorExit (env, clazz);
    TRACEOUT();
}

/*
 * Class:     com_sap_dbtech_udeserver_UDEComm
 * Method:    cancel
 * Signature: ()V
 */
externC JNIEXPORT void JNICALL Java_com_sap_dbtech_udeserver_UDEComm_cancel (
    JNIEnv * env,
    jobject self)
{
    NativeFieldT field;

    TRACEIN ("cancel");
    if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", RTECKC_ClientCommunication::NoError);
        TRACEOUT();
        return;
    }
    SAPDBErr_MessageList msgList;
    field.data->connection.Cancel (msgList);
    releaseNativeField (&field, env);
    TRACEOUT();
}

/*
 * Class:     com_sap_dbtech_udeserver_UDEComm
 * Method:    getRequestPacket
 * Signature: ()Lcom/sap/dbtech/util/StructuredMem;
 */
externC JNIEXPORT jobject JNICALL Java_com_sap_dbtech_udeserver_UDEComm_getRequestPacket (
    JNIEnv * env,
    jobject self)
{
    NativeFieldT field;
    jobject byteArray;
    jobject result;

    TRACEIN ("getConnectPacket");
    if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", RTECKC_ClientCommunication::NoError);
        TRACEOUT();
        return NULL;
    }
    byteArray = env->NewByteArray (field.data->maxRequestLen);
    if (env->ExceptionOccurred ()) {
        TRACEOUT();
        return NULL;
    }
    result = createStructuredBytes (env, byteArray);
    if (env->ExceptionOccurred ()) {
        TRACEOUT();
        return NULL;
    }
    releaseNativeField (&field, env);
    TRACEOUT();
    return result;
}

/*
 * Class:     com_sap_dbtech_udeserver_UDEComm
 * Method:    isConnected
 * Signature: ()Z
 */
externC JNIEXPORT jboolean JNICALL Java_com_sap_dbtech_udeserver_UDEComm_isConnected (
    JNIEnv * env,
    jobject self)
{
    NativeFieldT field;
    jboolean result;

    TRACEIN ("isConnected");
    if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", RTECKC_ClientCommunication::NoError);
        TRACEOUT();
        return JNI_FALSE;
    }
    result = field.data->requestPacket != NULL;
    releaseNativeField (&field, env);
    TRACEOUT();
    return result;
}

/*
 * Class:     com_sap_dbtech_udeserver_UDEComm
 * Method:    receive
 * Signature: ()Lcom/sap/dbtech/util/StructuredMem;
 */
externC JNIEXPORT jobject JNICALL Java_com_sap_dbtech_udeserver_UDEComm_receive (
    JNIEnv * env,
    jobject self)
{
    NativeFieldT    field;
    jobject         result = NULL;
    SAPDB_Byte    * replyPacket;
    SAPDB_UInt4     replyLen;
    RTECKC_ClientCommunication::CommRC commErr;
    jobject         byteArray;
    jbyte         * rawMem;
    jboolean        isCopy;

    TRACEIN ("receive");
    if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", RTECKC_ClientCommunication::NoError);
        TRACEOUT();
        return NULL;
    }
    SAPDBErr_MessageList msgList;
    commErr = field.data->connection.Receive (replyPacket, replyLen, msgList);
    if (commErr == RTECKC_ClientCommunication::NoError) {
        byteArray = env->NewByteArray (replyLen);
        rawMem = env->GetByteArrayElements ((jbyteArray) byteArray, &isCopy);
        memcpy (rawMem, replyPacket, replyLen);
        if (isCopy == JNI_TRUE) {
            env->ReleaseByteArrayElements ((jbyteArray)byteArray, rawMem, 0);
        }
        result = createStructuredBytes (env, byteArray);
    }
    else {
        /* trim message */
        throwRteException (env, msgList.Message (), commErr);
    }
    releaseNativeField (&field, env);
    TRACEOUT();
    return result;
}

externC JNIEXPORT void JNICALL Java_com_sap_dbtech_udeserver_UDEComm_release (
    JNIEnv * env,
    jobject self);

/*
 * Class:     com_sap_dbtech_udeserver_UDEComm
 * Method:    reconnect
 * Signature: ()V
 */
externC JNIEXPORT void JNICALL Java_com_sap_dbtech_udeserver_UDEComm_reconnect (
    JNIEnv * env,
    jobject self)
{
    TRACEIN ("reconnect");
    Java_com_sap_dbtech_udeserver_UDEComm_release (env, self);
    Java_com_sap_dbtech_udeserver_UDEComm_connect (env, self);
    TRACEOUT();
}

/*
 * Class:     com_sap_dbtech_udeserver_UDEComm
 * Method:    release
 * Signature: ()V
 */
externC JNIEXPORT void JNICALL Java_com_sap_dbtech_udeserver_UDEComm_release (
    JNIEnv * env,
    jobject self)
{
    NativeFieldT field;

    TRACEIN ("release");
    loadNativeField (&field, env, self);
    if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", RTECKC_ClientCommunication::NoError);
        TRACEOUT();
        return;
    }
    SAPDBErr_MessageList msgList;
    field.data->connection.Close (msgList);
    releaseNativeField (&field, env);
    TRACEOUT();
}

/*
 * Class:     com_sap_dbtech_udeserver_UDEComm
 * Method:    rawRequest
 * Signature: (Lcom/sap/dbtech/util/StructuredMem;I)V
 */
externC JNIEXPORT void JNICALL Java_com_sap_dbtech_udeserver_UDEComm_rawRequest (
    JNIEnv * env,
    jobject self,
    jobject data,
    jint len)
{
    NativeFieldT field;
    jbyte * rawMem;
    jboolean isCopy;
    RTECKC_ClientCommunication::CommRC commErr;

    TRACEIN ("request");
    if (!loadNativeField (&field, env, self)) {
        throwRteException (env, "invalid Session", RTECKC_ClientCommunication::NoError);
        TRACEOUT();
        return;
    }
    rawMem = env->GetByteArrayElements ((jbyteArray) data, &isCopy);
    memcpy (field.data->requestPacket, rawMem, len);
    SAPDBErr_MessageList msgList;
    commErr = field.data->connection.Send (field.data->requestPacket, len, msgList);
    if (commErr != RTECKC_ClientCommunication::NoError) {
        throwRteException (env, msgList.Message (), commErr);
    }
    env->ReleaseByteArrayElements ((jbyteArray)data, rawMem, 0);
    releaseNativeField (&field, env);
    TRACEOUT();
}

