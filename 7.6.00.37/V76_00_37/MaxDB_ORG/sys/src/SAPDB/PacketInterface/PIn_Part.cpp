/*!================================================================
 module:        PIn_Part.cpp

 -------------------------------------------------------------------

 responsible:   D030044

 special area:  SQL

 description:   Part

 -------------------------------------------------------------------

 Copyright (c) 2000-2005 SAP AG




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

#include "hsp77.h"
#include "SAPDB/PacketInterface/PIn_Part.h"
#include "SAPDB/SAPDBCommon/SAPDB_UTF8Basis.hpp"

/*----------------------------------------*/

PIn_Part::PIn_Part (tsp1_part * partArg)
{
  this->rawPart = partArg;
}

/*----------------------------------------*/


/*----------------------------------------*/

void PIn_Part::Init (tsp1_part_kind_Param partKind, int segmOffset, int segmSize)
{
  tsp1_part_header * header = &this->rawPart->sp1p_part_header ();
  
  header->sp1p_part_kind.becomes (partKind);
  header->sp1p_attributes.clear ();
  header->sp1p_arg_count = 0;
  header->sp1p_segm_offset = segmOffset;
  header->sp1p_buf_len = 0;
  header->sp1p_buf_size = segmSize - segmOffset - sizeof (tsp1_part_header);
}

/*----------------------------------------*/

const teo00_Byte * PIn_Part::GetReadData (int pos) const
{
  return REINTERPRET_CAST (teo00_Byte*, this->rawPart->sp1p_buf ().asCharp () + pos);
}

/*----------------------------------------*/

const teo00_Byte * PIn_Part::GetParameterData (int pos) const
{
  const teo00_Byte * dataBuf;
  const teo00_Byte * result;
  
  /*
  * get pointer
  */
  dataBuf = REINTERPRET_CAST (const teo00_Byte* , this->rawPart->sp1p_buf ().asCharp ());
  /*
  * check for null value
  */
  if (dataBuf [pos - 1] == csp_undef_byte) {
    result = NULL;
  }
  else {
    result = dataBuf + pos;
  }
  /*
  * return
  */
  return result;
}

/*----------------------------------------*/

teo00_Byte * PIn_Part::GetWriteData ()
{
  return REINTERPRET_CAST (teo00_Byte*, this->rawPart->sp1p_buf ().asCharp ())
    + this->rawPart->sp1p_buf_len ();
}

/*----------------------------------------*/

void PIn_Part::ExtendLength (int increment)
{
  PRECONDITION_DBG_MSP00 (this->rawPart->sp1p_buf_len () + increment < this->rawPart->sp1p_buf_size ());
  this->rawPart->sp1p_buf_len () += increment;
}

/*----------------------------------------*/

void PIn_Part::AddData (const void * buf, int len)
{
  PRECONDITION_DBG_MSP00 (this->rawPart->sp1p_buf_len () + len < this->rawPart->sp1p_buf_size ());
  memcpy (this->rawPart->sp1p_buf () + this->rawPart->sp1p_buf_len (), buf, len);
  this->rawPart->sp1p_buf_len () += len;
}

/*----------------------------------------*/

void PIn_Part::AddDataAsciiToUCS2 (const void * buf, int len, int swapped)
{
  PRECONDITION_DBG_MSP00 (this->rawPart->sp1p_buf_len () + len * 2 < this->rawPart->sp1p_buf_size ());
  unsigned int converted;
  sp81ASCIItoUCS2((tsp81_UCS2Char*)(this->rawPart->sp1p_buf () + this->rawPart->sp1p_buf_len ()), 
    len , swapped, &converted, (const unsigned char*)buf, len );
  this->rawPart->sp1p_buf_len () += len * 2;
}

