/*!
  @file           IFR_LOB.cpp
  @author         D039759
  @ingroup
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

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_LOB.h"
#include "Interfaces/Runtime/IFR_LOBHost.h"

//----------------------------------------------------------------------
IFR_LOB::IFR_LOB(IFR_Int2 column,
                 IFR_Int4 row,
                 IFR_HostType lobhosttype,
                 IFR_ConnectionItem& clink,
                 IFR_Bool input)
:m_column(column),
 m_row(row),
 m_lobhosttype(lobhosttype),
 m_clink(clink),
 m_getvalhost(0),
 m_putvalhost(0),
 m_status(Status_Valid_C),
 m_position(1)
{
    if(input) {
        m_putvalhost = m_clink.getPutvalHost();
    } else {
        m_getvalhost = m_clink.getGetvalHost();
    }
}

//----------------------------------------------------------------------
IFR_LOB::~IFR_LOB()
{
    // if the LOB is destructed, it is gone ...
    m_status = Status_Closed_C;
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_LOB::putData(void *data, IFR_Length* lengthindicator)
{
    DBUG_CONTEXT_METHOD_ENTER(IFR_LOB,putData, &m_clink);
    if(!assertOpen()) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(m_putvalhost == 0) {
        m_clink.error().setRuntimeError(IFR_ERR_LOB_READONLY_I, (IFR_Int4)getColumn());
        DBUG_RETURN(IFR_NOT_OK);
    } else {
        IFR_Retcode rc = m_putvalhost->putData(this, data, lengthindicator);
        if(rc == IFR_OK) {
            if(m_position != 0) {
                if(lengthindicator) {
                    m_position += *lengthindicator;
                } else {
                    m_position = 0;
                }
            }
        }
        DBUG_RETURN(rc);
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_LOB::getData(void *data,
                 IFR_Length *lengthindicator,
                 IFR_Length bytelength,
                 IFR_Length *posindicator,
                 IFR_Bool    terminate)
{
    DBUG_CONTEXT_METHOD_ENTER(IFR_LOB, getData, &m_clink);
    if(!assertOpen()) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(m_getvalhost == 0) {
        m_clink.error().setRuntimeError(IFR_ERR_LOB_WRITEONLY_I, (IFR_Int4)getColumn());
        DBUG_RETURN(IFR_NOT_OK);
    } else {
        IFR_Retcode rc = m_getvalhost->getData(this, data, lengthindicator, bytelength, posindicator, terminate);
        if(rc != IFR_NOT_OK) {
            if(m_position != 0) {
                if(posindicator) {
                    m_position = *posindicator;
                }
                if(lengthindicator) {
                    if(*lengthindicator < bytelength) {
                        m_position += *lengthindicator;
                    } else {
                        m_position += bytelength;
                        if(terminate) {
                            if(m_lobhosttype == IFR_HOSTTYPE_UCS2_LOB ||
                               m_lobhosttype == IFR_HOSTTYPE_UCS2_SWAPPED_LOB) {
                                if(bytelength >= 2) {
                                    m_position -= 2;
                                }
                            } else {
                                if(bytelength >= 1) {
                                    m_position -= 1;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            m_position = 0;
        }
        DBUG_RETURN(rc);
    }

    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Length
IFR_LOB::getLength()
{
    DBUG_CONTEXT_METHOD_ENTER(IFR_LOB, getLength, &m_clink);
    if(!assertOpen()) {
        DBUG_RETURN(-1);
    }
    if(m_getvalhost == 0) {
        m_clink.error().setRuntimeError(IFR_ERR_LOB_WRITEONLY_I, (IFR_Int4)getColumn());
        DBUG_RETURN(-1);
    } else {
        DBUG_RETURN(m_getvalhost->getLength(this));
    }
    DBUG_RETURN(-1);
}

//----------------------------------------------------------------------
IFR_Length
IFR_LOB::getLengthValue(IFR_Length length,
                        IFR_SQLType sqltype,
                        IFR_Bool binhex)
{
    if(length == -1) {
        return length;
    } else {
        switch(getDataHostType()) {
        case IFR_HOSTTYPE_ASCII: {
            switch(sqltype) {
            case IFR_SQLTYPE_STRB:
            case IFR_SQLTYPE_LONGB:
                if(binhex) {
                    return length * 2;
                }
            case IFR_SQLTYPE_STRA:
            case IFR_SQLTYPE_STRE:
            case IFR_SQLTYPE_LONGE:
            case IFR_SQLTYPE_LONGA:
                return length;
            case IFR_SQLTYPE_LONGUNI:
            case IFR_SQLTYPE_STRUNI:
                return length / 2;
            default:
                return length;
            }
        }
        case IFR_HOSTTYPE_UCS2:
        case IFR_HOSTTYPE_UCS2_SWAPPED: {
            switch(sqltype) {
            case IFR_SQLTYPE_STRB:
            case IFR_SQLTYPE_LONGB:
                if(binhex) {
                    return length * 4;
                } else {
                    return length;
                }
            case IFR_SQLTYPE_STRA:
            case IFR_SQLTYPE_STRE:
            case IFR_SQLTYPE_LONGE:
            case IFR_SQLTYPE_LONGA:
                return length * 2;
            case IFR_SQLTYPE_LONGUNI:
            case IFR_SQLTYPE_STRUNI:
                return length / 2;
            default:
                return length;
            }
        }
        case IFR_HOSTTYPE_BINARY:
            return length;
        default:
        case IFR_HOSTTYPE_UTF8:
            return -1;
        }
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_LOB::close()
{
    DBUG_CONTEXT_METHOD_ENTER(IFR_LOB, close, &m_clink);
    IFR_Retcode rc = IFR_OK;
    if(m_status == Status_Valid_C) {
        if(m_putvalhost == 0) {
            rc = m_getvalhost->closeLOB(this);
        } else {
            rc = m_putvalhost->closeLOB(this);
        }
    }
    m_position = 0;
    m_status   = Status_Closed_C;
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Bool
IFR_LOB::assertOpen(void)
{
    switch(m_status) {
    case Status_Valid_C:
        return true;
    case Status_Closed_C:
        m_clink.error().setRuntimeError(IFR_ERR_LOB_CLOSED_I, (IFR_Int4)getColumn());
        return false;
    case Status_NullOrDefault_C:
        m_clink.error().setRuntimeError(IFR_ERR_LOB_NULLDEFAULT_I, (IFR_Int4)getColumn());
        return false;
    case Status_Invalid_C:
        m_clink.error().setRuntimeError(IFR_ERR_LOB_INVALID_I, (IFR_Int4)getColumn());
        return false;
    }
    return false;
}

//----------------------------------------------------------------------
IFR_HostType
IFR_LOB::getDataHostType() const
{
    switch(m_lobhosttype) {
    case IFR_HOSTTYPE_BLOB:
        return IFR_HOSTTYPE_BINARY;
    case IFR_HOSTTYPE_ASCII_LOB:
        return IFR_HOSTTYPE_ASCII;
    case IFR_HOSTTYPE_UCS2_LOB:
        return IFR_HOSTTYPE_UCS2;
    case IFR_HOSTTYPE_UCS2_SWAPPED_LOB:
        return IFR_HOSTTYPE_UCS2_SWAPPED;
    case IFR_HOSTTYPE_UTF8_LOB:
        return IFR_HOSTTYPE_UTF8;
    default:
        return IFR_HOSTTYPE_PARAMETER_NOTSET;
    }
}

//----------------------------------------------------------------------
void
IFR_LOB::sqlTrace(IFR_TraceStream& s, IFR_LOBData& lobdata)
{
    if(lobdata.lobdata) {
        s << "LOB[column=" << lobdata.lobdata->m_column
          << ", row="
          << lobdata.lobdata->m_row
          << "]";
    } else {
        s << "*** UNINITIALIZED ***";
    }
}
