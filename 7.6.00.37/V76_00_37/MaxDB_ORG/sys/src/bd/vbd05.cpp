/*
  @file           vbd05.cpp
  @author         TorstenS
  @ingroup        DataAccess LOB interface
  @brief          

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2006 SAP AG

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

\endif
*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types
#include "gbd00.h"   // PASCAL: BD_kernel_constants_and_types

#include "gsp03.h"
#include "gsp03_3.h"
#include "ggg11.h"   
#include "hbd01.h"
#include "hbd01_1.h"
#include "hbd05.h"
#include "hbd13.h"
#include "hbd20_9.h"
#include "hbd30.h"
#include "hbd60.h"
#include "hbd61.h"
#include "hbd73.h"
#include "hbd998.h"
#include "hgg01_1.h" 
#include "hgg01_3.h"

#include "Converter/Converter_Version.hpp"
#include "Converter/Converter_ICommon.hpp"
#include "Converter/Converter_PageCollector.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "Trace/Trace_Entry.hpp"
#include "Transaction/Trans_Context.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

class bd05_Tracer
{
public:

    void AddRoot( const tsp00_PageNo root ){
        m_Root = root;
    }

protected:

    bd05_Tracer(
        tgg00_VtraceType_Param  traceType,
        tgg00_BasisError&       trError )
    :
    m_TraceType( tgg00_VtraceType::fromConst( traceType )),
    m_TrError( trError ),
    m_Root( NIL_PAGE_NO_GG00 )
    {}

    ~bd05_Tracer()
    {
        if( ! g01vtrace.vtrAll_gg00 && ! g01vtrace.vtrBdString_gg00 ){
            return;
        }
        tgg11_BdRootTrace   rootTrace;

        rootTrace.bdrTrType_gg11.becomes( bdtrRoot_egg11 );
        rootTrace.bdrError_gg11 = m_TrError;
        rootTrace.bdrRoot_gg11  = m_Root;

        Trace_CommonEntry tce( tgg00_Debug::fromConst( bd ), m_TraceType,
                               reinterpret_cast<tsp00_BytePtr>( &rootTrace ), sizeof( rootTrace ));
    }

protected:

    const tgg00_VtraceType  m_TraceType;
    tgg00_BasisError&       m_TrError;
    tsp00_PageNo            m_Root;
};

/*---------------------------------------------------------------------------*/

class bd05_MethodTracer : public bd05_Tracer
{
public:

    bd05_MethodTracer(
        tgg00_VtraceType_Param  traceType,
        tgg00_BasisError&       trError,
        const tsp00_Int4        value1 = bd05_MethodTracer::m_Invalid,
        const tsp00_Int4        value2 = bd05_MethodTracer::m_Invalid,
        const tsp00_Int4        value3 = bd05_MethodTracer::m_Invalid )
    :
    bd05_Tracer( tgg00_VtraceType::fromConst( traceType ), trError )
    {
        if( ! g01vtrace.vtrAll_gg00 && ! g01vtrace.vtrBdString_gg00 ){
            return;
        }
        tgg11_BdIntTrace    int4Trace;
        tsp00_Int4          traceSize;

        int4Trace.bdiTrType_gg11.becomes( bdtrInt_egg11 );
        traceSize = sizeof( int4Trace );

        if( bd05_MethodTracer::m_Invalid == value1 )
        { // no value given; bdrTrType_gg11 will be ignored
            traceSize = 0;
            return;
        }else{
            int4Trace.bdiInt_gg11[ 0 ] = value1; // at least one value given
        }
        if( bd05_MethodTracer::m_Invalid == value2 ){
            traceSize -= ( 2 * sizeof( int4Trace.bdiInt_gg11[ 0 ] ));
            return;
        }else{
            int4Trace.bdiInt_gg11[ 1 ] = value2; // at least two values given
        }
        if( bd05_MethodTracer::m_Invalid == value3 ){
            traceSize -= ( sizeof( int4Trace.bdiInt_gg11[ 0 ] ));
            return;
        }
        int4Trace.bdiInt_gg11[ 2 ] = value3; // three values given

        Trace_CommonEntry tce( tgg00_Debug::fromConst( bd ), m_TraceType,
                               reinterpret_cast<tsp00_BytePtr>( &int4Trace ), traceSize );
    }

private:

    static tsp00_Int4 m_Invalid;

};

tsp00_Int4 bd05_MethodTracer::m_Invalid = -MAX_INT4_SP00;

/*---------------------------------------------------------------------------*/

class bd05_AdvancedMethodTracer : public bd05_Tracer
{
public:

    bd05_AdvancedMethodTracer(
        tgg00_VtraceType_Enum   traceType,
        tgg00_BasisError&       trError,
        const tsp00_Int4        position,
        const tsp00_Int4        length,
        const tsp00_Int4        bufferSize,
        const tsp00_Int4        bufferPosition,
        const tsp00_Byte*       pBuffer )
    :
    bd05_Tracer( tgg00_VtraceType::fromConst( traceType ), trError )
    {
        if( ! g01vtrace.vtrAll_gg00 && ! g01vtrace.vtrBdString_gg00 ){
            return;
        }
        tgg11_BdWriteStrTrace   writeTrace;
        const tsp00_Int4        traceHeadSize = sizeof( writeTrace.bdwsHead_gg11 );

        writeTrace.bdwsHead_gg11.bdwhTrType_gg11.becomes( bdtrWriteStr_egg11 );
        writeTrace.bdwsHead_gg11.bdwhPos_gg11 = position;
        writeTrace.bdwsHead_gg11.bdwhLen_gg11 = length;

        tsp00_Int4 reducedLength = (( traceHeadSize + length ) > sizeof( tgg11_VtraceBody ))?
                                   sizeof( tgg11_VtraceBody ) - traceHeadSize : length;
        tgg00_BasisError auxError = e_ok;
        SAPDB_RangeMove( __FILE__, 1,
                         bufferSize, sizeof( writeTrace.bdwsStr_gg11 ),
                         pBuffer, bufferPosition,
                         writeTrace.bdwsStr_gg11, POS_OFF_DIFF_BD00,
                         reducedLength, auxError );
        SAPDBERR_ASSERT_STATE( e_ok == auxError );

        Trace_CommonEntry tce( tgg00_Debug::fromConst( bd ), m_TraceType,
                               reinterpret_cast<tsp00_BytePtr>( &writeTrace ),
                               traceHeadSize + reducedLength );
    }
};

/*---------------------------------------------------------------------------*/

class cbd05_IOBuffer
{
public:

    cbd05_IOBuffer()
    :
    m_DataIOBlockCount( IOMan_IDataInfo::GetInstance().GetDataIOBlockCount()),
    m_BlockSize( FrameCtrl_ICommon::GetInstance().GetPageSize()),
    m_BufferSize( 0 )
    {}