/*----------------------------------------*/
void PIn_Part::AddDataUTF8ToUCS2 (const void * pSource, int nSource, int swapped)
{
  PRECONDITION_DBG_MSP00 (this->rawPart->sp1p_buf_len () + nSource * 2 < this->rawPart->sp1p_buf_size ());

  SAPDB_UTF8Basis::UTF8ConstPointer srcBeg  = (SAPDB_UTF8Basis::UTF8ConstPointer) pSource;
  SAPDB_UTF8Basis::UTF8ConstPointer srcEnd  = (SAPDB_UTF8Basis::UTF8ConstPointer) (((tsp00_Byte *)pSource) + nSource);
  SAPDB_UTF8Basis::UTF8ConstPointer srcAt   = NULL;

  tsp81_UCS2Char *                  destBeg = (tsp81_UCS2Char *)                  (this->rawPart->sp1p_buf () + this->rawPart->sp1p_buf_len ());
  tsp81_UCS2Char *                  destEnd = (tsp81_UCS2Char *)                  (this->rawPart->sp1p_buf () + this->rawPart->sp1p_buf_size ());
  tsp81_UCS2Char *                  destAt  = NULL;

  SAPDB_UTF8Basis::KernelConvertToUTF16(srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt, swapped ? true : false);

  this->rawPart->sp1p_buf_len () += (tsp00_Int4)  (((tsp00_Byte *) destAt) - ((tsp00_Byte *)destBeg));
}


void PIn_Part::AddDataUTF8ToUCS2 (const void * pSource, int nSource)
{
  PRECONDITION_DBG_MSP00 (this->rawPart->sp1p_buf_len () + nSource * 2 < this->rawPart->sp1p_buf_size ());

  short swapTest = 1;

  SAPDB_UTF8Basis::UTF8ConstPointer srcBeg  = (SAPDB_UTF8Basis::UTF8ConstPointer) pSource;
  SAPDB_UTF8Basis::UTF8ConstPointer srcEnd  = (SAPDB_UTF8Basis::UTF8ConstPointer) (((tsp00_Byte *)pSource) + nSource);
  SAPDB_UTF8Basis::UTF8ConstPointer srcAt   = NULL;

  tsp81_UCS2Char *                  destBeg = (tsp81_UCS2Char *)                  (this->rawPart->sp1p_buf () + this->rawPart->sp1p_buf_len ());
  tsp81_UCS2Char *                  destEnd = (tsp81_UCS2Char *)                  (this->rawPart->sp1p_buf () + this->rawPart->sp1p_buf_size ());
  tsp81_UCS2Char *                  destAt  = NULL;

  SAPDB_UTF8Basis::KernelConvertToUTF16(srcBeg, srcEnd, srcAt, destBeg, destEnd, destAt, ((*(char *)&swapTest) ? true : false));

  this->rawPart->sp1p_buf_len () += (tsp00_Int4)  (((tsp00_Byte *) destAt) - ((tsp00_Byte *)destBeg));
}

/*----------------------------------------*/

void PIn_Part::AddDataUCS2ToAscii (const void * buf, int len, int swapped)
{
  PRECONDITION_DBG_MSP00 (this->rawPart->sp1p_buf_len () + len < this->rawPart->sp1p_buf_size ());
  unsigned int converted;
  sp81UCS2toASCII((unsigned char*)(this->rawPart->sp1p_buf () + this->rawPart->sp1p_buf_len ()), 
    len , &converted, (tsp81_UCS2Char*)buf, len, swapped );
  this->rawPart->sp1p_buf_len () += len;
}
/*----------------------------------------*/

void PIn_Part::AddArgument (const void * buf, int len)
{
  if (buf != NULL) {
    this->AddData (buf, len);
  }
  ++this->rawPart->sp1p_part_header ().sp1p_arg_count;
}

/*----------------------------------------*/

void PIn_Part::AddArgument ()
{
  ++this->rawPart->sp1p_part_header ().sp1p_arg_count;
}

/*----------------------------------------*/

