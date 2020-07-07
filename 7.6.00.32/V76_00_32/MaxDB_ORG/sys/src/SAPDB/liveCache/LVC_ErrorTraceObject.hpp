/*!**************************************************************************

  module      : LVC_ErrorTraceObject.hpp

  -------------------------------------------------------------------------

  responsible : d025251

  special area: 
  description : 


  last changed: 2001-05-15  10:26
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/


#ifndef LVC_ERRORTRACEOBJECT_HPP
#define LVC_ERRORTRACEOBJECT_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "ggg12.h"
#include "ggg92.h"
#include "hgg01.h"
#include "gbd900.h"

#include "Trace/Trace_Entry.hpp"
#include "FileDirectory/FileDir_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!*****************************************************************************

   class:        LVC_ErrorTraceObject
   description:  Used to write errors into the trace file
   
                 This class is used to log all errors which occur during
                 the release of an object frame. When an instance of this class
                 is destructed it writes the logged data into the trace file

*******************************************************************************/

class LVC_ErrorTraceObject
{
public:

    // method

    /*!-------------------------------------------------------------------------
    function:     LVC_ErrorTraceObject()
    description:  constructor
    arguments:    Trans      [in/out]     transaction context
                  ObjFileId  [in/out]     file id of the file accomodating the object to release
                  ObjFileNo  [in]         file number of file accomodating the object to release
                  OId        [in]         OId of the object to release
                  VtraceType [in]         label of function trying to release the object
    return value: none
    --------------------------------------------------------------------------*/

    LVC_ErrorTraceObject (tgg00_TransContext    &Trans,
                          tgg00_FileId          &ObjFileId,
                          const FileDir_FileNo &ObjFileNo,
                          const tgg92_KernelOid &OId,
                          tgg00_VtraceType_Enum  VtraceType);

    /*!-------------------------------------------------------------------------
    function:     LVC_ErrorTraceObject()
    description:  destructor

                  writes the error state of the trcae object into the trace file

    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    ~LVC_ErrorTraceObject();


    /*!-------------------------------------------------------------------------
    function:     WriteVersionErrorMessage()
    description:  reports a version error in the trace file 
    
                  When a version error is detedted this function writes all data
                  into the vtrace which are required to investigate the error

    arguments   : ObjFileNoOnPage [in] object file number which was found on the page accomodating
                                       the object to release
                  WantedObjVers   [in] expected version of the object to release
                  pObjFrame       [in] pointer to the object frame containing the object to release 
                                       
    return value: none
    --------------------------------------------------------------------------*/
    void WriteVersionErrorMessage (
        const FileDir_FileNo &ObjFileNoOnPage,
        const tgg91_PageRef        WantedObjVers,
        tbd900_ObjFrame           *pObjFrame);

    /*!-------------------------------------------------------------------------
    function:     SetStatus()
    description:  sets the error state of the trace object
    arguments:    Status [in] error state
    return value: none
    --------------------------------------------------------------------------*/
    void SetStatus (tgg12_RelObjStatus_Enum Status) 
    {
        m_RelObjStatus.becomes(Status);
    };

private:
    tgg00_TransContext      &m_Trans;
    tgg00_FileId            &m_ObjFileId;
    const FileDir_FileNo    &m_ObjFileNo;
    const tgg92_KernelOid   &m_OId;
    tgg12_RelObjStatus      m_RelObjStatus;
    tgg00_VtraceType_Enum   m_VtraceType;

    void WriteStartErrorMessage ();
    void WriteFinalErrorMessage ();
};

/*!*****************************************************************************

   endclass: LVC_ErrorTraceObject

*******************************************************************************/

