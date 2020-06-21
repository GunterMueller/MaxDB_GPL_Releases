/*!
  @file           IFR_ShortInfo.h
  @author         D039759
  @ingroup        IFR_Common
  @brief          Handling of short field infos
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
#ifndef IFR_SHORTINFO_H
#define IFR_SHORTINFO_H

#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "SAPDB/Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/IFR_String.h"
#include "SAPDB/Interfaces/Runtime/IFR_Datatypes.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_Vector.h"

IFR_BEGIN_NAMESPACE

/**
 * A structure which contains the short field info.
 */
class IFR_ShortInfo
{
public:
    struct NewPosInfo {
        IFR_Int2 position;
        IFR_Int1 readonly;
        IFR_Int1 serial;
    };

    IFR_UInt1 mode;              //!< NULL, NOT NULL, DEFAULT, ESCAPE CHARACTER
    IFR_UInt1 iotype;            //!< 0 = IN, 1 = OUT, 2 = INOUT
    IFR_UInt1 datatype;          //!< see <code>IFR_SQLType</code>.
    IFR_UInt1 frac;              //!< number of fractional digits
    IFR_UInt2 length;            //!< logical length (number of chars/digits)
    IFR_UInt2 iolength;          //!< length in bytes
    union {
        IFR_UInt4  bufpos;            //!< position in datapart
        NewPosInfo newpos;            //!< New structured position info
    } pos;
    
    /**
     * Get whether this one is input.
     * @return <code>true</code> if the iotype is IN or INOUT.
     */
    inline IFR_Bool isInput() const
    {
        return iotype==csp_info_input || iotype==csp_info_inout;
    }

    /**
     * Get whether this one is output.
     * @return <code>true</code> if the iotype is OUT or INOUT.
     */
    inline IFR_Bool isOutput() const
    {
        return iotype==csp_info_output || iotype==csp_info_inout;
    }

    /**
     * Gets whether the type is a floating-point number.
     * @return <code>true</code> if the type is a floating-point
     * number, <code>false</code> if it is anything else.
     */
    inline IFR_Bool isFloat() const
    {
        return datatype==IFR_SQLTYPE_VFLOAT ||
            datatype==IFR_SQLTYPE_FLOAT;
    }

    /**
     * Gets whether the type is a unicode column.
     * @return <code>true</code> if the type is a unicode type
     * number, <code>false</code> if it is anything else.
     */
    inline IFR_Bool isUnicode() const
    {
      return datatype==IFR_SQLTYPE_UNICODE ||
        datatype==IFR_SQLTYPE_LONGUNI ||
        datatype==IFR_SQLTYPE_VARCHARUNI ||
        datatype==IFR_SQLTYPE_STRUNI;
    }

    inline IFR_Bool isBinary() const
    {
        return datatype==IFR_SQLTYPE_STRB ||
            datatype==IFR_SQLTYPE_LONGB ||
            datatype==IFR_SQLTYPE_CHB ||
            datatype==IFR_SQLTYPE_VARCHARB;
    }

    /**
     * Copies textual representation into the given string.
     * @param str the string where to put the text
     * @param length the length of the string, here 32 byte is the default values. This length must include the
     * room of the terminator char.
     */
    void toString(char* str, int length=32);

    inline IFR_Byte getPaddingCharacter() const
    {
        switch(datatype) {
        case IFR_SQLTYPE_CHB:
        case IFR_SQLTYPE_VARCHARB:
            return '\0';
        default:
            return ' ';
        }
    }