    bool Initialize( const SAPDB_Int4 bufferSizeInPages )
    {
        SAPDBERR_ASSERT_STATE( bufferSizeInPages <= m_DataIOBlockCount );

        if( bufferSizeInPages > m_DataIOBlockCount ){
            return false;
        }
        SAPDB_Int4  bufferSize = bufferSizeInPages * m_BlockSize;
        SAPDB_Byte* pFrames = static_cast< SAPDB_Byte* >
                              ( RTE_ISystem::Instance().AllocSystemPages( bufferSize ));
        if( 0 == pFrames ){
            return false;
        }
        m_BufferSize = bufferSize;
        m_Frame.Assign( pFrames, m_BufferSize );
        return true;
    }

    Data_PageFrame& GetIOBuffer(){
        return m_Frame;
    }

    ~cbd05_IOBuffer()
    {
        if( ! m_Frame.IsAssigned()){
            return;
        }
        RTE_ISystem::Instance().FreeSystemPages( m_Frame.GetDataPtr(), m_BufferSize );
        m_Frame.Deassign();
    }

private:

    //  configured data page cluster size in pages
    const SAPDB_Int4  m_DataIOBlockCount;

    // size in bytes of a data page
    const SAPDB_Int4  m_BlockSize;

    // size in bytes of the continuous memory chunk
    SAPDB_Int4  m_BufferSize;

    // io frames
    Data_PageFrame  m_Frame;
};

/*---------------------------------------------------------------------------*/

struct tbd05_LobInfo{
    tbd_node_ptrs       pNode_bd05;
    tsp00_Int4          filePos_bd05;
    tsp00_Int4          fileLen_bd05;
    tbd_current_tree    current_bd05;
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

void
bd05_Copy(
    Trans_Context&      trans,
    tbd05_LobInfo&      sourceInfo,
    tbd05_LobInfo&      destInfo,
    tsp00_Int4          length,
    tsp00_Int4          maxFileLen );

/*---------------------------------------------------------------------------*/

inline tsp00_Int4
bd05_DeterminePageCount( const tbd_node_ptrs& pRoot )
{
    const tsp00_Int4 fileLength = pRoot.np_ptr()->ndStrFileSize_bd00();
    const tsp00_Int4 pageCount  = 1 +  /* root page */
                                  pRoot.np_ptr()->nd_level() +
                                  ( fileLength / ( PNOS_PER_PAGE_BD00 * MAX_BSTR_COVERING_BD00 )) +
                                  ( fileLength / MAX_BSTR_COVERING_BD00 );
    return pageCount;
}

/*---------------------------------------------------------------------------*/

inline void
bd05_ModifyLobCount(
    Trans_Context&          trans,
    const tbd_node_ptrs&    pRoots,
    const tgg00_Surrogate&  parentFileNo,
    const tsp00_Int4        pagesDelta )
{
    const Converter_Version convVersion =
        bd20GetDestinationConverterVersion( trans.Task().ID(), pRoots.np_cbptr());

    bd998ModifyLobPageCount( trans.OldTrans(), convVersion, parentFileNo, pagesDelta );
}

/*---------------------------------------------------------------------------*/

inline bool
bd05_CheckForClusteredRead(
    Converter_PageCollector&    pageCollection,
    cbd05_IOBuffer&             ioBuffer,
    const tsp00_Int4            fileLength,
    const tsp00_Int4            bytesToRead,
    tsp00_Int4&                 maxPagesToRead )
{
    const bool bDoClusteredRead = ( 0 != gg01_clustered_lobs );
    if( ! bDoClusteredRead ){
        return false; // clustered read for LOBs is not enabled
    }
    const tsp00_Int4 pageSize = FrameCtrl_ICommon::GetInstance().GetPageSize();
    const tsp00_Int4 minClusteredFileSize = pageCollection.GetMaxClusterSize() * pageSize;
    
    if(( fileLength < minClusteredFileSize)&&( bytesToRead < ( pageSize * 4 ))){
        return false; // LOB is too small
    }
    maxPagesToRead = ( bytesToRead / pageSize ) + 1;
    if( ! ioBuffer.Initialize( maxPagesToRead )){
        return false;
    }
    if( ! pageCollection.Initialize()){
        return false;
    }
    return true;;
}

/*===========================================================================*
 *  METHODS  -lll-                                                           *
 *===========================================================================*/

externC void
b05copy_bytestr (
    tgg00_TransContext&     t,
    tgg00_FileId&           sourceFileId,
    tsp00_Int4              fromPos,
    tgg00_FileId&           targetFileId,
    tsp00_Int4              toPos,
    const tgg00_Surrogate&  parentFileNo,
    tsp00_Int4              length,
    tsp00_Int4              newLobVersion )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05Copy", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode( e_shutdown );
        return;
    }
    trans.ResetErrorCode();

    tbd05_LobInfo       sourceInfo;
    tbd05_LobInfo       destInfo;
    tbd_current_tree    sourceCurrent;
    tbd_current_tree    destCurrent;
    bd05_MethodTracer   tracer( b05copy, trans.OldTrans().trError_gg00, fromPos, toPos, length );

    #ifdef SAPDB_SLOW    
    Kernel_VTrace   trace;
    if( DataTree_Trace.TracesLevel( 5 ))
    {
        trace << "bd05Copy() srcRoot: " << sourceFileId.fileRoot_gg00()
        << " destRoot: " << targetFileId.fileRoot_gg00() << FlushLine;
        trace << "from pos: " << fromPos << " to pos: "  << toPos
        << " length: "  << length  << " version: " << newLobVersion << FlushLine;
    }
    #endif

    sourceInfo.pNode_bd05.np_ptr()   = 0;
    sourceInfo.pNode_bd05.np_cbptr() = 0;
    destInfo.pNode_bd05.np_ptr()     = 0;
    destInfo.pNode_bd05.np_cbptr()   = 0;

    if( sourceFileId.fileName_gg00() == targetFileId.fileName_gg00()){
        trans.SetErrorCode( e_not_implemented );
        return;
    }
    if((( length < 1 )||( length > cbd_cspmaxint4_mxspbuf ))&&( length != cgg_whole_bytestr )){
        trans.SetErrorCode( e_file_limit );
        return;
    }
    if(( fromPos < 1 ) || ( fromPos > cbd_cspmaxint4_mxspbuf )){
        trans.SetErrorCode( e_file_limit );
        return;
    }
    if(( toPos != cgg_eo_bytestr )&&(( toPos < 1 )||( toPos > cbd_cspmaxint4_mxspbuf ))){
        trans.SetErrorCode( e_file_limit );
        return;
    }
    bd30GetTree( trans.OldTrans(), sourceFileId, sourceCurrent, m_column, LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk())
    {
        tbd_node_ptrs   pSourceNode   = sourceCurrent.currRootNptrs_bd00;
        targetFileId.fileBdUse_gg00() = targetFileId.fileBdUse_gg00().addElement( bd_write_acc );

        bd30GetTree( trans.OldTrans(), targetFileId, destCurrent, m_column, LOCK_TREE_EXCL_BD00, SYNC_TREE_LOCK_BD00 );
        if(  trans.IsOk())
        {
            tbd_node_ptrs pDestNode = destCurrent.currRootNptrs_bd00;
            tracer.AddRoot( destCurrent.curr_tree_id.fileRoot_gg00());

            const tsp00_Int4 source_flength  = pSourceNode.np_ptr()->ndStrFileSize_bd00();
            const tsp00_Int4 dest_flength    = pDestNode.np_ptr()->ndStrFileSize_bd00();

            const tsp00_Int4 pagesBefore = bd05_DeterminePageCount( pDestNode );
            const bool       bNewRoot    = ( dest_flength == 0 )? true:false;

            if( length == cgg_whole_bytestr ){
                length = source_flength;
            }
            fromPos = fromPos - 1;
            if( toPos == cgg_eo_bytestr ){
                toPos = dest_flength;
            }
            else{
                toPos = toPos - 1;
            }
            if(( fromPos + length) > source_flength )
            {
                length = source_flength - fromPos;
#ifdef SAPDB_SLOW    
                if( DataTree_Trace.TracesLevel( 5 )){
                    trace << "bd05Copy() trunc length: " <<  length << FlushLine;
                }
#endif                
            }
            if(
                ( fromPos > source_flength ) ||
                ( toPos > dest_flength     ) ||
                (((toPos + length) > cbd_cspmaxint4_mxspbuf))
            ){
                trans.SetErrorCode( e_file_limit );
            }
            else
            {
                tsp00_Int4 act_maxflen = (dest_flength - 1) / MAX_BSTR_COVERING_BD00;

                act_maxflen = (act_maxflen + 1) * MAX_BSTR_COVERING_BD00;
                sourceInfo.pNode_bd05   = pSourceNode;
                sourceInfo.fileLen_bd05 = source_flength;
                sourceInfo.current_bd05 = sourceCurrent;
                sourceInfo.filePos_bd05 = fromPos;
                destInfo.pNode_bd05     = pDestNode;
                destInfo.fileLen_bd05   = dest_flength;
                destInfo.current_bd05   = destCurrent;
                destInfo.filePos_bd05   = toPos;

                if( newLobVersion != NIL_STR_VERSION_GG00 ){
                    pDestNode.np_ptr()->ndStrVersion_bd00() = newLobVersion;
                }
                bd05_Copy( trans, sourceInfo, destInfo, length, act_maxflen );

                pDestNode   = destInfo.pNode_bd05;
                pSourceNode = sourceInfo.pNode_bd05;
                destCurrent.currRootNptrs_bd00.np_ptr()->ndStrParentID_bd00() = parentFileNo;
                tsp00_Int4 pagesDelta = bd05_DeterminePageCount( destCurrent.currRootNptrs_bd00 ) - pagesBefore;
                if( bNewRoot ){
                    ++pagesDelta;
                }
                bd05_ModifyLobCount( trans, destCurrent.currRootNptrs_bd00, parentFileNo, pagesDelta );
            }
            if( pDestNode.np_ptr() != 0){
                b13r_release_node( pDestNode, destCurrent, lru_normal );
            }
        }
        if( pSourceNode.np_ptr() != 0 ){
            b13r_release_node( pSourceNode, sourceCurrent, lru_normal );
        }
        bd30ReleaseTree( destCurrent );
        bd30ReleaseTree( sourceCurrent );
        if( trans.IsOk())
        {
            sourceFileId = sourceCurrent.curr_tree_id;
            targetFileId = destCurrent.curr_tree_id;
        }
        targetFileId.fileBdUse_gg00() = targetFileId.fileBdUse_gg00().delElement( bd_write_acc );;
    }
}

