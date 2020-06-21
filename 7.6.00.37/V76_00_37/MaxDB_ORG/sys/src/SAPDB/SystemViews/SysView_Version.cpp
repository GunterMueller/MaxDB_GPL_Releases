/****************************************************************************/
/*!

  @file        SysView_Version.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "Version" view class.

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
#include "gsp100.h"
#include "hsp100.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_Version.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/System/RTESys_SystemInfo.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*!
    @brief   constant for the maximal string length of the OS version
*/
#define OS_VERSION_STRING_LENGTH    64

/*!
    @brief   constant for the maximal string length of the CPU type
*/
#define CPU_TYPE_STRING_LENGTH      64


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

void SysView_Version::Create()
{

	m_Table->AppendCol (ITOCT_CHAR, SV_OPERATINGSYSTEM, OS_VERSION_STRING_LENGTH);
	m_Table->AppendCol (ITOCT_CHAR, SV_PROCESSORTYPE, CPU_TYPE_STRING_LENGTH);
	m_Table->AppendCol (ITOCT_FIXED, SV_ADDRESSINGMODE, 2);
	m_Table->AppendCol (ITOCT_CHAR, SV_ID, 40);
	m_Table->AppendCol (ITOCT_FIXED, SV_MAJORVERSION, 2);
	m_Table->AppendCol (ITOCT_FIXED, SV_MINORVERSION, 2);
	m_Table->AppendCol (ITOCT_FIXED, SV_CORRECTIONLEVEL, 2);
	m_Table->AppendCol (ITOCT_FIXED, SV_BUILD, 2);
	m_Table->AppendCol (ITOCT_CHAR, SV_KERNELVARIANT, 8);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Version::GetColCount()
{
	return SV_CC_VERSION;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Version::EstimateRows()
{
	return SV_ER_VERSION;
}

/*---------------------------------------------------------------------------*/

void SysView_Version::Execute()
{
    SAPDB_Char      sOS[OS_VERSION_STRING_LENGTH+2];
    SAPDB_Char      sCPU[CPU_TYPE_STRING_LENGTH+2];
    SAPDB_Char      sID[40+2];
    SAPDB_Char      sVariant[9+2];
    SAPDB_Int2      iAdrMode;
    SAPDB_Int2      iMayRel;
    SAPDB_Int2      iMinRel;
    SAPDB_Int2      iCorr;
    SAPDB_Int2      iBuild;

    m_Table->GetCatalogTable();

    if(m_Context.IsOk())
    {
        GetOSDetails (sOS, iAdrMode);
        GetCPUDetails (sCPU);
        GetKernelDetails (sID, iBuild, sVariant, iMayRel, iMinRel, iCorr);

        m_Table->MoveToCol (ITOVT_CHARPTR, sOS, (SAPDB_Int)strlen(sOS));
        m_Table->MoveToCol (ITOVT_CHARPTR, sCPU, (SAPDB_Int)strlen(sCPU));
        m_Table->MoveToCol (ITOVT_INT2, &iAdrMode, 0);
        m_Table->MoveToCol (ITOVT_CHARPTR, sID, (SAPDB_Int)strlen(sID));
        m_Table->MoveToCol (ITOVT_INT2, &iMayRel, 0);
        m_Table->MoveToCol (ITOVT_INT2, &iMinRel, 0);
        m_Table->MoveToCol (ITOVT_INT2, &iCorr, 0);
        m_Table->MoveToCol (ITOVT_INT2, &iBuild, 0);
        m_Table->MoveToCol (ITOVT_CHARPTR, sVariant, (SAPDB_Int)strlen(sVariant));
    }
}

void SysView_Version::GetOSDetails (SAPDB_Char *sOS, 
                                    SAPDB_Int2 &iBit)
{
    RTESys_GetOSVersion(sOS,OS_VERSION_STRING_LENGTH);
#ifdef BIT64
    iBit = 64;
#else
    iBit = 32;
#endif
}

void SysView_Version::GetCPUDetails (SAPDB_Char *sCPU)
{
    RTESys_GetCPUType(sCPU,CPU_TYPE_STRING_LENGTH);
}

void SysView_Version::GetKernelDetails (SAPDB_Char  *sIDString, 
                                        SAPDB_Int2  &iBuildNummer, 
                                        SAPDB_Char  *sKernelVariant,
                                        SAPDB_Int2  &iMayorRel,
                                        SAPDB_Int2  &iMinorRel,
                                        SAPDB_Int2  &iCorrLevel)
{
    tsp100_VersionID2   versionID;
    tsp00_Versionc      sVersID;

#if COMPILEMODE_MEO00 == SLOW_MEO00 
    #if defined(OMSTST)
        sp100_GetVersionString ( COMP_NAME_OMSTSTKNL_SP100, s100buildnumber, sVersID );
    #else
        sp100_GetVersionString ( COMP_NAME_SLOWKNL_SP100, s100buildnumber, sVersID );
    #endif
    strcpy(sIDString, sVersID);
    strcpy (sKernelVariant, "slow");  

#else
    #if COMPILEMODE_MEO00 == QUICK_MEO00 
        #if defined(OMSTST)
            sp100_GetVersionString ( COMP_NAME_OMSTSTKNL_SP100, s100buildnumber, sVersID );
        #else
            sp100_GetVersionString ( COMP_NAME_QUICKKNL_SP100, s100buildnumber, sVersID );
        #endif
        strcpy(sIDString, sVersID);
        strcpy (sKernelVariant, "quick");
    #else
        #if defined(OMSTST)
            sp100_GetVersionString ( COMP_NAME_OMSTSTKNL_SP100, s100buildnumber, sVersID );
        #else
            sp100_GetVersionString ( COMP_NAME_KERNEL_SP100, s100buildnumber, sVersID );
        #endif
        strcpy(sIDString, sVersID);
        strcpy (sKernelVariant, "fast");
    #endif
#endif

    sp100_GetVersionID ( VersionIDType2_esp100, s100buildnumberEx, &versionID );

    iMayorRel = versionID.MajorVersion_sp100;
    iMinorRel = versionID.MinorVersion_sp100;
    iCorrLevel = versionID.CorrLevel_sp100;
    iBuildNummer = versionID.BuildNumberPrefix_sp100;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
