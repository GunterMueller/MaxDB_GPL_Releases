/*!
  @file           IFR_LOBHost.cpp
  @author         D039759
  @ingroup        
  @brief          Basic LOB support classes.
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
// Prevents a Warning 5004 Uninitialized variable "static_#classcvt#693
// from the HP ANSI C++ B3910B X.03.33.01 compiler.
#ifdef HPUX
#pragma OPT_LEVEL 1
#endif


#include "Interfaces/Runtime/IFR_LOBHost.h"

//----------------------------------------------------------------------
IFR_LOBHost::IFR_LOBHost(SAPDBMem_IRawAllocator& allocator)
:m_lobs(allocator)
{}

//----------------------------------------------------------------------
IFR_LOBHost::~IFR_LOBHost()
{
    clearLOBs();
}

//----------------------------------------------------------------------
void
IFR_LOBHost::addLOB(IFR_LOB *lob, IFR_Bool& memory_ok)
{
    if (!lob) memory_ok = false;
    
    if(memory_ok) {
        m_lobs.InsertEnd(lob, memory_ok);
    }
    
    return;
}


//----------------------------------------------------------------------
IFR_Bool
IFR_LOBHost::checkLOB(IFR_LOB* lob)
{
    if(lob == 0) {
        return false;
    }
    IFR_size_t lobs_size=m_lobs.GetSize();
	for(IFR_size_t i=0; i<lobs_size; ++i) {
        if(m_lobs[i] == lob) {
            return true;
        }
	}
    return false;
}

//----------------------------------------------------------------------
void
IFR_LOBHost::invalidateLOBs()
{
    IFR_size_t lobs_size=m_lobs.GetSize();
    for(IFR_size_t i=0; i<lobs_size; ++i) {
        m_lobs[i]->invalidate();
    }
    return;
}

//----------------------------------------------------------------------
void
IFR_LOBHost::clearLOBs()
{
    IFR_size_t lobs_size=m_lobs.GetSize();
    for(IFR_size_t i=0; i<lobs_size; ++i) {
        IFRUtil_Delete(m_lobs[i], m_lobs.GetRawAllocator());
        m_lobs[i]=0;
    }
    IFR_Bool memory_ok_ignored = true;
    m_lobs.Resize(0, memory_ok_ignored);
}

//----------------------------------------------------------------------
IFR_GetvalHost::IFR_GetvalHost(IFR_Connection *connection, SAPDBMem_IRawAllocator& allocator)
:IFR_LOBHost(allocator),
 m_outputlongs(allocator),
 m_open_longs(allocator),
 m_connection(connection)
{}

//----------------------------------------------------------------------
IFR_GetvalHost::~IFR_GetvalHost()
{
    clearOutputLongs();
}

//----------------------------------------------------------------------
void 
IFR_GetvalHost::clearOutputLongs()
{
    IFR_size_t outputlongs_size=m_outputlongs.GetSize();
    for(IFR_size_t i=0; i<outputlongs_size; ++i) {
        IFRUtil_Delete(m_outputlongs[i], m_outputlongs.GetRawAllocator());
        m_outputlongs[i]=0;
    }
    IFR_Bool memory_ok_ignored=true;
    m_outputlongs.Resize(0, memory_ok_ignored);

    IFR_size_t openlongs_size = m_open_longs.GetSize();
    if(openlongs_size) {
        for(IFR_size_t j=0; j<openlongs_size; ++j) {
            m_open_longs[j].longdesc.valmode = IFRPacket_LongDescriptor::Close_C;
            m_connection->dropLongDescriptor(m_open_longs[j].longdesc, memory_ok_ignored);
        }
        m_open_longs.Resize(0, memory_ok_ignored);
        m_connection->dropGarbageLongDescriptors(memory_ok_ignored);
    }
    return;
}

//----------------------------------------------------------------------
void 
IFR_GetvalHost::closeOutputLongs()
{
    IFR_size_t outputlongs_size=m_outputlongs.GetSize();
    for(IFR_size_t i=0; i<outputlongs_size; ++i) {
        IFRConversion_Getval *gv=m_outputlongs[i];
        if(gv && !gv->isClosed()) {
            gv->close();
        }
        IFRUtil_Delete(gv, m_outputlongs.GetRawAllocator());
        m_outputlongs[i]=0;
    }
    IFR_Bool memory_ok_ignored=true;
    m_outputlongs.Resize(0, memory_ok_ignored);
/*  
    IFR_size_t openlongs_size = m_open_longs.GetSize();
    if(openlongs_size) {
        for(IFR_size_t j=0; j<openlongs_size; ++j) {
            m_open_longs[j].longdesc.valmode = IFRPacket_LongDescriptor::Close_C;
            m_connection->dropLongDescriptor(m_open_longs[j].longdesc, memory_ok_ignored);
        }
        m_open_longs.Resize(0, memory_ok_ignored);
        m_connection->dropGarbageLongDescriptors(memory_ok_ignored);
    }
*/
    return;
}