/*---------------------------------------------------------------------------*/

externC void
b05expand_bytestr (
    tgg00_TransContext& t,
    tgg00_FileId&       fileId,
    tsp00_Int4          length,
    char                fillChar,
    tsp00_Int4          newLobVersion )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05Expand", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode(  e_shutdown );
    }
    trans.ResetErrorCode();

    bd05_MethodTracer   tracer( b05length, trans.OldTrans().trError_gg00, length );
    tbd_current_tree    current;
    if( length < 1 ){
        trans.SetErrorCode(  e_buffer_limit );
        return;
    }
    fileId.fileBdUse_gg00() = fileId.fileBdUse_gg00().addElement( bd_write_acc );

    bd30GetTree( trans.OldTrans(), fileId, current, m_column, LOCK_TREE_EXCL_BD00, SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk() )
    {
        tracer.AddRoot( current.curr_tree_id.fileRoot_gg00());

        boolean         bUpdateNew = false;
        tsp00_Int4      expandLength;
        tsp00_Int4      partLength;
        tsp00_Int4      lastPageLength;
        tsp00_Int4      flength = current.currRootNptrs_bd00.np_ptr()->ndStrFileSize_bd00();
        tsp00_Int4      pos     = flength;
        tbd_node_ptrs   nptr    = current.currRootNptrs_bd00;

        if( length <= flength ){
            b13r_release_node (nptr, current, lru_normal);
        }
        else
        {
            lastPageLength = ( flength != 0 )? ((flength - 1) % MAX_BSTR_COVERING_BD00) + 1 : 0;
            expandLength    = length - flength;
            if( expandLength <= MAX_BSTR_COVERING_BD00 - lastPageLength ){
                partLength = expandLength;
            }
            else
            {
                if( lastPageLength == MAX_BSTR_COVERING_BD00 )
                {
                    if( expandLength > MAX_BSTR_COVERING_BD00 ){
                        partLength = MAX_BSTR_COVERING_BD00;
                    }
                    else{
                        partLength = expandLength;
                    }
                }
                else{
                    partLength = MAX_BSTR_COVERING_BD00 - lastPageLength;
                }
            }
            do {
                const bool  bIsExtended = true;
                const bool  bUseBuffer  = true;

                if(( lastPageLength + partLength) <= MAX_BSTR_COVERING_BD00 )
                {
                    if( newLobVersion != NIL_STR_VERSION_GG00 ){
                        nptr.np_ptr()->ndStrVersion_bd00() = newLobVersion;
                    }
                    if( flength <= MAX_BSTR_COVERING_BD00 ){
                        bd60PutIntoRoot( trans, current, nptr, pos, partLength, ! bUseBuffer,
                                         fillChar, 0, 0, 1, bIsExtended );
                    }
                    else{
                        bd60PutIntoLeaves( trans, current, nptr, pos, partLength, ! bUseBuffer,
                                           fillChar, 0, 0, 1, bIsExtended );
                    }
                }
                else
                {
                    bUpdateNew = true;
                    if( newLobVersion != NIL_STR_VERSION_GG00 ){
                        nptr.np_ptr()->ndStrVersion_bd00() = newLobVersion;
                    }
                    if( flength <= MAX_BSTR_COVERING_BD00 ){
                        bd60ExtendRoot( trans, current, nptr, pos, partLength, ! bUseBuffer,
                                        fillChar, 0, 0, 1 );
                    }
                    else{
                        bd60Extend( trans, current, nptr, pos, partLength, ! bUseBuffer, fillChar, 0, 0, 1 );
                    }
                }
                pos          = pos + partLength;
                flength      = flength + partLength;
                expandLength = expandLength - partLength;
                if( expandLength < MAX_BSTR_COVERING_BD00 ){
                    partLength = expandLength;
                }
                else{
                    partLength = MAX_BSTR_COVERING_BD00;
                }
                lastPageLength = MAX_BSTR_COVERING_BD00;
            }
            while(!(( ! trans.IsOk()) || ( expandLength == 0 )));

            if( trans.IsOk() )
            {
                if( nptr.np_ptr()->nd_id() != current.curr_tree_id.fileRoot_gg00() ){
                    b13w_release_node( nptr, current );
                }
                current.currRootNptrs_bd00.np_ptr()->ndStrFileSize_bd00() = length - expandLength;
                current.currRootUpdated_bd00                              = true;
            }
        }
        if(( ! trans.IsOk() ) && ( nptr.np_ptr() != 0 )){
            b13r_release_node (nptr, current, lru_normal);
        }
    }
    bd30ReleaseTree( current );

    if( trans.IsOk() ){
        fileId = current.curr_tree_id;
    }
    fileId.fileBdUse_gg00() = fileId.fileBdUse_gg00().delElement( bd_write_acc );
}

