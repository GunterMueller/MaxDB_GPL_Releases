/*!================================================================
 module:    IFRPACKET_REPLYSEGMENT_CPP

 responsible: D025086

 special area:
 description:

 see:

 change history:

      25. APRIL 2002 Initial version


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




 ===================================================================*/

// Prevents a Warning 5004 Uninitialized variable "static_i"
// from the HP ANSI C++ B3910B X.03.33.01 compiler.
// See http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1130907 - pragma not longer necessary
// #ifdef HPUX
// #pragma OPT_LEVEL 1
// #endif


#include <stdlib.h>
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/Packet/IFRPacket_ReplySegment.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/Util/IFRUtil_DefaultRawAllocator.h"
#include "Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "Interfaces/Runtime/Packet/IFRPacket_FunctionCode.h"

#define DBUG_NULL_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, (IFR_TraceStream *)0)

//----------------------------------------------------------------------
IFRPacket_ReplySegment::IFRPacket_ReplySegment(const IFRPacket_ReplyPacket &replyPacket)
:PIn_ReplySegment (const_cast<tsp1_segment*>(replyPacket.GetFirstSegment().GetRawSegment())),
 m_encoding(replyPacket.getEncoding())
{
    if(IsValid()) {
        m_Part = IFRPacket_Part(this->GetFirstPart(), m_encoding);
        memset(m_partcache, 0, sizeof(m_partcache));
        m_partcache_filled = false;
    }
}

//----------------------------------------------------------------------
IFR_Retcode IFRPacket_ReplySegment::getPart(IFRPacket_PartKind::PartKind partkind,
                                            IFRPacket_Part& part) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getPart);
    DBUG_PRINT(partkind);
    IFR_Retcode rc = IFR_NO_DATA_FOUND;
    part = IFRPacket_Part(((IFRPacket_ReplySegment *)this)->findPart(partkind), m_encoding);
    if (part.IsValid()) {
        rc=IFR_OK;
    }

    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ReplySegment::getPart(IFRPacket_DataPart& part) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getPart);
    DBUG_RETURN(getPart(IFRPacket_PartKind::Data_C, part));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ReplySegment::getPart(IFRPacket_VarDataPart& part) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getPart);
    DBUG_RETURN(getPart(IFRPacket_PartKind::VarData_C, part));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ReplySegment::getPart(IFRPacket_LongDataPart& part) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getPart);
    DBUG_RETURN(getPart(IFRPacket_PartKind::Longdata_C, part));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ReplySegment::getPart(IFRPacket_AbapOStreamPart& part) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getPart);
    DBUG_RETURN(getPart(IFRPacket_PartKind::AbapOStream_C, part));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ReplySegment::getPart(IFRPacket_AbapIStreamPart& part) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getPart);
    DBUG_RETURN(getPart(IFRPacket_PartKind::AbapIStream_C, part));
}



IFR_Retcode IFRPacket_ReplySegment::getErrorText(IFR_String &errText, IFR_Bool& memory_ok) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getErrorText);
    IFRPacket_ErrorTextPart part;
    IFR_Retcode rc = this->getPart(IFRPacket_PartKind::Errortext_C, part);
    if (rc == IFR_OK) {
        IFR_Int4 errorpos = getErrorPos();
        if(errorpos != -1) {
            char buffer[64];
            sp77sprintf(buffer, 64, "POS(%d) ", errorpos);
            errText.setBuffer(buffer, IFR_NTS, IFR_StringEncodingUTF8, memory_ok);
            if(memory_ok) {
                rc = part.getText(errText, memory_ok, true);
            } else {
                rc = IFR_NOT_OK;
            }
        } else {
            rc = part.getText(errText, memory_ok);
        }
    }
      
    

    DBUG_RETURN(rc);
}

IFR_Retcode IFRPacket_ReplySegment::getParseID(IFR_ParseID &ParseID) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getParseID);
    IFRPacket_ParseIDPart part;
    IFR_Retcode rc = this->getPart(IFRPacket_PartKind::Parsid_C, part);
    if (rc == IFR_OK) {
        DBUG_RETURN ( part.getParseID(ParseID) );
    }
    DBUG_RETURN ( rc );
}

IFR_Retcode IFRPacket_ReplySegment::getSelectParseID(IFR_ParseID &ParseID) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getSelectParseID);

  IFRPacket_ParseIDPart Part;
  IFR_Retcode rc = this->getPart(IFRPacket_PartKind::ParsidOfSelect_C, Part);
  if (rc == IFR_OK) {
    DBUG_RETURN ( Part.getParseID(ParseID) );
  }
  DBUG_RETURN ( rc );
}