void PIn_Part::AddParameterArg (
                                   const void * buf,
                                   int pos,
                                   int len,
                                   int sqllen,
                                   char typechar)
{
  char * dataBuf;
  int copyLen;
  int padLen;
  
  --sqllen; // adjust for defined byte
            /*
            * get pointer
  */
  dataBuf = this->rawPart->sp1p_buf ().asCharp ();
  /*
  * check for null value
  */
  if (buf == NULL) {
    dataBuf [pos - 1] = (char) csp_undef_byte; // STATIC_CAST (char, csp_undef_byte);
    IFSLOW_MSP00 (memset (dataBuf + pos, 0xff, sqllen));
  }
  else {
    dataBuf [pos - 1] = typechar;
    /*
    * copy data
    */
    if (len < sqllen) {
      copyLen = len;
      padLen = sqllen - len;
    }
    else {
      copyLen = sqllen;
      padLen = 0;
    }
    memcpy (dataBuf + pos, buf, copyLen);
    if (padLen > 0) {
      memset (dataBuf + pos + copyLen, typechar, padLen);
    }
  }
  /*
  * adjust part header
  */
  this->rawPart->sp1p_buf_len () =
    MAX_EO00 (this->rawPart->sp1p_buf_len (), pos + sqllen);
  ++this->rawPart->sp1p_arg_count ();
}

/*----------------------------------------*/

void PIn_Part::AddParameterArgUCS2 (const void * buf, 
                                       int pos, 
                                       int len,
                                       int isAsciiInput,
                                       int isAsciiOutput,
                                       int swapped,
                                       int sqllen, 
                                       tsp81_UCS2Char typechar)
{
  char * dataBuf;
  int copyLen;
  int padLen;
  
  --sqllen; // adjust for defined byte
            /*
            * get pointer
  */
  dataBuf = this->rawPart->sp1p_buf ().asCharp ();
  /*
  * check for null value
  */
  if (buf == NULL) {
    dataBuf [pos - 1] = (char) csp_undef_byte; // STATIC_CAST (char, csp_undef_byte);
    IFSLOW_MSP00 (memset (dataBuf + pos, 0xff, sqllen));
  }
  else {
    dataBuf [pos - 1] = csp_unicode_def_byte;
    /*
    * copy data
    */
    if ( isAsciiInput ) {
      if ( isAsciiOutput ) {
        if (len < sqllen) {
          copyLen = len;
          padLen = sqllen - len;
        }
        else {
          copyLen = sqllen;
          padLen = 0;
        }
        memcpy (dataBuf + pos, buf, copyLen);
      }
      else {
        unsigned int converted;
        sp81ASCIItoUCS2 ((tsp81_UCS2Char*)(dataBuf + pos), sqllen / 2, swapped,
          &converted, (const unsigned char*)buf, len );
        copyLen = 2 * converted;
        padLen = sqllen - copyLen;
      }
    }
    else {
      if ( isAsciiOutput ) {
        unsigned int converted;
        sp81UCS2toASCII((tsp00_Byte*)(dataBuf + pos), sqllen, &converted,
          (tsp81_UCS2Char*)buf, len, swapped );
        copyLen = converted;
        padLen = sqllen - copyLen;
      }
      else {
        len = len * 2;
        if (len < sqllen) {
          copyLen = len;
          padLen = sqllen - copyLen;
        }
        else {
          copyLen = sqllen;
          padLen = 0;
        }
        memcpy (dataBuf + pos, buf, copyLen);
      }
    }
    while (padLen > 0) {
      if ( isAsciiOutput ) {
        if (padLen > 0) {
          memset (dataBuf + pos + copyLen, ' ', padLen);
        }
      }
      else
        sp81UCS2strncpy ((tsp81_UCS2Char*)(dataBuf + pos + copyLen), &typechar, 1);
      copyLen = copyLen + 2;
      padLen  = padLen - 2;
    }
  }
  /*
  * adjust part header
  */
  this->rawPart->sp1p_buf_len () =
    MAX_EO00 (this->rawPart->sp1p_buf_len (), pos + sqllen);
  ++this->rawPart->sp1p_arg_count ();
}

/*----- PTS 1119960 ----------------------*/

void PIn_Part::AdjArgCntForMass(short sqln) 
{
  //this->rawPart->sp1p_arg_count () = this->rawPart->sp1p_arg_count () / sqln;
  this->rawPart->sp1p_arg_count () = sqln;   
}

/*----- PTS 1119960 ----------------------*/