/*---------------------------------------------------------------------------*/

externC void
b05finish_load (
    tgg00_TransContext& t,
    tgg00_FileId&       fileId,
    tsp00_PageNo        rightmostLeaf )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05FinishLoad", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode(  e_shutdown );
    }
    trans.ResetErrorCode();

    tbd_current_tree    current;

    fileId.fileBdUse_gg00() = fileId.fileBdUse_gg00().addElement( bd_write_acc );
    bd30GetTree( trans.OldTrans(), fileId, current, m_column, LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if ( trans.IsOk() )
    {
        tbd_node_ptrs   nptr;
        nptr.np_ptr()   = 0;
        nptr.np_cbptr() = 0;
        bd13GetNode( current, rightmostLeaf, plmLock_ebd00, nr_for_read, nptr );
        if( trans.IsOk() )
        {
            tbd_nodeptr&   pRoot  = current.currRootNptrs_bd00.np_ptr();
            pRoot->ndStrFileSize_bd00()  = nptr.np_ptr()->ndStrFileSize_bd00();
            pRoot->ndStrVersion_bd00()   = NIL_STR_VERSION_GG00;
            current.currRootUpdated_bd00 = true;
        }
        if( nptr.np_ptr() != 0 ){
            b13r_release_node( nptr, current, lru_normal );
        }
    }
    bd30ReleaseTree( current );
    fileId.fileBdUse_gg00() = fileId.fileBdUse_gg00().delElement( bd_write_acc );
}

/*---------------------------------------------------------------------------*/

externC void
b05get_str_version(
    tgg00_TransContext& t,
    tgg00_FileId&       fileId,
    tsp00_Int4&         lobVersion )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05GetLobVersion", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode(  e_shutdown );
    }
    trans.ResetErrorCode();

    bd05_MethodTracer   tracer( b05getset, trans.OldTrans().trError_gg00 );
    tbd_current_tree    current;

    bd30GetTree (t, fileId, current, m_column,! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk() )
    {
        tracer.AddRoot( current.curr_tree_id.fileRoot_gg00());
        lobVersion = current.currRootNptrs_bd00.np_ptr()->ndStrVersion_bd00();
        fileId     = current.curr_tree_id;
    }
    bd30ReleaseTree( current );
#ifdef SAPDB_SLOW
    if( DataTree_Trace.TracesLevel( 5 )){
        Kernel_VTrace() << "bd05GetVersion() version: " << lobVersion << FlushLine;
    }
#endif    
}

/*---------------------------------------------------------------------------*/

externC void
b05length_bytestr(
    tgg00_TransContext& t,
    tgg00_FileId&       fileId,
    tsp00_Int4&         filelength,
    tsp00_Int4&         pageCount,
    tsp00_Int4&         lobVersion )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05Length", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode(  e_shutdown );
    }
    trans.ResetErrorCode();

    bd05_MethodTracer   tracer( b05length, trans.OldTrans().trError_gg00 );
    tbd_current_tree    current;

    bd30GetTree( trans.OldTrans(), fileId, current, m_column, ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk() )
    {
        tracer.AddRoot( current.curr_tree_id.fileRoot_gg00());

        filelength  = current.currRootNptrs_bd00.np_ptr()->ndStrFileSize_bd00();
        lobVersion  = current.currRootNptrs_bd00.np_ptr()->ndStrVersion_bd00();
        fileId      = current.curr_tree_id;
        pageCount   = bd05_DeterminePageCount( current.currRootNptrs_bd00 );
    }
    bd30ReleaseTree (current);

#ifdef SAPDB_SLOW    
    if( DataTree_Trace.TracesLevel( 5 )){
        Kernel_VTrace() << "bd05Length() version: " << lobVersion << " length: " << filelength << FlushLine;
    }
#endif

}

/*---------------------------------------------------------------------------*/

