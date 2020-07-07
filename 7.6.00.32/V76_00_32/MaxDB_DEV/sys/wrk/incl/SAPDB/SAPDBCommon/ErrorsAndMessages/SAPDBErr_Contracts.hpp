/*!*********************************************************************

    @file         SAPDBErr_Contracts.hpp
    
    @author       DanielD

    @brief

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
***********************************************************************/

#ifndef SAPDBERR_CONTRACTS_HPP
#define SAPDBERR_CONTRACTS_HPP

/*!

  @class SAPDBErr_Lawyer
  @brief checks that contracts are kept

 */
class SAPDBErr_Lawyer
{
public:
    static void preconditionFailed (const char * filename, int line, 
        const char * conditiontext);
    static void postconditionFailed (const char * filename, int line, 
        const char * conditiontext);
    static void invariantFailed (const char * filename, int line, 
        const char * conditiontext);
    static void runtimeCheckFailed (const char * filename, int line, 
        const char * conditiontext, const char * comment = 0);
#if defined (SAPDB_FAST)
    static void missingChecksMustBeImplementedInProductionCode ();
#endif
private:
    SAPDBErr_Lawyer () {}
};


#if defined (SAPDB_QUICK)

#define SAPDBERR_PRECONDITION(cond)  if (!(cond)) SAPDBErr_Lawyer::preconditionFailed (__FILE__, __LINE__, #cond)
#define SAPDBERR_POSTCONDITION(cond) if (!(cond)) SAPDBErr_Lawyer::postconditionFailed (__FILE__, __LINE__, #cond)
#define SAPDBERR_INVARIANT(cond)     if (!(cond)) SAPDBErr_Lawyer::invariantFailed (__FILE__, __LINE__, #cond)
#define SAPDBERR_MISSINGCHECK(cond)  if (!(cond)) SAPDBErr_Lawyer::runtimeCheckFailed (__FILE__, __LINE__, #cond)
#define SAPDBERR_MISSINGCHECK2(cond, comment)  if (!cond) SAPDBErr_Lawyer::runtimeCheckFailed (__FILE__, __LINE__, #cond, comment)

#else

#define SAPDBERR_PRECONDITION(cond)
#define SAPDBERR_POSTCONDITION(cond)
#define SAPDBERR_INVARIANT(cond)
//#define SAPDBERR_MISSINGCHECK(cond)  SAPDBErr_Lawyer::missingChecksMustBeImplementedInProductionCode ()
//#define SAPDBERR_MISSINGCHECK2(cond, comment)  SAPDBErr_Lawyer::missingChecksMustBeImplementedInProductionCode ()
#define SAPDBERR_MISSINGCHECK(cond)
#define SAPDBERR_MISSINGCHECK2(cond, comment)

#endif

#endif

