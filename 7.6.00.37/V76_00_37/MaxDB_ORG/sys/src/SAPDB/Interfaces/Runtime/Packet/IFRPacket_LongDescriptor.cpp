/*!
  @file           IFRPacket_LongDescriptor.cpp
  @author         D039759
  @ingroup        IFR_Packet
  @brief          
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

#include "Interfaces/Runtime/Packet/IFRPacket_LongDescriptor.h"

IFR_BEGIN_NAMESPACE

static const char *
ValMode_ToString(const IFR_Int1 valmode)
{
    switch(valmode) {
    case IFRPacket_LongDescriptor::DataPart_C:
        return "(vm_datapart)";
   case IFRPacket_LongDescriptor::AllData_C:
        return "(vm_allpart)";
    case IFRPacket_LongDescriptor::LastData_C:
        return "(vm_lastdata)";
    case IFRPacket_LongDescriptor::NoData_C:
        return "(vm_nodata)";
    case IFRPacket_LongDescriptor::NoMoreData_C:
        return "(vm_nomoredata)";
    case IFRPacket_LongDescriptor::DataTrunc_C:
        return "(vm_datatrunc)";
    case IFRPacket_LongDescriptor::Close_C:
        return "(vm_close)";
    case IFRPacket_LongDescriptor::Error_C:
        return "(vm_error)";
   case IFRPacket_LongDescriptor::StartposInvalid_C:
        return "(vm_startposinvalid)";
    default:
        return "(invalid)";
    }
}

static void
InfoSet_ToString(IFR_Int1 infoset, char *s, IFR_size_t slen)
{
    const char *iset[8];
    const char *isep[8];
    for(int k=0; k<8; ++k) {
        iset[k]=isep[k]="";
    }
    iset[0]="none";
    int i=0;
    if(infoset & IFRPacket_LongDescriptor::ExTrigger_C) {
        iset[i]="ex_trigger";
        ++i;
    }
    if(infoset & IFRPacket_LongDescriptor::WithLock_C) {
        iset[i]="with_lock";
        if(i) isep[i-1]="|";
        ++i;
    }
    if(infoset & IFRPacket_LongDescriptor::NoClose_C) {
        iset[i]="no_close";
        if(i) isep[i-1]="|";
        ++i;
    }
    if(infoset & IFRPacket_LongDescriptor::NewRec_C) {
        iset[i]="new_rec";
        if(i) isep[i-1]="|";
        ++i;
    }
    if(infoset & IFRPacket_LongDescriptor::IsComment_C) {
        iset[i]="is_comment";
        if(i) isep[i-1]="|";
        ++i;
    }
    if(infoset & IFRPacket_LongDescriptor::IsCatalog_C) {
        iset[i]="is_catalog";
        if(i) isep[i-1]="|";
        ++i;
    }
    if(infoset & IFRPacket_LongDescriptor::Unicode_C) {
        iset[i]="unicode";
        if(i) isep[i-1]="|";
        ++i;
    }
    if(infoset & IFRPacket_LongDescriptor::WithoutShortcol) {
        iset[i]="without_shortcol";
        if(i) isep[i-1]="|";
        ++i;
    }
    sp77sprintf(s, slen, "(%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s)",
                iset[0],isep[0],
                iset[1],isep[1],
                iset[2],isep[2],
                iset[3],isep[3],
                iset[4],isep[4],
                iset[5],isep[5],
                iset[6],isep[6],
                iset[7],isep[7]);
    s[slen -1]='\0';
    return;
}


//------------------------------------------------------------------------------
IFR_TraceStream&
operator <<(IFR_TraceStream &s, const IFRPacket_LongDescriptor& descriptor)
{
    char infoset_str[256];
    InfoSet_ToString(descriptor.infoset, infoset_str, 256);

    s << "(descriptor=" << hex << inputlength(8) << descriptor.descriptor << dec 
      << ", tabid=" << hex << inputlength(8) << descriptor.descriptor << dec 
      << ", maxlen=" << descriptor.maxlen 
      << ", internpos=" << descriptor.internpos 
      << ", infoset=" << infoset_str 
      << ", valmode=" << (IFR_Int4)descriptor.valmode << " " << ValMode_ToString(descriptor.valmode)
      << ", valind=" << descriptor.valind 
      << ", valpos=" << descriptor.valpos
      << ", vallen=" << descriptor.vallen << ")";
    return s;
}

//------------------------------------------------------------------------------
IFRPacket_LongDescriptor IFRPacket_LongDescriptor::Close_LongDescriptor = {
	{ 0, 0, 0, 0, 0, 0, 0, 0 },             // descriptor
	{ 0, 0, 0, 0, 0, 0, 0, 0 },             // tab id
	0,                                      // maxlen
	0,                                      // intern_pos
	0,                                      // infoset
	0,                                      // state
	0,                                      // unused
	IFRPacket_LongDescriptor::LastPutval_C, // valmode 
	0,                                      // valind
	0,                                      // unused
	0,                                      // valpos
	0                                       // vallen
};	

//----------------------------------------------------------------------
void
IFRPacket_LongDescriptor::setStreamValMode(char *descriptor, 
                                           IFRPacket_LongDescriptor::ValMode valmode)
{
    // DBUG_METHOD_ENTER(IFRPacket_LongDescriptor, setStreamValMode);
    // DBUG_PRINT(valmode);
    IFR_Int1 int1_valmode=valmode;
    memcpy(descriptor + IFRPacket_LongDescriptor::ValMode_O, 
           &int1_valmode,
           sizeof(IFR_Int1));

    // Tracing disabled.
    // IFRPacket_LongDescriptor longdesc_trace;
    // memcpy(&longdesc_trace, descriptor, sizeof(IFRPacket_LongDescriptor));
    // DBUG_TRACE << "SET NEW VALMODE: " << longdesc_trace << endl;

    return;
}
	
IFR_END_NAMESPACE	
	
	
	
	