externC void
b05load_string (
    tbd_node_ptrs&      lnptr,
    tbd_node_ptrs&      nptr,
    tsp00_PageNo&       leftPageNo,
    tbd_current_tree&   current,
    tgg00_Surrogate&    parentFileNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05Load", DataTree_Trace, 5 );

    tgg00_FileId&   fileId = current.curr_tree_id;
    Trans_Context&  trans  = Trans_Context::GetContext( *current.curr_trans );

    if( b01downfilesystem ){
        trans.SetErrorCode( e_shutdown );
    }
    trans.ResetErrorCode();

    tbd_node_ptrs   rootptr;
    tsp00_PageNo    currPageNo;

    rootptr.np_ptr()   = 0;
    rootptr.np_cbptr() = 0;

    const tsp00_Int4 pagesBefore  = bd05_DeterminePageCount( current.currRootNptrs_bd00 );
    const bool       pagesNewRoot = ( current.currRootNptrs_bd00.np_ptr()->ndStrFileSize_bd00() == 0 );

    if( lnptr.np_ptr()->nd_id() == current.curr_tree_id.fileRoot_gg00())
    {
        bd61FirstLoadStep( trans, current, lnptr, nptr );
        leftPageNo = NIL_PAGE_NO_GG00;
    }
    else
    {
        if(( nptr.np_ptr() != 0 ) && ( nptr.np_ptr()->nd_bottom() > BODY_BEG_BD00 ))
        {
            const tgg00_PagePos saveBottom = nptr.np_ptr()->nd_bottom();
            b13init_default_data_page( fileId, LEAF_LEVEL_BD00, nptr.np_ptr()->nd_id(), nptr );
            nptr.np_ptr()->nd_bottom() = saveBottom;
            nptr.np_ptr()->nd_left()   = lnptr.np_ptr()->nd_id();
            nptr.np_ptr()->ndStrFileSize_bd00() = lnptr.np_ptr()->ndStrFileSize_bd00() +
                                                  nptr.np_ptr()->nd_bottom() - BODY_BEG_BD00;
            current.currRootNptrs_bd00.np_ptr()->ndStrFileSize_bd00() = nptr.np_ptr()->ndStrFileSize_bd00();
            currPageNo                 = nptr.np_ptr()->nd_id();
            lnptr.np_ptr()->nd_right() = nptr.np_ptr()->nd_id();
        }
        else{
            currPageNo = NIL_PAGE_NO_GG00;
        }
        if( leftPageNo == NIL_PAGE_NO_GG00 ){
            leftPageNo = nptr.np_ptr()->nd_id();
        }
        else
        {
            bd13GetNode( current, fileId.fileRoot_gg00(), plmLock_ebd00, nr_for_update, rootptr );
            if( trans.IsOk()){
                bd61AddIndex( trans, current, rootptr, fileId.fileRoot_gg00(), leftPageNo,
                              currPageNo, FIRST_INDEX_LEVEL_BD00 );
            }
            leftPageNo = NIL_PAGE_NO_GG00;
        }
        if(( trans.IsOk()) && ( nptr.np_ptr() != 0 ))
        {
            if( nptr.np_ptr()->nd_bottom() > BODY_BEG_BD00 )
            {
                b13w_release_node( lnptr, current );
                lnptr           = nptr;
                nptr.np_ptr()   = 0;
                nptr.np_cbptr() = 0;
            }
            else
                b13free_node( nptr, current );
        }
    }
    if( trans.IsOk())
    {
        tsp00_Int4 pagesDelta = bd05_DeterminePageCount( current.currRootNptrs_bd00 ) - pagesBefore;

        if( pagesNewRoot ){
            ++pagesDelta = pagesDelta;
        }
        if ( 0 != strcmp( parentFileNo, cgg_zero_id )) /* caused by new bdInfo size and loader compatibility */
        {
            bd05_ModifyLobCount( trans, current.currRootNptrs_bd00, parentFileNo, pagesDelta );
            current.currRootNptrs_bd00.np_ptr()->ndStrParentID_bd00() = parentFileNo;
        }
        current.currRootUpdated_bd00 = true;
    }
    else
    {
        if( nptr.np_ptr() != 0 ){
            b13r_release_node( nptr, current, lru_normal );
        }
        if( lnptr.np_ptr() != 0 ){
            b13r_release_node( lnptr, current, lru_normal );
        }
        if( rootptr.np_ptr() != 0 ){
            b13r_release_node( rootptr, current, lru_normal );
        }
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd05ReadLob(
    tgg00_TransContext&    t,
    tgg00_FileId&          fileId,
    tsp00_Int4             pos,
    tsp00_Int4&            length,
    tsp00_Int4             bufferSize,
    tsp00_Byte* const      pBuffer,
    tsp00_Int4             bufferPos )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05ReadLob", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode( e_shutdown );
        return;
    }
    trans.ResetErrorCode();

    bd05_MethodTracer   tracer( b05read, trans.OldTrans().trError_gg00, pos, length );
    b73cmd_count( iread_string );

    if(( length < 1 ) || ( length + bufferPos - 1 > bufferSize )){
        trans.SetErrorCode( e_buffer_limit );
        return;
    }
    if(( pos < 1 ) || ( pos > cbd_cspmaxint4_mxspbuf )){
        trans.SetErrorCode( e_file_limit );
        return;
    }
    const Data_PageRecoveryMode recMode( Data_Dynamic, Data_Recoverable );
    Converter_PageCollector     pageCollection( trans.Allocator(), recMode );
    cbd05_IOBuffer              ioBuffer;

    tbd_current_tree    current;
    bd30GetTree( trans.OldTrans(), fileId, current, m_column, ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk())
    {
        tracer.AddRoot( current.curr_tree_id.fileRoot_gg00());
        tbd_node_ptrs  nptr;
        tsp00_Int4     leafPos;

        nptr = current.currRootNptrs_bd00;
        const tsp00_Int4 flength = nptr.np_ptr()->ndStrFileSize_bd00();
        pos  = pos - 1;

        tsp00_Int4  maxPagesToRead = 0;
        const bool  bClusteredRead = bd05_CheckForClusteredRead( pageCollection, ioBuffer,
                                     flength, length, maxPagesToRead );
        if( pos < flength )
        {
            if(( pos + length) > flength ){
                length = flength - pos;
            }
            if((( pos % MAX_BSTR_COVERING_BD00 ) + length ) < MAX_BSTR_COVERING_BD00 )
            {  // the requested data is stored on one leaf page
                if( flength <= MAX_BSTR_COVERING_BD00 )
                {
                    leafPos = pos;
                    bd60Read( trans, nptr.np_ptr(), leafPos, length, bufferSize, pBuffer, bufferPos );
                }
                else
                {
                    bd61SearchLeaf( trans, current, nptr, pos, leafPos );
                    if( trans.IsOk()){
                        bd60Read( trans, nptr.np_ptr(), leafPos, length, bufferSize, pBuffer, bufferPos );
                    }
                }
            }
            else // the requested data is spread over more than one leaf page
            {
                if( bClusteredRead ){
                    bd61SearchLeafWithClusteredRead( trans, current, nptr, pageCollection,
                                                     ioBuffer.GetIOBuffer(), maxPagesToRead, pos, leafPos );
                }else{
                    bd61SearchLeaf( trans, current, nptr, pos, leafPos );
                }
                if ( trans.IsOk()){
                    bd60Get( trans, current, nptr, leafPos, length, bufferSize, pBuffer, bufferPos );
                }
            }
            if( nptr.np_ptr() != 0 ){
                b13r_release_node( nptr, current, lru_normal );
            }
        }
        else
        {
            if(( flength == 0 ) || ( pos == flength ))
            {
                length = 0;
                trans.SetErrorCode( e_key_not_found );
            }
            else{
                trans.SetErrorCode( e_file_limit );
            }
        }
        if( nptr.np_ptr() != 0 ){
            b13r_release_node( nptr, current, lru_normal );
        }
        bd30ReleaseTree( current );
        if( trans.IsOk()){
            fileId = current.curr_tree_id;
        }
    }
}

/*---------------------------------------------------------------------------*/

