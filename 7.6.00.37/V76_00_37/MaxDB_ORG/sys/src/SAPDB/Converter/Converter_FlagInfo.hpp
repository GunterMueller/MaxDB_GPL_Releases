/*!
  @file           Converter_FlagInfo.hpp
  @author         TorstenS
  @author         AlexanderK
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2000-2004 SAP AG

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
*/


#ifndef CONVERTER_FLAGINFO_HPP
#define CONVERTER_FLAGINFO_HPP

#include "DataAccess/Data_Types.hpp"

/*!
   @class          Converter_FlagInfo
   @brief
 */

class Converter_FlagInfo
{
public:

    /*! @name Type definitions */

    /*!
       @brief          Flag info data

                    You cannot change this type without performing additional tasks!
                    If you change it to 8 bytes, critically check all flag info routines.
                    There may arise alignment problems.

     */
    typedef SAPDB_UInt4                         FlagInfoData;


private:

    /*!
       @brief          Returns bit position for flag of specified entry
                       within the corresponding flag info block
       @param          EntryNo [in] entry no
       @return         (SAPDB_UInt)
     */
    static SAPDB_UInt FlagInfoBitPos (const SAPDB_UInt&   EntryNo)
    {
        // this is meant:
        // return EntryNo % (SAPDB_BITS_PER_BYTE * sizeof(FlagInfo));

        // this is faster:
        return EntryNo & 0x1F;
    }

public:

    /*!
       @brief          Single bit bitmask for every bit in a FlagInfo block
     */
    static FlagInfoData FlagInfoBitMask[];

    /*!
       @brief          Returns sequence number of the flag info block where a flag
                       of the specified entry is stored
       @param          EntryNo [in] entry no
       @return         (SAPDB_UInt)
     */
    static SAPDB_UInt FlagInfoBlockNo (const SAPDB_UInt&  EntryNo)
    {
        // this is meant:
        // return EntryNo / (SAPDB_BITS_PER_BYTE * sizeof(FlagInfo));

        // this is faster:
        return EntryNo >> 5;
    };

    /*!
    @brief
    @return         (SAPDB_UInt)
  */
    Converter_FlagInfo (FlagInfoData * FlagInfoDataBlockPtr,
                        SAPDB_UInt     BlockOffset)
    {
        m_FlagInfoDataPtr = FlagInfoDataBlockPtr + FlagInfoBlockNo(BlockOffset);
    }

    /*!
    @brief
    @param          EntryNo [in] entry no
    @return         (SABDB_Bool)
  */
    SAPDB_Bool FlagIsSet (SAPDB_UInt EntryNo) const
    {
      return ((*m_FlagInfoDataPtr & FlagInfoBitMask[FlagInfoBitPos(EntryNo)]) != 0);
    }

    /*!
    @brief
    @param          EntryNo [in] entry no
    @return         none
  */
    void SetFlag (SAPDB_UInt EntryNo)
    {
        *m_FlagInfoDataPtr |= FlagInfoBitMask[FlagInfoBitPos(EntryNo)];
    }

    /*!
    @brief
    @param          EntryNo [in] entry no
    @return         none
  */
    void DelFlag (SAPDB_UInt EntryNo)
    {
        *m_FlagInfoDataPtr &= ~FlagInfoBitMask[FlagInfoBitPos(EntryNo)];
    }

    /*!
    @brief
    @return         (FlagInfoData*)
  */
    FlagInfoData* FlagInfoDataPtr()
    {
        return m_FlagInfoDataPtr;
    }

private:

    FlagInfoData * m_FlagInfoDataPtr;

};

#endif // CONVERTER_FLAGINFO_HPP