//----------------------------------------------------------------------
IFRConversion_Getval*
IFR_GetvalHost::getOutputLong(IFR_Int4 index) 
{
    return m_outputlongs[index];
}

//----------------------------------------------------------------------
IFRConversion_Getval *
IFR_GetvalHost::findOutputLong(IFR_Int2 column, IFR_Int4 row) 
{
    if (!column) return 0;

    if(row != 0) {
        IFR_size_t outputlong_count = m_outputlongs.GetSize();
        for(IFR_size_t i=0; i<outputlong_count; ++i) {
            if(m_outputlongs[i]->getRow() == row && m_outputlongs[i]->getColumn() == column) {
                return m_outputlongs[i];
            }
        }
    } else {
        IFR_size_t outputlong_count = m_outputlongs.GetSize();
        for(IFR_size_t i=0; i<outputlong_count; ++i) {
            if(m_outputlongs[i]->getColumn() == column) {
                return m_outputlongs[i];
            }
        }
    }
    
    return 0;
}

//----------------------------------------------------------------------
void
IFR_GetvalHost::addOutputLong(IFRConversion_Getval* getval, IFR_Bool& memory_ok)
{
    if (!getval != 0) {
      memory_ok = false;
      return;
    }

    // Limit the size
    if(m_outputlongs.GetSize() == 32767) {
        memory_ok = false;
        return;
    }
    
    if(memory_ok) {
        getval->setValIndex((IFR_Int2)m_outputlongs.GetSize());
        m_outputlongs.InsertEnd(getval, memory_ok);
    }
    
    IFR_size_t open_longs_size = m_open_longs.GetSize();
    IFR_Int4 r = getval->getRow();
    IFR_Int4 c = getval->getColumn();
    for(IFRUtil_Vector<IFR_OpenLongDescriptor>::Iterator ii=m_open_longs.Begin();
        ii!=m_open_longs.End();
        ++ii) {
        if(r == ii->row &&
           c == ii->column) {
            m_open_longs.Delete(ii);
            break;
        }
    }
            

    return;
}

//----------------------------------------------------------------------
void 
IFR_GetvalHost::checkOpenLongs(IFRConversion_ConverterList& cols,
                               IFR_Int4 start,
                               IFR_Int4 end,
                               IFR_Int4 recordSize,
                               IFRPacket_DataPart& datapart)
{
    if(datapart.isValid()) {
        IFR_Int4 rowstart = 0;
        for(IFR_Int4 i=start; i<=end; ++i, rowstart += recordSize) {
            datapart.setCurrentRowStart(rowstart);    
            for(IFR_Int4 j = 0; j<(IFR_Int4)cols.size(); ++j) {
                if(cols[j]->isLong() && cols[j]->shortinfo().isOutput()) {
                    char *p = (char *) datapart.getOutputData(cols[j]->shortinfo().pos.bufpos - 1);
                    // check for null value
                    if(*p == csp_undef_byte) {
                        continue;
                    } else {
                        IFRPacket_LongDescriptor ld;
                        memcpy(&ld, p+1, sizeof(IFRPacket_LongDescriptor));
                        if(ld.valmode == IFRPacket_LongDescriptor::DataPart_C ||
                           ld.valmode == IFRPacket_LongDescriptor::DataTrunc_C) {
                            addOpenLong(i, j+1, ld);
                            return; // there can be only one long ...
                        }
                    }
                }
            }
        }
    } 
        
}
    