externC void
b05trunc_bytestr(
    tgg00_TransContext&     t,
    tgg00_FileId&           fileId,
    const tgg00_Surrogate&  parentFileNo,
    tsp00_Int4              newFileLength,
    tsp00_Int4              newLobVersion )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05Trunc", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode(  e_shutdown );
        return;
    }
    trans.ResetErrorCode();

    bd05_MethodTracer   tracer( b05length, trans.OldTrans().trError_gg00, newFileLength );
    tbd_current_tree    current;

    fileId.fileBdUse_gg00() = fileId.fileBdUse_gg00().addElement( bd_write_acc );
    bd30GetTree( trans.OldTrans(), fileId, current, m_column, LOCK_TREE_EXCL_BD00, SYNC_TREE_LOCK_BD00);
    if( trans.IsOk() )
    {
        tracer.AddRoot( current.curr_tree_id.fileRoot_gg00());

        const tsp00_Int4 flength       = current.currRootNptrs_bd00.np_ptr()->ndStrFileSize_bd00();
        const bool       bAlreadyEmpty = (flength == 0);
        const tsp00_Int4 pagesBefore   = bd05_DeterminePageCount( current.currRootNptrs_bd00 );
        if( newLobVersion != NIL_STR_VERSION_GG00 ){
            current.currRootNptrs_bd00.np_ptr()->ndStrVersion_bd00() = newLobVersion;
        }
        tbd_node_ptrs pRoots;
        pRoots = current.currRootNptrs_bd00;
        if( newFileLength < flength )
        {
            if( flength <= MAX_BSTR_COVERING_BD00 ){
                bd61TruncRoot( current, pRoots, newFileLength  );
            }
            else{
                bd61Trunc( trans, current, pRoots, newFileLength, false );
            }
        }
        if( pRoots.np_ptr() != 0 ){
            b13r_release_node( pRoots, current, lru_normal );
        }
        current.currRootNptrs_bd00.np_ptr()->ndStrParentID_bd00() = parentFileNo;
        tsp00_Int4 pagesDelta = bd05_DeterminePageCount( current.currRootNptrs_bd00 ) - pagesBefore;

        if(( current.currRootNptrs_bd00.np_ptr()->ndStrFileSize_bd00() == 0 ) && ! bAlreadyEmpty ){
            --pagesDelta;
        }
        bd05_ModifyLobCount( trans, current.currRootNptrs_bd00, parentFileNo, pagesDelta );
    }
    bd30ReleaseTree( current );
    if( trans.IsOk() ){
        fileId = current.curr_tree_id;
    }
    fileId.fileBdUse_gg00() = fileId.fileBdUse_gg00().delElement( bd_write_acc );
}

/*---------------------------------------------------------------------------*/

externC void
b05write_bytestr(
    tgg00_TransContext&     t,
    tgg00_FileId&           fileId,
    const tgg00_Surrogate&  parentFileNo,
    tsp00_Int4              pos,
    tsp00_Int4              length,
    tsp00_Int4              bufferSize,
    const tsp00_Byte*       pBuffer,
    tsp00_Int4              bufferPos,
    tsp00_Int4              newLobVersion )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05Write", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode( e_shutdown );
        return;
    }
    trans.ResetErrorCode();

    bd05_AdvancedMethodTracer tracer( b05write, trans.OldTrans().trError_gg00, pos, length,
                                      bufferSize, bufferPos, pBuffer );
#ifdef SAPDB_SLOW
    Kernel_VTrace   trace;
    if( DataTree_Trace.TracesLevel( 5 )){
        trace << "bd05Write() pos: " << pos << " length: " << length << FlushLine;
    }
#endif

    b73cmd_count( iwrite_string );

    if(( length < 1 ) || ( length > bufferSize ) || (( bufferPos + length - 1) > bufferSize )){
        trans.SetErrorCode(  e_buffer_limit );
        return;
    }
    if(( pos != cgg_eo_bytestr ) && (( pos < 1 )||( pos > cbd_cspmaxint4_mxspbuf ))){
        trans.SetErrorCode(  e_file_limit );
        return;
    }
    tbd_current_tree  current;
    fileId.fileBdUse_gg00() = fileId.fileBdUse_gg00().addElement( bd_write_acc );
    bd30GetTree( trans.OldTrans(), fileId, current, m_column, LOCK_TREE_EXCL_BD00, SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk() )
    {
        tracer.AddRoot( current.curr_tree_id.fileRoot_gg00());
        tbd_node_ptrs  nptr;
        nptr = current.currRootNptrs_bd00;
        const tsp00_Int4 flength      = nptr.np_ptr()->ndStrFileSize_bd00();
        const tsp00_Int4 pagesBefore  = bd05_DeterminePageCount( current.currRootNptrs_bd00 );
        const bool       pagesNewRoot = ( 0 == flength )? true : false;
        pos  = ( pos == cgg_eo_bytestr )? flength : --pos;

        #ifdef SAPDB_SLOW    
        if( DataTree_Trace.TracesLevel( 6 )){
            trace << "bd05Write() pos: " << pos << " fileLength: " << flength << FlushLine;
        }
        #endif  

        if(( pos > flength )||(( pos + length ) >= cbd_cspmaxint4_mxspbuf )){
            trans.SetErrorCode(  e_file_limit );
        }
        else
        {
            const bool       bUseBuffer     = true;
            const char       fillChar       = bsp_c1;
            const tsp00_Int4 lastPageLength = ( flength != 0 )?(( flength - 1 ) % MAX_BSTR_COVERING_BD00) + 1 : 0;
            const tsp00_Int4 extLength      = length - ( flength - pos );
            const bool bIsExtended          = ( extLength > 0 )? true : false;
            if(( lastPageLength + extLength ) <= MAX_BSTR_COVERING_BD00 )
            { // PTS 1001460 Torsten Strahl 1998-08-02
                if( newLobVersion != NIL_STR_VERSION_GG00 ){
                    nptr.np_ptr()->ndStrVersion_bd00() = newLobVersion;
                }
                if( flength <= MAX_BSTR_COVERING_BD00 ){
                    bd60PutIntoRoot( trans, current, nptr, pos, length, bUseBuffer, fillChar, bufferSize,
                                     pBuffer, bufferPos, bIsExtended );
                }
                else{
                    bd60PutIntoLeaves( trans, current, nptr, pos, length, bUseBuffer, fillChar, bufferSize,
                                       pBuffer, bufferPos, bIsExtended );
                }
            }
            else
            {   // PTS 1001460 Torsten Strahl 1998-08-02
                if( newLobVersion != NIL_STR_VERSION_GG00 ){
                    nptr.np_ptr()->ndStrVersion_bd00() = newLobVersion;
                }
                if( flength <= MAX_BSTR_COVERING_BD00 ){
                    bd60ExtendRoot( trans, current, nptr, pos, length,bUseBuffer, fillChar,
                                    bufferSize, pBuffer, bufferPos );
                }
                else{
                    bd60Extend( trans, current, nptr, pos, length, bUseBuffer, fillChar,
                                bufferSize, pBuffer, bufferPos );
                }
            }
        }
        current.currRootNptrs_bd00.np_ptr()->ndStrParentID_bd00() = parentFileNo;
        tsp00_Int4 pagesDelta  = bd05_DeterminePageCount( current.currRootNptrs_bd00 ) - pagesBefore;
        if( pagesNewRoot ){
            ++pagesDelta;
        }
        bd05_ModifyLobCount( trans, current.currRootNptrs_bd00, parentFileNo, pagesDelta );

        if(( ! trans.IsOk() ) && ( nptr.np_ptr() != 0 )){
            b13r_release_node( nptr, current, lru_normal );
        }
    }
    bd30ReleaseTree( current );
    if( trans.IsOk() ){
        fileId = current.curr_tree_id;
    }
    fileId.fileBdUse_gg00() = fileId.fileBdUse_gg00().delElement( bd_write_acc );
}

