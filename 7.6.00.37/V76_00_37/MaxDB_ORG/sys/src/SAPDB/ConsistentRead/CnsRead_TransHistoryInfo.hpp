/*!***************************************************************************

  module:       CnsRead_TransHistoryInfo.hpp

  responsible : ThomasA

  special area: Consistent Read

  description:  Defines the transient row header containing transaction and history information 

  last changed: 2000-08-10  12:27
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef CNSREAD_TRANSHISTORYINFO_HPP
#define CNSREAD_TRANSHISTORYINFO_HPP

#include "ggg00.h"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*!
   @brief defines information needed for consistency check of a row
*/
class CnsRead_TransHistoryInfo : public tgg00_ObjTransInfo
{
public :
    CnsRead_TransHistoryInfo();
    CnsRead_TransHistoryInfo(const tgg00_TransContext& currentTrans);
    /*!
       @brief returns a reference to the history, i.e. to the list of before images
    */
    inline const tgg91_PageRef& GetBeforeRef() const;
    /*!
       @brief checks whether the trans history information is still needed
     */
    bool IsRequired() const;
    /*!
       @brief stores a given reference to a before image into the m_transInfo
     */
    void SetBeforeRef (const tgg91_PageRef& beforeRef);
    /*!
       @brief stores a given trans id
     */
    inline void SetUpdateTrans(const tgg91_TransNo& updTrans);
    /*!
      @brief assignment
     */
    inline void operator= (const tgg00_ObjTransInfo& Source);
};

inline const tgg91_PageRef& CnsRead_TransHistoryInfo::GetBeforeRef() const
{
    return otrBeforeRef_gg00;
}

inline void CnsRead_TransHistoryInfo::SetUpdateTrans(const tgg91_TransNo& updTrans)
{
    otrUpdTransId_gg00 = updTrans;
}

inline void CnsRead_TransHistoryInfo::operator= (const tgg00_ObjTransInfo& Source)
{
    SAPDB_MemCopyNoCheck (this, &Source, sizeof(*this));
}

#endif