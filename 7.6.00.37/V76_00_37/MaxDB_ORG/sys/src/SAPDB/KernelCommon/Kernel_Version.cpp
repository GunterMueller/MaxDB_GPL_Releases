/*!
  @file         Kernel_Version.cpp
  @author       MartinB
  @author       UweH
  @ingroup      KernelCommon
  @brief        Implementation of methods for dealing with the kernel-version
  @since        2003-03-13
*/
/*!
\if EMIT_LICENCE

  copyright:    Copyright (c) 2001-2005 SAP AG


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
#include <memory.h>
#include <stdio.h>
#include "hgg11.h"   // g11kernel_version

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "KernelCommon/Kernel_Messages.hpp"
#include "KernelCommon/Kernel_Exception.hpp"
#include "Messages/Msg_List.hpp"
#include "SAPDBCommon/Messages/SDBMsg_KernelCommon.h"

#include "KernelCommon/Kernel_Version.hpp"

/// This is needed to activate substitution in gsp100.h
#define VSP100_CPP true
#include "gsp100.h"

/* --------------------------------------------------------------------------- */
void Kernel_Version::GetExtraInfoFromSoftware (ComponentType    &componentType,
                                               BuildKind        &buildInfo,
                                               DevelopmentPhase &developPhase,
                                               InstanceType     &builtForInstance)
{
    #if defined(OMSTST)
    componentType = OMTTestKernel;
    #else
        #ifdef SAPDB_SLOW
        componentType = SlowKernel;
        #else
            #ifdef SAPDB_QUICK
            componentType = QuickKernel;
            #else
            componentType = FastKernel;
            #endif
        #endif
    #endif
    
    if ( ! strcmp (SAPDB_RELSTAT_SP100, "PROD") )
        buildInfo = ProductionBuild;
    else if ( ! strcmp (SAPDB_RELSTAT_SP100, "TEST") )
        buildInfo = DevelopmentBuild;
	else
        buildInfo = UndefBuildKind;

    if ( ! strcmp (SAPDB_BUILD_STATE_SP100, "DEV") )
        developPhase = DevelopPhase;
    else if ( ! strcmp (SAPDB_BUILD_STATE_SP100, "COR") )
        developPhase = CorrectionPhase;
    else if ( ! strcmp (SAPDB_BUILD_STATE_SP100, "RAMP") )
        developPhase = RampPhase;
	else
        developPhase = UndefDevelopmentPhase;

    if ( ! strcmp (SAPDB_PURPOSE_SP100, "ALL") )
        builtForInstance = AllInstances;
    else if ( ! strcmp (SAPDB_PURPOSE_SP100, "LC") )
        builtForInstance = LiveCacheInstance;
    else if ( ! strcmp (SAPDB_PURPOSE_SP100, "OLTP") )
        builtForInstance = OLTPInstance;
	else
        builtForInstance = UndefInstance;
}
/* --------------------------------------------------------------------------- */
void Kernel_Version::GetExtraInfo (VersionString     version,
                                   ComponentType    &componentType,
                                   BuildKind        &buildInfo,
                                   DevelopmentPhase &developPhase,
                                   InstanceType     &builtForInstance)
{
    if (!memcmp(version, COMP_NAME_KERNEL_SP100, COMPONENT_NAME_LEN))
        componentType = FastKernel;
    else if (!memcmp(version, COMP_NAME_QUICKKNL_SP100, COMPONENT_NAME_LEN))
        componentType = QuickKernel;
    else if (!memcmp(version, COMP_NAME_SLOWKNL_SP100, COMPONENT_NAME_LEN))
        componentType = SlowKernel;
    else if (!memcmp(version, COMP_NAME_OMSTSTKNL_SP100, COMPONENT_NAME_LEN))
        componentType = OMTTestKernel;
    else
        componentType = UndefComponent;

    SAPDB_Int4 extraNumber;

    g11ParseExtraNumber (version, extraNumber);

    SAPDB_Int4 numInstanceType = extraNumber % 10;
    extraNumber = (extraNumber - numInstanceType) / 10;
    switch (numInstanceType)
    {
        case OLTPInstance:
            builtForInstance = OLTPInstance;
            break;
        case LiveCacheInstance:
            builtForInstance = LiveCacheInstance;
            break;
        case AllInstances:
            builtForInstance = AllInstances;
            break;
        case UndefInstance:
        default:
            builtForInstance = UndefInstance;
    }
        
    SAPDB_Int4 numDevelopmentPhase = extraNumber % 10;
    extraNumber = (extraNumber - numDevelopmentPhase) / 10;
    switch (numDevelopmentPhase)
    {
        case DevelopPhase:
            developPhase = DevelopPhase;
            break;
        case CorrectionPhase:
            developPhase = CorrectionPhase;
            break;
        case RampPhase:
            developPhase = RampPhase;
            break;
        default:
            developPhase = UndefDevelopmentPhase;
    }

    SAPDB_Int4 numBuildKind = extraNumber % 10;
    switch (numBuildKind)
    {
        case DevelopmentBuild:
            buildInfo = DevelopmentBuild;
            break;
        case ProductionBuild:
            buildInfo = ProductionBuild;
            break;
        default:
            buildInfo = UndefBuildKind;
            break;
    }
}
/* --------------------------------------------------------------------------- */
Kernel_Version::Kernel_Version()
: m_VersionNumbers(MAJOR_VERSION_NO_SP100,
                   MINOR_VERSION_NO_SP100,
                   CORRECTION_LEVEL_SP100,
                   BUILD_PREFIX_SP100)
{}
/* --------------------------------------------------------------------------- */
Kernel_Version::Kernel_Version (const CompactVersionString * compactVersion)
{
    const VersionNumbers * newVersions = reinterpret_cast<const VersionNumbers *>(compactVersion);
    m_VersionNumbers = *newVersions;
}
/* --------------------------------------------------------------------------- */
Kernel_Version::Kernel_Version (const VersionNumbers &compactVersion)
: m_VersionNumbers(compactVersion)
{}
/* --------------------------------------------------------------------------- */
Kernel_Version::Kernel_Version (VersionString kernelVersion)
{
    SAPDB_Int4 auxMajorRelease;
    SAPDB_Int4 auxMinorRelease;
    SAPDB_Int4 auxCorrectionLevel;
    SAPDB_Int4 auxBuildNo;

    g11ParseVersion ( kernelVersion,
                      auxMajorRelease,
                      auxMinorRelease,
                      auxCorrectionLevel,
                      auxBuildNo );

    VersionNumber majorRelease;
    VersionNumber minorRelease;
    VersionNumber correctionLevel;
    VersionNumber buildNo;

    if ( auxMajorRelease >= 0 )
        majorRelease = auxMajorRelease;

    if ( auxMinorRelease >= 0 )
        minorRelease = auxMinorRelease;
        
    if ( auxCorrectionLevel >= 0 )
        correctionLevel = auxCorrectionLevel;

    if ( auxBuildNo >= 0 )
        buildNo = auxBuildNo;

    m_VersionNumbers = VersionNumbers(majorRelease,minorRelease,correctionLevel,buildNo);
}
/* --------------------------------------------------------------------------- */
Kernel_Version::Kernel_Version (VersionNumber     majorVersion,
                                VersionNumber     minorVersion,
                                VersionNumber     correctionLevel,
                                VersionNumber     buildNo)