IFR_Retcode
IFRPacket_ReplySegment::getResultTableName(IFR_String &resulttablename, IFR_Bool& memory_ok) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getResultTableName);
    IFRPacket_ResultTableNamePart Part;
    IFR_Retcode rc = this->getPart(IFRPacket_PartKind::Resulttablename_C, Part);
    if (rc == IFR_OK)
      rc = Part.getText(resulttablename, memory_ok);
    DBUG_RETURN( rc );
}



IFR_Byte *IFRPacket_ReplySegment::getPartDataPos ()
{
    return const_cast<IFR_Byte*>(m_Part.GetReadData ());
}

IFR_Retcode IFRPacket_ReplySegment::parseColumnNames (IFRUtil_Vector<IFR_String> **columnNames,
                                                      SAPDBMem_IRawAllocator& allocator,
                                                      IFR_Bool& memory_ok)
{
  DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, parseColumnNames);
  //<<< MEMCHECK
  if(!memory_ok) {
    DBUG_RETURN(IFR_NOT_OK);
  }
  //>>> MEMCHECK
  IFR_Retcode rc = IFR_OK;
  IFR_Int4 columnCount;
  IFR_String dummy(allocator);
  IFRUtil_Vector<IFR_String> *result = new IFR_ALLOCATOR(allocator)
      IFRUtil_Vector<IFR_String>(allocator, 0, dummy, memory_ok);
  //<<< MEMCHECK
  if(!memory_ok || result==0) {
   IFRUtil_Delete(result, allocator);
    memory_ok = false;
    DBUG_RETURN(IFR_NOT_OK);
  }
  //>>> MEMCHECK
  char nameLen;
  IFR_Byte *pos;

  IFRPacket_ColumnNamesPart part;
  rc = this->getPart (IFRPacket_PartKind::Columnnames_C, part);
  if (rc != IFR_OK) {
      IFRUtil_Delete(result, allocator);
      DBUG_RETURN(rc);
  }

  columnCount = part.getPartArguments();
  result->Resize (columnCount, memory_ok);
  //<<< MEMCHECK
  if(!memory_ok) {
    IFRUtil_Delete(result, allocator);
    DBUG_RETURN(IFR_NOT_OK);
  }
  //>>> MEMCHECK
  pos = this->getPartDataPos ();
  for (int i = 0; i < columnCount; i++) {
    memcpy (&nameLen, pos, sizeof (IFR_Byte));
    (*result)[i].setBuffer ((char*) pos + 1,
            nameLen,
            m_encoding,
            memory_ok);
    //<<< MEMCHECK
    if(!memory_ok) {
        IFRUtil_Delete(result, allocator);
        DBUG_RETURN(IFR_NOT_OK);
    }
    //>>> MEMCHECK

    pos += nameLen + 1;
  }

  *columnNames = result;

  DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
void
IFRPacket_ReplySegment::getSQLState(char *dest) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getSQLState);
    if(!IsValid()) {
        dest[0]='\0';
    }
    memcpy(dest, rawSegment->sp1r_sqlstate().asCharp(), 5);
    dest[5]='\0';
}




//----------------------------------------------------------------------
IFR_Retcode IFRPacket_ReplySegment::getResultCount (IFR_Int4& ResCount) const
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getResultCount);
    IFRPacket_ResultCountPart part;
    IFR_Retcode rc=this->getPart(IFRPacket_PartKind::Resultcount_C, part);
    if(rc == IFR_OK) {
      rc = part.getResultCount(ResCount);
      DBUG_PRINT(ResCount);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_ReplySegment::getSerialPart(IFRPacket_DataPart& part)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getResultCount);
    DBUG_RETURN(getPart(IFRPacket_PartKind::Serial_C, part));
}

/**
 * Returns the next part in a segment.
 *
 * <H3>returns:</H3>
 * IFRPacket_Part class.
 */
IFRPacket_Part IFRPacket_ReplySegment::getNextPart ()
{
  PIn_Part part = PIn_ReplySegment::GetNextPart (m_Part);

  m_Part = IFRPacket_Part(part, m_encoding);

  return m_Part;
}

/**
 * Returns the first part of a segment.
 *
 * <H3>returns:</H3>
 * IFRPacket_Part class.
 */
IFRPacket_Part IFRPacket_ReplySegment::getFirstPart ()
{
  PIn_Part part = PIn_ReplySegment::GetFirstPart();

  m_Part = IFRPacket_Part(part, m_encoding);

  return m_Part;
}

int IFRPacket_ReplySegment::partKind ()
{
  char knd;

  tsp1_part_header *hdr = m_Part.GetRawHeader ();
  memcpy (&knd, hdr, sizeof (char));

  return knd;
}

