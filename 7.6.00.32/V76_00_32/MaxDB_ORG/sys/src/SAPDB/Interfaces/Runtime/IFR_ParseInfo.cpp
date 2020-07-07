/*!
  @file           IFR_ParseInfo.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Util
  @brief          Information for parsed statements
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


#include "SAPDB/Interfaces/Runtime/IFR_ParseInfo.h"
#include "SAPDB/Interfaces/Runtime/IFR_Connection.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_FunctionCode.h"
#include "SAPDB/Interfaces/Runtime/IFR_ParseInfoCache.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestSegment.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_ReplySegment.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_PartEnum.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_Part.h"

IFR_BEGIN_NAMESPACE

//----------------------------------------------------------------------
IFR_ParameterMetaData::~IFR_ParameterMetaData()
{}

//----------------------------------------------------------------------
IFR_ResultSetMetaData::~IFR_ResultSetMetaData()
{}


//----------------------------------------------------------------------
IFR_ParseInfoData::IFR_ParseInfoData(const IFRPacket_FunctionCode& functioncode,
                                     const IFR_String& sqlcommand,
                                     const IFR_Int4 isolationlevel,
                                     IFR_Connection* connection,
                                     IFR_StringEncoding commandencoding,
                                     IFR_Int4 paraminfohash,
                                     IFR_Bool& memory_ok)
:IFRUtil_RuntimeItem(*connection),
 m_paraminfos(connection->allocator),
 m_inputparamcount(0),
 m_outputparamcount(0),
 m_outputcolumncount(0),
 m_functioncode(functioncode),
 m_sqlcommand(sqlcommand, memory_ok),
 m_isolationlevel(isolationlevel),
 m_inputparamrowsize(0),
 m_synthparsid(true),
 m_noexecute(false),
 m_appendmode(IFR_Connection::AppendAllowed_C),
 m_haslongs(false),
 m_lastinputlong(-1),
 m_lastoutputlong(-1),
 m_commandencoding(commandencoding),
 m_lock(0),
 m_refcount(1),
 m_connection(connection),
 m_cached(false),
 m_reparsestamp(0),
 m_paraminfohash(paraminfohash)
{
    if(!memory_ok) {
        return;
    }
    SQLDBC_IRuntime::Error error;
    runtime.createMutex(m_lock, allocator, error);
    if(m_lock == 0) {
        memory_ok = false;
    }
}

//----------------------------------------------------------------------
IFR_ParseInfoData::~IFR_ParseInfoData()
{
    if(m_lock) {
        SQLDBC_IRuntime::Error ignored;
        runtime.destroyMutex(m_lock, allocator, ignored);
    }
}

#define DBUG_PARSEINFO_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x,y,m_data)

//----------------------------------------------------------------------
IFR_ParseInfo::IFR_ParseInfo(const IFR_String& sqlcommand,
                             IFR_Int4 isolationlevel,
                             IFRPacket_FunctionCode functioncode,
                             IFR_Connection *connection,
                             IFR_StringEncoding commandencoding,
                             IFR_Int4 paraminfohash,
                             IFR_Bool& memory_ok)
{
    DBUG_CONTEXT_METHOD_ENTER(IFR_ParseInfo, IFR_ParseInfo, connection);
    if(!memory_ok) {
        connection->error().setMemoryAllocationFailed();
        m_data = 0;
        return;
    }
    m_data=new IFR_ALLOCATOR(sqlcommand.getAllocator()) IFR_ParseInfoData(functioncode, sqlcommand, isolationlevel, 
                                                                          connection, commandencoding, paraminfohash,
                                                                          memory_ok);
    if(m_data==0 || !memory_ok) {
        connection->error().setMemoryAllocationFailed();
        IFRUtil_Delete(m_data, sqlcommand.getAllocator());
        m_data = 0;
        return;
    }
    m_reparsestamp = 0;
}


//----------------------------------------------------------------------
IFR_ParseInfo::~IFR_ParseInfo()
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, ~IFR_ParseInfo);
    if(m_data) {
        if(m_data->decRef() <= 0) {
            IFR_Bool memory_ok=true;
            IFR_Connection *connection = m_data->m_connection;
            IFR_ParseInfoCache *pcache = 0;
            if(connection) { pcache= m_data->m_connection->getParseInfoCache(); }
            if(pcache != 0) {
                pcache->checkReleaseParseInfo(m_data, memory_ok);
            } else {
                // The cache has gone, so we will just throw the data away 
                // (the connection has gone too, and the kernel does no longer 
                //  know the parse ids).
                IFRUtil_Delete(m_data, m_data->allocator);
            }               
        } 
    }
}

//----------------------------------------------------------------------
IFR_Retcode IFR_ParseInfo::setParseID (const IFR_ParseID& parseid, IFR_Bool& memory_ok)
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, setParseID);
    if(!memory_ok) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(parseid.isMassCommand()) {
        m_data->dropParseID(m_data->m_massparseid, memory_ok);
        if(!memory_ok) {
            DBUG_RETURN(IFR_NOT_OK);
        }  
        m_data->m_massparseid = parseid;
        if(m_data->m_parseid.isValid()) {
            // if it was natural, it must be cleaned up with a drop command.
            if(!m_data->m_synthparsid) {
                m_data->dropParseID(m_data->m_parseid, memory_ok);
            } 
            if(!memory_ok) {
                DBUG_RETURN(IFR_NOT_OK);
            }
        }
        m_data->m_parseid = parseid;
        m_data->m_parseid.setSingleCommand();
        m_data->m_synthparsid=true;
    } else {
        if(!m_data->m_synthparsid) {
            m_data->dropParseID(m_data->m_parseid, memory_ok);
        } 
        m_data->m_parseid = parseid;
        m_data->m_synthparsid=false;
    }
    if(m_data->m_noexecute) {
        m_data->m_noexecute = true;
    }
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode IFR_ParseInfo::describeParseID (const IFR_StringEncoding m_CommandEncoding, 
                                            IFR_ConnectionItem& clink) 
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, describeParseID);
    IFRUtil_StackAllocator<128> stackalloc_describe;
    IFR_Bool memory_ok=true;
    IFRPacket_RequestPacket requestPacket(*(this->m_data));
    IFR_Retcode rc = this->m_data->m_connection->getRequestPacket (requestPacket, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    SAPDBMem_IRawAllocator& allocator = requestPacket.allocator;
    requestPacket.setEncoding(m_CommandEncoding);
    IFRPacket_RequestSegment segment(requestPacket, IFRPacket_CommandMessageType::Dbs_C);      
    IFRPacket_CommandPart command;
    rc = segment.addPart(command);
    IFR_String sqlCmd(requestPacket.getEncoding(), 
                      stackalloc_describe);
    sqlCmd.append("DESCRIBE", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    if(!memory_ok) {
        clink.error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    if (IFR_OK != command.setText(sqlCmd, clink.error())) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    segment.closePart();
    IFRPacket_ParseIDPart parseidPart;
    rc = segment.addPart(parseidPart);
    parseidPart.addParseID(m_data->m_parseid);
    segment.closePart();
    segment.close();
    
    IFRPacket_ReplyPacket replyPacket;
    rc = this->m_data->m_connection->sqlaexecute (requestPacket, replyPacket, 
                                                  IFR_Connection::AppendNotAllowed_C, clink.error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    } else if(clink.error()) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    IFRPacket_ReplySegment replysegment(replyPacket);
    IFRPacket_PartEnum partEnum (replysegment);
    IFRConversion_ConverterList convList(allocator);
    IFR_Int2 ColCount=0; 
    IFRUtil_Vector<IFR_String> *columnNames = 0;
    
    while (partEnum.hasMoreElements()) {
        int knd = partEnum.getPartKind();
        if (knd == IFRPacket_PartKind::Columnnames_C) {
            replysegment.parseColumnNames (&columnNames, 
                                           allocator,
                                           memory_ok);
            if(!memory_ok) {
                clink.error().setMemoryAllocationFailed();
                DBUG_RETURN(IFR_NOT_OK);
            }
        } else if (knd == IFRPacket_PartKind::Shortinfo_C) {
            IFRPacket_ShortInfoPart part;
            partEnum.getPart(part);
            part.parseShortFields (convList, clink);
        }
        partEnum.next();
    }
    
    this->lock();
    this->addShortInfosAndColumnNames (convList, columnNames);
    this->unlock();
    
    // column names have been temporary only !!! 
    IFRUtil_Delete(columnNames, allocator);
    
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode 
IFR_ParseInfo::addShortInfosAndColumnNames(IFRConversion_ConverterList& convList,
                                           IFRUtil_Vector<IFR_String>* columnames)
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, addShortInfosAndColumnNames);
    
    if(IFR_DBUG_TRACE) {
        for(IFR_UInt4 t=0; t < convList.size(); ++t) {
            DBUG_TRACE << "COLUMN " << (t+1) << ": " << convList[t]->shortinfo() << endl;
        }
    }

    
    IFR_Bool memory_ok = true;
    m_data->m_paraminfos.append(convList, memory_ok);
    m_data->m_appendmode       = IFR_Connection::AppendAllowed_C;
    IFR_Bool ok = true;
    IFR_Bool variableinput = m_data->m_paraminfos.hasVariableInput();
    destroyParamInfos(true);
    IFR_size_t convListSize = m_data->m_paraminfos.size();
    
    if (columnames == 0 || columnames->GetSize() == 0){
        // no columnnames were delivered - all paraminfos are real parameters  
        for(IFR_UInt2 i=0; i< convListSize; ++i) {
            m_data->m_paraminfos[i]->setIndex(i+1);
            if(m_data->m_paraminfos[i]->supportsInput()) {
                m_data->m_inputparamcount++;
                if(variableinput) {
                    IFR_Int4 iolength = m_data->m_paraminfos[i]->getIOLength();
                    if(iolength > 251) {
                        iolength += 2;
                    }
                    m_data->m_inputparamrowsize += iolength;
                } else {
                    IFR_UInt4 currentfieldend=m_data->m_paraminfos[i]->getBufpos() 
                        + m_data->m_paraminfos[i]->getIOLength() - 1;
                    m_data->m_inputparamrowsize = m_data->m_inputparamrowsize < currentfieldend 
                        ? currentfieldend 
                        : m_data->m_inputparamrowsize;
                }
            }
            if(m_data->m_paraminfos[i]->supportsOutput()) {
                if(isQuery()) {
                    m_data->m_outputcolumncount++;
                } else {
                    m_data->m_outputparamcount++;
                }
            }
            
            if(m_data->m_paraminfos[i]->isLong()) {
            	m_data->m_haslongs=true;
            	if(m_data->m_paraminfos[i]->supportsInput()) {
                    m_data->m_appendmode=IFR_Connection::AppendNotAllowed_C;
	            	m_data->m_lastinputlong=i;
            	} 
            	if(m_data->m_paraminfos[i]->supportsOutput()) {
	            	m_data->m_lastoutputlong=i;
            	}            		
            } else if(m_data->m_paraminfos[i]->isABAPStream()) {
                m_data->m_appendmode = IFR_Connection::StreamMode_C;
            }
        }
        if (!this->isQuery()){
            m_data->m_outputcolumncount = 0;
        } 
     } else {
        if (this->isQuery() 
            && m_data->m_functioncode != IFRPacket_FunctionCode::DBProcWithResultSetExecute_FC ) {
            //columnnames are names of the result columns - asumes parsing with "with_info" flag
            int columncount=columnames->GetSize();
            DBUG_PRINT(columncount);
            m_data->m_outputcolumncount = 0;
            
            for(IFR_UInt2 i=0; i<convListSize; ++i) {
                m_data->m_paraminfos[i]->setIndex(i+1);
                
                if(m_data->m_paraminfos[i]->supportsInput()) {
                    m_data->m_inputparamcount++;
                    if(variableinput) {
                        IFR_Int4 iolength = m_data->m_paraminfos[i]->getIOLength();
                        if(iolength > 251) {
                            iolength += 2;
                        }
                        m_data->m_inputparamrowsize += iolength;
                    } else {
                        IFR_UInt4 currentfieldend=m_data->m_paraminfos[i]->getBufpos() 
                            + m_data->m_paraminfos[i]->getIOLength() - 1;
                        m_data->m_inputparamrowsize = m_data->m_inputparamrowsize < currentfieldend 
                            ? currentfieldend 
                            : m_data->m_inputparamrowsize;
                    }
                }
                if(m_data->m_paraminfos[i]->supportsOutput()) {
                    m_data->m_paraminfos[i]->setName((*columnames)[m_data->m_outputcolumncount], ok);
                    if(!ok) {
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                    m_data->m_outputcolumncount++;
                }
                if(m_data->m_paraminfos[i]->isLong()) {
                    m_data->m_haslongs=true;
                    m_data->m_appendmode = IFR_Connection::AppendNotAllowed_C;
                    if(m_data->m_paraminfos[i]->supportsInput()) {
                        m_data->m_lastinputlong=i;
                    } 
                    if(m_data->m_paraminfos[i]->supportsOutput()) {
                        m_data->m_lastoutputlong=i;
                    }            		
                } else if(m_data->m_paraminfos[i]->isABAPStream()) {
                    m_data->m_appendmode = IFR_Connection::StreamMode_C;
                }
            }      
        } else {
            //columnnames are names of the in/out parameter - assumes parsing with "with_info" flag
            //number of column names and number of parameters must match
            int columncount=columnames->GetSize();
            DBUG_PRINT(columncount);
            IFR_Bool allnames= convListSize==columncount;
            
            for(IFR_UInt2 i=0; i<convListSize; ++i) {
                m_data->m_paraminfos[i]->setIndex(i+1);
                
                if(m_data->m_paraminfos[i]->supportsInput()) {
                  m_data->m_inputparamcount++;
                  if(allnames) {
                      m_data->m_paraminfos[i]->setName((*columnames)[i], ok);
                      if(!ok) {
                          DBUG_RETURN(IFR_NOT_OK);
                      }
                  }
                  if(variableinput) {
                      IFR_Int4 iolength = m_data->m_paraminfos[i]->getIOLength();
                      if(iolength > 251) {
                          iolength += 2;
                      }
                      m_data->m_inputparamrowsize += iolength;
                  } else {
                      IFR_UInt4 currentfieldend=m_data->m_paraminfos[i]->getBufpos() 
                          + m_data->m_paraminfos[i]->getIOLength() - 1;
                      m_data->m_inputparamrowsize = m_data->m_inputparamrowsize < currentfieldend 
                          ? currentfieldend 
                          : m_data->m_inputparamrowsize;
                  }
              }
              if(m_data->m_paraminfos[i]->supportsOutput()) {
                  if(allnames) {
                      m_data->m_paraminfos[i]->setName((*columnames)[i], ok);
                      if(!ok) {
                          DBUG_RETURN(IFR_NOT_OK);
                      }
                  } else if( columncount >= m_data->m_outputparamcount) {
                      m_data->m_paraminfos[i]->setName((*columnames)[m_data->m_outputparamcount], ok);
                      if(!ok) {
                          DBUG_RETURN(IFR_NOT_OK);
                      }
                  }
                  m_data->m_outputparamcount++;
              }
              if(m_data->m_paraminfos[i]->isLong()) {
              	m_data->m_haslongs=true;
                m_data->m_appendmode = IFR_Connection::AppendNotAllowed_C;
              	if(m_data->m_paraminfos[i]->supportsInput()) {
  	            	m_data->m_lastinputlong=i;
              	} 
              	if(m_data->m_paraminfos[i]->supportsOutput()) {
  	            	m_data->m_lastoutputlong=i;
              	}            		
              } else if(m_data->m_paraminfos[i]->isABAPStream()) {
                  m_data->m_appendmode = IFR_Connection::StreamMode_C;
              }
          }
          if (m_data->m_functioncode != IFRPacket_FunctionCode::DBProcWithResultSetExecute_FC){
              m_data->m_outputcolumncount = 0;          
          }  
        }// end else isSelect
    }    
    

    DBUG_PRINT(m_data->m_inputparamcount);
    DBUG_PRINT(m_data->m_outputparamcount);
    DBUG_PRINT(m_data->m_haslongs);
    DBUG_RETURN(IFR_OK);
}    


//----------------------------------------------------------------------
IFR_Retcode 
IFR_ParseInfo::setShortInfosAndColumnNames(IFRConversion_ConverterList& convList,
                                           IFRUtil_Vector<IFR_String>* columnames)
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, setShortInfosAndColumnNames);
    
    if(IFR_DBUG_TRACE) {
        for(IFR_UInt4 t=0; t < convList.size(); ++t) {
            DBUG_TRACE << "PARAMETER " << (t+1) << ": " << convList[t]->shortinfo() << endl;
        }
    }
    
    
    checkParamInfos(convList);
    destroyParamInfos();
    
    IFR_Bool memory_ok = true;
    IFR_size_t convListSize = convList.size();
    IFR_Bool variableinput = convList.hasVariableInput();
    m_data->m_paraminfos.swap(convList, memory_ok);
    m_data->m_appendmode       = IFR_Connection::AppendAllowed_C;
    IFR_Bool ok = true;
    m_data->m_inputparamrowsize = 0;
    IFR_Bool query = isQuery();
    
    if (columnames == 0 || columnames->GetSize() == 0){
        // no columnnames were delivered - all paraminfos are real parameters  
        for(IFR_UInt2 i=0; i< convListSize; ++i) {
            m_data->m_paraminfos[i]->setIndex(i+1);
            if(m_data->m_paraminfos[i]->supportsInput()) {
                m_data->m_inputparamcount++;
                if(variableinput) {
                    IFR_Int4 iolength = m_data->m_paraminfos[i]->getIOLength();
                    if(iolength > 251) {
                        iolength += 2;
                    }
                    m_data->m_inputparamrowsize += iolength;
                } else {
                    IFR_UInt4 currentfieldend=m_data->m_paraminfos[i]->getBufpos() 
                        + m_data->m_paraminfos[i]->getIOLength() - 1;
                    m_data->m_inputparamrowsize = m_data->m_inputparamrowsize < currentfieldend 
                        ? currentfieldend 
                        : m_data->m_inputparamrowsize;
                }
            }
            if(m_data->m_paraminfos[i]->supportsOutput()) {
                if(query) {
                    // distinguish between UNDEF and normal increment
                    if(m_data->m_outputcolumncount < 0) {
                        m_data->m_outputcolumncount = 1;
                    } else {
                        m_data->m_outputcolumncount++;
                    }
                } else {
                    m_data->m_outputparamcount++;
                }
            }
            
            if(m_data->m_paraminfos[i]->isLong()) {
            	m_data->m_haslongs=true;
            	if(m_data->m_paraminfos[i]->supportsInput()) {
                    m_data->m_appendmode=IFR_Connection::AppendNotAllowed_C;
	            	m_data->m_lastinputlong=i;
            	} 
            	if(m_data->m_paraminfos[i]->supportsOutput()) {
	            	m_data->m_lastoutputlong=i;
            	}            		
            } else if(m_data->m_paraminfos[i]->isABAPStream()) {
                m_data->m_appendmode = IFR_Connection::StreamMode_C;
            }
        }
        if (!query){
            m_data->m_outputcolumncount = 0;
        } 
     } else {
        if (query && m_data->m_functioncode != IFRPacket_FunctionCode::DBProcWithResultSetExecute_FC ) {
            //columnnames are names of the result columns - asumes parsing with "with_info" flag
            int columncount=columnames->GetSize();
            DBUG_PRINT(columncount);
            m_data->m_outputcolumncount = 0;
            
            for(IFR_UInt2 i=0; i<convListSize; ++i) {
                m_data->m_paraminfos[i]->setIndex(i+1);
                
                if(m_data->m_paraminfos[i]->supportsInput()) {
                    m_data->m_inputparamcount++;
                    if(variableinput) {
                        IFR_Int4 iolength = m_data->m_paraminfos[i]->getIOLength();
                        if(iolength > 251) {
                            iolength += 2;
                        }
                        m_data->m_inputparamrowsize += iolength;
                    } else {
                        IFR_UInt4 currentfieldend=m_data->m_paraminfos[i]->getBufpos() 
                            + m_data->m_paraminfos[i]->getIOLength() - 1;
                        m_data->m_inputparamrowsize = m_data->m_inputparamrowsize < currentfieldend 
                            ? currentfieldend 
                            : m_data->m_inputparamrowsize;
                    }
                }
                if(m_data->m_paraminfos[i]->supportsOutput()) {
                    m_data->m_paraminfos[i]->setName((*columnames)[m_data->m_outputcolumncount], ok);
                    if(!ok) {
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                    m_data->m_outputcolumncount++;
                }
                if(m_data->m_paraminfos[i]->isLong()) {
                    m_data->m_haslongs=true;
                    m_data->m_appendmode = IFR_Connection::AppendNotAllowed_C;
                    if(m_data->m_paraminfos[i]->supportsInput()) {
                        m_data->m_lastinputlong=i;
                    } 
                    if(m_data->m_paraminfos[i]->supportsOutput()) {
                        m_data->m_lastoutputlong=i;
                    }            		
                } else if(m_data->m_paraminfos[i]->isABAPStream()) {
                    m_data->m_appendmode = IFR_Connection::StreamMode_C;
                }
            }      
        } else {
            //columnnames are names of the in/out parameter - assumes parsing with "with_info" flag
            //number of column names and number of parameters must match
            int columncount=columnames->GetSize();
            DBUG_PRINT(columncount);
            IFR_Bool allnames= convListSize==columncount;
            
            for(IFR_UInt2 i=0; i<convListSize; ++i) {
                m_data->m_paraminfos[i]->setIndex(i+1);
                
                if(m_data->m_paraminfos[i]->supportsInput()) {
                  m_data->m_inputparamcount++;
                  if(allnames) {
                      m_data->m_paraminfos[i]->setName((*columnames)[i], ok);
                      if(!ok) {
                          DBUG_RETURN(IFR_NOT_OK);
                      }
                  }
                  if(variableinput) {
                      IFR_Int4 iolength = m_data->m_paraminfos[i]->getIOLength();
                      if(iolength > 251) {
                          iolength += 2;
                      }
                      m_data->m_inputparamrowsize += iolength;
                  } else {
                      IFR_UInt4 currentfieldend=m_data->m_paraminfos[i]->getBufpos() 
                          + m_data->m_paraminfos[i]->getIOLength() - 1;
                      m_data->m_inputparamrowsize = m_data->m_inputparamrowsize < currentfieldend 
                          ? currentfieldend 
                          : m_data->m_inputparamrowsize;
                  }
              }
              if(m_data->m_paraminfos[i]->supportsOutput()) {
                  if(allnames) {
                      m_data->m_paraminfos[i]->setName((*columnames)[i], ok);
                      if(!ok) {
                          DBUG_RETURN(IFR_NOT_OK);
                      }
                  } else if( columncount >= m_data->m_outputparamcount) {
                      m_data->m_paraminfos[i]->setName((*columnames)[m_data->m_outputparamcount], ok);
                      if(!ok) {
                          DBUG_RETURN(IFR_NOT_OK);
                      }
                  }
                  m_data->m_outputparamcount++;
              }
              if(m_data->m_paraminfos[i]->isLong()) {
              	m_data->m_haslongs=true;
                m_data->m_appendmode = IFR_Connection::AppendNotAllowed_C;
              	if(m_data->m_paraminfos[i]->supportsInput()) {
  	            	m_data->m_lastinputlong=i;
              	} 
              	if(m_data->m_paraminfos[i]->supportsOutput()) {
  	            	m_data->m_lastoutputlong=i;
              	}            		
              } else if(m_data->m_paraminfos[i]->isABAPStream()) {
                  m_data->m_appendmode = IFR_Connection::StreamMode_C;
              }
          }
          if (m_data->m_functioncode != IFRPacket_FunctionCode::DBProcWithResultSetExecute_FC){
              m_data->m_outputcolumncount = 0;          
          }  
        }// end else isSelect
     }    
    
    DBUG_PRINT(m_data->m_inputparamcount);
    DBUG_PRINT(m_data->m_outputparamcount);
    DBUG_PRINT(m_data->m_haslongs);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFRConversion_ConverterList& IFR_ParseInfo::getParameterInfos() const
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, getParameterInfos);
    return m_data->m_paraminfos;
}

//----------------------------------------------------------------------
IFR_Connection::AppendMode
IFR_ParseInfo::getAppendMode() const
{
    return m_data->m_appendmode;
}

//----------------------------------------------------------------------
void
IFR_ParseInfo::destroyParamInfos(IFR_Bool preserveparameters)
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, destroyParamInfos);
    m_data->destroyParamInfos(preserveparameters);
}

//----------------------------------------------------------------------
void
IFR_ParseInfoData::destroyParamInfos(IFR_Bool preservevector)
{
    DBUG_METHOD_ENTER(IFR_ParseInfoData, destroyParamInfos);
    if(!preservevector) {
        m_paraminfos.clear();
    }
    m_inputparamcount    =  0;
    m_outputparamcount   =  0;
    m_outputcolumncount  = -1; 
    m_inputparamrowsize  =  0;
    m_haslongs           = false;
    m_lastinputlong      = -1;
    m_lastoutputlong     = -1;
}

//----------------------------------------------------------------------
IFR_Bool
IFR_ParseInfo::isQuery() const
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, isQuery);
    DBUG_RETURN(m_data->m_functioncode.isQuery());
}

//----------------------------------------------------------------------
void
IFR_ParseInfo::setNoExecute(IFR_Bool noExecute)
{
    m_data->m_noexecute = noExecute;
}

//----------------------------------------------------------------------
IFR_Bool
IFR_ParseInfo::isNoExecute() const
{
    return m_data->m_noexecute;
}

//----------------------------------------------------------------------
IFR_Bool
IFR_ParseInfo::isMassCommand() const
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, isMassCommand);
    DBUG_RETURN(m_data->m_functioncode.isMassCommand());
}

void IFR_ParseInfo::setFunctionCode (const IFRPacket_FunctionCode& functionCode)
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, setFunctionCode);
    DBUG_PRINT(functionCode);
    m_data->m_functioncode = functionCode;
}


//----------------------------------------------------------------------
IFR_UInt4
IFR_ParseInfo::getInputParameterRowSize()
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, getInputParameterRowSize);
    DBUG_RETURN(m_data->m_inputparamrowsize);
}


//----------------------------------------------------------------------
const IFR_String& 
IFR_ParseInfo::getSQLCommand() const
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, getSQLCommand);
    return m_data->m_sqlcommand;
}

//----------------------------------------------------------------------
IFR_Int2 
IFR_ParseInfo::getInputParameterCount() const
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, getInputParameterCount);
    DBUG_RETURN(m_data->m_inputparamcount);
}


//----------------------------------------------------------------------
IFR_Int2 
IFR_ParseInfo::getOutputParameterCount() const
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, getOutputParameterCount);
    DBUG_RETURN(m_data->m_outputparamcount);
}

//----------------------------------------------------------------------
void
IFR_ParseInfoData::dropParseID(IFR_ParseID &parseid, IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_ParseInfoData, dropParseID);
    DBUG_PRINT(parseid);
    // validity of parse id is checked automatically using compare with connection
    // id.
    if(m_connection && (parseid.getConnectionID() == m_connection->getConnectionID())) {
        m_connection->dropParseID(parseid, memory_ok);
    } 
    return;
}


//----------------------------------------------------------------------
void 
IFR_ParseInfoData::runDecomission(IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_ParseInfoData, runDecomission);
    // The normal parse id.
    if(!m_synthparsid && m_connection && m_parseid.isValid()) {
        if(m_parseid.getConnectionID() == m_connection->getConnectionID()) {
            m_connection->dropParseID(m_parseid, memory_ok);
        } 
    }
    // The mass parse id.
    if(m_connection && m_massparseid.isValid()) {
        if(m_massparseid.getConnectionID() == m_connection->getConnectionID()) {
            m_connection->dropParseID(m_massparseid, memory_ok);
        } 
    }
    destroyParamInfos();
}

//----------------------------------------------------------------------
void
IFR_ParseInfo::lock()
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, lock);
    m_data->lock();
}

//----------------------------------------------------------------------
void
IFR_ParseInfo::unlock()
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, unlock);
    m_data->unlock();
}


//----------------------------------------------------------------------
IFR_Bool 
IFR_ParseInfo::isCached() const
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, isCached);
    DBUG_RETURN(m_data->isCached());
}

//----------------------------------------------------------------------
IFRPacket_FunctionCode
IFR_ParseInfo::getFunctionCode() const
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, getFunctionCode);
    DBUG_PRINTS(m_functioncode,m_data->m_functioncode);
    return m_data->m_functioncode;
}

//----------------------------------------------------------------------
void
IFR_ParseInfo::invalidate()
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, invalidate);
    m_data->m_parseid.invalidate();
    m_data->m_massparseid.invalidate();
}

static char hexchar[] ={ '0', '1', '2', '3', '4', '5', '6', '7', 
                         '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' 
};

IFR_TraceStream& operator << (IFR_TraceStream& s, const class IFR_ParseID &p )
{
  char tmp[9];tmp[8]='\0';
  const unsigned char *data = p.getParseID();
  if (!data) {
    s << "(null)";
    return s;
  }
  for(int i = 0; i < IFR_ParseID_Size; i+=4) {
    char *p=tmp;
    for(int j=0; j <= 3; j++) {
      *p=hexchar[ (*data >> 4) & 0xF ];
      p++;
      *p=hexchar[ (*data) & 0xF ];
      p++;
      data++;
    }
    s << tmp << " ";
  }
  s << "[" << p.getConnectCount() << "]";
  return s;
}

IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_ParseInfo &parseinfo )
{
  s << *parseinfo.getParseID();
  if (!parseinfo.isCached()) {
    s << " not cached";
  }
  return s;
}
//------------------------------------------------------------------------------
IFR_Bool
IFR_ParseInfo::hasLongs ()
{
  return m_data->m_haslongs;
}

//----------------------------------------------------------------------
IFRConversion_Converter  * 
IFR_ParseInfo::findParamInfo (IFR_Int2 param)
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, findParamInfo);
    if(param < 0 || (IFR_UInt4)param > m_data->m_paraminfos.size()) {
        return 0;
    } else {
        return m_data->m_paraminfos[param-1]; 
    }
}

//----------------------------------------------------------------------
IFRConversion_Converter * 
IFR_ParseInfo::findColInfo (IFR_Int2 columnIndex)
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, findColInfo);
    if(columnIndex < 0  || columnIndex > m_data->m_outputcolumncount) {
        return 0;
    } else {
        return m_data->m_paraminfos[m_data->m_inputparamcount + columnIndex - 1];        
    }
}

//----------------------------------------------------------------------
IFR_Int2
IFR_ParseInfo::getColumnCount()
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, getColumnCount);
    DBUG_RETURN(m_data->m_outputcolumncount);
}

//----------------------------------------------------------------------
IFR_Int2
IFR_ParseInfo::getParameterCount() const
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, getParameterCount);
    if(m_data->m_outputcolumncount > 0) {
        DBUG_RETURN((IFR_Int2)(m_data->m_paraminfos.size() - m_data->m_outputcolumncount) );
    } else { 
        DBUG_RETURN((IFR_Int2)(m_data->m_paraminfos.size()));
    }
}

//----------------------------------------------------------------------
IFR_Int2
IFR_ParseInfo::getLastLongInput() const
{
	return m_data->m_lastinputlong;
}

//----------------------------------------------------------------------
void
IFR_ParseInfo::checkParamInfos(IFRConversion_ConverterList& convList)
{
    DBUG_PARSEINFO_METHOD_ENTER(IFR_ParseInfo, checkParamInfos);

    IFR_size_t stored_size = m_data->m_paraminfos.size();
    
    // If there is nothing to compare, return OK.
    if(stored_size == 0) {
        return;
    }
    
    IFR_size_t sz = convList.size();
    
    // Check number of parameters.
    if(stored_size != sz) {
        ++m_data->m_reparsestamp;
        return;
    }
    for(IFR_UInt4 i=0; i<sz; ++i) {
        if(!convList[i]->shortinfo().isEquivalent(m_data->m_paraminfos[i]->shortinfo())) {
            ++m_data->m_reparsestamp;
            return;
        }
    }
    return;
}

//----------------------------------------------------------------------
IFR_Int2
IFR_ParseInfo::getLastLongOutput() const
{
	return m_data->m_lastoutputlong;
}

//----------------------------------------------------------------------
IFRConversion_Converter**
IFR_ParseInfo::getColumnInfos()
{
    if(m_data->m_outputcolumncount != -1) {
        return & (m_data->m_paraminfos[m_data->m_inputparamcount]);
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
IFR_Bool
IFR_ParseInfo::hasChangedFields()
{
    IFR_Bool result;
    if(m_data->m_reparsestamp != m_reparsestamp) {
        result = true;
    } else {
        result = false;
    }
    m_reparsestamp = m_data->m_reparsestamp;
    return result;
}

//----------------------------------------------------------------------
void
IFR_ParseInfo::sqlTrace(IFR_TraceStream& s)
{
    if(m_data->m_inputparamcount > 0 || m_data->m_outputparamcount > 0) {
        // we start with the sum ...
        IFR_UInt4 tracecount = m_data->m_inputparamcount + m_data->m_outputparamcount;
        s << "PARAMETERS:" << endl;
        s << "I   T              L    P   IO    N" << endl;
        IFRConversion_ConverterList& c = getParameterInfos();
        for(IFR_UInt4 i=0; i < tracecount; ++i) {
            if(!isQuery() || m_data->m_functioncode == IFRPacket_FunctionCode::DBProcWithResultSetExecute_FC) {
                c[i]->sqlTrace(s);
                if(c[i]->shortinfo().isInput()) {
                    if(c[i]->shortinfo().isOutput()) {
                        s << " INOUT ";
                        // ... two in one. We must reduce the overall number !!!
                        --tracecount;
                    } else {
                        s << " IN    ";
                    }
                } else {
                    s << " OUT   ";
                }
                s << c[i]->getName() << endl;
            } else {
                c[i]->sqlTrace(s);
                if(!c[i]->shortinfo().isOutput()) {
                    if(c[i]->shortinfo().isInput()) {
                        if(c[i]->shortinfo().isOutput()) {
                            s << " INOUT ";
                            // ... two in one. We must reduce the overall number !!!
                            --tracecount;
                        } else {
                            s << " IN    ";
                        }
                    } else {
                        s << " OUT   ";
                    }
                    s << c[i]->getName() << endl;
                }
            }
        }
    }
    if(m_data->m_outputcolumncount) {
        s << "COLUMNS:" << endl
          << "I   T              L    P   N" << endl;
        IFRConversion_Converter **c=getColumnInfos();
        for(IFR_Int4 j=0; j<m_data->m_outputcolumncount; ++j) {
            c[j]->sqlTrace(s);
            s << " " << c[j]->getName() << endl;
        }
    }
    return;
}

//----------------------------------------------------------------------
IFR_Bool 
IFR_ParseInfo::isParsedWithParamDesc() const
{ 
    return m_data->m_paraminfohash != 0;
}

IFR_END_NAMESPACE
