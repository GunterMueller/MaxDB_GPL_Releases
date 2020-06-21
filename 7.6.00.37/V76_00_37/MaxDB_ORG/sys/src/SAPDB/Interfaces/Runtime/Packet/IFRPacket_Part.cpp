/*!
  @file           IFRPacket_Part.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Packet
  @brief          Various methods for specific data in parts.
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

// Prevents a Warning 5004 Uninitialized variable "static_i"
// from the HP ANSI C++ B3910B X.03.33.01 compiler.
// See http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1130907 - pragma not longer necessary
// #ifdef HPUX
// #pragma OPT_LEVEL 1
// #endif

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_Environment.h"
#include "Interfaces/Runtime/IFR_Connection.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_BinaryToHex.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Factory.h"

IFR_BEGIN_NAMESPACE

#define DBUG_NULL_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, (IFR_TraceStream *)0)

//----------------------------------------------------------------------
IFR_Int2
IFRPacket_Part::getInt2(IFR_Int4 position) const
{
    IFR_Int2 i2;
    const char *p=(const char *) this->GetRawHeader();
    memcpy(&i2, p + position, sizeof(IFR_Int2));
    return i2;
}

//----------------------------------------------------------------------
void 
IFRPacket_Part::setPartAttribute(int attribute)
{
    char *p=(char *) this->GetRawHeader();
    ++p; // !! attribute is at offset 1 !!!
    *p |= (char)attribute;
    return;
}

//----------------------------------------------------------------------
IFR_Int1
IFRPacket_Part::getInt1(IFR_Int4 position) const
{
    IFR_Int1 i1;
    const char *p=(const char *) this->GetRawHeader();
    memcpy(&i1, p + position, sizeof(IFR_Int1));
    return i1;
}

//----------------------------------------------------------------------
IFR_Int4
IFRPacket_Part::getInt4(IFR_Int4 position) const 
{
    IFR_Int4 i4;
    const char *p=(const char *) this->GetRawHeader();
    memcpy(&i4, p + position, sizeof(IFR_Int4));
    return i4;
}
    
//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_Part::getText(IFR_String& text, IFR_Bool& memory_ok, IFR_Bool append)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_Part, getText);
    if(!memory_ok || !isValid()) {
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }

    const teo00_Byte *data=GetReadData();
    IFR_Int4 len=getBufferLength();
    DBUG_PRINT(len);
    if(getPartArguments()>0 && len>0 && data) {
        if(append) {
            text.append((char*)data, m_encoding, len, memory_ok);
        } else {
            text.setBuffer((char*)data, len, m_encoding, memory_ok);
        }
        if(!memory_ok) {
            DBUG_RETURN(IFR_NO_DATA_FOUND);
        }  
        DBUG_RETURN(IFR_OK);
    } else {
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }
}   

//----------------------------------------------------------------------
const char *
IFRPacket_Part::getPartKindName(IFR_Int4 partkind)
{
    switch(partkind) {
#define IFR_CASE(x) case (x) : return #x
        IFR_CASE(sp1pk_nil);
        IFR_CASE(sp1pk_appl_parameter_description);
        IFR_CASE(sp1pk_columnnames);
        IFR_CASE(sp1pk_command);
        IFR_CASE(sp1pk_conv_tables_returned);
        IFR_CASE(sp1pk_data);
        IFR_CASE(sp1pk_errortext);
        IFR_CASE(sp1pk_getinfo);
        IFR_CASE(sp1pk_modulname);
        IFR_CASE(sp1pk_page);
        IFR_CASE(sp1pk_parsid);
        IFR_CASE(sp1pk_parsid_of_select);
        IFR_CASE(sp1pk_resultcount);
        IFR_CASE(sp1pk_resulttablename);
        IFR_CASE(sp1pk_shortinfo);
        IFR_CASE(sp1pk_user_info_returned);
        IFR_CASE(sp1pk_surrogate);
        IFR_CASE(sp1pk_bdinfo);
        IFR_CASE(sp1pk_longdata);
        IFR_CASE(sp1pk_tablename);
        IFR_CASE(sp1pk_session_info_returned);
        IFR_CASE(sp1pk_output_cols_no_parameter);
        IFR_CASE(sp1pk_key);
        IFR_CASE(sp1pk_serial);
        IFR_CASE(sp1pk_relative_pos);
        IFR_CASE(sp1pk_abap_istream);
        IFR_CASE(sp1pk_abap_ostream);
        IFR_CASE(sp1pk_abap_info);
        IFR_CASE(sp1pk_checkpoint_info);
        IFR_CASE(sp1pk_procid);
        IFR_CASE(sp1pk_long_demand);
        IFR_CASE(sp1pk_message_list);
        IFR_CASE(sp1pk_vardata_shortinfo);
        IFR_CASE(sp1pk_vardata);
        IFR_CASE(sp1pk_feature);
        IFR_CASE(sp1pk_clientid);
    default:
        return "unknown/invalid";
    };
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ErrorTextPart::setErrorText(IFR_ErrorHndl& error)
{
    if(!isValid()) {
        return IFR_NOT_OK;
    }
    
    const char *errortext = error.getErrorText();
    IFR_size_t  errortext_length = strlen(errortext);
    
    if(getRemainingBytes() >= errortext_length) {
        AddData(errortext, (IFR_Int4)errortext_length);    
        GetRawPart()->sp1p_part_header().sp1p_arg_count = 1;
        return IFR_OK;
    } else {
        return IFR_NOT_OK;
    }
}


//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ParseIDPart::getParseID(IFR_ParseID& parseid) const
{
  DBUG_NULL_METHOD_ENTER(IFRPacket_ParseIDPart,getParseID);
    if(!isValid()) {
        return IFR_NO_DATA_FOUND;
    }

    const teo00_Byte *data=GetReadData();
    if(getPartArguments()>0 && getBufferLength()==IFR_ParseID_Size && data) {
        parseid.setParseID(data);
        return IFR_OK;
    } else {
        return IFR_NO_DATA_FOUND;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ShortInfoPart::getShortInfos(IFR_ShortInfo* shortinfo)
{
  DBUG_NULL_METHOD_ENTER(IFRPacket_ShortInfoPart,getShortInfos);
    IFR_Int4 args;
    if(!isValid() || (args = getPartArguments())==0) {
        return IFR_NO_DATA_FOUND;
    }
    char *p=(char*) GetReadData();
    for(IFR_Int4 i=0; i<args; ++i,p+=12) {
        memcpy(&shortinfo[i], p, 12);
    }
#if 0
    // detect unicode columns and update the encoding for this
    IFR_StringEncoding encoding = this->getEncoding();
    if (encoding == IFR_StringEncodingAscii) {
      encoding = (IFR_EnvironmentSwapKind == SwapNormal) ? IFR_StringEncodingUCS2 : IFR_StringEncodingUCS2Swapped;
      for(IFR_Int4 i=0; i<args; ++i) {      
        if (shortinfo[i].isUnicode())          
          shortinfo[i].encoding = encoding;
        else
          shortinfo[i].encoding = IFR_StringEncodingAscii;
      }      
    }
    else {
      for(IFR_Int4 i=0; i<args; ++i) {      
        shortinfo[i].encoding = encoding;
      }
    }
#endif
    return IFR_OK;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ShortInfoPart::parseShortFields(IFRConversion_ConverterList& convList,
                                          IFR_ConnectionItem& clink,
                                          IFR_Bool ignoreOutput)
{
  DBUG_CONTEXT_METHOD_ENTER(IFRPacket_ShortInfoPart, parseShortFields, &clink);
  DBUG_PRINT(ignoreOutput);
  

  IFR_Int4 colcnt;
  if(!isValid() || (colcnt = getPartArguments())==0) {
      DBUG_RETURN(IFR_NO_DATA_FOUND);
  }
  SAPDBMem_IRawAllocator& allocator=clink.getConnection()->allocator;
  IFR_ShortInfo *si=new IFR_ALLOCATOR(allocator) IFR_ShortInfo[colcnt];
  if(si == 0) {
      clink.error().setMemoryAllocationFailed();
      DBUG_RETURN(IFR_NOT_OK);
  }
  getShortInfos(si);
  IFR_Bool memory_ok = true;
  for(IFR_Int4 i=0; i<colcnt; ++i) {
      if(ignoreOutput && si[i].isOutput()) {
          continue;
      }
      IFRConversion_Converter *c = 
          IFRConversion_Factory::createInstance(si, i,
                                                *(clink.getConnection()),
                                                m_encoding != IFR_StringEncodingAscii,
                                                clink.error());
      if(c != 0) {
          convList.add(c, memory_ok);          
      }
      if(clink.error()) {
          if(c != 0) {
              IFRUtil_Delete(c, allocator);
          }
          convList.clear();
          IFRUtil_DeleteArray(si, colcnt, IFR_ALLOCATOR(allocator));
          DBUG_RETURN(IFR_NOT_OK);
      }
      if(c == 0 || !memory_ok) {
          if(c != 0) {
              IFRUtil_Delete(c, allocator);
          }
          convList.clear();
          IFRUtil_DeleteArray(si, colcnt, IFR_ALLOCATOR(allocator));
          clink.error().setMemoryAllocationFailed();
          DBUG_RETURN(IFR_NOT_OK);
      } 
  } 
  IFRUtil_DeleteArray(si, colcnt, IFR_ALLOCATOR(allocator));
  DBUG_RETURN(IFR_OK);  
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_CommandPart::setText(const IFR_String &text, IFR_ErrorHndl &error)
{
  DBUG_NULL_METHOD_ENTER(IFRPacket_CommandPart, setText);
    IFR_Retcode rc=addText(text, error);
    if(rc==IFR_OK) {
        AddArgument();
    }
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_CommandPart::addText(const IFR_String& text, IFR_ErrorHndl &error)
{
  DBUG_NULL_METHOD_ENTER(IFRPacket_CommandPart, addText);
  DBUG_PRINT(text);
    if(!isValid()) {
        return IFR_NOT_OK;
    }

    IFR_Retcode rc=IFR_OK;
    IFR_StringEncoding text_encoding=text.getEncoding();
    IFR_StringEncoding encoding=getEncoding();
    IFR_size_t text_length=text.getLength();
    
    if(encoding == text_encoding) {
        if(getRemainingBytes() < text_length) {
          error.setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
          rc = IFR_DATA_TRUNC;
        } else {
            AddData(text.getBuffer(), (IFR_Int4)text_length);
        } 
        return rc;
     }

    // command encoding is ascii, source encoding is something other.
    if(encoding == IFR_StringEncodingAscii) {
        if(getRemainingBytes() < text_length) {
          error.setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
          rc = IFR_DATA_TRUNC;
        } else {
            if(text_encoding == IFR_StringEncodingUCS2 ||
               text_encoding == IFR_StringEncodingUCS2Swapped) {
              AddDataUCS2ToAscii(text.getBuffer(),
                                 text.getStrLen(),
                                 text_encoding == IFR_StringEncodingUCS2 ? 0 : 1);
            } else { 
                const char *p = text.getBuffer();
                IFR_Bool onlyAscii = true;
                for(IFR_size_t i=0; i<text_length; ++i) {
                    if(p[i] & 0x80) {
                        onlyAscii = false;
                        break;
                    }
                }
                if(onlyAscii) {
                    if(getRemainingBytes() < text_length) {
                      error.setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
                      rc = IFR_DATA_TRUNC;
                    } else {
                        AddData(text.getBuffer(), (IFR_Int4)text_length);
                    } 
                    return rc;
                } else {
                  error.setRuntimeError(IFR_ERR_NOT_IMPLEMENTED("UTF8 to ASCII conversion"));
                  return IFR_NOT_OK;
                }
            }
        }
        return rc;
    } else  if(encoding == IFR_StringEncodingUCS2 || encoding == IFR_StringEncodingUCS2Swapped) {
        if(text.getEncoding() == IFR_StringEncodingAscii) {
            if(getRemainingBytes() < text.getLength() * 2) {
                error.setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
                return IFR_DATA_TRUNC;
            } else {
                AddDataAsciiToUCS2(text.getBuffer(), text.getLength(),
                                   encoding == IFR_StringEncodingUCS2 ? 0 : 1);
            }
        } else if (text_encoding == IFR_StringEncodingUCS2 || text_encoding == IFR_StringEncodingUCS2Swapped) {
            if(getRemainingBytes() < text.getLength()) {
                error.setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
                return IFR_DATA_TRUNC;
            }
            tsp1_part* part = GetRawPart();
            char *d= (part->sp1p_buf() + part->sp1p_buf_len());
            const char *s= text.getBuffer();
            IFR_size_t text_length=text.getLength();
            for(IFR_size_t i=0; i<text_length; i+=2) {
                *d     = *(s + 1);
                *(d+1) = *s;
                d+=2;
                s+=2;
            }
            part->sp1p_buf_len() += text_length;
        } else { // UTF 8 not yet there

            if((IFR_Length)getRemainingBytes() < text.getStrLen() * 2) {
                error.setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
                return IFR_DATA_TRUNC;
            } else {
                AddDataUTF8ToUCS2(text.getBuffer(), text.getLength(),
                                  encoding == IFR_StringEncodingUCS2 ? 0 : 1);
            }

        }
        return rc;
    } else { // command encoding is UTF 8 - not yet implemented
      error.setRuntimeError(IFR_ERR_NOT_IMPLEMENTED("command encoding UTF8"));
      return IFR_NOT_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_CommandInfoPart::addCommandInfo(const IFR_String& text, IFR_Int4 _lineNumber, IFR_ErrorHndl &error)
{
  DBUG_NULL_METHOD_ENTER(IFRPacket_CommandInfoPart, addCommandInfo);
  DBUG_PRINT(text);
  DBUG_PRINT(_lineNumber);
  if(!isValid()) {
    return IFR_NOT_OK;
  }
  char lineNum[12];
  unsigned char text_length = (unsigned char) text.getLength();
  unsigned char lineNumLen  = (unsigned char) sp77sprintf (lineNum, sizeof(lineNum),"%d", _lineNumber);

  IFR_Retcode rc=IFR_OK;
    
  if(getRemainingBytes() < (IFR_UInt4)(text_length+lineNumLen+2)) {
     error.setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
     rc = IFR_DATA_TRUNC;
  } else {
     AddData(&text_length, 1);
     AddData(text.getBuffer(), (IFR_Int4)text_length);
     AddData(&lineNumLen, 1);
     AddData(lineNum, (IFR_Int4)lineNumLen);
     this->setArgCount(2);    
  } 
  return rc;
}
//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ParseIDPart::addParseID(IFR_ParseID& parseid)
{
  DBUG_NULL_METHOD_ENTER(IFRPacket_ParseIDPart, addParseID);
  DBUG_PRINT(parseid);
    if(!isValid()) {
        return IFR_NOT_OK;
    } else {
        AddArgument(parseid.getParseID(), IFR_ParseID_Size);
        return IFR_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ParseIDPart::setParseID(IFR_ParseID& parseid)
{
    if(!isValid()) {
        return IFR_NOT_OK;
    }
    unsigned char *writedata = (unsigned char *)GetReadData(0);
    memcpy(writedata, parseid.getParseID(), IFR_ParseID_Size);
    return IFR_OK;
}


//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_DataPart::addBinaryParameter(const void *buffer,
                                       IFR_Int4 length)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_DataPart, addBinaryParameter);
    DBUG_PRINT(length);
    if(m_variableinput) {
        char *elementstart = GetRawPart()->sp1p_buf().asCharp() + GetRawPart()->sp1p_buf_len();
        char *datastart;
        m_extent = GetRawPart()->sp1p_buf_len();
        if(length > 250) {
            elementstart[0] = (char) 0xFF;
            elementstart[1] = (length >> 8);
            elementstart[2] = (length & 0xFF);
            datastart = elementstart + 3;
        } else {
            elementstart[0] = length;
            datastart = elementstart + 1;
        }
        memcpy(datastart, buffer, length);
        if(length > 250) {
            GetRawPart()->sp1p_buf_len () += length + 3;
        } else {
            GetRawPart()->sp1p_buf_len () += length + 1;
        }
        DBUG_RETURN(IFR_OK);
    } else {
        DBUG_RETURN(IFR_NOT_OK);
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_DataPart::addBinaryParameter(const void *buffer,
                                       IFR_Int4 length,
                                       IFR_ShortInfo &shortinfo)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_DataPart, addBinaryParameter);
    DBUG_PRINT(length);
    if(m_variableinput) {
        char *elementstart = GetRawPart()->sp1p_buf().asCharp() + GetRawPart()->sp1p_buf_len();
        char *datastart;
        m_extent = GetRawPart()->sp1p_buf_len();
        IFR_Int4 maxlen = shortinfo.iolength - 1;
        IFR_Int4 usedlength = maxlen > length ? length : maxlen;
        if(maxlen > 250) {
            elementstart[0] = (char) 0xFF;
            elementstart[1] = (usedlength >> 8);
            elementstart[2] = (usedlength & 0xFF);
            datastart = elementstart + 3;
        } else {
            elementstart[0] = usedlength;
            datastart = elementstart + 1;
        }
        memcpy(datastart, buffer, usedlength);
        if(maxlen > 250) {
            GetRawPart()->sp1p_buf_len () += usedlength + 3;
        } else {
            GetRawPart()->sp1p_buf_len () += usedlength + 1;
        }
        if(usedlength < length) {
            DBUG_RETURN(IFR_DATA_TRUNC);
        } else {
            DBUG_RETURN(IFR_OK);
        }
    } else {
        IFR_Int4 len;
        IFR_Int4 offset=m_massextent;
        char *data = GetRawPart()->sp1p_buf().asCharp();
        char *p    = data + offset + shortinfo.pos.bufpos - 1;
        IFR_UInt2 iolength = shortinfo.iolength;
        IFR_Retcode rc = IFR_OK;
        if(0 == buffer && length != 0) {
            *p = (char) csp_undef_byte;
            // memset(p+1, 0, iolength -1);
        } else {
            *p = shortinfo.getDefinedByte(this->getEncoding()!=IFR_StringEncodingAscii);
            ++p;
            --iolength;
            if(length > iolength) {
                memcpy(p, buffer, iolength);
                rc = IFR_DATA_TRUNC;
            } else {
                memcpy(p, buffer, length);
                memset(p + length, 0, iolength - length);
            }
        }
        len = (IFR_Int4)(shortinfo.pos.bufpos + iolength);
        GetRawPart()->sp1p_buf_len() =  MAX (this->GetRawPart()->sp1p_buf_len (), offset+len);
        // no argument count modification
        return rc;
    }
}


//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_DataPart::addParameter(const void *buffer,
                                 IFR_Int4 length,
                                 IFR_StringEncoding srcEncoding,
                                 IFR_StringEncoding targetEncoding,
                                 IFR_ShortInfo &shortinfo,
                                 IFR_Bool bin2hex)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_DataPart, addParameter);
    DBUG_PRINT(length);
    DBUG_PRINT(srcEncoding);
    
    if(m_variableinput) {
        IFR_Retcode rc = IFR_OK;
        char *elementstart = GetRawPart()->sp1p_buf().asCharp() + GetRawPart()->sp1p_buf_len();
        m_extent = GetRawPart()->sp1p_buf_len();
        char *datastart;
        IFR_UInt4 maxlen = shortinfo.iolength - 1;
        if(maxlen > 250) {
            datastart = elementstart + 3;
            elementstart[0] = (char) 255;
        } else {
            datastart = elementstart + 1;
        }
        const tsp77encoding* target_encoding_sp77 = IFR_ENCODING(targetEncoding);
        const tsp77encoding* source_encoding_sp77 = IFR_ENCODING(srcEncoding);
        IFR_UInt4 destbyteswritten;
        IFR_UInt4 srcbytesparsed;
        tsp78ConversionResult convres;
        if(bin2hex) {
            convres = IFRUtil_HexToBinary(datastart,
                                          maxlen,
                                          &destbyteswritten,
                                          source_encoding_sp77,
                                          buffer, length,
                                          &srcbytesparsed);
        } else {
            convres = sp78convertBuffer(target_encoding_sp77, datastart, maxlen,
                                        &destbyteswritten,
                                        source_encoding_sp77, buffer, length, 
                                        &srcbytesparsed);
        }
        if (convres != sp78_Ok && convres != sp78_TargetExhausted) {
            return IFR_NOT_OK;
        }
        if (convres == sp78_TargetExhausted) {
            rc = IFR_DATA_TRUNC;
        }
        if(maxlen > 250) {
            elementstart[1] = (destbyteswritten >> 8) & 0xFF;
            elementstart[2] = (destbyteswritten & 0xFF);
            GetRawPart()->sp1p_buf_len() += destbyteswritten + 3;
        } else {
            elementstart[0] = (destbyteswritten & 0xFF);
            GetRawPart()->sp1p_buf_len() += destbyteswritten + 1;
        }
        DBUG_RETURN(rc);
    } else {
        IFR_Int4 len;
        IFR_Int4 offset=m_massextent;
        char *data = GetRawPart()->sp1p_buf().asCharp();
        char *p    = data + offset + shortinfo.pos.bufpos - 1;
        IFR_UInt2 iolength = shortinfo.iolength;
        IFR_Retcode rc = IFR_OK;
        if(0 == buffer && length != 0) {
            *p = (char) csp_undef_byte;
        } else {
            *p = shortinfo.getDefinedByte(this->getEncoding()!=IFR_StringEncodingAscii);
            ++p;
            --iolength;
            
            const tsp77encoding* target_encoding_sp77 = IFR_ENCODING(targetEncoding);
            
            IFR_UInt4 destbyteswritten;
            IFR_UInt4 srcbytesparsed;
            tsp78ConversionResult convres;
            if(length != 0) {
                if(bin2hex) {
                    convres = IFRUtil_HexToBinary(p, iolength, &destbyteswritten,
                                                  IFR_ENCODING(srcEncoding), buffer, length, 
                                                  &srcbytesparsed);
                } else {
                    convres = sp78convertBuffer(target_encoding_sp77, p, iolength,
                                                &destbyteswritten,
                                                IFR_ENCODING(srcEncoding), buffer, length, 
                                                &srcbytesparsed);
                }
            } else {
                // fake a conversion result
                convres = sp78_Ok;
                destbyteswritten = 0;
            }
            if (convres != sp78_Ok && convres != sp78_TargetExhausted) {
                return IFR_NOT_OK;
            }
            if (convres == sp78_TargetExhausted) {
                rc = IFR_DATA_TRUNC;
            } else {
                void *f = p+destbyteswritten;
                unsigned int padlen = iolength-destbyteswritten;
                target_encoding_sp77->fillString(&f, 
                                                 &padlen,
                                                 padlen / target_encoding_sp77->terminatorSize, 
                                                 shortinfo.getPaddingCharacter());          
            }
        }
        len = (IFR_Int4)(shortinfo.pos.bufpos + iolength);
        
        GetRawPart()->sp1p_buf_len() =  MAX (this->GetRawPart()->sp1p_buf_len (), offset+len);
        // no argument count modification
        return rc;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_DataPart::appendBinaryToParameter(const void *buffer,
                                            IFR_Int4 length,
                                            IFR_ShortInfo &shortinfo,
                                            IFR_Length& offset)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_DataPart, appendBinaryToParameter);
    DBUG_PRINT(length);
    DBUG_PRINT(offset);
    if(m_variableinput) {
        char *datastart = GetRawPart()->sp1p_buf().asCharp() + m_extent;
        IFR_Int4 maxlength = shortinfo.iolength - 1;
        if(offset >= maxlength) {
            DBUG_RETURN(IFR_DATA_TRUNC);
        }
        char *datapos = datastart + ((maxlength) > 250 ? 3 : 1) + offset;
        if(maxlength - offset < length) {
            memcpy(datapos, buffer, maxlength - offset);
            GetRawPart()->sp1p_buf_len() 
                = MAX( GetRawPart()->sp1p_buf_len(),
                       m_extent + ((maxlength) > 250 ? 3 : 1) + maxlength );
            offset = maxlength;
            if(maxlength > 250) {
                datastart[0] = (char) 255;
                datastart[1] = (char) (maxlength >> 8);
                datastart[2] = (char) (maxlength & 0xFF);
            } else {
                datastart[0] = maxlength;
            }
            DBUG_RETURN(IFR_DATA_TRUNC);
        } else {
            memcpy(datapos, buffer, length);
            offset += length;
            GetRawPart()->sp1p_buf_len() 
                = MAX( GetRawPart()->sp1p_buf_len(),
                       m_extent + ((maxlength) > 250 ? 3 : 1) + offset);
            if(maxlength > 250) {
                datastart[0] = (char) 255;
                datastart[1] = (char) (offset >> 8);
                datastart[2] = (char) (offset & 0xFF);
            } else {
                datastart[0] = offset;
            }
            DBUG_RETURN(IFR_OK);
        }
    } else {
        IFR_Int4 dataoffset=m_massextent;
        char *data = GetRawPart()->sp1p_buf().asCharp();
        char *p    = data + dataoffset + shortinfo.pos.bufpos + offset;
        IFR_UInt2 iolength = shortinfo.iolength-1; // iolength has the defined byte
        IFR_Retcode rc = IFR_OK;
        
        if(offset >= iolength) {
            DBUG_RETURN(IFR_DATA_TRUNC);
        }

        if(iolength-offset < length) {
            memcpy(p, buffer, iolength - offset);
            offset += (iolength - offset);
            rc = IFR_DATA_TRUNC;
        } else {
            memcpy(p, buffer, length);
        offset += length;
        }
        // It is not extended, as an add operation was done previously.
        return rc;
    }
}



//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_DataPart::appendToParameter(const void *buffer,
                                      IFR_Int4 length,
                                      IFR_StringEncoding srcEncoding,
                                      IFR_StringEncoding targetEncoding,
                                      IFR_ShortInfo &shortinfo,
                                      IFR_Length& offset,
                                      IFR_Bool bin2hex)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_DataPart, appendToParameter);
    DBUG_PRINT(length);
    DBUG_PRINT(srcEncoding);
    DBUG_PRINT(offset);
    
    if(m_variableinput) {
            char *datastart = GetRawPart()->sp1p_buf().asCharp() + m_extent;
        IFR_Int4 maxlength = shortinfo.iolength - 1;
        if(offset >= maxlength) {
            DBUG_RETURN(IFR_DATA_TRUNC);
        }
        char *datapos = datastart + ((maxlength) > 250 ? 3 : 1) + offset;
        IFR_UInt4 destbyteswritten;
        IFR_UInt4 srcbytesparsed;
        
        tsp78ConversionResult convres;
        if(bin2hex) {
            convres = IFRUtil_HexToBinary(datapos, maxlength - offset,
                                          &destbyteswritten,
                                          IFR_ENCODING(srcEncoding), buffer, length, 
                                          &srcbytesparsed);
        } else {
            convres = sp78convertBuffer(IFR_ENCODING(targetEncoding), datapos, maxlength - offset,
                                        &destbyteswritten,
                                        IFR_ENCODING(srcEncoding), buffer, length, 
                                        &srcbytesparsed);
        }
        if(convres != sp78_Ok && convres != sp78_TargetExhausted) {
            DBUG_RETURN(IFR_NOT_OK);
        } 
        offset += destbyteswritten;
        // extend the buffer
        GetRawPart()->sp1p_buf_len() 
            = MAX( GetRawPart()->sp1p_buf_len(),
                   m_extent + ((maxlength) > 250 ? 3 : 1) + offset );
        if(maxlength > 250) {
            datastart[0] = (char) 255;
            datastart[1] = (char) (offset >> 8);
            datastart[2] = (char) (offset & 0xFF);
        } else {
            datastart[0] = offset;
        }
        if(convres == sp78_TargetExhausted) {
            DBUG_RETURN(IFR_DATA_TRUNC);
        } else {
            DBUG_RETURN(IFR_OK);
        }
    } else {
        IFR_Int4 dataoffset=m_massextent;
        char *data = GetRawPart()->sp1p_buf().asCharp();
        char *p    = data + dataoffset + shortinfo.pos.bufpos + offset;
        IFR_UInt2 iolength = shortinfo.iolength-1; // iolength has the defined byte
        
        if(offset >= iolength) {
            DBUG_RETURN(IFR_DATA_TRUNC);
        }
        
        IFR_Retcode rc = IFR_OK;
        IFR_UInt4 destbyteswritten;
        IFR_UInt4 srcbytesparsed;
        tsp78ConversionResult convres =
            sp78convertBuffer(IFR_ENCODING(targetEncoding), p, iolength - offset,
                              &destbyteswritten,
                              IFR_ENCODING(srcEncoding), buffer, length, 
                              &srcbytesparsed);
        if (convres != sp78_Ok && convres != sp78_TargetExhausted) {
            return IFR_NOT_OK;
        }
        if (convres == sp78_TargetExhausted) {
            offset += destbyteswritten;
            rc = IFR_DATA_TRUNC;
        } else {
            // No padding, as the first add has padded.
            offset += destbyteswritten;
        }
        
        // It is not extended, as an add operation was done previously.
        return rc;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_DataPart::addNullInput(IFR_ShortInfo& shortinfo)
{
    if(m_variableinput) {
        char *data = GetRawPart()->sp1p_buf().asCharp() + GetRawPart()->sp1p_buf_len();
        *data = (char) 0xFE;
        GetRawPart()->sp1p_buf_len() += 1;
        return IFR_OK;
    } else {
        addData((IFR_Int1)csp_undef_byte, shortinfo.pos.bufpos, shortinfo.iolength);
        return IFR_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_DataPart::addDefaultInput(IFR_ShortInfo& shortinfo)
{
    if(m_variableinput) {
        char *data = GetRawPart()->sp1p_buf().asCharp() + GetRawPart()->sp1p_buf_len();
        *data = (char) 0xFD;
        GetRawPart()->sp1p_buf_len() += 1;
        return IFR_OK;
    } else {
        addData((IFR_Int1)csp_default_byte, shortinfo.pos.bufpos, shortinfo.iolength);
        return IFR_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_DataPart::finishData(IFR_Length length, IFR_ShortInfo& shortinfo)
{
    if(m_variableinput) {
        IFR_UInt4 maxlength = shortinfo.iolength - 1;
        char *data = GetRawPart()->sp1p_buf().asCharp() + GetRawPart()->sp1p_buf_len();
        if(maxlength > 250) {
            data[0] = (char) 255;
            data[1] = (char) (maxlength >> 8);
            data[2] = (char) (maxlength & 0xFF);
            GetRawPart()->sp1p_buf_len() += (3 + maxlength);
        } else {
            data[0] = maxlength;
            GetRawPart()->sp1p_buf_len() += (1 + maxlength);
        }
        return IFR_OK;
    } else {
        IFR_Int4 offset = m_massextent;
        char *data = GetRawPart()->sp1p_buf().asCharp();
        *(data + offset + shortinfo.pos.bufpos -1) = shortinfo.getDefinedByte(this->getEncoding()!=IFR_StringEncodingAscii);
        IFR_Int4 len = shortinfo.pos.bufpos + shortinfo.iolength - 1;
        GetRawPart()->sp1p_buf_len() =  
            MAX (this->GetRawPart()->sp1p_buf_len (), offset+len);
        return IFR_OK;
    }
}

//----------------------------------------------------------------------
void
IFRPacket_DataPart::addData(IFR_Int1 definedbyte,
                            IFR_Int4 position,
                            IFR_Int4 iolength)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_DataPart, addData);
    DBUG_PRINT(definedbyte);
    DBUG_PRINT(position);
    DBUG_PRINT(iolength);
    
    IFR_Int4 len;
    IFR_Int4 offset = m_massextent;
    char *data = GetRawPart()->sp1p_buf().asCharp();
    * (data + offset + position -1) = definedbyte;
    len = position + iolength - 1;

    GetRawPart()->sp1p_buf_len() =  
        MAX (this->GetRawPart()->sp1p_buf_len (), offset+len);
}


//----------------------------------------------------------------------
void
IFRPacket_DataPart::addData(void *buffer,
                            IFR_Int4 position,
                            IFR_Int4 iolength)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_DataPart, addData);
    DBUG_PRINT(buffer);
    DBUG_PRINT(position);
    DBUG_PRINT(iolength);
    
    IFR_Int4 len;
    IFR_Int4 offset = m_massextent;
    char *data = GetRawPart()->sp1p_buf().asCharp();
    char *p    = data + offset + position - 1;
    memcpy(p, buffer, iolength);
    len = position + iolength - 1;

    GetRawPart()->sp1p_buf_len() =  MAX (this->GetRawPart()->sp1p_buf_len (), offset+len);
    return;
}

IFR_Retcode
IFRPacket_ResultCountPart::getResultCount(IFR_Int4& ResCount) const
{
  IFR_Retcode rc = IFR_NO_DATA_FOUND;
  ResCount = 0;
  if(isValid()) {
    IFR_Byte *data = const_cast<IFR_Byte *>(GetReadData());
    if(getPartArguments() && data) {
      IFR_Int4 len=getBufferLength();
      if (len > 0 && *data != csp_undef_byte) {
        data++; // strip defined byte
        rc = IFRUtil_VDNNumber::numberToInt4(data, ResCount, len);
      }
      else
        rc = IFR_OVERFLOW;
    } 
  }
  return rc;
}

IFR_Retcode
IFRPacket_TableNamePart::getResultName(IFR_String& ResName, IFR_Bool& memory_ok) const
{
  IFR_Retcode rc = IFR_NO_DATA_FOUND;
  ResName.setBuffer("", 0, IFR_StringEncodingAscii, memory_ok);
  if(isValid()) {
    const char *data = REINTERPRET_CAST(const char *,GetReadData());
    if(getPartArguments() && data) {
      IFR_Int4 len=getBufferLength();
      if (len > 0) {
        ResName.setBuffer(data, len, this->getEncoding(), memory_ok);
        rc = IFR_OK;
      }
    } 
  }
  return rc;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ResultCountPart::setResultCount(IFR_Int4 resultcount)
{
    char vdn[21];
    static const int iolength = 7;
    tsp1_part *rawpart=GetRawPart();

    // check whether there are enough bytes 
    if(BytesRemaining() < iolength - rawpart->sp1p_buf_len()) {
        return IFR_NOT_OK;
    }
    rawpart->sp1p_part_header().sp1p_arg_count = 1;        
    rawpart->sp1p_buf_len() = iolength;
    memset (vdn, 0, sizeof(vdn));
    vdn[0] = csp_defined_byte;
    IFRUtil_VDNNumber::int4ToNumber(resultcount, (unsigned char *)(vdn + 1), 10); // vdn[0] is the defined byte
    IFR_Retcode rc = IFRUtil_VDNNumber::checkVDNNumber((unsigned char *)(vdn + 1), iolength);
    if (rc == IFR_OK) {
      memcpy(rawpart->sp1p_buf(), vdn, iolength);
    }
    return rc;
}

IFR_Retcode
IFRPacket_ResultCountPart::setFetchSize(IFR_Int2 fetchsize)
{
  return this->setResultCount(fetchsize);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ResultCountPart::setUndefResultCount()
{
    static unsigned char vdn [7] = {
        csp_undef_byte, 
        0xFF, 0xFF, 0xFF, 
        0xFF, 0xFF, 0xFF
    };
    tsp1_part *rawpart=GetRawPart();
    
    // check whether there are enough bytes 
    if(getRemainingBytes() < sizeof(vdn) - rawpart->sp1p_buf_len()) {
        return IFR_NOT_OK;
    }
    rawpart->sp1p_part_header().sp1p_arg_count = 1;    
    rawpart->sp1p_buf_len() = sizeof(vdn);
    memcpy(rawpart->sp1p_buf(), vdn, sizeof(vdn));
    return IFR_OK;
    
}

//----------------------------------------------------------------------
IFR_Bool
IFRPacket_DataPart::hasRoomFor(IFR_UInt4 bytes)
{
    // !!! hack !!! hasRoomFor must go to the segment, or the
    // packet, and there check and honor the index of the segment,
    // and minimum Size for reply
    
    // the rule is 
    // - minimum size for reply on a 1st segment is the size of
    //   an error text only segment
    //   (256 / 512 ) bytes for text + header
    // - minimum size on 2nd segment is 8K
    // - minimum size on 3rd and up s again like 1st
    // 
    // 
    return getRemainingBytes() >= bytes + 4096;
}


//----------------------------------------------------------------------
void
IFRPacket_DataPart::addEmptyStream(char *longdescriptor, IFR_Bool lastdata)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_DataPart, addEmptyStream);
    IFR_Int1 valmode = lastdata?IFRPacket_LongDescriptor::LastData_C:IFRPacket_LongDescriptor::AllData_C;
    IFR_Int4 valpos_vallen = 0; 
    memcpy(longdescriptor + IFRPacket_LongDescriptor::ValMode_O,
           &valmode, 
           sizeof(IFR_Int1));
    memcpy(longdescriptor + IFRPacket_LongDescriptor::ValPos_O,
           &valpos_vallen, 
           sizeof(IFR_Int4));
    memcpy(longdescriptor + IFRPacket_LongDescriptor::ValLen_O,
           &valpos_vallen, 
           sizeof(IFR_Int4));
    return;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_DataPart::addStreamData(char *&datastart,
                                  char *dataend,
                                  char *longdescriptor,
                                  IFR_StringEncoding sourceencoding,
                                  IFR_StringEncoding targetencoding,
                                  IFR_ConnectionItem& clink,
                                  IFR_Bool bin2hex)
{
    DBUG_NULL_METHOD_ENTER(IFRPacket_DataPart, addStreamData);
    IFR_Retcode rc=IFR_OK;

    IFR_Int4 datasize = getRemainingBytes();

    datasize /=  8;
    datasize *=  8;   
    
    IFR_Int4 longdesc_valpos;
    IFR_Int4 longdesc_vallen;
    memcpy(&longdesc_valpos, longdescriptor + IFRPacket_LongDescriptor::ValPos_O, sizeof(IFR_Int4));
    memcpy(&longdesc_vallen, longdescriptor + IFRPacket_LongDescriptor::ValLen_O, sizeof(IFR_Int4));
 
    // we initialise the VALPOS if it is not, and we will initialise
    // in that case the data part to something.
    if(longdesc_valpos == 0) {
        longdesc_valpos = GetRawPart()->sp1p_buf_len() + 1;
        memcpy(longdescriptor + IFRPacket_LongDescriptor::ValPos_O, &longdesc_valpos, sizeof(IFR_Int4));
        setStreamValMode(longdescriptor, IFRPacket_LongDescriptor::DataPart_C);
    }

    tsp00_Uint4 bytes_written = 0;
    tsp00_Uint4 srcbytes_parsed = 0;

    char *dest = GetRawPart()->sp1p_buf().asCharp() + GetRawPart()->sp1p_buf_len();
    IFR_Int4 source_size = dataend - datastart;
    
    tsp78ConversionResult conv_res;
    if(bin2hex) {
        conv_res = IFRUtil_HexToBinary(dest,
                                       datasize,
                                       &bytes_written,
                                       IFR_ENCODING(sourceencoding),
                                       datastart,
                                       source_size,
                                       &srcbytes_parsed);

    } else {
        conv_res = sp78convertBuffer(IFR_ENCODING(targetencoding),
                                     dest,
                                     datasize,
                                     &bytes_written,
                                     IFR_ENCODING(sourceencoding),
                                     datastart,
                                     source_size,
                                     &srcbytes_parsed);
    }
    switch(conv_res) {
    case sp78_TargetExhausted:
        rc = IFR_DATA_TRUNC;
    case sp78_Ok:
        break;
    default:
        clink.error().setRuntimeError(IFR_ERR_STRING_CONVERSION);
        DBUG_RETURN(IFR_NOT_OK);
    }
    GetRawPart()->sp1p_buf_len() += bytes_written;
    longdesc_vallen += bytes_written;
    memcpy(longdescriptor + IFRPacket_LongDescriptor::ValLen_O, &longdesc_vallen, sizeof(IFR_Int4));
    datastart += srcbytes_parsed;
    
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
void 
IFRPacket_DataPart::setStreamValMode(char *longdescriptor,
                                     IFR_Int1 valmode)
{
    IFRPacket_LongDescriptor::setStreamValMode(longdescriptor, (IFRPacket_LongDescriptor::ValMode) valmode);
    return;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_DataPart::addDescriptor(IFRPacket_LongDescriptor& longdesc)
{
    // We must ensure that the descriptor is inserted *behind* the current data.
    // Otherwise it will be inserted at the beginning of the part.
    moveRecordBase();    
    
    IFR_Int4 datasize = getRemainingBytes() & ~7;
    
    if(datasize < IFRPacket_LongDescriptor::IOLength) {
        return IFR_NOT_OK;
    }
    
    char      *p = GetRawPart()->sp1p_buf().asCharp() + m_massextent;
    *p = 0;
    memcpy(p+1, &longdesc, IFRPacket_LongDescriptor::Size);
    GetRawPart()->sp1p_buf_len() = MAX(GetRawPart()->sp1p_buf_len(), m_massextent + IFRPacket_LongDescriptor::IOLength);
    GetRawPart()->sp1p_part_header().sp1p_arg_count += 1;
    return IFR_OK;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_VarDataPart::nextRow()
{
    if(isValid()) {
        if(m_current_position == 0) {
            m_current_position = (unsigned char *) GetReadData();
            m_rowcolumns = (((IFR_Int2) *m_current_position) << 8) +
                (IFR_Int2) (*m_current_position + 1);
            m_currentcolumn = 0;
            m_current_position += 2;
            return IFR_OK;
        } else {
            char *dummy_data;
            IFR_Length dummy_length;
            while(m_currentcolumn < m_rowcolumns) {
                if(nextField(dummy_data, dummy_length) != IFR_OK) {
                    return IFR_NOT_OK;
                }
            }
            if((m_current_position - GetReadData()) >= getBufferLength()) {
                return IFR_NO_DATA_FOUND;
            }
            m_rowcolumns = (((IFR_Int2) *m_current_position) << 8) +
                (IFR_Int2) (*m_current_position + 1);
            m_currentcolumn = 0;
            m_current_position += 2;
            return IFR_OK;
        }
    } else {
        return IFR_NOT_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_VarDataPart::nextField(char *& data, IFR_Length& datalength)
{
    if(isValid()) {
        datalength = *(m_current_position);
        if(datalength == 255) {
            datalength = (((IFR_Length)(*m_current_position + 1)) << 8)
                + (IFR_Length)(*m_current_position + 2);
            m_current_position += 2;
        } else if(datalength == 254) {
            datalength = IFR_NULL_DATA;
        } 
        ++m_current_position;
        data = (char *)m_current_position;
        if(datalength != IFR_NULL_DATA) {
            m_current_position += datalength;
        }
        ++m_currentcolumn;
        return IFR_OK;
    } else {
        return IFR_NOT_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_LongDataPart::closePutval()
{
    return addDescriptor(IFRPacket_LongDescriptor::Close_LongDescriptor);
}

//----------------------------------------------------------------------
void
IFRPacket_ApplParamPart::addArgument(IFR_SQLType datatype,
                                     IFR_Int1    frac,
                                     IFR_size_t  length)
{
    IFR_size_t currentArguments=(IFR_size_t)getPartArguments();
    char *data = GetRawPart()->sp1p_buf().asCharp() + currentArguments * 4;
    *data=(IFR_Int1)datatype;
    *(data+1)=frac;
    IFR_Int2 used_length = length > MAX_IFR_INT2 ? MAX_IFR_INT2 : (IFR_Int2)length;
    memcpy(data+2, &used_length, sizeof(IFR_Int2));
    GetRawPart()->sp1p_buf_len() =  
        MAX (this->GetRawPart()->sp1p_buf_len (), (IFR_Int4) (currentArguments +1) * 4);
    ++(GetRawPart()->sp1p_part_header().sp1p_arg_count);
    return;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_FeaturePart::requestFeature(IFRPacket_Feature::Feature feature)
{
    IFR_Int2 fcount = getPartArguments();
    char *data = GetRawPart()->sp1p_buf().asCharp();
    for(IFR_Int2 i=0; i<fcount; ++i) {
        if(data[i*2] == (char) feature) {
            return IFR_OK;
        }
    }
    
    if(BytesRemaining() < 2) {
        return IFR_NOT_OK;
    }

    data += (fcount * 2);
    data[0] = (char) feature;
    data[1] = 1;
    GetRawPart()->sp1p_buf_len() =  
        MAX (this->GetRawPart()->sp1p_buf_len (), fcount * 2 + 2);
    ++(GetRawPart()->sp1p_part_header().sp1p_arg_count);
    return IFR_OK;
}


//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_ColumnNamesPart::getColumnNames(IFRUtil_Vector<IFR_String>& columnnames, 
                                          IFR_Bool& memory_ok)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return IFR_NO_DATA_FOUND;
    }
    //>>> MEMCHECK
    if(!isValid()) {
        return IFR_NO_DATA_FOUND;
    }
    IFR_Int2 pa=getPartArguments();
    columnnames.Clear();
    const teo00_Byte *data=GetReadData();    
    for(IFR_Int2 i=0; i<pa; ++i) {
        IFR_String current((const char*)(data+1), 
                           (IFR_UInt4)((IFR_UInt1)*data), 
                           getEncoding(), 
                           columnnames.GetRawAllocator(), 
                           memory_ok);
        if(!memory_ok) {
            return IFR_NO_DATA_FOUND;
        }                                        
        data+=((IFR_UInt1)*data)+1;
        columnnames.InsertEnd(current, memory_ok);
        if(!memory_ok) {
            return IFR_NO_DATA_FOUND;
        }
    }
    return IFR_OK;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_LongDemandPart::addPosParams(const IFR_Bool immediately, 
                                       const IFR_Int4 position, 
                                       const IFR_UInt4 length)
{
  char booldata[2];
  char vdn[21];
  IFR_Retcode rc = IFR_OK;

  const int bool_iolength = 2;
  const int vdn_iolength = 7;
  const int iolength = bool_iolength + 2 * vdn_iolength;
  tsp1_part *rawpart = GetRawPart();
  
  // check whether there are enough bytes 
  if (BytesRemaining () < iolength) {
    return IFR_NOT_OK;
  }
  rawpart->sp1p_part_header().sp1p_arg_count += 1;        
  rawpart->sp1p_buf_len() += iolength;

  booldata[0]=0;
  if (immediately) {
    booldata[1] = 1;
  } else {
    booldata[1] = 0;
  }
  memcpy (rawpart->sp1p_buf()+m_extent, booldata, bool_iolength);

  memset (vdn, 0, sizeof (vdn));
  vdn[0] = csp_defined_byte;
  IFRUtil_VDNNumber::int4ToNumber (position, (unsigned char *)(vdn + 1), 10); // vdn[0] is the defined byte
  rc = IFRUtil_VDNNumber::checkVDNNumber((unsigned char *)(vdn + 1), vdn_iolength);
  if (rc == IFR_OK) {
    memcpy(rawpart->sp1p_buf()+m_extent+bool_iolength, vdn, vdn_iolength);
  }
  memset (vdn, 0, sizeof (vdn));
  vdn[0] = csp_defined_byte;
  IFRUtil_VDNNumber::int4ToNumber (length, (unsigned char *)(vdn + 1), 10); // vdn[0] is the defined byte
  rc = IFRUtil_VDNNumber::checkVDNNumber((unsigned char *)(vdn + 1), vdn_iolength);
  if (rc == IFR_OK) {
    memcpy(rawpart->sp1p_buf()+m_extent+bool_iolength+vdn_iolength, vdn, vdn_iolength);
  }

  m_extent = MAX (m_extent, iolength);

  return rc;
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_VarDataShortInfoPart::parseShortFields(IFRConversion_ConverterList& convList,
                                                 IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRPacket_VarDataShortInfoPart, parseShortFields, &clink);
    IFR_Retcode rc = IFRPacket_ShortInfoPart::parseShortFields(convList, clink);
    if(rc == IFR_OK) {
        IFR_Bool memory_ok = true;
        convList.buildIndex(memory_ok);
        if(!memory_ok) {
            clink.error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
class IFR_TraceStream;
IFR_TraceStream& operator << (IFR_TraceStream& s, const enum IFRPacket_PartKind::PartKind p )
{
  switch(p) {
#define IFRCASE(x) case(IFRPacket_PartKind::x):{s << #x;break;}
    IFRCASE(Nil_C);
    IFRCASE(ApplParameterDescription_C);
    IFRCASE(Columnnames_C);
    IFRCASE(Command_C);
    IFRCASE(ConvTablesReturned_C);
    IFRCASE(Data_C);
    IFRCASE(Errortext_C);
    IFRCASE(Getinfo_C);
    IFRCASE(Modulname_C);
    IFRCASE(Page_C);
    IFRCASE(Parsid_C);
    IFRCASE(ParsidOfSelect_C);
    IFRCASE(Resultcount_C);
    IFRCASE(Resulttablename_C);
    IFRCASE(Shortinfo_C);
    IFRCASE(UserInfoReturned_C);
    IFRCASE(Surrogate_C);
    IFRCASE(Bdinfo_C);
    IFRCASE(Longdata_C);
    IFRCASE(Tablename_C);
    IFRCASE(SessionInfoReturned_C);
    IFRCASE(OutputColsNoParameter_C);
    IFRCASE(Key_C);
    IFRCASE(Serial_C);
    IFRCASE(AbapIStream_C);
    IFRCASE(AbapOStream_C);
    IFRCASE(AbapInfo_C);
    IFRCASE(LongDemand_C);
    IFRCASE(MessageList_C);
#if IFR_HAS_VARDATA
    IFRCASE(VarDataShortInfo_C);
    IFRCASE(VarData_C);
#endif
#if IFR_HAS_FEATUREPART
    IFRCASE(Feature_C);
#endif
#undef IFRCASE
  default:{s << "(unknown " << (int)p << ")";break;}
  }
  return s;
}

IFR_END_NAMESPACE