inline void
LVC_ErrorTraceObject::WriteVersionErrorMessage (
    const FileDir_FileNo   &FileNo,
    const tgg91_PageRef     WantedObjVers,
    tbd900_ObjFrame        *pObjFrame)
{
    /* write message into vtrace */
    {
        tgg12_OmsVersionErrorTrace TrLineVersError;
        
        TrLineVersError.oveTrType_gg12.becomes  (otVersionError_egg12);
        TrLineVersError.oveTrType2_gg12.becomes (ot2Nil_egg12);
        TrLineVersError.oveObjId_gg12           = m_OId;
        TrLineVersError.oveWantedVers_gg12      = WantedObjVers;
        TrLineVersError.ove.ObjFileNo_C8_gg12() = 
            *reinterpret_cast<const tsp00_C8*>(FileNo.GetCharArray()); 
        
        if (NULL != pObjFrame)
        {
            TrLineVersError.oveObjVers_gg12         = pObjFrame->objHeader_bd900.ohdTransInfo_gg92.otrBeforeRef_gg00;
            TrLineVersError.oveFrameVers_gg12       = pObjFrame->objHeader_bd900.ohdFrameVersion_gg92;
        }
        
        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(m_VtraceType), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineVersError ), sizeof (TrLineVersError) );
    }
    {
        tgg12_OmsErrorTrace        TrLineError;
        
        TrLineError.oerrTrType_gg12.becomes (otError_egg12);
        TrLineError.oerrTrType2_gg12.becomes (ot2Nil_egg12);
        TrLineError.oerrRoot_gg12      = m_ObjFileId.fileRoot_gg00();
        TrLineError.oerrError_gg12     = m_Trans.trError_gg00;
        TrLineError.oerr.ObjFileNo_C8_gg12() = 
            *reinterpret_cast<const tsp00_C8*>(m_ObjFileNo.GetCharArray());   
        
        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(m_VtraceType), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineError ), sizeof (TrLineError) );
    }
    /* write note into knldiag */
    g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, "LVC_Error :WrongObjState", m_Trans.trError_gg00 );
}

/*---------------------------------------------------------------------------*/

inline void
LVC_ErrorTraceObject::WriteStartErrorMessage()
{
    tgg12_OmsOidTrace TrLineOId;
    
    TrLineOId.ooidTrType_gg12.becomes(otOid_egg12);
    TrLineOId.ooidTrType2_gg12.becomes(ot2Input_egg12);
    TrLineOId.ooidObjId_gg12   = m_OId;
    
    Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(m_VtraceType), 
        REINTERPRET_CAST( tsp00_BytePtr, &TrLineOId ), sizeof (TrLineOId) );
}

/*---------------------------------------------------------------------------*/

inline void
LVC_ErrorTraceObject::WriteFinalErrorMessage ()
{   
    {
        tgg12_OmsRelOidTrace TrLineRelOId;
        
        if ((e_ok != m_Trans.trError_gg00) && (m_RelObjStatus == rosObjSucessfullyReleased_gg12))
            m_RelObjStatus.becomes(rosUnknownError_gg12);
        
        TrLineRelOId.roidTrType_gg12.becomes(otRelOid_egg12);
        TrLineRelOId.roidTrType2_gg12.becomes(ot2Output_egg12);
        TrLineRelOId.roidObjId_gg12        = m_OId;
        TrLineRelOId.roidRelObjStatus_gg12 = m_RelObjStatus;
        
        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(m_VtraceType), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineRelOId ), sizeof (TrLineRelOId) );
    }   
    if (e_ok != m_Trans.trError_gg00)
    {
        tgg12_OmsErrorTrace TrLineOut;
        
        TrLineOut.oerrTrType_gg12.becomes  (otError_egg12);
        TrLineOut.oerrTrType2_gg12.becomes (ot2Nil_egg12);
        TrLineOut.oerrRoot_gg12            = m_ObjFileId.fileRoot_gg00();
        TrLineOut.oerr.ObjFileNo_C8_gg12() = *reinterpret_cast<const tsp00_C8*>(m_ObjFileNo.GetCharArray());   
        TrLineOut.oerrError_gg12           = m_Trans.trError_gg00;
        
        Trace_CommonEntry traceEntry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(m_VtraceType), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineOut ), sizeof (TrLineOut) );
    }
}

/*---------------------------------------------------------------------------*/

inline
LVC_ErrorTraceObject::LVC_ErrorTraceObject (tgg00_TransContext      &Trans,
                                            tgg00_FileId            &ObjFileId,
                                            const FileDir_FileNo    &ObjFileNo,
                                            const tgg92_KernelOid   &OId,
                                            tgg00_VtraceType_Enum   VtraceType)
:
m_Trans       (Trans),
m_ObjFileId   (ObjFileId),
m_ObjFileNo   (ObjFileNo),
m_OId         (OId),
m_VtraceType  (VtraceType)
{
    m_RelObjStatus.becomes(rosObjSucessfullyReleased_gg12);

    if (g01vtrace.vtrBdObject_gg00)
        WriteStartErrorMessage();
};

/*---------------------------------------------------------------------------*/

inline
LVC_ErrorTraceObject::~LVC_ErrorTraceObject ()
{
    if (g01vtrace.vtrBdObject_gg00 || (m_Trans.trError_gg00 != e_ok))
        WriteFinalErrorMessage();
}

/*---------------------------------------------------------------------------*/

#endif  /* LVC_ERRORTRACEOBJECT_HPP */
