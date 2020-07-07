/**************************************************************************/
/*!

  @file         Kernel_Version.hpp
  @author       MartinB
  @author       UweH
  @ingroup      KernelCommon
  @brief        Declaration of the version-handling-class
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

#ifndef KERNEL_VERSION_HPP
#define KERNEL_VERSION_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"
#include "gsp00.h"

class Msg_List;

/*!
    @brief        deals with kernel-versions: creation, comparison and extraction of values from 
                  kernel-values    
    @ingroup      KernelCommon
*/
class Kernel_Version {
    
    public:

        /// defines a version number with invalid value
        typedef SAPDB_Invalidatable<SAPDB_UInt1,SAPDB_MAX_UINT1> VersionNumber;

        /// compactified software version struct
        struct VersionNumbers
        {
            VersionNumber majorRelease;     ///< e.g. 7
            VersionNumber minorRelease;     ///< e.g. 5
            VersionNumber correctionLevel;  ///< e.g. 00
            VersionNumber build;            ///< e.g. 08
            /// standard constructor, if no parameters are given the current
            /// software version is assigned
            VersionNumbers(VersionNumber newMajorRelease    = VersionNumber(),
                           VersionNumber newMinorRelease    = VersionNumber(),
                           VersionNumber newCorrectionLevel = VersionNumber(),
                           VersionNumber newBuild           = VersionNumber())
            {
                majorRelease    = newMajorRelease;
                minorRelease    = newMinorRelease;
                correctionLevel = newCorrectionLevel;
                build           = newBuild;
            }
        };
        
        /// bytes of compact version
        typedef SAPDB_Byte CompactVersionString[sizeof(VersionNumbers)];

        /// defines the speed of the Kernel
        enum ComponentType
        { 
            SlowKernel,
            QuickKernel,
            FastKernel,
            OMTTestKernel,
            UndefComponent
        };
        
        /// string length of a component name
        #define COMPONENT_NAME_LEN 9    

        /// defines where the kernel had been build (first digit of 2nd build-number-block)
        enum BuildKind
        {
            DevelopmentBuild = 0, ///< compiled on a development  machine (Build xxx-0xx-...)
            ProductionBuild  = 1, ///< compiled on a make factory machine (Build xxx-1xx-...)
            UndefBuildKind   = 2  ///< build kind not defined
        };
        
        /// defines the development phase of the kernel
        enum DevelopmentPhase
        {
            DevelopPhase          = 0, ///< kernel is under development (Build xxx-x0x-...)
            CorrectionPhase       = 1, ///< kernel is in correction phase (Build xxx-x1x-...)
            RampPhase             = 2, ///< kernel is on ramp (Build xxx-x2x-...)
            UndefDevelopmentPhase = 3  ///< developmentphase is not defined
        };

        /// only for RAMP there is coede which instance-types are supported
        enum InstanceType
        { 
            AllInstances      = 0, ///< this supports everything (i.e. OLTP/LC) (Build xxx-xx0-...)
            OLTPInstance      = 1, ///< this is an OLTP-Instance (Build xxx-xx1-...)
            LiveCacheInstance = 2, ///< this is an LiveCache-Instance (Build xxx-xx2-...)
            UndefInstance     = 3
        };
           
        /// Format of a Kernel String: "SlowKnl   7.4.4    Build 001-000-000-000"
        typedef tsp00_Version VersionString;
        
        /// This extracts the extra version info from the given string
        static void GetExtraInfo (VersionString     version,
                                  ComponentType    &componentType,
                                  BuildKind        &buildInfo,
                                  DevelopmentPhase &developPhase,
                                  InstanceType     &builtForInstance);

        /// This extracts the extra version from current software version
        static void GetExtraInfoFromSoftware (ComponentType    &componentType,
                                               BuildKind        &buildInfo,
                                               DevelopmentPhase &developPhase,
                                               InstanceType     &builtForInstance);
        
        /// constructs a kernel-version from the actually running kernel
        Kernel_Version();

        /// constructs a kernelVersion out of a given kernel-string
        Kernel_Version (VersionString kernelVersion);

        /// constructs a kernelVersion out of a given compact version String
        Kernel_Version (const CompactVersionString * compactVersion);
        
        /// constructs a kernelVersion out of a given compact version
        Kernel_Version (const VersionNumbers &compactVersion);
        
        /// constructs a kernel-version from given parameters
        Kernel_Version (VersionNumber     majorVersion,
                        VersionNumber     minorVersion,
                        VersionNumber     correctionLevel = VersionNumber(),
                        VersionNumber     buildNo         = VersionNumber() );

        /// compares two Kernel_Versions. Only the numerical values are taken into account 
        bool operator > (const Kernel_Version rightOp ) const;
        /// compares two Kernel_Versions. Only the numerical values are taken into account 
        bool operator == (const Kernel_Version rightOp ) const;
        /// compares two Kernel_Versions. Only the numerical values are taken into account 
        bool operator >= (const Kernel_Version rightOp ) const;
        /// compares two Kernel_Versions. Only the numerical values are taken into account 
        bool operator != (const Kernel_Version rightOp ) const
        {
            return ! (*this == rightOp);
        }
        /// compares two Kernel_Versions. Only the numerical values are taken into account 
        bool operator < (const Kernel_Version rightOp ) const
        {
            return ! (*this >= rightOp);
        }
        /// compares two Kernel_Versions. Only the numerical values are taken into account 
        bool operator <= (const Kernel_Version rightOp ) const
        {
            return rightOp > *this;
        }

        const VersionNumber          MajorVersion() const;               ///< Accessor
        const VersionNumber          MinorVersion() const;               ///< Accessor
        const VersionNumber          CorrectionLevel() const;            ///< Accessor
        const VersionNumber          BuildNo() const;                    ///< Accessor
        const CompactVersionString * GetCompactVersion() const;        ///< Accessor

        /// appends a message detail to the messagelist with current version
        void AppendAsMessage(SAPDB_Char  const * const  filename,
                             SAPDB_UInt4 const          linenumber,
                             Msg_List                  &errlist,
                             SAPDB_Char  const * const  title = 0) const;
        
        /// This is the self test of this class
        static bool Test(Msg_List &errlist);
        
    private:
        VersionNumbers m_VersionNumbers;
};


#endif