/*---------------------------------------------------------------------------*/

externC void
bd05ExtractLob(
    tgg00_TransContext& t,
    tsp00_PageNo        root,
    tsp00_VFilename&    hostFileName )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05ExtractLob", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode(  e_shutdown );
        return;
    }
    trans.ResetErrorCode();

    tgg00_FileId        fileId;
    tbd_current_tree    current;

    fileId                 = b01niltree_id;
    fileId.fileRoot_gg00() = root;
    fileId.fileType_gg00().clear();
    fileId.fileType_gg00().addElement( ftsPerm_egg00 );
    fileId.fileType_gg00().addElement( ftsByteStr_egg00 );
    fileId.fileTfn_gg00().becomes( tfnColumn_egg00 );

    bd30GetTree( trans.OldTrans(), fileId, current, m_column, ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk()){
        bd60Extract( trans, current, hostFileName );
    }
    bd30ReleaseTree( current );
    if(
        ( trans.OldTrans().trError_gg00 == e_old_fileversion )||
        ( trans.OldTrans().trError_gg00 == e_file_not_found ))
    {
        trans.SetErrorCode( e_invalid_root );
    }
}

/*---------------------------------------------------------------------------*/

externC void
bd05DropLob(
    tgg00_TransContext& t,
    tgg00_FileId&       fileId,
    tgg00_Surrogate&    parentFileNo)
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05Drop", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode(  e_shutdown );
        return;
    }
    trans.ResetErrorCode();

    bd05_MethodTracer   tracer( b05length, trans.OldTrans().trError_gg00, 0 );
    tbd_current_tree    current;

    fileId.fileBdUse_gg00() = fileId.fileBdUse_gg00().addElement( bd_release_acc );
    bd30GetTree( trans.OldTrans(), fileId, current, m_drop, LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk())
    {
        tracer.AddRoot( current.curr_tree_id.fileRoot_gg00());
        const tsp00_Int4 totalPageCount = bd05_DeterminePageCount( current.currRootNptrs_bd00 );
        const tsp00_Int4 fileLength     = current.currRootNptrs_bd00.np_ptr()->ndStrFileSize_bd00();
        bd60Drop( trans, current );

        if(
            ( trans.OldTrans().trError_gg00 != e_disk_not_accessible ) &&
            ( trans.OldTrans().trError_gg00 != e_do_not_drop_fdir    ) &&
            ( trans.OldTrans().trError_gg00 != e_ok                  )
        )
        {
            g01opmsg (sp3p_knldiag, sp3m_info, bd05ErrorDuringDrop_csp03,
                      csp3_n_btree, "Error during drop file  ", trans.OldTrans().trError_gg00);
            g01optextmsg (sp3p_knldiag, sp3m_info, bd05ErrorDuringDrop_csp03,
                          csp3_n_btree, "CHECK DATA WITH UPDATE may be needed    ");
            trans.ResetErrorCode();  /* ignore all kind of errors */
        }
        if( 0 != strcmp( parentFileNo, cgg_zero_id )&& ( 0 != fileLength ))
        {
            const Converter_Version convVersion = Converter_ICommon::Instance().Version();
            bd998ModifyLobPageCount( trans.OldTrans(), convVersion, parentFileNo, - totalPageCount );
            if( trans.OldTrans().trError_gg00 == e_file_not_found ){ /* parent file is gone too */
                trans.ResetErrorCode();
            }
        }
    }
    bd30ReleaseTree( current );
    fileId.fileBdUse_gg00() = fileId.fileBdUse_gg00().delElement( bd_release_acc );
}

/*---------------------------------------------------------------------------*/

externC void
bd05PrefetchLob(
    tgg00_TransContext& t,
    tgg00_FileId&       fileId,
    tsp00_Int4          lobPosition,
    tsp00_Int4          maxBytesToRead )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05PrefetchLob", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode(  e_shutdown );
        return;
    }
    trans.ResetErrorCode();

    bd05_MethodTracer   tracer( b05read, trans.OldTrans().trError_gg00, lobPosition, 0 );

    const Data_PageRecoveryMode recMode( Data_Dynamic, Data_Recoverable );
    Converter_PageCollector     pageCollection( trans.Allocator(), recMode );
    cbd05_IOBuffer              ioBuffer;
    tbd_current_tree            current;

    bd30GetTree( trans.OldTrans(), fileId, current, m_column,! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk())
    {
        tbd_node_ptrs nptrs;
        tsp00_Int4    leafPosition;

        tracer.AddRoot( current.curr_tree_id.fileRoot_gg00());
        nptrs = current.currRootNptrs_bd00;

        tsp00_Int4  maxPagesToRead = 0;
        const bool  bClusteredRead = bd05_CheckForClusteredRead( pageCollection, ioBuffer,
                                     nptrs.np_ptr()->ndStrFileSize_bd00(), maxBytesToRead, maxPagesToRead );

        if( bClusteredRead ){
            bd61SearchLeafWithClusteredRead( trans, current, nptrs, pageCollection, ioBuffer.GetIOBuffer(),
                                             maxPagesToRead, lobPosition, leafPosition );
        }
        else
        {
            bd61SearchLeaf( trans, current, nptrs, lobPosition, leafPosition );
            while(( trans.IsOk())&&( nptrs.np_ptr() != 0 )&&( 0 < maxBytesToRead ))
            {
                const tsp00_PageNo nextLeaf = nptrs.np_ptr()->nd_right();
                b13r_release_node( nptrs, current, lru_normal );
                if( nextLeaf != NIL_PAGE_NO_GG00 ){
                    bd13GetNode( current, nextLeaf, plmLock_ebd00, nr_for_read, nptrs );
                }
                if( trans.IsOk()){
                    maxBytesToRead -= MAX_BSTR_COVERING_BD00;
                }
            }
        }
        if( nptrs.np_ptr() != 0 ){
            b13r_release_node( nptrs, current, lru_normal );
        }
    }
    bd30ReleaseTree( current );
    trans.ResetErrorCode();  /* ignore all errors */
}

/*---------------------------------------------------------------------------*/