    /**
     * Gets the 'defined byte' for the data type stored in
     * this short info object.
     * @param forUnicode whether this is queried for an unicode database.
     * @return the value of the defined byte for the datatype.
     */
    inline IFR_Byte getDefinedByte(IFR_Bool forUnicode=false) const
    {
        if(forUnicode) {
            switch(datatype) {
            case IFR_SQLTYPE_CHA:
            case IFR_SQLTYPE_CHE:
            case IFR_SQLTYPE_VARCHARA:
            case IFR_SQLTYPE_VARCHARE:
            case IFR_SQLTYPE_LONGA:
            case IFR_SQLTYPE_LONGE:
            case IFR_SQLTYPE_STRA:
            case IFR_SQLTYPE_STRE:
                return ' ';
            case IFR_SQLTYPE_DATE:
            case IFR_SQLTYPE_TIME:
            case IFR_SQLTYPE_TIMESTAMP:
            case IFR_SQLTYPE_UNICODE:
            case IFR_SQLTYPE_VARCHARUNI:
            case IFR_SQLTYPE_LONGUNI:
            case IFR_SQLTYPE_STRUNI:
                return 1;
            default:
                return 0;
            }
        } else {
            switch(datatype) {
            case IFR_SQLTYPE_CHA:
            case IFR_SQLTYPE_CHE:
            case IFR_SQLTYPE_VARCHARA:
            case IFR_SQLTYPE_VARCHARE:
            case IFR_SQLTYPE_DATE:
            case IFR_SQLTYPE_TIME:
            case IFR_SQLTYPE_TIMESTAMP:
            case IFR_SQLTYPE_LONGA:
            case IFR_SQLTYPE_LONGE:
            case IFR_SQLTYPE_STRA:
            case IFR_SQLTYPE_STRE:
                return ' ';
                // these do not really exist
            case IFR_SQLTYPE_UNICODE:
            case IFR_SQLTYPE_VARCHARUNI:
            case IFR_SQLTYPE_LONGUNI:
            case IFR_SQLTYPE_STRUNI:
                return 1;
            default:
                return 0;
            }
        }
    }

    /**
     * Gets whether this datatype is LONG data.
     */
    inline IFR_Bool isLong() const
    {
        switch(datatype) {
        case IFR_SQLTYPE_LONGUNI:
        case IFR_SQLTYPE_LONGA:
        case IFR_SQLTYPE_LONGE:
        case IFR_SQLTYPE_LONGB:
        case IFR_SQLTYPE_STRA:
        case IFR_SQLTYPE_STRE:
        case IFR_SQLTYPE_STRB:
        case IFR_SQLTYPE_STRUNI:
            return true;
        default:
            return false;
        }
    }


    inline IFR_Bool isABAPStream() const
    {
        return datatype == IFR_SQLTYPE_ABAPTABHANDLE;
    }

    /**
     * Gets whether this datatype is an integral type,
     * i.e. no CHAR, VARCHAR or LONG type.
     */
    inline IFR_Bool isIntegral() const
    {
        return datatype == IFR_SQLTYPE_FIXED ||
            datatype == IFR_SQLTYPE_FLOAT    ||
            datatype == IFR_SQLTYPE_DATE     ||
            datatype == IFR_SQLTYPE_TIME     ||
            datatype == IFR_SQLTYPE_VFLOAT   ||
            datatype == IFR_SQLTYPE_TIMESTAMP||
            datatype == IFR_SQLTYPE_NUMBER   ||
            datatype == IFR_SQLTYPE_SMALLINT ||
            datatype == IFR_SQLTYPE_INTEGER  ||
            datatype == IFR_SQLTYPE_BOOLEAN;
    }


    /**
     * Checks whether this short info is equivalent to
     * another one. Two short infos are equivalent if
     * all fields except 'mode' and 'bufpos' are equal.
     * @param other The short field info to compare this one with.
     * @return <code>true</code> if this and <var>other</var> are equivalent,
     *         <code>false</code> if not.
     */
    inline IFR_Bool isEquivalent(IFR_ShortInfo& other)
    {
        return iotype == other.iotype &&
            datatype == other.datatype &&
            frac == other.frac &&
            length == other.length &&
            iolength == other.iolength;
    }

};

/**
 * @ingroup IFR_Trace
 * Trace operator
 * @param s The trace stream.
 * @param shortinfo The shortinfo to trace.
 * @return The stream <code>s</code>.
 */
IFR_TraceStream& operator << (IFR_TraceStream& s,
                              const IFR_ShortInfo& shortinfo);

IFR_END_NAMESPACE

#endif
