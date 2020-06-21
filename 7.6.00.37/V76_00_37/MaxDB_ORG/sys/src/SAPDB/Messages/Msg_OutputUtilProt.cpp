/*!
  @file    Msg_OutputUtilProt.cpp
  @author  Robin
  @ingroup Common
  @brief   UtilProt Message output class
  
  implementation of diagnostic output used in the database kernel


  -------------------------------------------------------------------------

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#include "Messages/Msg_OutputFile.hpp"
#include "heo07.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "KernelCommon/ParameterNames/KernelParam_RunTime.h"
#include "geo007_2.h"
#include "RunTime/System/RTESys_Time.h"
#include "hsp100.h"
#include "RunTime/RTE_Speed.h"
#include "geo007_1.h"
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_Names.h"

/*!
  @class OutputFileUtilProt
 */
class OutputFileUtilProt : public Msg_OutputFile
{
public:
    OutputFileUtilProt (  const SAPDB_UInt8 fixedSizeInBytes, Msg_IOutput *pErrorArchive )
        : Msg_OutputFile    ( SAPDB_ADMIN_COMMANDS_FILE_NAME    /*SAPDB_Char const      *filename*/
                            , (SAPDB_UTF8 *)"util_prot_lock"    /*SAPDB_UTF8 const      *lockname*/
                            , ContinueExistingOutput            /*HandleExistingOutput const handleExistingOutput*/
                            , fixedSizeInBytes                  /*SAPDB_UInt8 const      fixedSizeInBytes*/
                            , pErrorArchive
                            )
    {
    }

    virtual SAPDB_Bool Output(Msg_List  &outputMsgList, Msg_List &errList, PassToErrorArchive toArchive )
    {
        outputMsgList.SetOutputIdentification( 0        /*m_SequenceNumber - will be really set in the Output() metmod*/
                                      ,sqlgetthreadid() /* SAPDB_UInt4 const outputThreadId=0*/
                                                        /*, SAPDB_UInt4 const outputTaskId=0*/
                                                        /*, SAPDB_UInt4 const outputSchedulerId=0 */);
        SAPDB_Bool retval = Msg_OutputFile::Output(outputMsgList, errList, toArchive );
        return retval;
    }

    virtual SAPDB_Bool BeginOfTerminationPhase(Msg_List &errList)
    {
        Lock();
        SAPDB_Bool retVal = Msg_OutputFile::BeginOfTerminationPhase(errList);
        Unlock();
        return retVal;
    }

private:
};

static OutputFileUtilProt *pInstance=NULL;

extern "C" void Msg_OutputToUtilProt(const SAPDB_Char *pFormatString,SAPDB_UInt length)
{
    SAPDB_Char *message = (SAPDB_Char *)alloca ( length+1 );
    
    SAPDB_strncpy_zero_terminated ( message, pFormatString, length+1 ) ;

    if(!pInstance)
    {
        // now create the diagnostic file itself (aka dbm.utl): existing files are renamed, file size is limited (cyclic write)
        SAPDBMEM_STATIC_RAW_ALLOCATE(UtilProt_InstanceData, sizeof(OutputFileUtilProt)-4);
        Msg_List errList;
        RTEConf_Parameter::Integer sizeFromParams;
        (RTEConf_ParameterAccess::Instance())->GetInteger(KERNELPARAM_UTILITY_PROTSIZE,sizeFromParams,errList);
        pInstance = new(UtilProt_InstanceData) OutputFileUtilProt   ( sizeFromParams*1024,NULL );
    }
    Msg_IOutput &utilProt = *pInstance;
    Msg_List errList;
#ifndef NO_OLD_DIAG_FILES
    Msg_List theList = Msg_List( Msg_List::Info, 0, MSG_MSGOUTPUT_COMPONENT, __FILE__, __LINE__, message);
#else
    Msg_List theList = Msg_List( Msg_List::Info, 0, "RTEKERN", __FILE__, __LINE__, message);
#endif
    utilProt.Output( theList, errList );
}