: m_VersionNumbers(majorVersion,minorVersion,correctionLevel,buildNo)
{}
/* --------------------------------------------------------------------------- */
bool Kernel_Version::operator== (const Kernel_Version rightOp ) const
{
    // check mandantory attributes of the kernel_Version
    if (m_VersionNumbers.majorRelease != rightOp.m_VersionNumbers.majorRelease)
        return false;
        
    if (m_VersionNumbers.minorRelease != rightOp.m_VersionNumbers.minorRelease)
        return false;
        
    if ( m_VersionNumbers.correctionLevel.IsValid()
         &&
         rightOp.m_VersionNumbers.correctionLevel.IsValid()
         &&
         m_VersionNumbers.correctionLevel != rightOp.m_VersionNumbers.correctionLevel )
        return false;
        
    if ( m_VersionNumbers.build.IsValid()
         &&
         rightOp.m_VersionNumbers.build.IsValid()
         &&
         m_VersionNumbers.build != rightOp.m_VersionNumbers.build )
        return false;

    return true;
}
/* --------------------------------------------------------------------------- */
bool Kernel_Version::operator> (const Kernel_Version rightOp ) const
{
    if ( m_VersionNumbers.majorRelease > rightOp.m_VersionNumbers.majorRelease )
        return true;

    if ( m_VersionNumbers.majorRelease == rightOp.m_VersionNumbers.majorRelease
         &&
         m_VersionNumbers.minorRelease > rightOp.m_VersionNumbers.minorRelease )
        return true;

    if ( m_VersionNumbers.correctionLevel.IsInvalid()
         ||
         rightOp.m_VersionNumbers.correctionLevel.IsInvalid() )
         return false;
         
    if ( m_VersionNumbers.majorRelease == rightOp.m_VersionNumbers.majorRelease
         &&
         m_VersionNumbers.minorRelease == rightOp.m_VersionNumbers.minorRelease
         &&
         m_VersionNumbers.correctionLevel > rightOp.m_VersionNumbers.correctionLevel )
         return true;

    if ( m_VersionNumbers.build.IsInvalid()
         ||
         rightOp.m_VersionNumbers.build.IsInvalid() )
         return false;
         
    if ( m_VersionNumbers.majorRelease    == rightOp.m_VersionNumbers.majorRelease
         &&
         m_VersionNumbers.minorRelease    == rightOp.m_VersionNumbers.minorRelease
         &&
         m_VersionNumbers.correctionLevel == rightOp.m_VersionNumbers.correctionLevel
         &&
         m_VersionNumbers.build           >  rightOp.m_VersionNumbers.build )
         return true;

    return false;
}
/* --------------------------------------------------------------------------- */
bool Kernel_Version::operator>= (const Kernel_Version rightOp ) const
{
    if ( *this > rightOp )
        return true;

    if ( *this == rightOp )
        return true;
     
    return false;
}
/* --------------------------------------------------------------------------- */
const Kernel_Version::VersionNumber Kernel_Version::MajorVersion() const
{
    return m_VersionNumbers.majorRelease;
}
/* --------------------------------------------------------------------------- */
const Kernel_Version::VersionNumber Kernel_Version::MinorVersion() const
{
    return m_VersionNumbers.minorRelease;
}
/* --------------------------------------------------------------------------- */
const Kernel_Version::VersionNumber Kernel_Version::CorrectionLevel() const
{
    return m_VersionNumbers.correctionLevel;
}
/* --------------------------------------------------------------------------- */
const Kernel_Version::VersionNumber Kernel_Version::BuildNo() const
{
    return m_VersionNumbers.build;
}
/* --------------------------------------------------------------------------- */
const Kernel_Version::CompactVersionString * Kernel_Version::GetCompactVersion() const
{
    return reinterpret_cast<const CompactVersionString*>(&m_VersionNumbers);
}