void PIn_Part::setAttributes(const tsp1_part_attr attrib)
{
  this->rawPart->sp1p_attributes () = attrib;
}

/*----------------------------------------*/

typedef void partDumper_FT (
                            PIn_TraceStream * trace,
                            int argCount,
                            int len,
                            const teo00_Byte * data,
                            int buflimit);

typedef struct PartDumpInfoT {
  const char * name;
  partDumper_FT * func;
} PartDumpInfoT;

/*----------------------------------------*/

static void PIn_StdDump (
                           PIn_TraceStream * trace,
                           int argCount,
                           int len,
                           const teo00_Byte * data,
                           int buflimit)
{
  if (buflimit > 0) {
    len = MIN_EO00 (len, buflimit);
  }
  if (trace != NULL) {
    trace->hex (NULL, data, len);
  }
}

/*----------------------------------------*/

static PartDumpInfoT dumpInfoC [] = {
  {"nil", PIn_StdDump},
  {"appl_parameter_description", PIn_StdDump},
  {"columnnames", PIn_StdDump},
  {"command", PIn_StdDump},
  {"conv_tables_returned", PIn_StdDump},
  {"data", PIn_StdDump},
  {"errortext", PIn_StdDump},
  {"getinfo", PIn_StdDump},
  {"modulname", PIn_StdDump},
  {"page", PIn_StdDump},
  {"parsid", PIn_StdDump},
  {"parsid_of_select", PIn_StdDump},
  {"resultcount", PIn_StdDump},
  {"resulttablename", PIn_StdDump},
  {"shortinfo", PIn_StdDump},
  {"user_info_returned", PIn_StdDump},
  {"surrogate", PIn_StdDump},
  {"bdinfo", PIn_StdDump},
  {"longdata", PIn_StdDump},
  {"tablename", PIn_StdDump},
  {"session_info_returned", PIn_StdDump},
  {"output_cols_no_parameter", PIn_StdDump},
  {"key", PIn_StdDump},
  {"serial", PIn_StdDump},
  {"relative_pos", PIn_StdDump},
  {"abap_istream", PIn_StdDump},
  {"abap_ostream", PIn_StdDump},
  {"abap_info", PIn_StdDump},
  {"checkpoint_info", PIn_StdDump},
  {"procid", PIn_StdDump},
  {"long_demand", PIn_StdDump},
  {"message_list", PIn_StdDump},
  {"vardata_shortinfo", PIn_StdDump},
  {"vardata", PIn_StdDump},
  {"feature", PIn_StdDump},
};

/*----------------------------------------*/

void PIn_Part::TraceOn (PIn_TraceStream * trace, int buflimit)
{
  tsp1_part_header * header = &this->rawPart->sp1p_part_header ();
  tsp1_part_kind_Enum partKind = header->sp1p_part_kind;
  PartDumpInfoT * dumpInfo;
  char tmpBuf [100];
  PartDumpInfoT tmpInfo;
  
  /*
  * get dump function
  */
  if ((sp1pk_nil <= partKind) && (partKind < sizeof(dumpInfoC)/sizeof(dumpInfoC[0]))) {
    dumpInfo = &dumpInfoC [partKind];    
  }
  else {
    sp77sprintf (tmpBuf, sizeof (tmpBuf), "Unknown Part %d", partKind);
    tmpInfo.name = tmpBuf;
    tmpInfo.func = PIn_StdDump;
    dumpInfo = &tmpInfo;
  }
  if (trace != NULL) {
    /*
     * dump header
     */
    trace->printf ("        %s Part %d Arguments (%d of %d bytes) (Segment at %d)\n",
		   dumpInfo->name, header->sp1p_arg_count, header->sp1p_buf_len,
		   header->sp1p_buf_size, header->sp1p_segm_offset);
    /*
     * call dump function
     */
    const void * raw = this->rawPart->sp1p_buf ();
    const teo00_Byte * data = REINTERPRET_CAST (const teo00_Byte *, raw);
    dumpInfo->func (trace, header->sp1p_arg_count, header->sp1p_buf_len,
		    data, buflimit);
  }
}

/*----------------------------------------*/

