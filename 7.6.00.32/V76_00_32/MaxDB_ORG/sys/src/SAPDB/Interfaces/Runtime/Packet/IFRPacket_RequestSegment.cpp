/*!
  @file           IFRPacket_RequestSegment.cpp
  @author         D025086
  @ingroup        IFR_Packet
  @brief          Request Segment.
  @see            

\if EMIT_LICENCE

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



\endif
*/
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_Trace.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/Packet/IFRPacket_RequestSegment.h"
#include "Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h"

#define DBUG_PACKET_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x,y,m_requestpacket)

//----------------------------------------------------------------------
IFRPacket_RequestSegment
::IFRPacket_RequestSegment(IFRPacket_RequestPacket& requestpacket,
                           int messagetype,
                           IFR_Bool resetpacket)
  :m_requestpacket(&requestpacket)
{
  DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, IFRPacket_RequestSegment);
  DBUG_PRINT(messagetype);
  DBUG_PRINT(resetpacket);
    if(m_requestpacket && m_requestpacket->isValid()) {
        if(resetpacket) {
            m_requestpacket->reset();
        }
        m_segment = m_requestpacket->addSegment(messagetype);
    }
}

//----------------------------------------------------------------------
IFRPacket_RequestSegment::IFRPacket_RequestSegment(const IFRPacket_RequestSegment& copy)
:m_requestpacket(copy.m_requestpacket),
 m_segment(copy.m_segment),
 m_lastpart(copy.m_lastpart)
{}


//----------------------------------------------------------------------
IFRPacket_RequestSegment::IFRPacket_RequestSegment(IFRPacket_RequestPacket &requestPacket, 
                                                   int messagetype,
                                                   int sqlmode,
                                                   const IFR_Bool parsingAgain,
                                                   const IFR_Bool dontReset)
:m_requestpacket(&requestPacket)
{ 
  DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, IFRPacket_RequestSegment);
  DBUG_PRINT(messagetype);
  DBUG_PRINT(sqlmode);
  DBUG_PRINT(parsingAgain);
  DBUG_PRINT(dontReset);
    if(m_requestpacket && m_requestpacket->isValid()) {
        if(!dontReset) {
            m_requestpacket->reset();
        }
        m_segment = m_requestpacket->addSegment(messagetype, parsingAgain, sqlmode);
    };
}



//----------------------------------------------------------------------
IFR_Bool 
IFRPacket_RequestSegment::isValid() const
{
    return m_segment.IsValid();
}

//----------------------------------------------------------------------
void
IFRPacket_RequestSegment::setSQLState(const char *sqlstate)
{
    if(!sqlstate || !(*sqlstate)) {
        tsp1_segment_header *header = &(m_segment.GetRawSegment()->sp1s_segm_header());
        memcpy(header->sp1r_sqlstate(), "00000", 5);
    } else {
        tsp1_segment_header *header = &(m_segment.GetRawSegment()->sp1s_segm_header());
        memcpy(header->sp1r_sqlstate(), sqlstate, 5);
    }
}

