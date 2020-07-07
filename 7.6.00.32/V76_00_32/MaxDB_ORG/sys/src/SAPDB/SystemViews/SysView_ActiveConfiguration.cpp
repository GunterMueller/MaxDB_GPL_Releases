/****************************************************************************/
/*!

  @file        SysView_ActiveConfiguration.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "ActiveConfiguration" view class.

  @see        

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SystemViews/SysView_Defines.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "SystemViews/SysView_ActiveConfiguration.hpp"
#include "SQLManager/SQLMan_Context.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


void SysView_ActiveConfiguration::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ActiveConfiguration::Create", SysView_Trace, 5);

    m_Table->AppendCol (ITOCT_CHAR, SV_PARAMETERNAME, 32);
    m_Table->AppendCol (ITOCT_CHAR, SV_PERMANENT, 3);
    m_Table->AppendCol (ITOCT_CHAR, SV_CHANGEABLE, 10);
    m_Table->AppendCol (ITOCT_CHARUNICODE, SV_VALUE, 256);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ActiveConfiguration::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ActiveConfiguration::GetColCount", SysView_Trace, 5);

    return SV_CC_ACTIVECONFIGURATION;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ActiveConfiguration::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ActiveConfiguration::EstimateRows", SysView_Trace, 5);

    return SV_ER_ACTIVECONFIGURATION;
}

/*---------------------------------------------------------------------------*/

void SysView_ActiveConfiguration::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ActiveConfiguration::Execute", SysView_Trace, 5);

    RTEConf_ParameterReader             paramReader;

    RTEConf_Parameter::Name				name;
    SAPDB_Char							*value;
	SAPDB_Bool                          updateOnline;
    SAPDB_Bool                          permanentChange;
    SAPDBErr_MessageList                err;

    m_Table->GetCatalogTable();
    
    if (m_Context.IsOk())
    {
        paramReader.StartIteration();
        
        while ( (m_Context.IsOk()) && (paramReader.Next(name, value, 
            updateOnline, permanentChange, err) ))
        {
            if( strncmp( (SAPDB_Char *)name, "CONTROLPASSWORD", strlen((SAPDB_Char *)name) ) != 0 )
            {
                SAPDB_Char *changeable  = "NO";
                m_Table->MoveToCol (ITOVT_CHARPTR, (void *)name, (SAPDB_Int)strlen((SAPDB_Char *)name));
                m_Table->MoveToCol (ITOVT_YESNO, &permanentChange, 0);
                if (SAPDB_TRUE == updateOnline)
                {
                    changeable  = "YES";
                }
                AddColumn (changeable);
                m_Table->MoveToCol (ITOVT_UTF8PTR, value, (SAPDB_Int)strlen((SAPDB_Char *)value));
            }
        }
        
        paramReader.EndIteration (err);
    }
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