externC void
bd05ModifyFdirLength(
    tgg00_TransContext&     t,
    const pasbool           bDoIncrement,
    tgg00_FileId&           fileId,
    const tgg00_Surrogate&  parentFileNo)
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05Modify", DataTree_Trace, 5 );

    Trans_Context&  trans = Trans_Context::GetContext( t );

    if( b01downfilesystem ){
        trans.SetErrorCode(  e_shutdown );
        return;
    }
    trans.ResetErrorCode();

    bd05_MethodTracer   tracer( b05length, trans.OldTrans().trError_gg00 );
    tbd_current_tree    current;

    bd30GetTree( trans.OldTrans(), fileId, current, m_column, ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );
    if( trans.IsOk())
    {
        tracer.AddRoot( current.curr_tree_id.fileRoot_gg00());
        if( 0 < current.currRootNptrs_bd00.np_ptr()->ndStrFileSize_bd00())
        {
            const tsp00_Int4        pageCount   = bd05_DeterminePageCount( current.currRootNptrs_bd00 );
            const Converter_Version convVersion =
                bd20GetDestinationConverterVersion( trans.Task().ID(), current.currRootNptrs_bd00.np_cbptr());
            if( bDoIncrement ){
                bd998ModifyLobPageCount( trans.OldTrans(), convVersion, parentFileNo, pageCount );
            }
            else{
                bd998ModifyLobPageCount ( trans.OldTrans(), convVersion, parentFileNo, -1 * pageCount );
            }
        }
    }
    bd30ReleaseTree (current);
    if( trans.IsOk() ){
        fileId  = current.curr_tree_id;
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
bd05_Copy(
    Trans_Context&      trans,
    tbd05_LobInfo&      sourceInfo,
    tbd05_LobInfo&      destInfo,
    tsp00_Int4          length,
    tsp00_Int4          maxFileLen )
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd05_Copy", DataTree_Trace, 6 );

    tsp00_Int4   f_lpos;
    tsp00_Page   buffer;
    tsp00_Byte*  pBuffer = reinterpret_cast< tsp00_Byte* >(&buffer);

    const tsp00_PageNo currRoot = destInfo.pNode_bd05.np_ptr()->nd_id();

    if( sourceInfo.fileLen_bd05 > MAX_BSTR_COVERING_BD00 ){
        bd61SearchLeaf( trans, sourceInfo.current_bd05, sourceInfo.pNode_bd05, sourceInfo.filePos_bd05, f_lpos );
        if( ! trans.IsOk()){
            return;
        }
    }
    else{
        f_lpos = sourceInfo.filePos_bd05;
    }
    if(
        ( currRoot == sourceInfo.current_bd05.curr_tree_id.fileRoot_gg00()) &&
        ( sourceInfo.fileLen_bd05 > MAX_BSTR_COVERING_BD00                )
    )
    {
        bd13GetNode( destInfo.current_bd05, currRoot, plmLock_ebd00, nr_for_update, destInfo.pNode_bd05 );
    }
    if( ! trans.IsOk()){
        return;
    }
    const char  fillChar    = bsp_c1;
    const bool  bUseBuffer  = true;
    bool        bFirstCall  = true;
    bool        bIsExtended = false;
    tsp00_Int4  remainLen   = length;

    do {
        const tsp00_Int4 currLen = ( remainLen > sizeof( buffer ))? sizeof( buffer ): remainLen;

        if(( f_lpos + remainLen ) < MAX_BSTR_COVERING_BD00 ){
            bd60Read( trans, sourceInfo.pNode_bd05.np_ptr(), f_lpos, currLen, sizeof( buffer ), pBuffer, 1 );
        }
        else{
            bd60Get( trans, sourceInfo.current_bd05, sourceInfo.pNode_bd05, f_lpos, currLen,
                     sizeof( buffer ), pBuffer, 1 );
        }
        if( ! trans.IsOk()){
            return;
        }
        if(( destInfo.filePos_bd05 + currLen ) > maxFileLen )
        {
            if( destInfo.fileLen_bd05 <= MAX_BSTR_COVERING_BD00 ){
                bd60ExtendRoot( trans, destInfo.current_bd05, destInfo.pNode_bd05, destInfo.filePos_bd05,
                                currLen, bUseBuffer, fillChar, sizeof( buffer ), pBuffer, 1);
            }
            else
            {
                if( destInfo.pNode_bd05.np_ptr()->nd_id() != destInfo.pNode_bd05.np_ptr()->nd_root())
                {
                    b13w_release_node( destInfo.pNode_bd05, destInfo.current_bd05 );
                    bFirstCall = true;
                    if( ! trans.IsOk()){
                        return;
                    }
                    bd13GetNode( destInfo.current_bd05, currRoot, plmLock_ebd00,
                                 nr_for_update, destInfo.pNode_bd05 );
                }
                if( ! trans.IsOk()){
                    return;
                }
                bd60Extend( trans, destInfo.current_bd05, destInfo.pNode_bd05, destInfo.filePos_bd05,
                            currLen, bUseBuffer, fillChar, sizeof( buffer ), pBuffer, 1 );
            }
        }
        else
        {
            if( ! bIsExtended ){
                bIsExtended = ( destInfo.filePos_bd05 + currLen) > destInfo.fileLen_bd05;
            }
            if( destInfo.fileLen_bd05 <= MAX_BSTR_COVERING_BD00 ){
                bd60PutIntoRoot( trans, destInfo.current_bd05, destInfo.pNode_bd05, destInfo.filePos_bd05, currLen,
                                 bUseBuffer, fillChar, sizeof( buffer ), pBuffer, 1, bIsExtended );
            }
            else{
                bd60ContPutLeaves( trans, destInfo.current_bd05, destInfo.pNode_bd05, destInfo.filePos_bd05, currLen,
                                   pBuffer, sizeof( buffer), bFirstCall, bIsExtended, destInfo.fileLen_bd05 );
            }
        }
        if( ! trans.IsOk()){
            return;
        }
        remainLen = remainLen - currLen;
        destInfo.filePos_bd05  = destInfo.filePos_bd05 + currLen;
        f_lpos                 =(f_lpos + currLen) % MAX_BSTR_COVERING_BD00;
        if(( destInfo.pNode_bd05.np_ptr() == 0 )&&( remainLen > 0 ))
        {
            bFirstCall = true;
            bd13GetNode( destInfo.current_bd05, currRoot, plmLock_ebd00, nr_for_update, destInfo.pNode_bd05 );
            if( ! trans.IsOk()){
                return;
            }
            destInfo.fileLen_bd05 = destInfo.pNode_bd05.np_ptr()->ndStrFileSize_bd00();
            maxFileLen            = ( destInfo.fileLen_bd05 - 1 ) / MAX_BSTR_COVERING_BD00;
            maxFileLen            = ( maxFileLen + 1 ) * MAX_BSTR_COVERING_BD00;
        }
    } while( trans.IsOk()&& ( 0 != remainLen ));

    if( ! trans.IsOk()){
        return;
    }
    if( sourceInfo.pNode_bd05.np_ptr() != 0 )
    {
        if( sourceInfo.pNode_bd05.np_ptr()->nd_id() != destInfo.current_bd05.curr_tree_id.fileRoot_gg00()){
            b13r_release_node( sourceInfo.pNode_bd05, sourceInfo.current_bd05, lru_normal );
        }
    }
    if( destInfo.pNode_bd05.np_ptr() != 0 ){
        b13w_release_node( destInfo.pNode_bd05, destInfo.current_bd05 );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