//----------------------------------------------------------------------
void
IFRPacket_RequestSegment::setErrorCode(IFR_Int4 errorCode)
{
    m_segment.GetRawSegment()->sp1r_returncode() = errorCode;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_RequestSegment::addPart(IFRPacket_PartKind::PartKind partkind,
                                  IFRPacket_Part& part)
{
  DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, addPart);
  DBUG_PRINT(partkind);
    this->closePart();
    m_segment.AddPart((tsp1_part_kind_Enum)partkind, m_lastpart);
    part = m_lastpart;
    part.setEncoding(this->getEncoding());
    if (part.isValid())     
        DBUG_RETURN(IFR_OK);
    else
        DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_RequestSegment::addPart(IFRPacket_PartKind::PartKind partkind,
                                  IFRPacket_DataPart& part)
{
  DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, addPart);
  IFR_Retcode rc = addPart(partkind, (IFRPacket_Part&)part);
  if(rc == IFR_OK) {
      if(partkind == IFRPacket_PartKind::VarData_C) {
          part.setVariableInput(true);
      }
      part.clearOffsets();
  }
  DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addPart(IFRPacket_ParseIDPart &part)
{
    return addPart(IFRPacket_PartKind::Parsid_C, part);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addPart(IFRPacket_ErrorTextPart &part)
{
    return addPart(IFRPacket_PartKind::Errortext_C, part);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addPart(IFRPacket_ApplParamPart &part)
{
    return addPart(IFRPacket_PartKind::ApplParameterDescription_C, part);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addPart(IFRPacket_DataPart &part)
{
    IFR_Retcode rc=addPart(IFRPacket_PartKind::Data_C, part);
    if(rc == IFR_OK) {
        part.clearOffsets();
    }
    return rc;
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addPart(IFRPacket_LongDataPart &part)
{
    IFR_Retcode rc=addPart(IFRPacket_PartKind::Longdata_C, part);
    if(rc == IFR_OK) {
        part.clearOffsets();
    }
    return rc;
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addPart(IFRPacket_LongDemandPart &part)
{
    IFR_Retcode rc=addPart(IFRPacket_PartKind::LongDemand_C, part);
    if(rc == IFR_OK) {
        part.clearOffsets();
    }
    return rc;
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addPart(IFRPacket_CommandPart &part)
{
    return addPart(IFRPacket_PartKind::Command_C, part);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addPart(IFRPacket_TableNamePart &part)
{
    return addPart(IFRPacket_PartKind::Resulttablename_C, part);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addPart(IFRPacket_ResultCountPart &part)
{
    return addPart(IFRPacket_PartKind::Resultcount_C, part);
}
//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addPart(IFRPacket_CommandInfoPart &part)
{
    return addPart(IFRPacket_PartKind::AbapInfo_C, part);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addPart(IFRPacket_FeaturePart &part)
{
    return addPart(IFRPacket_PartKind::Feature_C, part);
}

//----------------------------------------------------------------------
void 
IFRPacket_RequestSegment::closePart()
{
    DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, closePart);
    if (m_lastpart.isValid()) {
        m_segment.ClosePart(m_lastpart);
        m_lastpart.Invalidate();
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_RequestSegment::rollbackLastPart()
{
    if(!m_lastpart.isValid()) {
        return IFR_NOT_OK;
    } else {
        tsp1_segment_header *header = m_segment.GetRawHeader();
        --(header->sp1s_no_of_parts());
        return IFR_OK;
    }
}

//----------------------------------------------------------------------
const IFR_StringEncoding 
IFRPacket_RequestSegment::getEncoding() const
{
    DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, getEncoding);
    DBUG_RETURN(m_requestpacket->getEncoding());
}

//----------------------------------------------------------------------
void
IFRPacket_RequestSegment::setCommitImmediately()
{
    tsp1_segment_header *header=m_segment.GetRawHeader();
    header->sp1c_commit_immediately() = 1;
    return;
}

//----------------------------------------------------------------------
void
IFRPacket_RequestSegment::setProducer(const tsp1_producer_Param & oProducer)
{
    m_segment.SetProducer (oProducer);
    return;
}

//----------------------------------------------------------------------
void
IFRPacket_RequestSegment::setParseWithPrepare()
{
    tsp1_segment_header *header=m_segment.GetRawHeader();
    header->sp1c_prepare() = 1;
    return;
}

//----------------------------------------------------------------------
void
IFRPacket_RequestSegment::setScrollableOption(IFR_Bool scrollableoption)
{
    tsp1_segment_header *header=m_segment.GetRawHeader();
    if(scrollableoption) {
        header->sp1c_command_options().clear().addElement(sp1co_scrollable_cursor_on);
    }
    return;
}

//----------------------------------------------------------------------
void
IFRPacket_RequestSegment::setNoResultSetCloseNeeded()
{
    tsp1_segment_header *header=m_segment.GetRawHeader();
    header->sp1c_command_options().addElement(sp1co_no_resultset_close_needed);
    return;
}

//----------------------------------------------------------------------
void
IFRPacket_RequestSegment::close()
{
  DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, close);
    if(m_requestpacket && m_requestpacket->IsValid()) {
        m_requestpacket->CloseSegment(this->m_segment);
        this->m_segment.Invalidate();
    }
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addFetchSize (IFR_Int2 fetchsize)
{
  DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, addFetchSize);
  DBUG_PRINT(fetchsize);
  if (fetchsize <= 0)
    return addUndefResultCount();
  
  closePart();
  IFR_Retcode rc=IFR_OK;
  IFRPacket_ResultCountPart resultcountpart;
  if((rc=addPart(resultcountpart)) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  if((rc = resultcountpart.setFetchSize(fetchsize)) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  closePart();
  DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestSegment::addResultCount (IFR_Int4 count)
{
  DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, addResultCount);
  DBUG_PRINT(count);
  if (count == -1) 
    DBUG_RETURN(addUndefResultCount());
  closePart();
  IFR_Retcode rc=IFR_OK;
  IFRPacket_ResultCountPart resultcountpart;
  if((rc=addPart(resultcountpart)) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  if((rc = resultcountpart.setResultCount(count)) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  closePart();
  DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_RequestSegment::addUndefResultCount()
{
  DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, addUndefResultCount);
    closePart();
    IFR_Retcode rc=IFR_OK;
    IFRPacket_ResultCountPart resultcountpart;
    if((rc=addPart(resultcountpart)) != IFR_OK) {
      DBUG_RETURN(rc);
    }
    if((rc = resultcountpart.setUndefResultCount()) != IFR_OK) {
      DBUG_RETURN(rc);
    }
    closePart();
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_RequestSegment::addParseID(IFR_ParseID& parseid)
{
  DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, addParseID);
    closePart();
    IFR_Retcode rc=IFR_OK;
    IFRPacket_ParseIDPart parseidpart;
    if((rc=addPart(parseidpart)) != IFR_OK) {
      DBUG_RETURN(rc);
    }
    if((rc = parseidpart.addParseID(parseid)) != IFR_OK) {
      DBUG_RETURN(rc);
    }
    closePart();
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFRPacket_RequestSegment&
IFRPacket_RequestSegment::operator =(const IFRPacket_RequestSegment& assign)
{
    DBUG_PACKET_METHOD_ENTER(IFRPacket_RequestSegment, operator =);
    
    if(this == &assign) {
        return *this;
    } else {
        m_requestpacket=assign.m_requestpacket;
        m_segment=assign.m_segment;
        m_lastpart=assign.m_lastpart;
        return *this;
    }
}
    
//----------------------------------------------------------------------
void
IFRPacket_RequestSegment::setMassCommand()
{
    if(m_segment.GetRawSegment()) {
        m_segment.GetRawSegment()->sp1s_segm_header().sp1c_mass_cmd() = 1;
    }
}