//----------------------------------------------------------------------
void
IFR_GetvalHost::addOpenLong(IFR_Int4 row, IFR_Int4 col, IFRPacket_LongDescriptor& descriptor)
{
    // We will add this descriptor only if there isn't a long that 
    // is already handled by a getval instance.
    if(findOutputLong(row, col) == 0) {
        IFR_OpenLongDescriptor ol(row, col, descriptor);
        IFR_Bool memory_ok_ignored = true;
        m_open_longs.InsertEnd(ol, memory_ok_ignored);
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_GetvalHost::updateOutputLongs(IFRPacket_ReplySegment& segment,
                                  IFR_ConnectionItem& clink,
                                  IFR_Int4& updateCount,
                                  IFR_UInt8& longDataReadSizeCounter)
{
    DBUG_CONTEXT_METHOD_ENTER(IFR_GetvalHost, updateOutputLongs, &clink);
    
    IFRPacket_LongDataPart longdatapart;
    if(segment.getPart(longdatapart) != IFR_OK) {
        updateCount = 0;
        DBUG_RETURN(IFR_OK);
    }
    IFR_Int2  longdata_args=longdatapart.getPartArguments();
    IFR_Int2  valindex;
    IFR_UInt4 vallen;
    IFR_size_t outputlong_size = m_outputlongs.GetSize();
    char  *descrptr=longdatapart.getOutputData(0) + 1;
    longDataReadSizeCounter+=longdatapart.getBufferLength();
    for(IFR_Int2 i=0; i<longdata_args; ++i) {
        memcpy(&valindex, descrptr + IFRPacket_LongDescriptor::ValInd_O, sizeof(IFR_Int2));
        memcpy(&vallen, descrptr + IFRPacket_LongDescriptor::ValLen_O, sizeof(IFR_Int4));
        
        if(valindex<0 || (IFR_size_t)valindex >= outputlong_size) {
            updateCount = i;
            clink.error().setRuntimeError(IFR_ERR_INVALID_VALINDEX_I, (IFR_Int4)valindex);
            DBUG_RETURN(IFR_NOT_OK);
        }
        
        m_outputlongs[valindex]->updateDescriptor(descrptr);
        descrptr += IFRPacket_LongDescriptor::Size + vallen + 1;
    }
    updateCount = longdata_args;
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_GetvalHost::closeGetval(IFRConversion_Getval* getval, IFR_Bool& memory_ok)
{
    // Check if it matches.
    if(!getval) {
        return IFR_NOT_OK;
    } else if(getval != getOutputLong(getval->getValIndex())) {
        return IFR_NOT_OK;
    }
    
    return IFR_OK;
}


// //----------------------------------------------------------------------
// IFR_Retcode 
// IFR_GetvalHost::closeOutputLongs(IFRPacket_RequestPacket& packet, 
//                                  IFR_ConnectionItem& clink)
// {
//     DBUG_CONTEXT_METHOD_ENTER(IFR_GetvalHost, closeOutputLongs, &clink);

//     IFR_Int2 outputlongs_size = (IFR_Int2) m_outputlongs.GetSize();

//     // There is no segment to be added if there are no long values
//     if(outputlongs_size == 0) {
//         DBUG_RETURN(IFR_OK);
//     } 

//     // Now loop over all long values. The close command needs to be sent in INTERNAL mode. 
//     IFR_Int1 last_sqlmode = packet.switchSqlMode(IFR_INTERNAL);
//     IFRPacket_RequestSegment close_segment(packet, IFRPacket_CommandMessageType::Getval_C);
//     IFRPacket_LongDataPart longdatapart;
//     IFR_Retcode rc = IFR_OK;

//     // We know that we have at least one item at this point, so we 
//     // can create the longdata part.
//     if(close_segment.isValid()) {
//         if((rc = close_segment.addPart(longdatapart)) != IFR_OK) {
//             clink.error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
//             packet.switchSqlMode(last_sqlmode);
//             DBUG_RETURN(rc);
//         }
//     } else {
//         clink.error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
//         packet.switchSqlMode(last_sqlmode);
//         DBUG_RETURN(rc);
//     }
    
//     for(IFR_Int2 i=0; i<outputlongs_size; ++i) {
//         IFRConversion_Getval *gv=m_outputlongs[i];
//         // gv->setClose();
//         gv->putDescriptor(longdatapart, longdatapart.bufferLength()+1);
//     }
//     // Argument count is what we have done - the number of items in the vector.
//     longdatapart.setArgCount(outputlongs_size);
//     close_segment.closePart();
//     close_segment.close();
//     packet.switchSqlMode(last_sqlmode);
//     DBUG_RETURN(IFR_OK);
// }

//----------------------------------------------------------------------
// IFR_PutvalHost
//----------------------------------------------------------------------
IFR_PutvalHost::IFR_PutvalHost(SAPDBMem_IRawAllocator& allocator)
:IFR_LOBHost(allocator),
 m_inputlongs(allocator)
{
}

//----------------------------------------------------------------------
IFR_PutvalHost::~IFR_PutvalHost()
{
    clearInputLongs();
}

//----------------------------------------------------------------------
void
IFR_PutvalHost::clearInputLongs()
{
    IFR_size_t inputlongs_size=m_inputlongs.GetSize();
    for(IFR_size_t i=0; i<inputlongs_size; ++i) {
        IFRUtil_Delete(m_inputlongs[i], m_inputlongs.GetRawAllocator());
        m_inputlongs[i]=0;
    }
    IFR_Bool memory_ok_ignored = true;
    m_inputlongs.Resize(0, memory_ok_ignored);
}

//----------------------------------------------------------------------
IFRConversion_Putval *
IFR_PutvalHost::findInputLong(IFR_Int2 column, IFR_Int4 row) 
{
    if (!column) return 0;

    if(row != 0) {
        IFR_size_t inputlong_count = m_inputlongs.GetSize();
        for(IFR_size_t i=0; i<inputlong_count; ++i) {
            if(m_inputlongs[i]->getRow() == row && m_inputlongs[i]->getColumn() == column) {
                return m_inputlongs[i];
            }
        }
    } else {
        IFR_size_t inputlong_count = m_inputlongs.GetSize();
        for(IFR_size_t i=0; i<inputlong_count; ++i) {
            if(m_inputlongs[i]->getColumn() == column) {
                return m_inputlongs[i];
            }
        }
    }
    
    return 0;
}

//----------------------------------------------------------------------
void
IFR_PutvalHost::addInputLong(IFRConversion_Putval* putval, IFR_Bool& memory_ok)
{
    if (!putval) {
      memory_ok = false;
    }

    // Limit the size
    if(m_inputlongs.GetSize() == 32767) {
        memory_ok = false;
        return;
    }
    

    if(memory_ok) {
        IFR_size_t inputlong_size = m_inputlongs.GetSize();
        putval->setValIndex((IFR_Int2)inputlong_size);    
        m_inputlongs.InsertEnd(putval, memory_ok);
        
        if(memory_ok && inputlong_size>0) {
            IFR_UInt4 i = 0;
            while(i < inputlong_size) {
                if(m_inputlongs[i]->getBufpos() > putval->getBufpos()) {
                    break;
                }
                ++i;
            }
            if(i != inputlong_size) {
                for(IFR_UInt4 j=inputlong_size; j>i; --j) {
                    m_inputlongs[j] = m_inputlongs[j-1];
                    m_inputlongs[j]->setValIndex((IFR_Int2)(j));
                }
                m_inputlongs[i] = putval;
                putval->setValIndex((IFR_Int2)i);
            }
        }
    }
    
    return;
}