IFR_Byte *IFRPacket_ReplySegment::getPartDataPos () const
{
  return const_cast<IFR_Byte*>(m_Part.GetReadData ());
}



//----------------------------------------------------------------------
IFR_Bool
IFRPacket_ReplySegment::getSQLError(IFR_ErrorHndl& error,
                                    SAPDBMem_IRawAllocator& allocator,
                                    IFR_Bool notrace)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_ReplySegment, getError);
    char tmp_sqlstate[6];
    if(ErrorCode()) {
        error.setAllocator(allocator);
        IFR_String errortext(IFR_StringEncodingUTF8, allocator);
        getSQLState(tmp_sqlstate);
        IFR_Bool memory_ok=true;
        IFR_Retcode rc=getErrorText(errortext, memory_ok);
        //<<< MEMCHECK
        if(!memory_ok) {
            error.setMemoryAllocationFailed();
            DBUG_RETURN(true);
        }
        //>>> MEMCHECK
        if(rc == IFR_OK) {
            error.setSQLError(ErrorCode(), tmp_sqlstate, errortext, notrace);
        } else {
            error.setSQLError(ErrorCode(), tmp_sqlstate, "Message not available", IFR_StringEncodingAscii, notrace);
        }
        DBUG_RETURN(true);
    } else {
        DBUG_RETURN(false);
    }
}

/**
 * Returns the functioncode of the executed SQL command.
 *
 * <H3>returns:</H3>
 * IFRPacket_FunctionCode class.
 */
IFRPacket_FunctionCode IFRPacket_ReplySegment::FunctionCode() const
{
  return IFRPacket_FunctionCode(this->rawSegment->sp1r_function_code());
}

//----------------------------------------------------------------------
IFR_UInt1 IFRPacket_ReplySegment::getTraceLevel() const
{
    if(IsValid()) {
        return this->rawSegment->sp1r_tracelevel();
    } else {
        return 0;
    }
}


//----------------------------------------------------------------------
IFR_Int4 IFRPacket_ReplySegment::getErrorPos() const
{
    if(IsValid()) {
        return this->rawSegment->sp1r_errorpos();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
IFR_SQLMode
IFRPacket_ReplySegment::getSQLMode()
{
    if(IsValid()) {
        tsp1_sqlmode_Enum mode =  this->rawSegment->sp1c_sqlmode();
        return (IFR_SQLMode) mode;
    } else {
        return IFR_Nil;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ReplySegment::getABAPTabId(IFR_Int4& abaptabid,
                                     IFR_Bool& output,
                                     IFR_Int4& argumentcount)
{
    IFRPacket_Part part = findPart(IFRPacket_PartKind::AbapIStream_C);
    if(part.isValid()) {
        output = false;
        argumentcount = part.getPartArguments();
        memcpy(&abaptabid, part.GetReadData(), sizeof(IFR_Int4));
        return IFR_OK;
    }
    part = findPart(IFRPacket_PartKind::AbapOStream_C);
    if(part.isValid()) {
        output = true;
        argumentcount = part.getPartArguments();
        memcpy(&abaptabid, part.GetReadData(), sizeof(IFR_Int4));
        return IFR_OK;
    }
    return IFR_NO_DATA_FOUND;
}

//----------------------------------------------------------------------
IFRPacket_PartKind::PartKind
IFRPacket_ReplySegment::hasABAPStreams()
{
    IFRPacket_Part part = findPart(IFRPacket_PartKind::AbapIStream_C);
    if(!part.isValid()) {
        part = findPart(IFRPacket_PartKind::AbapOStream_C);
        if(part.isValid()) {
            return IFRPacket_PartKind::AbapOStream_C;
        } else {
            return IFRPacket_PartKind::Nil_C;
        }
    } else {
        return IFRPacket_PartKind::AbapIStream_C;
    }
}

//----------------------------------------------------------------------
IFRPacket_Part
IFRPacket_ReplySegment::findPart(IFR_Int4 partkind)
{
    if(m_partcache_filled) {
        if(partkind < sp1pk_last_part_kind) {
            IFRPacket_Part result(m_partcache[partkind], m_encoding);
            return result;
        } else {
            IFRPacket_Part result;
            return result;
        }
    } else {
        PIn_Part part = GetFirstPart();
        while(part.IsValid()) {
            m_partcache[part.GetRawHeader()->sp1p_part_kind] = part.GetRawPart();
            part = GetNextPart(part);
        }
        m_partcache_filled = true;
        return findPart(partkind);
    }
}
