/*!
 * \file    LVC_IliveCacheSink.hpp
 * \author  IvanS, MarkusSi, ThomasA
 * \brief   LiveCache Interface used by DB-Procedures.
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



*/


#if defined(LIVECACHE_SINK_ONLY_MEMBER_FUNCS)
#undef  __LVC_ILIVECACHESINK_HPP
#undef  virtual
#undef  PURE_VIRTUAL
#endif

#ifndef __LVC_ILIVECACHESINK_HPP
#define __LVC_ILIVECACHESINK_HPP

#if defined(LIVECACHE_SINK_ONLY_MEMBER_FUNCS)
#define PURE_VIRTUAL
#else
#define PURE_VIRTUAL  =0

#include "geo00.h"
#include "ggg00.h"
#include "ggg01.h"
#include "livecachetypes.h" // nocheck

#include "liveCache/LVC_MethodNames.hpp"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_IterDirection.hpp"
#include "Oms/OMS_ContainerHandle.hpp"      // PTS 1124356 FF 2003-09-23 new FileDir

extern "C" const GUID IID_IliveCacheSink;

class SAPDBMem_IRawAllocator;
class OmsAllocatorInterface;
class OMS_Context;

//externC {

  /*!
   * \brief OMS liveCache Sink.
   *
   * Definition of liveCache kernel sink, i.e. definition of those functions that
   * can be called from the liboms to obtain kernel support.
   *
   * \ingroup oms
   */
  struct IliveCacheSink : public IUnknown
  {
  public:
#endif
  /*-----------------------------------------------------------------------*/

  /*! 
    * \brief Reads rows of ABAP tables into buffers.
    *
    * Since the ABAP table resides at the application site, a request
    * to the precompiler runtime must be performed. This is realized
    * by a call of vreply. The precompiler runtime answers by a call
    * of sql_request.
    * 
    * \param descriptor [in,out] contains the identification of the ABAP tables to be read. As a result the
    *        structure contains the following informations for each stream delivered :
    *        [in]  osdStreamId, the identification of the stream (ABAP table)
    *        [out] osdRowCount, the number of rows returned
    *        [out] osdBufSize, the size of the buffer containing the rows of the stream (ABAP table)
    *        [out] osdBufPtr, the pointer to the buffer containing the rows of the stream (ABAP table)
    *        [out] osdEndOfStream, true if the end of the stream has been reached
    * \param error [out] error code of the operation, e_ok if no error occured (out).
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
    */
  virtual HRESULT STDMETHODCALLTYPE ABAPRead(
    tgg01_StreamDescriptor &descriptor,
    short                  &error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Writes rows from a buffer into an ABAP table.
    *
    * Since the ABAP table resides at the application site, a request to
    * the precompiler runtime must be performed. This is realized by
    * a call of vreply. The precompiler runtime answers by a call of
    * sql_request.
    *
    * \param descriptor [in,out] contains the identification of the ABAP tables to be written. Each member of the descriptor contains
    *        the following information :
    *        [in] osdStreamId, the identification of the stream (ABAP table)
    *        [in,out] osdRowCount, the number of rows to be transferred to the application. Out : the
    *                 number of rows, that have been transferred.
    *        [in] osdRowSize, the size of the one row of of the stream (ABAP table)
    *        [in] osdBufSize, the size of the buffer containing the rows of the stream (ABAP table)
    *        [in] osdBufPtr, the pointer to the buffer containing the rows to be transferred
    * \param error [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE ABAPWrite(
    tgg01_StreamDescriptor &descriptor,
    short                  &error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Allocates a piece of memory which is used as a sql packet.
    *
    * In the course of a dbproc execution the sqlc++ class uses this packet
    * to send sql requests to the kernel.
    * 
    * \param p  [out] pointer to the packet (NULL, if no more memory is available),
    * \param sz [out] size of the packet.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE AllocPacket( 
    unsigned char  * *p,
    tsp00_Int4  *sz) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  ///*!
  // * \brief Sets the sql code of the kernel.
  // *
  // * \param b_error [in] identifies the error. This error is transformed
  // *   into an sql error by a call of a07_b_put_error.
  // *
  // * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  // */
  //virtual HRESULT STDMETHODCALLTYPE BasisError( 
  //  short b_error) PURE_VIRTUAL;


  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Close an open version.
    *
    * \param lpb_error   [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE CloseVersion( 
    short         *lpb_error) PURE_VIRTUAL;
      
  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Writes a message into knldiag.
    *
    * \param buflen [in] length of the message,
    * \param lpv    [in] pointer to the message.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE ConsoleMsg( 
    tsp00_Int4 buflen,
    unsigned char  *lpv) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief A version is created.
    * 
    * A new context is created.
    * The context returned by this call must be provided for all following OMS calls 
    * in that version.
    *
    * \param lpConsistentView [in] the consistent view definining the version, 
    *                              (only needed for the simulator!)
    * \param lpVersionContext [out] the version context. 
    * \param lpb_error        [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE CreateVersion( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpVersionContext,
    short          *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Creates an iterator running over a defined key range.
    *
    * This function creates a key iterator which runs over all objects with
	  * keys between the given start and stop keys and it returns the the first objects
    * from the requested interval.
    * The iterator is returned via a void pointer. 
    * If the pointer to the start key is nil and the start key len is zero the iterator 
    * starts with the smallest key. If the stop key pointer is nil and the stop key len is 
    * zero the iterator stops at the largest key. The iterator can be increased or 
    * decreased using the functions NextObjFromKeyIterator resp. PrevObjFromKeyIterator. 
    *
    * \param consistentView  [in,out] kb identification of the current consistent view,
    * \param objFile         [in] bd identification of the container containing the object.
    * \param lpVersionContext NULL, if the function is called outside of a version. 
    *                        Otherwise the version context returned by CreateOpenVersion,
    * \param pKeyRangeIterator [out] pointer to the newly created iterator,
    * \param keyLen          [in] the key length of the container,
    * \param pStartKey       [in] a pointer to the start key of length keyLen.
    * \param pLowerKey       [in] a pointer to the lower boundary key of length keyLen
    * \param pUpperKey       [in] a pointer to the upper boundary key of length keyLen
    * \param noOfOid         [in,out] as in-parameter this parameter determines the maximal number of 
    *                        OIDs which can be read by a single call,
    *                        as out-parameter this parameter contains the number of OIDs which have
    *                        been read from the kernel,
    * \param pResulOid       [out] array with the OID which have been read from the kernel,
    * \param pObjVers        [out] array of log-positions of the objects
    * \LogReadAccesses       [out] number of jumps back in the history necessary to find the wanted object image.
    * \param e               [out] error code of the operation, e_ok if no error occured,
    * \param direction       [in] if there is no object with a matching object for the start-key, then
    *                        this parameter determines whether the iterator should return the next 
    *                        largest (direction = OMS_ASCENDING) or next smallest (OMS_DESCENDING) object.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  // PTS 1119480 Interface changed
  virtual HRESULT STDMETHODCALLTYPE CreateKeyRangeIterator(
    tgg91_TransNo            &consistentView,
    tgg00_FileId             &objFile,
    tgg01_OmsVersionContext  *lpVersionContext,
    void                    **pKeyRangeIterator,
    tsp00_Int4                objBodySize,
    size_t                    keyLen,
    const void               *pStartKey,              
    const void               *pLowerKey,              
    const void               *pUpperKey,      
    tsp00_Int4               &noOfOid,
    SAPDB_UInt8              *pResultOId,   
    tgg91_PageRef            *pObjVers,
    unsigned char            *pBody,
    tsp00_Int4               &LogReadAccesses,
    tgg00_BasisError         &e,
    OmsIterDirection          direction = OMS_ASCENDING
  ) PURE_VIRTUAL;
      
  /*-----------------------------------------------------------------------*/

  /*
    * \brief Deletes all objects of a container.
    *
    * All objects of the file given by the object file id are deleted. 
    * If there is any object which cannot be deleted since the current 
    * image does not belong to the consistent view of the current 
    * transaction the error e_object_dirty is returned. In principle 
    * this operations works like the command b04DelObj 
    * which is applied to all objects of a file.
    *
    * \param consistentView   [in,out] kb identification of the current consistent view,
    * \param lpContainerId    [in] bd identification of the container,
    * \param lpVersionFile    [in] NULL, if the function is called outside of a version, otherwise
    *                         a version context returned by CreateOpenVersion,
    * \param lpDeleted        [out] number of objects deleted,
    * \param lpb_error        [out] error code of the operation, e_ok if no error occured,
    * \param errorOid         [out] the oid of the object causing the error.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE DeleteAllObj( 
    tgg91_TransNo&  consistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionFile,
    int            &lpDeleted,
    short          &lpb_error,
    SAPDB_UInt8    &errorOid) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*
    * \brief Deletes a single object.
    *
    * The object identified by an object identifier (OID) is deleted.
    * If the current image stored in the object frame given by the OId does 
    * not contain the object with the wanted version the error e_object_dirty 
    * is returned. 
    *
    * \param lpConsistentView [in] kb identification of the current consistent view,
    * \param lpContainerId    [in] bd identification of the container containing the object,
    * \param lpVersionContext [in] NULL, if the function is called outside of a version, otherwise
    *                         a version context returned by CreateOpenVersion,
    * \param lpoid            [in] object identification identifying the object to be deleted,
    * \param lpobjseq         [in] since a frame can contain a number of different objects by the time
    *                         a unique definition of an object requires beside the OId also a kind
    *                         of time stamp. WantedObjVers is such a time stamp (actually it is the
    *                         log position),
    * \param reused           [in]  tells, whether the object owns a locally reused oid,
    * \param lpb_error        [out] error code of the operation, e_ok if no error occurred (out).
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE DeleteObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    unsigned char  *lpoid,
    unsigned char  *lpobjseq,
    bool            reused,   
    bool            contObj,    // PTS 1124278
    short  *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*
    * \brief Deletes an array of objects.
    *
    * The objects identified by the object identifiers (OID) are deleted.
    * If the current image stored in the object frame given by the OId does 
    * not contain the object with the wanted version the error e_object_dirty 
    * is returned. 
    *
    * This method deletes the specified objects in a loop and therefore it can 
    * be used to reduce the communication overhead between the liboms and the
    * kernel by providing a mass interface.
    *
    * \param cnt              [in] number of entries in the array,
    * \param pConsistentView  [in] kb identification of the current consistent view,
    * \param ppContainerId    [in] array with the bd identifications of the container containing the object,
    * \param pVersionContext  [in] NULL, if the function is called outside of a version, 
    *                             otherwise the version context returned by CreateOpenVersion,
    * \param pOid             [in] array with the object identifications of the objects to be deleted,
    * \param pObjSeq          [in] array with the log-positions of the objects,
    * \param pReused          [in] array which tells, whether the object owns a locally reused oid,
    * \param clustered        [in]     if true, then all given oids belong to the same page
    * \param errCnt           [out] number of errors that occured,
    * \param pDBError         [out] array with the error codes of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    *
    * \since PTS 1121449
    */
  virtual HRESULT STDMETHODCALLTYPE DeleteObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    bool                    *pReused, 
    bool                    *pContObj,    // PTS 1124278
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Destroy key range iterator.
    *
    * Releases all the KB resource held by the iterator.
    *
    * \param pKeyRangeIterator [in] pointer to the key range iterator.
    */
  virtual HRESULT STDMETHODCALLTYPE DestroyKeyRangeIterator(
    void* pKeyRangeIterator) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief End consistent view.
    *
    * Ends the current consistent view. Subsequently, the current transaction
    * has an undefined consistent view state.
    *
    * \param lpConsistentView [in,out] kb identification of the consistent view,
    * \param lpb_error        [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
    */
  virtual HRESULT STDMETHODCALLTYPE EndConsistentView( 
    unsigned char  *lpConsistentView,
    short           *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Enter critical section
    *
    * Enters a critical section by calling vbegexcl.
    *
    * \param regionId identifies the critical section to be entered.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE EnterCriticalSection(
    short regionId) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Gets the container id for each of the given oids 
    *
    * Gets the container id for each of the given oids
    *
    * \param containerId     [out] bd identification of the container containing the object,
    * \param pVersionContext [in] NULL, if the function is called outside of a version, 
    *                        otherwise the version context returned by CreateOpenVersion,
    * \param noOfOid         [in] number of oids which are given,
    * \param pOid            [in] array with the object identifiers,
    * \param pDBError        [out] array with the error codes of the operation, e_ok if no error occured (out).
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
    virtual HRESULT STDMETHODCALLTYPE GetContainerId(		
      tgg01_ContainerId              *pcontainerId, 
      const tgg01_OmsVersionContext  *pVersionContext,
      tsp00_Int4                      noOfOid,
      const SAPDB_UInt8              *pOid,
      tgg00_BasisError               *pDBError) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  ///*!
  // * \brief Get error message
  // *
  // * Yields an error description for a given sql code.
  // *
  // * \param errorno  [in] the sql code,
  // * \param lpbuflen [in,out] in: the size of the buffer identified by lpv, 
  // *                 out: the length of the description,
  // * \param lpv      [out] buffer which takes the error description (out).
  // *
  // * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  // */
  //virtual HRESULT STDMETHODCALLTYPE GetErrorMsg( 
  //  short           errorno,
  //  tsp00_Int4     *lpbuflen,
  //  unsigned char  *lpv) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Read an object
    *
    * Reads a consistent image of an object via an object identified (OID).
    *
    * This function returns the image of an object defined by the given OId. 
    * The returned image must fit with the consistent view of the current transaction.
    *
    * \param consistentView [in,out] kb identification of the current consistent view. As the first read
    *                       access in a transaction determines the consistent view, the consistent view 
    *                       might be changed after the call.
    * \param containerId    [in,out] bd identification of the container containing the object (in/out),
    *                       If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
    *                       the root and the fileno of the file containing the wanted object
    *                       is returned in the fileId,
    * \param pVersionContext [in] NULL, if the function is called outside of a version, otherwise
    *                       a version context returned by CreateOpenVersion,
    * \param oid            [in] object identifier of the  wanted object,
    * \param doLock         [in] \c true, if the object shall be locked; \c false otherwise,
    * \param shared         [in] \c true, if the object shall be locked in shared mode; \c false locked in exclusive mode 
    * \param objSeq         [in,out] time stamp (=log position) of the consistent object image. If this parameter is
    *                       specified, then the object can be read directly from the correct log position and
    *                       no traversal of the history is needed.
    * \param bodySize       [in] size of the destination buffer where the found object image has to be copied,
    * \param pBody          [in] pointer to the destination buffer, i.e. the memory location where the object is written to,
    * \param bodyLen        [out] length of the object,
    * \param histLogCount   [out] number of jumps back in the history necessary to find the wanted object image,
    * \param e              [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE GetObj( 
    tgg91_TransNo                 &consistentView,
    tgg01_ContainerId             &containerId, 
    const tgg01_OmsVersionContext *pVersionContext,
    const SAPDB_UInt8             &oid,
    bool                           doLock,
    bool                           shared, 
    tgg91_PageRef                 &objSeq,
    tgg91_TransNo                 &updTransId,
    size_t                         bodySize,
    unsigned char                 *pBody,
    size_t                        &bodyLen,
    tsp00_Int4                    &histLogCount,
    tgg00_BasisError              &e) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Read an array of objects
    *
    * Reads an array of consistent images of the objects via OIDs.
    *
    * This method reads the specified objects in a loop and therefore it can be used to reduce 
    * the communication overhead between the liboms and the kernel by providing a mass interface.
    *
    * \param consistentView [in,out] kb identification of the current consistent view,
    * \param pContainerId   [in,out] array with the bd identification of the container containing the object,
    *                       If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
    *                       the root and the fileno of the file containing the wanted object
    *                       is returned in the fileId,
    * \param pVersionContext [in] NULL, if the function is called outside of a version, 
    *                        otherwise the version context returned by CreateOpenVersion,
    * \param noOfOid        [in] number of objects which should be read
    * \param pOid           [in] array of object identifiers of the requested objects,
    * \param pObjVers       [in,out] array of 'Log-OIDs' which can be used to read the object without having
    *                       to traverse the history chain
    * \param pBodySize      [in] array with the  size of the destination buffer where the found object image has to be copied,
    * \param doLock         [in] \c true, if the object shall be locked; \c false otherwise,
    * \param shared         [in] \c true, if the object shall be locked in shared mode; \c false locked in exclusive mode 
    * \param ppObj          [in] array of pointers to the destination buffer, i.e. memory where the objects are returned (out),
    * \param histLogCount   [out] number of jumps back in the history necessary to find the wanted object image (out),
    * \param errorCount     [out] number of errors that occured
    * \param e              [out] array of error codes of the operation for each single object, e_ok if no error occured (out).
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE GetObjects(
    tgg91_TransNo                 &consistentView,
    tgg01_ContainerId             *pContainerId, 
    const tgg01_OmsVersionContext *pVersionContext,
    tsp00_Int4                     noOfOid,
    const SAPDB_UInt8             *pOid,
    tgg91_PageRef                 *pObjVers,
    tgg91_TransNo                 *pUpdTransId,
    size_t                        *pBodySize,
    bool                           doLock,
    bool                           shared, 
    unsigned char                **ppObj,
    tsp00_Int4                    &histLogCount,
    tsp00_Int4                    &errorCount,
    tgg00_BasisError              *e) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Reads a consistent image of an object via a key.
    *
    * This function returns the image of an object defined by a key. The returned image must fit
    * with the consistent view of the current transaction.
    *
    * \param consistentView [in,out] kb identification of the current consistent view. As the first
    *                       read access in a transaction determines the consistent view, the value
    *                       might get changed by this call,
    * \param lptree         [in] bd identification of the container containing the object,
    * \param pVersionContext  [in] NULL, if the function is called outside of a version. 
    *                       Otherwise the version context returned by CreateOpenVersion.
    * \param keylen         [in] length of the key in bytes,
    * \param lpkey          [in] pointer to the key,
    * \param doLock         [in] \c true, if the object shall be locked; \c false otherwise, 
    * \param shared         [in] \c true, if the object shall be locked in shared mode; \c false locked in exclusive mode 
    * \param bodysize       [in] size of the destination buffer where the found object image has to be copied,
    * \param lpbody         [in] pointer to the destination buffer, i.e. memory location where the object is copied to,
    * \param lpoid          [out] object identifier of the wanted object,
    * \param lpobjseq       [out] time stamp (=log position)of the consistent object image,
    * \param hist_log_count [out] number of jumps back in the history necessary to find the wanted object image,
    * \param lpb_error      [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE GetObjWithKey( 
    unsigned char  *lpConsistentView,
    unsigned char  *lptree,
    unsigned char  *lpVersionContext,
    size_t          keylen,
    unsigned char  *lpkey,
    bool            doLock,
    bool            shared,
    size_t          bodysize,
    unsigned char  *lpbody,
    SAPDB_UInt8    *lpoid,
    unsigned char  *lpobjseq,
    tgg91_TransNo  *pUpdTransId,
    int            *hist_log_count,
    short          *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Reads a whole array of objects via the key.
    *
    * This function returns the images of objects defined by the key. The returned images must fit
    * with the consistent view of the current transaction.
    *
    * This method reads the objects, corresponding to the given keys,  in a loop and 
    * therefore it can be used to reduce the communication overhead between the 
    * liboms and the kernel by providing a mass interface.
    *
    * \param pConsistentView [in,out] kb identification of the current consistent view. As the first
    *                        read access in a transaction determines the consistent view, the value
    *                        might get changed by this call,
    * \param pContainerId    [in] array with the bd identifications of the container containing the object,
    * \param pVersionContext [in] NULL, if the function is called outside of a version. 
    *                        Otherwise the version context returned by CreateOpenVersion.
    * \param pKeyLen         [in] array with the length of the keys in bytes,
    * \param noOfObj         [in] number of objects that should be read,
    * \param ppKey           [in] array with the pointers to the key,
    * \param doLock          [in] \c true, if the object shall be locked; \c false otherwise, 
    * \param shared         [in] \c true, if the object shall be locked in shared mode; \c false locked in exclusive mode 
    * \param pBodySize       [in] array with the sizes of the destination buffer where the found 
    *                        object image has to be copied,
    * \param ppBody          [in] array with the pointers to the destination buffer, i.e. memory 
    *                        location where the object is copied to,
    * \param pOid            [out] array with the object identifiers of the wanted object,
    * \param pObjSeq         [out] array with the time stamps (=log positions)of the consistent object image,
    * \param histLogCount    [out] number of jumps back in the history necessary to find the wanted object image,
    * \param errorCnt        [out] number of errors that occured,
    * \param pDBError        [out] array with the error codes of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    *
    * \since PTS 1122194
    */
  virtual HRESULT STDMETHODCALLTYPE GetObjectsWithKey( 
    tgg91_TransNo            *pConsistentView,
    tgg01_ContainerId        *pContainerId,
    tgg01_OmsVersionContext  *pVersionContext,
    size_t                   *pKeyLen,
    tsp00_Int4                noOfObj,
    unsigned char           **ppKey,
    bool                      doLock,
    bool                      shared,
    size_t                   *pBodySize,
    unsigned char           **ppBody,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef            *pObjSeq,
    tgg91_TransNo            *pUpdTransId,
    tsp00_Int4               &histLogCnt,
    tsp00_Int4               &errorCnt,
    tgg00_BasisError         *pDBError) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Set return code.
    *
    * Sets the sql return code for a HRESULT error.
    *
    * \param HResultError [in] the HRESULT error.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE HResultError( 
    HRESULT h_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Check for lock.
    *
    * Check, if an object is locked by the caller.
    *
    * \param lpContainerId [in] bd identification of the container containing the object,
    * \param lpoid         [in] object identification (OID) of the object,
    * \param shared        [in] should be checked for exclusive or shared lock?
    * \param lpb_error     [out] error code of the operation, 
    *                            e_ok if the object is locked by the caller, 
    *                            e_object_not_locked if the object is not locked by the caller.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE IsLocked( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    bool            shared,
    short          *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Leave critical section.
    *
    * Leaves a critical section by calling vendexcl.
    *
    * \param regionId identifies the critical section to leave (in).
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE LeaveCriticalSection(
    short regionId) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Lock object.
    *
    * Locks an object. If an object cannot be locked since the wanted version does not correspond to the current image
    * of the object the error e_object_dirty is returned. 
    *
    * \param lpConsistentView [in] kb identification of the current consistent view, 
    * \param lpContainerId    [in,out] bd identification of the container containing the object,
    *                         If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
    *                         the root and the fileno of the file containing the wanted object
    *                         is returned in the fileId. 
    * \param lpoid            [in] object identifier of the object to be locked,
    * \param lpobjseq         [in] wanted time stamp (=log position) of the consistent object image, 
    * \param lpb_error        [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE LockObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    unsigned char  *lpobjseq,
    short          *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Lock object.
  *
  * Locks an array of objects. If an object cannot be locked since the wanted version does 
  * not correspond to the current image of the object the error e_object_dirty is returned. 
  * If a timeout is specified, it is checked, that the locking of all objects does not take
  * more time than the specified value. If more time would be needed, then the procedure
  * is left. If less time is needed the remaining time is returned.
  *
  * This method locks every specified oid in a loop and therefore it can be used 
  * to reduce the communication overhead between the liboms and the kernel by 
  * providing a mass interface.
  *
  * \param cnt              [in,out] number of oids which should be locked. After the call, this counter tells how
  *                         many objects have been considered for locking. This means that the counter will only be
  *                         different if the processing has been stopped because of a timeout.
  * \param pConsistentView  [in] kb identification of the current consistent view, 
  * \param ppContainerId    [in,out] bd identification of the container containing the object,
  *                         If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
  *                         the root and the fileno of the file containing the wanted object
  *                         is returned in the fileId. 
  * \param pOid             [in] Array with the object identifiers of the objects to be locked,
  * \param pObjSeq          [in] Array with the wanted time stamps (=log positions) of the consistent object images, 
  * \param timeout          [in,out] maximal time for the locking of all objects (if timeout!=0). 
  *                         After the call the variable equals the remaining time = original timeout - running time,
  * \param errCnt           [out] number of errors which occur,
  * \param pDBError         [out] array with the error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  *
  * \since PTS 1121449
  */
  virtual HRESULT STDMETHODCALLTYPE LockObjMass(
    tsp00_Int4              &cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    tsp00_Int8              &timeout,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Multipurpose method.
    *
    * This method is used for various control functions. Undocumented.
    *
    * \param MessType  [in]
    * \param MessType2 [in]
    * \param lpbuflen  [in,out]
    * \param lpv       [in,out]
    * \param lpb_error [out]
    *
    * \todo Document possible calls.
    */
  virtual HRESULT STDMETHODCALLTYPE MultiPurpose( 
    unsigned char   MessType,
    unsigned char   MessType2,
    tsp00_Int4     *lpbuflen,
    unsigned char  *lpv,
    short          *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief  New consistent view.
    *
    * Creates a new consistent view. If the current transaction already holds a
    * consistent view when this method is called, this view is released and
    * a new consistent view is returned.
    *
    * \param lpConsistentView [in,out] kb identification of the consistent view,
    * \param lpb_error        [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE NewConsistentView( 
    bool            isOmsVersion,             /* PTS 1130354 */
    unsigned char  *lpConsistentView,
    short          *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief New object.
    *
    * Reserves an empty object frame for new object and returns it's OId. 
    * If the object owns a key this key is written into the object frame.
    *
    * \param lpConsistentView [in] kb identification of the current consistent view,
    * \param lpContainerId    [in] bd identification of the container containing the object,
    * \param lpVersionContext [in] NULL, if the function is called outside of a version. Otherwise
    *                          a version context returned by CreateOpenVersion,
    * \param KeyPos           [in] position of the key (if present),
    * \param KeyColSize       [in] length of the key (if present),
    * \param pKeyColumn       [in] pointer to the key if present; NULL otherwise,
    * \param lpoid            [out] object identifier of the new object,
    * \param lpobjseq         [out] time stamp (=log position) of the new object image, 
    * \param lpb_error        [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE NewObj( 
    unsigned char  *lpConsistentView,
    unsigned char  *lpContainerId,
    unsigned char  *lpVersionContext,
    size_t          KeyPos,
    size_t          KeyColSize,
    unsigned char  *KeyColumn,
    bool            contObj,    // PTS 1124278
    SAPDB_UInt8    *lpoid,
    unsigned char  *lpobjseq,
    tgg91_TransNo  *pUpdTransId,
    short  *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Reserves an new frame in the kernel for every entry and returns the corresponding oids
  * For every entry in the array this routine reserves an empty object frame 
  * to store a part of a new object and returns the corresponding OId. 
  *
  * This method creates the new objects in a loop and therefore it can be used to reduce 
  * the communication overhead between the liboms and the kernel by providing a mass interface.
  * 
  * \param cnt              [in]  number of objects that should be created.
  * \param pConsistentView  [in]  kb identification of the current consistent view. The consistent view
  *                               is the same for every entry
  * \param ppContainerId    [in]  bd identification of the container containing the object. Every object
  *                               can belong to another container, 
  * \param lpVersionContext [in]  NULL, if the function is called outside of a version. 
  *                               Otherwise the version context returned by CreateOpenVersion.
  * \param pKeyPos          [in]  array with the positions of the key (if present).
  * \param pKeyColSize      [in]  array with the length of the key (if present).
  * \param ppKeyColumn      [in]  array with pointers to the keys if present; NULL otherwise.
  * \param pOid             [out] array with the object identifiers of the new object.
  * \param pObjSeq          [out] array with the time stamp (=log position) of the new object images. 
  * \param errCnt           [out] number of errors that have occured.
  * \param pDBError         [out] array with the error codes of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  *
  * \since PTS 1121449
  */
  virtual HRESULT STDMETHODCALLTYPE NewObjMass( 
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    size_t                  *pKeyPos,
    size_t                  *pKeyColSize,
    unsigned char          **ppKeyColumn, 
    bool                    *pContObj,    // PTS 1124278
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    tgg91_TransNo           *pUpdTransId,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \ brief Returns the next oids from the class iterator
  *
  * This function returns an array of oids following a given oid.         
  * The returned oids fits with the consistent view of the current transaction.
  *
  * \param ConsistentView  [in,out] kb identification of the current consistent view,
  * \param ContainerId     [in]  bd identification of the container containing the object,
  * \param pVersionContext [in]  NULL, if the function is called outside of a version. 
  *                              Otherwise the version context returned by CreateOpenVersion,
  * \param bOnlyHdrVarObjects [in] Flag is only relevant for var-objects. if \c true, then only oids of header
  *                              objects are returned; otherwise all objects in the specified container.
  * \param CurrOid         [in]  The oid which has been read last and which therefore can be used as 
  *                              starting point for this call. If the method is called for the first time
  *                              the CurrOid should equal the nil-oid.
  * \param noOfOid         [in,out] As in-parameter this parameter determines the maximal number of 
  *                              OIDs which can be read by a single call (=size of the pOid-array).
  *                              As out-parameter this parameter contains the number of OIDs which have
  *                              been read from the kernel.
  * \param pOid            [out] array with the OIDs which have been read from the kernel.
  * \param pObjVers        [out] array with the log positions of the objects,
  * \param LogReadAccesses [out] number of jumps back in the history necessary to find the wanted object image.
  * \param pb_error        [out] error code of the operation, e_ok if no error occurred, e_no_next_object if no
  *                                next object has been found.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  * 
  * \since 7.4.4 (PTS 1120478)
  */
  virtual HRESULT STDMETHODCALLTYPE NextOids(
    tgg91_TransNo           &ConsistentView,
    tgg00_FileId            &ContainerId,
    tgg01_OmsVersionContext *pVersionContext,
    bool                     bOnlyHdrVarObjects,
    SAPDB_UInt8              CurrOid,
    tsp00_Int4              &noOfOid,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjVers,
    tsp00_Int4              &LogReadAccesses,
    tgg00_BasisError        *pb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Get next objects from key-iterator
    * 
    * This function implements a mass read of the next largest objects from the iterator. 
    * Before calling this method the iterator must have been created using the method
    * CreateKeyRangeIterator.
    * If the last object has been read, then the iterator sets the error e_no_next_object.
    *
    * \param ConsistentView  [in,out] kb identification of the current consistent view,
    * \param ObjFile        [in] bd identification of the container to be traversed.
    * \param VersionContext [in] NULL, if the function is called outside of an unloaded version. 
    *                            Otherwise the pointer to a version context.
    * \param pKeyIterator   [in] pointer to the key range iterator
    * \param KeyLen         [in] the key length of the objects.
    * \param pRestartKey    [in] a pointer to the key which should be used to position the iterator
    *                            before reading the next objects. The object which the identical key
    *                            is not included in the return list.
    * \param NoOfOId        [in/out] as in-parameter this parameter determines the maximal number of 
    *                            OIDs which can be read by a single call.
    *                            as out-parameter this parameter contains the number of OIDs which have
    *                            been read from the kernel
    * \param pResulOId      [out] array with the OID which have been read from the kernel.
    * \param pObjVers       [out] array with the log positions of the objects,
    * \param objHistReadCount [out] number of jumps back in the history necessary to find the wanted object image.
    * \param e              [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */      
  virtual HRESULT STDMETHODCALLTYPE NextObjFromKeyIterator (
    const tgg91_TransNo     &ConsistentView,  // PTS 1127520 
    tgg01_ContainerId       &ObjFile,
    tgg01_OmsVersionContext *VersionContext,
    void                    *pKeyIterator,
    tsp00_Int4               objBodySize,
    size_t                   KeyLen,
    void                    *pRestartKey, 
    tsp00_Int4              &NoOfOId,
    SAPDB_UInt8             *pResultOId, 
    tgg91_PageRef           *pObjVers,
    unsigned char           *pBody,
    tsp00_Int4              &objHistReadCount,
    tgg00_BasisError        &e
    ) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Get previous objects from key-iterator
    * 
    * This function implements a mass read of the next smallest objects from the iterator. 
    * Before calling this method the iterator must have been created using the method
    * CreateKeyRangeIterator.
    * If the last object has been read, then the iterator sets the error e_no_next_object.
    *
    * \param ConsistentView  [in,out] kb identification of the current consistent view,
    * \param ObjFile        [in] bd identification of the container to be traversed.
    * \param VersionContext [in] NULL, if the function is called outside of an unloaded version. 
    *                            Otherwise the pointer to a version context.
    * \param pKeyIterator   [in] pointer to the key range iterator
    * \param KeyLen         [in] the key length of the objects.
    * \param pRestartKey    [in] a pointer to the key which should be used to position the iterator
    *                            before reading the next objects. The object which the identical key
    *                            is not included in the return list.
    * \param NoOfOId        [in/out] as in-parameter this parameter determines the maximal number of 
    *                            OIDs which can be read by a single call.
    *                            as out-parameter this parameter contains the number of OIDs which have
    *                            been read from the kernel
    * \param pResulOId      [out] array with the OID which have been read from the kernel.
    * \param pObjVers       [out] array with the log positions of the objects,
    * \param objHistReadCount [out] number of jumps back in the history necessary to find the wanted object image.
    * \param e              [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */      
  virtual HRESULT STDMETHODCALLTYPE PrevObjFromKeyIterator (
    const tgg91_TransNo     &ConsistentView,  // PTS 1127520 
    tgg01_ContainerId       &ObjFile,
    tgg01_OmsVersionContext *VersionContext,
    void                    *pKeyIterator,
    tsp00_Int4               objBodySize,
    size_t                   KeyLen,
    void                    *pRestartKey, 
    tsp00_Int4              &NoOfOId,
    SAPDB_UInt8             *pResultOId, 
    tgg91_PageRef           *pObjVers,
    unsigned char           *pBody,
    tsp00_Int4              &objHistReadCount,
    tgg00_BasisError        &e
    ) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  *
  * \brief Creates a container of a persistent class if it does not exist already. 
  *
  * Creates a container of a persistent class and meta data decribing the container is 
  * stored in the catalog. If the container exists already then the given meta data
  * is checked against the stored metadata.
  *
  * \param SchemaId         [in]  identifies the schema.
  * \param   lpGUID         [in]  global unique identifier idenitifying the class.
  * \param   size           [in]  sizeof of an instance of the class.
  * \param   ContainerNo    [in]  number of the container.
  * \param   keyPos         [in]  start position of the key in the object.
  * \param   keyLen         [in]  length of the key
  * \param   keyPartitioned [in]  is the key partitioned?
  * \param   useCachedKeys  [in]  Set the default for cachedKeys.
  * \param   isVarObject    [in]  Container stores variable sized objects.
  * \param   useInternalTrans [in] Should the creation of the container run in a separate transaction
  * \param   noWait         [in]  Should the call wait if the container is currently locked, or return 
  *                               an error directly.
  * \param   lpContainerId  [out] bd identification of the container.
  * \param   lpb_error      [out] error code of the operation, e_ok if no error occured.
  * 
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE CreateContainer(
    tsp00_Int4           SchemaId,
    GUID                *lpGUID,
    tsp00_KnlIdentifier *lpClassName,
    size_t               size,
    OmsContainerNo       ContainerNo,
    size_t               keyPos,          // PTS 1122540
    size_t               keyLen,          // PTS 1122540
    bool                 keyPartitioned,  // PTS 1122540
    bool                 useCachedKeys,
    bool                 isVarObject,
    bool                 useInternalTrans,
    bool                 noWait,
    unsigned char       *lpContainerId,
    short               *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief  Resets an unloaded version to its inital state
  *
  * Resets an unloaded version to its inital state, i.e. empties the
  * files containing the changes made in that version.
  *
  * \param lpVersionContext [in]  version context identifying the version.
  * \param error            [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE ResetVersion( 
    unsigned char *lpVersionContext,
    short         &error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/
 
  //virtual HRESULT STDMETHODCALLTYPE Rollback( 
  //    /* [in] */ tsp00_Int4 buflen,
  //    /* [in] */ unsigned char  *lpv,
  //    /* [out] */ short  *lpb_error) PURE_VIRTUAL;
 
  /*-----------------------------------------------------------------------*/

  /*!
  * \brief 
  *
  * \param errorno  [in] 
  * \param buflen   [in] 
  * \param lpv      [in] 
  * \param codeType [in] 
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  */
  virtual HRESULT STDMETHODCALLTYPE SetError( 
    short                 errorno,
    tsp00_Int4            buflen,
    const unsigned char  *lpv,
      tsp00_CodeType       codeType ) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief sends a packet containing a SQL request to the kernel.
  *            
  * Sends a packet containing a SQL request to the kernel. This function
  * is used by the sqlc++ class whenever a SQL statement has to be executed.
  *
  * \param buflen    [in]  length of the packet. Has no meaning for inproc execution.
  * \param lpv       [in]  pointer to the packet containing the sql request.
  * \param lpb_error [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE SQLRequest( 
    tsp00_Int4      buflen,
    unsigned char  *lpv) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Initializes an iterator (for unloaded versions) running over a defined key range
    *       
    * This function initializes a key iterator which runs over all objects with
	  * keys between the given start and stop keys and it returns the first object with
    * a key in the given interval. The iterator can be incremented (resp. decremented) using
    * the function VersionNextKey resp. VersionPrevKey.
    *
    * \param consistentView   [in,out]  kb identification of the current consistent view.
    * \param lpVersionContext [in]  NULL, if the function is called outside of a version. Otherwise
    *                         a version context returned by CreateOpenVersion.
    * \param objFile          [in]  bd identification of the container containing the object.
    * \param keyLen           [in]  the key length of the container.
    * \param pStartKey        [in]  a pointer to the start key of length keyLen
    * \param pLowerKey        [in]  a pointer to the lower key (=lower boundary of the specified interval) of
    *                         length keyLen
    * \param pUpperKey        [in]  a pointer to the upper key (=upper boundary of the specified interval) of
    *                         length keyLen
    * \param ResulOid         [out] OID of the object the iterator is pointing to.
    * \param e                [out] error code of the operation, e_ok if no error occured.
    * \param direction        [in]  if there is no object with a matching key for the start-key, then
    *                         this parameter determines whether the iterator should return the next 
    *                         largest (direction = OMS_ASCENDING) or next smallest (OMS_DESCENDING) object.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
    virtual HRESULT STDMETHODCALLTYPE StartUnloadedVersionIter(
        tgg91_TransNo           &consistentView,
        tgg01_OmsVersionContext *lpVersionContext,
        tgg00_FileId            &objFile,
        size_t                   keyLen,
        void                    *pStartKey,            
        const void              *pLowerKey,
        const void              *pUpperKey,
        size_t                   ObjBodySize,           
        void                    *pObjBody,             
        SAPDB_UInt8             &ResultOId, 
        short                   &e,
        OmsIterDirection        direction = OMS_ASCENDING
        ) PURE_VIRTUAL;
        
  /*-----------------------------------------------------------------------*/

  /*!
  * \brief commits the current subtransaction.
  *
  * Commits the current subtransaction.
  *
  * \param lpb_error [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE SubtransCommit( 
    short  *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief rolls back the current subtransaction.
  *
  * Rolls back the current subtransaction.
  *
  * \param lpb_error [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE SubtransRollback( 
    short  *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Creates a new subtransaction.
  *
  * \brief Creates a new subtransaction.
  *
  * \param lpb_error [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE SubtransStart( 
    short  *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Unlocks the object identified by an OID. 
  *
  * Unlocks the object identified by an OID. The caller must own a lock for the 
  * specified object.
  *    
  * \param  lpContainerId [in,out] bd identification of the container containing the object.
  *                       If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
  *                       the root and the fileno of the file containing the wanted object
  *                       is returned in the fileId. 
  * \param lpoid          [in] identifies the object that has to be unlocked.
  * \param lpb_error      [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE UnlockObj( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    short          *lpb_error) PURE_VIRTUAL;
       
  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Unlocks the shared lock of the object identified by an OID. 
  *
  * Unlocks the shared lock of the object identified by an OID. 
  * The caller must own a lock for the specified object.
  *    
  * \param  lpContainerId [in,out] bd identification of the container containing the object.
  *                       If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
  *                       the root and the fileno of the file containing the wanted object
  *                       is returned in the fileId. 
  * \param lpoid          [in] identifies the object that has to be unlocked.
  * \param lpb_error      [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE UnlockSharedObj( 
    unsigned char  *lpContainerId,
    SAPDB_UInt8    *lpoid,
    short          *lpb_error) PURE_VIRTUAL;
       
  /*-----------------------------------------------------------------------*/

  /*! 
  * \brief Replaces the current image of an object by a new one.
  *
  *  Replaces the current image of an object by a new one. The caller must have locked the object.
  *
  * \param lpConsistentView [in]     kb identification of the current consistent view.
  * \param lpContainerId    [in,out] bd identification of the container containing the object.
  *                                  If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
  *                                  the root and the fileno of the file containing the wanted object
  *                                  is returned in the fileId. 
  * \param lpVersionContext [in]     NULL, if the function is called outside of a version. Otherwise
  *                                  a version context returned by CreateOpenVersion.
  * \param lpoid            [in]     Oid of the object which should be updated,  
  * \param lpobjseq         [in,out] time stamp (=log position)of the consistent object image. 
  * \param bodySize         [in]     Size of the destination buffer where the found object image has to be copied.
  * \param lpbody           [in]     pointer to the destination buffer, i.e. memory where the object is returned.
  * \param keyPos           [in]     start position of the key in the object
  * \param keyLen           [in]     length of the key. If the object does not have a key, then keyLen == 0
  * \param contObj          [in]     if the parameter equals true, then the corresponding object is a 
  *                                  continuation object which belongs to an variable sized object
  * \param reused           [in]     tells, whether the object owns a locally reused oid,
  * \param e                [out]    error code of the operation, e_ok if no error occured, e_no_next_object if no
  *                                  next object has been found.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE UpdateObj( 
      unsigned char  *lpConsistentView,
      unsigned char  *lpContainerId,
      unsigned char  *lpVersionContext,
      unsigned char  *lpoid,
      unsigned char  *lpobjseq,
      size_t          bodysize,
      unsigned char  *lpbody,
      size_t          keyPos,       // PTS 1122540
      size_t          keyLen,       // PTS 1122540
      bool            contObj,      // PTS 1124278
      bool            reused,       // PTS 1127661
      short          *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*! 
  * \brief Replaces the current images of the objects by a new ones.
  *
  *  Replaces the current images of the objects by a new ones. The caller must have locked the objects.
  *
  * This method updates the specified objects in a loop and therefore it can be
  * used to reduce the communication overhead between the liboms and the kernel
  * by providing a mass interface.
  *
  * \param cnt              [in]     number of objects that should be updated
  * \param pConsistentView  [in]     kb identification of the current consistent view.
  * \param ppContainerId    [in,out] bd identification of the containers containing the objects.
  *                                  If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
  *                                  the root and the fileno of the file containing the wanted object
  *                                  is returned in the fileId. 
  * \param pVersionContext  [in]     NULL, if the function is called outside of a version. 
  *                                  Otherwise the version context returned by CreateOpenVersion.
  * \param pOid             [in]     array with the oids of the objects which should be updated,  
  * \param pObjSeq          [in,out] array with the time stamps (=log positions)of the consistent object images. 
  * \param pBodySize        [in]     array with the sizes of the destination buffer where the found object images have to be copied.
  * \param ppBody           [in]     array with pointers to the destination buffers, i.e. memory where the objects are returned.
  * \param pKeyPos          [in]     array with the start positions of the keys in the objects,
  * \param pKeyLen          [in]     array with the length of the keys. If the object i does not have a key, then pKeyLen[i] == 0
  * \param pReused          [in]     array which tells, whether the object owns a locally reused oid,
  * \param clustered        [in]     if true, then all given oids belong to the same page
  * \param errCnt           [out]    number of errors that occured,
  * \param pDBError         [out]    array with the error codes of the operation, e_ok if no error occured, e_no_next_object if no
  *                                  next object has been found.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  *
  * \since PTS 1121449
  */
  virtual HRESULT STDMETHODCALLTYPE UpdateObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    size_t                  *pBodySize,
    unsigned char          **ppBody,
    size_t                  *pKeyPos,    // PTS 1122540
    size_t                  *pKeyLen,    // PTS 1122540
    bool                    *pReused,    // PTS 1127661
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*! 
  * \brief Replaces the current images of variable sized objects by new ones.
  *
  *  Replaces the current images of the objects by a new ones. The caller must have locked the objects.
  *
  * This method updates the specified objects in a loop and therefore it can be
  * used to reduce the communication overhead between the liboms and the kernel
  * by providing a mass interface.
  *
  * \param cnt              [in]     number of objects that should be updated
  * \param pConsistentView  [in]     kb identification of the current consistent view.
  * \param ppContainerId    [in,out] bd identification of the containers containing the objects.
  *                                  If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
  *                                  the root and the fileno of the file containing the wanted object
  *                                  is returned in the fileId. 
  * \param pVersionContext  [in]     NULL, if the function is called outside of a version. 
  *                                  Otherwise the version context returned by CreateOpenVersion.
  * \param pOid             [in]     array with the oids of the objects which should be updated,  
  * \param pObjSeq          [in,out] array with the time stamps (=log positions)of the consistent object images. 
  * \param pObjSize         [in]     array with the sizes of the complete var-objects (a var-object might be splitted across 
  *                                  several objects with sizes <= 8kB)
  * \param pBodySize        [in]     array with the sizes of the destination buffer where the found object images have to be copied.
  * \param ppBody           [in]     array with pointers to the destination buffers, i.e. memory where the objects are returned.
  * \param pNextOid         [in]     array with the oid which points to the next oid belonging to the same object (this is
  *                                  needed if an object (>8kB) is splitt into several smaller Objects)
  * \param pContObj         [in]     if an entry in this array equals true, then the corresponding object is a 
  *                                  continuation object which belongs to an variable sized object
  * \param pReused          [in]     array which tells, whether the object owns a locally reused oid,
  * \param errCnt           [out]    number of errors that occured,
  * \param pDBError         [out]    array with the error codes of the operation, e_ok if no error occured, e_no_next_object if no
  *                                  next object has been found.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  *
  * \since PTS 1121449
  */
  virtual HRESULT STDMETHODCALLTYPE UpdateVarObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    size_t                  *pObjSize,
    size_t                  *pBodySize,
    unsigned char          **ppBody,
    SAPDB_UInt8             *pNextOid,
    bool                    *pContObj,          // PTS 1124278
    bool                    *pReused,           // PTS 1127661
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief reads the next largest objects from the iterator of unloaded versions.
  *
  * Reads the next largest objects from the iterator of unloaded versions.
  * Before calling this method the iterator must have been created using the function
  * StartUnloadedVersionIter.
  * After the last object was read the iterator sets the error e_no_next_object.
  *
  * \param VersionContext   [in]   NULL, if the function is called outside of an unloaded version. Otherwise
  *                         pointer to a version context.
  * \param ObjFile          [in]   bd identification of the container to be traversed.
  * \param pKey             [in/out] in-parameter: key of the current object. This key is used as starting
  *                         point for the determination of the next object.
  *                         out-parameter: key of the object which is returned.
  * \param KeyLen           [in]   the key length of the object.
  * \param Inclusive        [in]   if inclusive equals true and there is an object with key equal to the given
  *                         key (pKey), then this object is returned. 
  *                         If inclusive equals false, then the next largest object is returned.
  * \param pStopKey         [in]   The upper boundary key.
  * \param Oid              [out]  OID of the object which has been read.
  * \param e                [out]  error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE VersionNextKey(
    tgg01_OmsVersionContext* VersionContext,
    const tgg01_ContainerId& ObjFile,
    void*                    pKey,
    size_t                   KeyLen,
    bool                     Inclusive,
    const void*              pStopKey,
    SAPDB_UInt8&              Oid,
    tgg00_BasisError&        e) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief reads the next smallest objects from the iterator of unloaded versions.
  *
  * Reads the next smallest objects from the iterator of unloaded versions.
  * Before calling this method the iterator must have been created using the function
  * StartUnloadedVersionIter.
  * After the last object was read the iterator sets the error e_no_next_object.
  *
  * \param VersionContext   [in]   NULL, if the function is called outside of an unloaded version. Otherwise
  *                         pointer to a version context.
  * \param ObjFile          [in]   bd identification of the container to be traversed.
  * \param pKey             [in/out] in-parameter: key of the current object. This key is used as starting
  *                         point for the determination of the next object.
  *                         out-parameter: key of the object which is returned.
  * \param KeyLen           [in]   the key length of the object.
  * \param Inclusive        [in]   if inclusive equals true and there is an object with key equal to the given
  *                         key (pKey), then this object is returned. 
  *                         If inclusive equals false, then the next smallest object is returned.
  * \param pStopKey         [in]   The lower boundary key.
  * \param Oid              [out]  OID of the object which has been read.
  * \param e                [out]  error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  *
  * \since PTS 1119480
  */
  virtual HRESULT STDMETHODCALLTYPE VersionPrevKey(
    tgg01_OmsVersionContext* VersionContext,
    const tgg01_ContainerId& ObjFile,
    void*                    pKey,
    size_t                   KeyLen,
    bool                     Inclusive,
    const void*              pStopKey,
    SAPDB_UInt8&             Oid,
    tgg00_BasisError&        e) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief writes a message into the kerne vtrace
  *
  * Writes a message into the kerne vtrace
  *
  * \param buflen [in] length of the message
  * \param lpv    [in] pointer to the message
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  */
  virtual HRESULT STDMETHODCALLTYPE Vtrace( 
    tsp00_Int4      buflen,
    unsigned char  *lpv) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief returns a pointer stored by SetDefaultContext
  *
  * Returns a pointer stored by SetDefaultContext
  *
  * \param p                            [out] a pointer stored by SetDefaultContext
  * \param pCancelByte                  [out] a pointer to the cancel byte
  * \param optimizedStreamCommunication [out] true, if the application supports the optimized stream protocol
  * \param taskId                       [out] the task identification
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  */
  virtual HRESULT STDMETHODCALLTYPE GetDefaultContext(
    void         **p, 
    pasbool      **pCancelByte,
    bool         &optimizedStreamCommunication, /* PTS 1130281 */
    tsp00_TaskId  &taskId /* PTS 1110315 */) PURE_VIRTUAL;
        
  /*-----------------------------------------------------------------------*/

  /*!
  * \brief stores a pointer which can be inquired by GetDefaultContext
  *
  * Stores a pointer which can be inquired by GetDefaultContext
  *
  * \param p [in] a pointer 
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  */
  virtual HRESULT STDMETHODCALLTYPE SetDefaultContext(
    void* p) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief returns a pointer stored by SetSqlSessionContext
  *
  * Returns a pointer stored by SetSqlSessionContext
  *
  * \param p [out] a pointer stored by SetSqlSessionContext
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  *
  * \since PTS 1120794
  */
  virtual HRESULT STDMETHODCALLTYPE GetSqlSessionContext(
    void** p,
    tsp00_TaskId  &taskId) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief stores a pointer which can be inquired by GetSqlSessionContext
  *
  * Stores a pointer which can be inquired by GetSqlSessionContext
  *
  * \param  p [in] a pointer.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  *
  * \since PTS 1120794
  */
  virtual HRESULT STDMETHODCALLTYPE SetSqlSessionContext(
    void* p) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Opens a message file
  *
  * Opens a message file.
  * If doAppend is true and the file already exists it is opened but writing takes place at end of file.
  * MaxFileSize is ignored in such a case.
  * If doAppend is false or the file does not exist, a file is created. If file already exists and 
  * is not still opened it is overwritten! So be carefull, choosing the filename.
  *
  * A second open to the same file, simply increases a reference count. MaxFileSize is ignored in such a case.
  *
  * For each open, a close must be issued.
  *
  * \param lpFileName [in] the file name
  * \param FileSize   [in] the maximum size of the file. If 0 the file size is ignored. fileSize has
  *                   no meaning, if the file already exists and is not overwritten.
  *                   If not 0 writing restarts at beginning of file after MaxFileSize was reached.
  * \param doAppend   [in] \c true : if the file already exist it is opened but writing takes place at end of file.
  *                   \c false : if the file already exists, it is overwritten
  * \param FileHandle [out] the index of the message file (NOT a file handle!!!)
  * \param lpb_error  [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  */
  virtual HRESULT STDMETHODCALLTYPE OpenFile(
    char        *lpFileName,
    tsp00_Uint4  FileSize,
    boolean      doAppend, 
    tsp00_Int4  *FileHandle,
    short       *lpb_error) PURE_VIRTUAL;
          
  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Writes a null terminated string into a message file
  *
  * Writes a null terminated string into a message file.
  * This expects a 0 terminated 'C String' as input. The message written is not flushed
  * immediatly, but stored in internal buffers. Flushing takes place on dbpCloseMessageFile.
  * The length of the message is limited to 255 bytes.
  * 
  * \note If length of message exceeds MaxFileSize, only the last 'piece' of the message will
  * be visible.
  *
  * \param FileHandle [in] The file index returned by OpenFile
  * \param msg        [in] The message to be written.
  * \param lpb_error  [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  */
  virtual HRESULT STDMETHODCALLTYPE WriteFile(
    tsp00_Int4 FileHandle,
    char   *msg,
    short  *lpb_error) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief closes a message file
  *
  * Closes a message file. This flushes the content of the buffers and allows the index 
  * to be used again by a different dbpOpenMessageFile call.
  *
  * If file was opened more than once, the reference count is decreased but the file
  * is kept open.
  *
  * But always the file is implicitly synchronized as if SyncFile() was called.
  *
  * \param FileHandle [in] : the file index returned by dbpOpenMessageFile
  * \param lpb_error  [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  */
  virtual HRESULT STDMETHODCALLTYPE CloseFile(
    tsp00_Int4  FileHandle,
    short      *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief flushes buffered messages into a message file
  *
  * This function flushes the content of the buffers without closing the file. 
  * Not really needed for small files, since flushing takes place automatically before dbpCloseMessageFile.
  *
  * \note Using this after each write, will reduce performance of the database dramatically...
  *
  * \param lpFileName [in]  The name of the message file as specified in OpenFile. NULL if the
  *                   file is identified via FileHandle.
  * \param FileHandle [in]  the file index returned by dbpOpenMessageFile, has no meaning, if
  *                   lpFileName is given.
  * \param lpb_error  [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  */
  virtual HRESULT STDMETHODCALLTYPE SyncFile(
    char       *lpFileName,
    tsp00_Int4  FileHandle,
    short      *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief return current date and time
  *
  * Return current date and time
  *
  * \param Date [out] current date.
  * \param Time [out] current time.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  */
  virtual HRESULT STDMETHODCALLTYPE GetDateTime(
    tsp00_Date* Date,
    tsp00_Time* Time) PURE_VIRTUAL;
        
  /*-----------------------------------------------------------------------*/

  /*!
  * \brief return the current clock
  * 
  * Return the current clock
  *
  * \param Sec  [out] seconds.
  * \param MSec [out] milliseconds
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  */
  virtual HRESULT STDMETHODCALLTYPE GetClock(
    tsp00_Int4* Sec,
    tsp00_Int4* MSec) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief drops a version.
  * 
  * Drops a version
  *
  * \param lpVersionContext [in]  version context identifying the version.
  * \param lpb_error        [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case.
  */
  virtual HRESULT STDMETHODCALLTYPE DropVersion( 
    unsigned char  *lpVersionContext,
    short          *lpb_error) PURE_VIRTUAL;
   
  /*-----------------------------------------------------------------------*/

  /*!
  * \brief creates a schema. 
  *
  * Creates a schema. Stored meta data describing the schema in the catalog
  *
  * \param lpSchemaName [in]  the schema name in wyde representation, must be unique.
  * \param lpSchemaId   [out] the schema identification. Serves as as handle for following calls
  * \param lpb_error    [out] error code of the operation, e_ok if no error occured.
  *  
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE CreateSchema(
    const OmsTypeWyde *lpSchemaName,
    tsp00_Int4        *lpSchemaId,
    short             *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Checks the existence of a schema
  *
  * Checks the existence of a schema
  *
  * \param lpSchemaName [in]  the schema name in wyde representation.
  * \param lpSchemaId   [out] schema handle. 0 if schema does not exist.
  * \param lpb_error    [out] error code of the operation, e_ok if no error occured.
  *   
  * \return value: HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE ExistsSchema(
    const OmsTypeWyde *lpSchemaName,
    tsp00_Int4        *lpSchemaId,
    short             *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief drops a schema
  *
  * Drops a schema, i.e. deletes meta data describing the schema from the catalog
  *
  * \param lpSchemaId   [in ] schema handle, identifies the schema.
  * \param lpb_error    [out] error code of the operation, e_ok if no error occured.
  *  
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE DropSchema(
    tsp00_Int4  SchemaId,
    short      *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Yields a schema name for a given schema handle.
  *
  * Yields a schema name for a given schema handle.
  *
  * \param lpSchemaId   [in ] schema handle, identifies the schema.
  * \param lpSchemaName [out] the schema name in wyde reprensentation.
  * \param lpb_error    [out] e_ok, if the schema exists; otherwise e_unknown_name
  *  
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE GetSchemaName(
    tsp00_Int4    SchemaId,
    OmsTypeWyde  *lpSchemaName,
    short        *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
    * \brief Checks for existence of file or directory
    *
    * Called to test for existence of file or directory. 
    * Sets doesExist to true if file or directory exists, false otherwise or if error occured.
    * If doesExist is false isDirectory is always set to false.
    *
    * \param lpFileName   [in]  The file/directory name
    * \param existsFile   [out] Set to true if file or directory was found, false if not.
    * \param isDirectory  [out] Set to true to indicate directory was found, false if not.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    */
  virtual HRESULT STDMETHODCALLTYPE ExistsFile(
    char    *lpFileName,
    bool    &existsFile,
    bool    &isDirectory,
    short   &error) PURE_VIRTUAL;
        
  /*-----------------------------------------------------------------------*/

  /*!
  * \brief drops a message file
  *
  * This tries to drop a named MessageFile. If the file does not exist or is still open,
  * it will not be dropped and an error is thrown instead.
  *
  * \param lpFileName [in] The file name of the file to be dropped.
  * \param lpb_error  [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE DropFile(
    char  *lpFileName,
    short &error) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Get path to directory where message files are stored
  *
  * This returns the path to the directory that is used for all relativ specified message files.
  * It should be identical to the WorkingDirectory of the database.
  *
  * \param pathName  [out]  The path to the root of all relativ specified message files
  * \param lpb_error [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE GetWorkingDirectory(
    tsp00_Pathc& pathName,
    short  &error) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief gets the DBROOT dependent "sap" path ( libraries, shared libraries... )
  *
  * Gets the DBROOT dependent "sap" path ( libraries, shared libraries... )
  *
  * \param pathName  [out]  the required path. 
  * \param lpb_error [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE GetDbrootSapPath(
    tsp00_Pathc &pathName,
    short       &error) PURE_VIRTUAL;
    
  /*-----------------------------------------------------------------------*/

  /*!
  * \brief loads a shared library
  *
  * Loads a shared library
  *
  * \param path         [in] The name of the library to be loaded. The name can 
  *                          include the path but it should no include the fileextension. 
  * \param loadGobal    [in] \c true  : load with 'global' scope of symbol-resolution
  *                          \c false : load with 'local' scope of symbol-resolution
  * \param hd           [out] handle for the loaded library.
  * \param errtext      [out] error description, if the library could not be loaded.
  * \param errtext_size [in]  length of the error description.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE LoadLibraryEx( 
    const char    *path, 
    bool           loadGlobal,
    HANDLE        *hd,
    tsp00_ErrText &errtext,
    int            errtext_size) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief unloads a shared library
  *
  * Unloads a shared library
  *
  * \param hd           [in]  handle to the shared library.
  * \param ok           [out] true, if no error occured; otherwise false.
  * \param errtext      [out] error description, if the library could not be loaded.
  * \param errtext_size [in] length of the error description.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE FreeLibrary( 
    HANDLE         hd,
    bool          &ok,
    tsp00_ErrText &errtext,
    int            errtext_size) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  *
  * \brief drops a class container
  *
  * Drops a class container and deletes corresponding meta data from catalog.
  *
  * \param schema   [in] schema handle of the schema which contains the class container
  * \param guid     [in] global unique identifier describing the class stored in the container.
  * \param cno      [in] container number identifying the container.
  * \param error    [out] error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE DropContainer(
    OmsSchemaHandle   schema,
    const GUID       &guid,
    OmsContainerNo    cno,
    tgg00_BasisError &error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief yields information about a class container. 
  *
  * Yields information about a class container. 
  *
  * \param containerHandle [in]  container handle identifying the class container.
  * \param guid            [out] global unique identifier describing the class stored in the container.
  * \param containerHandle [out] schema handle of the schema the container belongs to.
  * \param cno             [out] container number of the class container.
  * \param error           [out] error code of the operation, e_ok if no error occured.
  *
  * \return  HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE GetContainerInfo(
    OMS_ContainerHandle &containerHandle,   // PTS 1124356 FF 2003-09-23 new FileDir
    GUID            &guid,
    OmsSchemaHandle &schema,
    OmsContainerNo  &cno,
    short           &error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Checks whether object is still accessible from any transaction
  * 
  * Checks whether object is still accessible from any transaction. This function
  * is used by an application driven garbage collection in the context of ensuring
  * referencial integrity.
  *
  * \param oid     [in]  oid     identification of the object which should be checked
  * \param isUsed  [in]  ignoreVersions  ignore consistent views used within omsVersions
  * \param isUsed  [out] isUsed  is the object still used
  * \param e       [out] error code of the operation, e_ok if no error occured.
  * \param OldestTransNo [out] no. of oldest transaction possibly using history of object
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
	virtual HRESULT STDMETHODCALLTYPE IsObjHistoryUsed( 
		const SAPDB_UInt8  &oid,
		bool                ignoreVersions,            // PTS 1130354
		bool               &isUsed,
		tgg00_BasisError   &e,
		OmsTypeUInt8       &OldestTransNo /*PTS 1130354*/) PURE_VIRTUAL;

	/*-----------------------------------------------------------------------*/

  /*!
  * \brief calls the dispatcher
  *
  * Calls the dispatcher
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  *
  * \since PTS 1107849
  */
  virtual HRESULT STDMETHODCALLTYPE Reschedule() PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief calls vsleep
  *
  * Calls vsleep
  *
  * \param sleepTime [in] time to sleep
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  *
  * \since PTS 1132163
  */
  virtual HRESULT STDMETHODCALLTYPE Sleep(
      const OmsTypeUInt1 sleepTime) PURE_VIRTUAL;
       
  /*-----------------------------------------------------------------------*/

  /*!
  * \brief  Locks an object and marks it as updated
  * 
  * Locks an object and marks it as updated. If an object cannot be locked since the 
  * wanted version does not correspond to the current image of the object the error 
  * e_object_dirty is returned. 
  *
  * \param lpConsistentView [in]     kb identification of the current consistent view.
  * \param lpContainerId    [in,out] bd identification of the container containing the object.
  *                                  If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
  *                                  the root and the fileno of the file containing the wanted object
  *                                  is returned in the fileId. 
  * \param lpoid            [in]     object identifier of the object to be locked.
  * \param lpobjseq         [in,out] wanted time stamp (=log position) of the consistent object image. 
  * \param lpb_error        [out]    error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE LockUpdObj( 
      unsigned char  *lpConsistentView,
      unsigned char  *lpContainerId,
      SAPDB_UInt8    *lpoid,
      unsigned char  *lpobjseq,
      short          *lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief  Locks all specified objects and marks them as updated
  * 
  * Locks all specified objects and marks it them updated. If an object cannot be locked since 
  * the wanted version does not correspond to the current image of the object the error 
  * e_object_dirty is returned. 
  *
  * This method locks all specified objects in a loop and therefore it can be used 
  * to reduce the communication overhead between the liboms and the kernel by providing
  * a mass interface.
  *
  * \param cnt              [in]     number of objects that should be processed
  * \param pConsistentView  [in]     kb identification of the current consistent view.
  * \param ppContainerId    [in,out] bd identification of the container containing the object.
  *                                  If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
  *                                  the root and the fileno of the file containing the wanted object
  *                                  is returned in the fileId. 
  * \param pOid             [in]     array with the object identifiers of the objects to be locked.
  * \param pObjSeq          [in,out] array with the wanted time stamps (=log position) of the consistent object images. 
  * \param clustered        [in]     if true, then all given oids belong to the same page
  * \param errCnt           [out]    number of errors that occured,
  * \param pDBError         [out]    array with the error codes of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  *
  * \since PTS 1121449
  */
  virtual HRESULT STDMETHODCALLTYPE LockUpdObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    bool                     clustered,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/
  /*!
  * \brief Aquire shared locks on an array of object.
  *
  * Aquire shared locks on an array of object. If an object cannot be locked since the wanted 
  * version does not correspond to the current image of the object the error e_object_dirty is 
  * returned. 
  * If a timeout is specified, it is checked, that the locking of all objects does not take
  * more time than the specified value. If more time would be needed, then the procedure
  * is left. If less time is needed the remaining time is returned.
  *
  * This method locks every specified oid in a loop and therefore it can be used 
  * to reduce the communication overhead between the liboms and the kernel by 
  * providing a mass interface.
  *
  * \param cnt              [in,out] number of oids which should be locked. After the call, this counter tells how
  *                         many objects have been considered for locking. This means that the counter will only be
  *                         different if the processing has been stopped because of a timeout.
  * \param pConsistentView  [in] kb identification of the current consistent view, 
  * \param ppContainerId    [in,out] bd identification of the container containing the object,
  *                         If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
  *                         the root and the fileno of the file containing the wanted object
  *                         is returned in the fileId. 
  * \param pOid             [in] Array with the object identifiers of the objects to be locked,
  * \param pObjSeq          [in] Array with the wanted time stamps (=log positions) of the consistent object images, 
  * \param timeout          [in,out] maximal time for the locking of all objects (if timeout!=0). 
  *                         After the call the variable equals the remaining time = original timeout - running time,
  * \param errCnt           [out] number of errors which occur,
  * \param pDBError         [out] array with the error code of the operation, e_ok if no error occured.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  *
  * \since PTS 1130435
  */
   virtual HRESULT STDMETHODCALLTYPE LockSharedObjMass(
    tsp00_Int4              &cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    tsp00_Int8              &timeout,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/
  /*!
    * \brief Aquire shared lock on object.
    *
    * Aquire shared lock on object. If an object cannot be locked since the wanted version does not correspond 
    * to the current image of the object the error e_object_dirty is returned. 
    *
    * \param lpConsistentView [in] kb identification of the current consistent view, 
    * \param lpContainerId    [in,out] bd identification of the container containing the object,
    *                         If the fileid is of the type unknown (ObjFileId.fileObjFileType_gg00 == oftUnknownType_egg00)
    *                         the root and the fileno of the file containing the wanted object
    *                         is returned in the fileId. 
    * \param lpoid            [in] object identifier of the object to be locked,
    * \param lpobjseq         [in] wanted time stamp (=log position) of the consistent object image, 
    * \param lpb_error        [out] error code of the operation, e_ok if no error occured.
    *
    * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
    *
    * \since PTS 1130435
    */
  virtual HRESULT STDMETHODCALLTYPE LockSharedObj(
    unsigned char  * lpConsistentView,
    unsigned char  * lpContainerId,
    SAPDB_UInt8    * lpoid,
    unsigned char  * lpobjseq,
    short          * lpb_error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief signals a task to continue execution.
  *
  * Signals a task to continue execution after a wait call supending the task.
  *
  * \param TaskId [in] the task identification of the task to be signaled
  * \param ok     [in] \c true signals the task, that the resource the task was waiting for has been
  *                    granted. Otherwise a timeout has been occured (\c false)
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  * 
  * \since PTS 1110315
  */
  virtual HRESULT STDMETHODCALLTYPE Signal(
    tsp00_TaskId TaskId, 
    tsp00_Bool ok) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief suspends the current task.
  *
  * Suspends the current task, until it is signaled to continue execution.
  *
  * \param ok [out] signals the task, whether the resource the task was waiting for has been
  *           granted (\c true). Otherwise a timeout has been occured (\c false)
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  *
  * \since PTS 1110315
  */
  virtual HRESULT STDMETHODCALLTYPE Wait(
    tsp00_Bool& ok) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief yields, whether a container exists in the catalog.
  *
  * Yields, whether a container exists in the catalog.
  *
  * \param schemaId        [in] schema handle of the schema which contains the class container
  * \param guid            [in] global unique identifier describing the class stored in the container.
  * \param containerNo     [in] container number identifying the container.
  * \param objectSize      [in] Check whether objectsize in the kernel is still the same as in liboms.
  *                             If objectSize = 0, then perform no check.
  * \param keyLen          [in] Check whether key-length in the kernel is still the same as in liboms.
  *                             If keyLen = 0, then perform no check.
  * \param containerHandle [out] FileId of the container in the kernel (used as containerHandle in liboms)
  * \param useCachedKeys   [out] What is the default setting for this container regarding cached-keys
  * \param partitionedKeys [out] Are the keys distributed among several B*-trees?
  * \param error           [out] error code of the operation, e_ok if container exists
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE ExistsContainer(
    tsp00_Int4           schemaId,
    const GUID          &guid,
    OmsContainerNo       containerNo,
    size_t               objectSize,
    size_t               keyLen,
    tgg01_ContainerId   &containerId,
    bool                &useCachedKeys,
    bool                &partitionedKeys,
    short               &error) PURE_VIRTUAL; // PTS 1110334
  /*-----------------------------------------------------------------------*/

  /*!
  * \brief acquires a shared lock on the specified catalog.
  *
  * Acquires a shared lock on the specified catalog.
  *
  * \param containerHandle [in ] FileId of the container in the kernel (used as containerHandle in liboms)
  * \param error           [out] error code of the operation, e_ok if container could be locked
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE LockContainer(
    const tgg00_FileId &containerId,
    short              &error) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief returns the current heap usage of the caller
  *
  * Returns the current heap usage of the caller
  *
  * \param heapUsage [out] heap usage in bytes
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE GetSessionHeapUsage(
    tsp00_8ByteCounter& heapUsage) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief calculates the statistics of the last dbproc call
  *
  * Calculates the statistics of the last dbproc call
  *
  * \param AllocatorOnly [in] if \c true, only allocator statistics are returned; otherwise
  *                      full info is returned.
  * \param pMonitorInfo  [in,out] the address of the monitor info. This is an address lying
  *                      in the application dll. 
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE EvalCallStatistics(
        bool            AllocatorOnly,
        OmsMonitorData& MonitorData) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Implementation of omsSimDumpToHtml* methods of OmsAbstractObject.
  *
  * Implementation of omsSimDumpToHtml* methods of OmsAbstractObject.
	* This makes sense only in simulator.
  *
  * \param handle [in] OMS handle
	* \param obj    [in] OID of the object to dump
	* \param str    [in] destination for the dump data
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE SimCtlDumpToHtml(
		class OmsHandle           &handle,
		SAPDB_UInt8                obj,
		class OmsSimDumpCollector &str) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Implementation of omsSimSetObjectName method of OmsAbstractObject.
  *
  * Implementation of omsSimSetObjectName method of OmsAbstractObject.
	* This makes sense only in simulator.
  *
  * \param handle	[in]	OMS handle
	* \param obj		[in]	OID of the object
	* \param name		[in]	new name
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE SimCtlSetObjectName(
		class OmsHandle &handle,
		SAPDB_UInt8      obj,
		const char      *name) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Implementation of omsSimGetObjectName method of OmsAbstractObject.
  *
  * Implementation of omsSimGetObjectName method of OmsAbstractObject.
	* This makes sense only in simulator.
  *
  * \param handle	[in]	OMS handle
	* \param obj		[in]	OID of the object
	* \param name		[in]	pointer where to store pointer to name
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE SimCtlGetObjectName(
	  class OmsHandle &handle,
	  SAPDB_UInt8      obj,
	  const char     **name) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief This callback is called on destroy of OmsHandle in simulator to detach the sink.
  *
  * This callback is called on destroy of OmsHandle in simulator to detach the sink.
  *
  * \param handle	[in]	OMS handle
	* \param sinkptr	[in]	pointer to sink pointer - NULLed if last instance
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE SimCtlDestroyHandleCB(
	  class OmsHandle				&handle,
	  struct IliveCacheSink		**session) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * Return HTML link to an object in simulator dump.
  *
  * Return HTML link to an object in simulator dump.
	* This makes sense only in simulator.
  *
  * \param handle [in]	OMS handle
	* \param obj    [in]	OID of the object
	* \param data   [in]	pointer where to store pointer to HTML code
	* \param flags  [in]	dump flags
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE SimCtlGetHTMLRef(
		class OmsHandle  &handle,
		SAPDB_UInt8       obj,
		const char      **data,
		int              flags) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Is the current session in the requested oms region?
  *
  * Is the current session in the requested oms region?
  *
  * \param regionId [in] the required regionId.
  * \param inRegion [out] true, if current session is in region
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE IsInRegion(
    int    regionId,
    bool  &inRegion,
    short &error) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief attaches to a sequence. 
  *
  * Attaches to a sequence. If the sequence does not already exist, it is created automatically
  * 
  * \param sequenceName [in] the name of the sequence
  * \param sequenceId   [out] the internal name of the sequence
  * \param sequenceInfo [out] sequence properties
  * \param sqlError     [out] sql error code
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE AttachSequence(
    const tsp00_KnlIdentifier &sequenceName,
    tgg00_Surrogate           &sequenceId,
    tgg00_SeqInfo             &sequenceInfo,
    short                     &sqlError) PURE_VIRTUAL; 

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief returns the next number generated by a sequence.
  *
  * Returns the next number generated by a sequence.
  * 
  * \param sequenceId   [in] the internal name of the sequence
  * \param sequenceInfo [in,out] sequence properties
  * \param nextVal      [out] the generated number 
  * \param sqlError     [out] sql error code
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE NextVal(
    const tgg00_Surrogate &sequenceId,
    tgg00_SeqInfo         &sequenceInfo,
    tsp00_Uint4           &nextVal,
    short                 &sqlError) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief returns true, if liveCache user's (SAPR3) default code is unicode
  *
  * Returns \c true, if liveCache user's (SAPR3) default code is unicode
  *
  * \param isUnicodeInstance [out] \c true, if liveCache user's (SAPR3) default code is unicode,
  *                          otherwise \false
  * \param error             [out] basis error
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE IsUnicodeInstance(
    bool& isUnicodeInstance,
    short& sqlError) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Insert new version into the version dictionary
  *
  * Insert new version into the version dictionary
  *
  * \note This method is only called if sink belongs to a proc-server.
  *
  * \param versionId       [in]  identification of the new version
  * \param pVersionContext [in]  pointer to the context containing the new version
  * \param error           [out] error code of the operation
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE VersionDictInsert(
    const OmsVersionId  &versionId,
    const OMS_Context   *pVersionContext,
    tgg00_BasisError    &error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Lookup a version in the version dictionary
  *
  * Lookup a version in the version dictionary
  *
  * \note This method is only called if sink belongs to a proc-server.
  *
  * \param versionId        [in]  identification of the version
  * \param ppVersionContext [out] pointer to the context containing the requested version
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE VersionDictFind(
    const OmsVersionId   &versionId,
    OMS_Context         **ppVersionContext) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Deletes a version from the version dictionary
  *
  * Deletes a version from the version dictionary
  *
  * \note This method is only called if sink belongs to a proc-server.
  *
  * \param versionId       [in]  identification of the version
  * \param error           [out] error code of the operation
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE VersionDictDrop(
    const OmsVersionId  &versionId,
    tgg00_BasisError    &error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Shuts down the version dictionary
  *
  * Shuts down the version dictionary
  *
  * \note This method is only called if sink belongs to a proc-server.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE VersionDictShutdown() PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Unloads the oldest version
  *
  * Unloads the oldest version
  *
  * \note This method is only called if sink belongs to a proc-server.
  *
  * \param unloaded [out] Was there a version which has been unloaded.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE VersionDictUnloadOldestVersion(
    bool &unloaded) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Marks a version as non-unloadable
  *
  * Marks a version as non-unloadable
  *
  * \note This method is only called if sink belongs to a proc-server.
  *
  * \param pVersionContext [in] Pointer to the context of the version which 
  *                             should be marked.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE VersionDictMarkNotUnloadable(
    OMS_Context *pVersionContext) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Marks a version as unloadable
  *
  * Marks a version as unloadable
  *
  * \note This method is only called if sink belongs to a proc-server.
  *
  * \param pVersionContext [in] Pointer to the context of the version which 
  *                             should be marked.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE VersionDictMarkUnloadable(
    OMS_Context *pVersionContext) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Creates an iterator for the global version directory
  *
  * Creates an iterator for the global version directory
  *
  * \note This method is only called if sink belongs to a proc-server.
  *
  * \param lockMode  [in]  which kind of locking should the iterator use (no, shared or exclusive)
  * \param ppIter    [out] Pointer to the iterator
  * \param ppContext [out] Pointer to the first version context in the
  *                  version directory, NULL if there are no versions.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE VersionDictCreateIter(
    OMS_LockMode   lockMode, 
    void         **ppIter, 
    OMS_Context  **ppContext) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Returns the next version in the global version directory
  *
  * Returns the next version in the global version directory
  *
  * \note This method is only called if sink belongs to a proc-server.
  *
  * \param ppIter    [in] Pointer to the iterator which has been created by 
  *                       the call of VersionDictCreateIter
  * \param ppContext [out] Pointer to the next version context in the
  *                        version directory, NULL if there are no more versions.
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE VersionDictGetNext(
    void         *pIter, 
    OMS_Context **ppContext) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/

  /*!
  * \brief Destructs the iterator on the global version directory
  *
  * Destructs the iterator on the global version directory
  *
  * \note This method is only called if sink belongs to a proc-server.
  *
  * \param pIter    [in] Pointer to the iterator which has been created by 
  *                      the call of VersionDictCreateIter
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE VersionDictDestructIter(
    void         *pIter) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/
  /*!
  * \brief Return sink type.
  *
  * Return one of the following sink types:
  *   - 0 - normal sink,
  *   - 1 - simulator sink,
  *   - 2 - ProcServer sink.
  */
  virtual int STDMETHODCALLTYPE GetSinkType() PURE_VIRTUAL;


  /*-----------------------------------------------------------------------*/
  /*!
  * \brief inserts a comment to the actual transaction context
  *
  * \param commentLength  [in] length (number of characters) of comment string  
  * \param pComment       [in] comment string
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE SetTransactionComment(
    tsp00_Int2        commentLength,
    const OmsTypeWyde *pComment,
    short             &Error) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/
  /*!
  * \brief returns whether the testkernel has provoked a bad alloc
  *
  * \param active  [out] \c true if a bad alloc has been provoked; else \c false
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE IsTestKnlBadAllocActive(
    bool    &active) PURE_VIRTUAL;

  /*-----------------------------------------------------------------------*/
  /*!
  * \brief Resets the stored monitoring data
  *
  * \return HRESULT of the operation. Has no meaning (S_OK) in inproc case. 
  */
  virtual HRESULT STDMETHODCALLTYPE ResetMonitor() PURE_VIRTUAL;

#if defined(LIVECACHE_SINK_ONLY_MEMBER_FUNCS)
#undef LIVECACHE_SINK_ONLY_MEMBER_FUNCS
#else
  };
//}
#endif

#endif  /* __LVC_ILIVECACHESINK_HPP */
