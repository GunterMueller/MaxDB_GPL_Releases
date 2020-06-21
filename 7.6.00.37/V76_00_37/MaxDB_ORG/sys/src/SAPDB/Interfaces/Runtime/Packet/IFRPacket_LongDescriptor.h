/*!
  @file           IFRPacket_LongDescriptor.h
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
#ifndef IFRPACKET_LONGDESCRIPTOR_H
#define IFRPACKET_LONGDESCRIPTOR_H

#include "SAPDB/SAPDBCommon/SAPDB_ReuseLib.h"
#include "Interfaces/Runtime/IFR_Common.h"

IFR_BEGIN_NAMESPACE

/**
 * @ingroup IFR_Packet
 * @brief Structure for the LONG descriptor.
 * This is copied from code in <code>gsp00</code>.
 */
struct IFRPacket_LongDescriptor
{
    /**
     * Offsets within the LONG descriptor.
     */
    enum Offset
    {
        ValMode_O = 27,  //!< Offset of the VALMODE field.
        ValInd_O = 28,   //!< Offset of the VALIND field.
        ValPos_O  = 32,  //!< Offset of the VALPOS field.
        ValLen_O  = 36,  //!< Offset of the VALLEN field.
        Size      = 40,  //!< Total size of LONG descriptor.
        IOLength  = 41   //!< IO Length of LONG descriptor.
    };

    /**
     * Enumeration for the 'infoset' field of a LONG descriptor.
     */
    enum InfoSet {
        ExTrigger_C     =  1,
        WithLock_C      =  2,
        NoClose_C       =  4, //!< Set by application, keep LONG open after reading to the end.
        NewRec_C        =  8,
        IsComment_C     = 16,
        IsCatalog_C     = 32,
        Unicode_C       = 64,
        WithoutShortcol = 128
    };

    /**
     * Status Information about the data.
     */
    enum ValMode {
        DataPart_C        = 0, //!< One of several data portions for the LONG column.
        AllData_C         = 1, //!< The complete data of this LONG column.
        LastData_C        = 2, //!< The last data part for this LONG column.
        NoData_C          = 3, //!< No data for this LONG column in this part.
        NoMoreData_C      = 4, //!< ???
        LastPutval_C      = 5, //!< Final descriptor - no more data is being sent.
        DataTrunc_C       = 6, //!< In a GETVAL operation, the space reserved in
                               //!< <code>vallen</code> isn't enough for the LONG data.
        Close_C           = 7, //!< The application wants to close the GETVAL in the mid
                               //!< of retrieving data.
        Error_C           = 8, //!< The application wants to rollback a partially
                               //!< done PUTVAL (version 7.4.4 and up)
        StartposInvalid_C = 9  //!< The start position in the long demand part is invalid.
    };

    char       descriptor[8]; //!< Internally used by kernel.
    char       tabid[8];      //!< Internally used by kernel.
    IFR_Int4   maxlen;        //!< The length of the whole LONG when reading.
    IFR_Int4   internpos;     //!< The position that is being read/written next (starting with 1).
    IFR_Int1   infoset;       //!< Status Information (see <code>InfoSet</code> enumeration).
    IFR_Int1   state;         //!< Internally used by kernel.
    IFR_Int1   unused_1;      //!< Internally used by kernel.
    IFR_Int1   valmode;       //!< Status information about data (see <code>ValMode</code> enumeration).
    IFR_Int2   valind;        //!< Field for private use of the application.
    IFR_Int2   unused_2;      //!< Internally used by the kernel.
    IFR_Int4   valpos;        //!< The position of the data within the current part.
    IFR_Int4   vallen;        //!< The length of the data within the current part.
    
    /**
     * The LONG descriptor that is sent to signal a close.
     */
    static IFRPacket_LongDescriptor Close_LongDescriptor; 

    /**
     * Sets the VALMODE of a descriptor.
     * @param descriptor Pointer to the descriptor.
     * @param valmode The VALMODE to set.
     */
    static void setStreamValMode(char *descriptor, ValMode valmode);

    
};

/**
 * @ingroup IFR_Trace
 * Trace operator
 * @param s The trace stream.
 * @param descriptor The descriptor to trace.
 * @return The stream <code>s</code>.
 */
 IFR_TraceStream& operator << (IFR_TraceStream& s, 
                               const IFRPacket_LongDescriptor& descriptor);

IFR_END_NAMESPACE
                               
#endif 