/* --------------------------------------------------------------------------- */

void Kernel_Version::AppendAsMessage(SAPDB_Char  const * const  filename,
                                     SAPDB_UInt4 const          linenumber,
                                     Msg_List                  &errlist,
                                     SAPDB_Char  const * const  title) const
{
    errlist.AddDetail (SDBMSG_KERNELCOMMON_VERSION_INFO,
          Msg_Arg("TITLE",           title==0?"Kernel_Version":title),
          Msg_Arg("MAJOR_NUMBER",    MajorVersion().IsValid()    ? (SAPDB_Char const *)SAPDB_ToString(MajorVersion())    : "*"),
          Msg_Arg("MINOR_NUMBER",    MinorVersion().IsValid()    ? (SAPDB_Char const *)SAPDB_ToString(MinorVersion())    : "*"),
          Msg_Arg("CORRECTION_LEVEL",CorrectionLevel().IsValid() ? (SAPDB_Char const *)SAPDB_ToString(CorrectionLevel()) : "*"),
          Msg_Arg("BUILD_NUMBER",    BuildNo().IsValid()         ? (SAPDB_Char const *)SAPDB_ToString(BuildNo())         : "*"));
}

/* --------------------------------------------------------------------------- */
bool Kernel_Version::Test(Msg_List &errlist)
{
    Kernel_Version checkVersion1;
    Kernel_Version checkVersion2;

    checkVersion1 = Kernel_Version (7,4);
    checkVersion2 = Kernel_Version (7,4,4,2);
    if ( checkVersion1 != checkVersion2 )
    {
        errlist = Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "check == 7.4");
        checkVersion1.AppendAsMessage(__CONTEXT__, errlist);
        checkVersion2.AppendAsMessage(__CONTEXT__, errlist);
        return false;
    }
    checkVersion1 = Kernel_Version (7,4,4);
    checkVersion2 = Kernel_Version (7,4,4,2);
    if ( checkVersion1 != checkVersion2 )
    {
        errlist = Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "check == 7.4.4");
        checkVersion1.AppendAsMessage(__CONTEXT__, errlist);
        checkVersion2.AppendAsMessage(__CONTEXT__, errlist);
        return false;
    }
    checkVersion1 = Kernel_Version (7,4,4,2);
    checkVersion2 = Kernel_Version (7,4,4,2);
    if ( checkVersion1 != checkVersion2 )
    {
        errlist = Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "check == 7.4.4.2");
        checkVersion1.AppendAsMessage(__CONTEXT__, errlist);
        checkVersion2.AppendAsMessage(__CONTEXT__, errlist);
        return false;
    }
    checkVersion1 = Kernel_Version (7,4,4,2);
    checkVersion2 = Kernel_Version (7,4,4,1);
    if ( checkVersion1 < checkVersion2 )
    {
        errlist = Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "check 7.4.4.1 < 7.4.4.2");
        checkVersion1.AppendAsMessage(__CONTEXT__, errlist);
        checkVersion2.AppendAsMessage(__CONTEXT__, errlist);
        return false;
    }

    checkVersion1 = Kernel_Version();

    VersionString currentKernelVersion;
    g11kernel_version (currentKernelVersion);

    checkVersion2 = Kernel_Version(currentKernelVersion);

    if ( checkVersion1 != checkVersion2 )
    {
        errlist = Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "check == 1");
        checkVersion1.AppendAsMessage(__CONTEXT__, errlist);
        checkVersion2.AppendAsMessage(__CONTEXT__, errlist);
        return false;
    }

    ComponentType    componentType;
    BuildKind        buildInfo;
    DevelopmentPhase developPhase;
    InstanceType     builtForInstance;

    GetExtraInfoFromSoftware(componentType,buildInfo,developPhase,builtForInstance);

    if ( componentType
    #ifdef SAPDB_SLOW
	!= Kernel_Version::SlowKernel )
    #else
        #ifdef SAPDB_QUICK
        != Kernel_Version::QuickKernel )
        #else
            #ifdef SAPDB_FAST
            != Kernel_Version::FastKernel )
            #endif
        #endif
	#endif
    {
        errlist = Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "check component type");
        checkVersion1.AppendAsMessage(__CONTEXT__, errlist);
        return false;
    }

    checkVersion2 = Kernel_Version(checkVersion1.GetCompactVersion());
    if ( checkVersion1 != checkVersion2 )
    {
        errlist = Kernel_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED, "check == 2");
        checkVersion1.AppendAsMessage(__CONTEXT__, errlist);
        checkVersion2.AppendAsMessage(__CONTEXT__, errlist);
        return false;
    }
    // remove unnecessary messages
    errlist.ClearMessageList();
    return true;
}
