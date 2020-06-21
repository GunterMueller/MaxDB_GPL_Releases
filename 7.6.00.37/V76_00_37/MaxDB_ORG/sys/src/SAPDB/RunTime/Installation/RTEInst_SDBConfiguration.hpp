/*!
  \file    RTEInst_SDBConfiguration.hpp
  \author  MarcW
  \ingroup RTE Installation
  \brief   get/set configuration parameters

\if EMIT_LICENCE

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


\endif
*/

#if !defined(RTEInst_SDBConfiguration_hpp)
#define RTEInst_SDBConfiguration_hpp

#include "gsp01.h"

/*!
    @brief set/get independent data path and independent programs path
*/
class RTEInst_SDBConfiguration {
public:
    /*! @brief empty constructor */
    RTEInst_SDBConfiguration() {};

    /*! @brief empty destructor */
    ~RTEInst_SDBConfiguration() {};

   /*!
        @brief set independent data path in registry
        @param newPath [IN] new path value
        @return RTEInst_SDBConfigConstants::OK in case of success, otherwise a different value
        @see RTEInst_SDBConfigConstants
   */
   int setIndepDataPath(const char* newPath);

   /*!
        @brief print independent data path to STDOUT
        @param [IN] printWithKey if true, the key IndepDataPath is printed in front of the path
        @return RTEInst_SDBConfigConstants::OK in case of success, otherwise a different value
        @see RTEInst_SDBConfigConstants
   */
   int getIndepDataPath(bool printWithKey = false);

   /*!
        @brief set independent programs path in registry
        @param newPath [IN] new path value
        @return RTEInst_SDBConfigConstants::OK in case of success, otherwise a different value
        @see RTEInst_SDBConfigConstants

   */
   int setIndepProgramsPath(const char* newPath);

   /*!
        @brief print independent programs path to STDOUT
        @param [IN] printWithKey if true, the key IndepProgramsPath is printed in front of the path
        @return RTEInst_SDBConfigConstants::OK in case of success, otherwise a different value
        @see RTEInst_SDBConfigConstants
   */
   int getIndepProgramsPath(bool printWithKey = false);

   /*!
        @brief print independent programs path and independent data path to STDOUT
        @return RTEInst_SDBConfigConstants::OK in case of success, otherwise a different value
        @see RTEInst_SDBConfigConstants
   */
   int listAllParameters();

private:
    typedef tsp00_Bool (*PathSetterFuntionPointer)( tsp00_Pathc& inPath, tsp01_RteError* rteError );
    typedef tsp00_Bool (*PathGetterFuntionPointer)( tsp00_Pathc& outPath, int terminationCharFlag, tsp01_RteError* rteError );
    typedef tsp00_Bool (*RegistryCheckFuntionPointer)();
    
    
    int setPath(const char* newPath, PathSetterFuntionPointer pathSetterFuntion);
    int getPath(const char* key, PathGetterFuntionPointer pathGetterFunction, RegistryCheckFuntionPointer checkFunction, bool printWithKey);

    bool movePathTo( tsp00_Pathc& pathArray, const char* path );
    void printValue( const char* key, const char* value, bool takenFromRegistry, bool printWithKey);
    void printRTEError(tsp01_RteError& rteError);
};


#endif //RTEInst_SDBConfiguration
