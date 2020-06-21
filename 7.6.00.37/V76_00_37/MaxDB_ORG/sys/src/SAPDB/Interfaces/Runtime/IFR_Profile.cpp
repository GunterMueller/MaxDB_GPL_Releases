/*!
  @file           IFR_Profile.cpp
  @author         MarcoP
  @ingroup        IFR_Profile
  @brief          Collect profile data of SQLDBC
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
#include "SAPDB/Interfaces/Runtime/IFR_Profile.h"
#include "SAPDB/Interfaces/Runtime/IFR_Environment.h"

//======================================================================
// IFR_Profile
//======================================================================
//----------------------------------------------------------------------
IFR_Profile::IFR_Profile(IFRUtil_LinkedList<IFR_Environment>* list)
: m_EnvironmentList(list)
{
  this->initCounters();
}

//----------------------------------------------------------------------
void IFR_Profile::collectCounters() {
  IFR_Environment * profile = static_cast<IFR_Environment*>  (this->m_EnvironmentList->First());
   if (!profile) return;
   SAPDB_ULong allocCnt=0,deallocCnt=0;
   profile->runtime.getGlobalAllocator().GetCallStatistics(allocCnt, deallocCnt);
   m_counter[IFR_ALLOCATECOUNT]  = allocCnt;
   m_counter[IFR_DEALLOCATECOUNT]= deallocCnt;
   do {
     profile->collectCounters();
     profile->submitCounters(this->m_counter);
   } while (0 != (profile = static_cast<IFR_Environment*> (this->m_EnvironmentList->Next(*profile))));
}

//----------------------------------------------------------------------
IFR_UInt8 IFR_Profile::getCounter(IFR_ProfileCountValues value) {
    return m_counter[value];
}

//----------------------------------------------------------------------
void IFR_Profile::resetProfileCounters() {  
  this->collectCounters();	
  initCounters();
}

//----------------------------------------------------------------------
const char* IFR_Profile::getProfileCounterName(IFR_ProfileCountValues value) {  
   IFR_STATIC_ASSERT(SQLDBC_PROFILE_MAX == 35); //if you modify the SQLDBC_ProfileCountValues then you have to modify this switch too.
   switch(value){
	 case (SQLDBC_ALLOCATECOUNT              ): { return "ALLOCATECOUNT"          ;} 
	 case (SQLDBC_DEALLOCATECOUNT            ): { return "DEALLOCATECOUNT"        ;}
	 case (SQLDBC_CONNECTIONCOUNT            ): { return "CONNECTIONCOUNT"        ;}
	 case (SQLDBC_COMMITCOUNT                ): { return "COMMITCOUNT"            ;}
	 case (SQLDBC_ROLLBACKCOUNT              ): { return "ROLLBACKCOUNT"          ;}
	 case (SQLDBC_SETISOLATIONCOUNT          ): { return "SETISOLATIONCOUNT"      ;}
	 case (SQLDBC_SENDCOUNT                  ): { return "SENDCOUNT"              ;}
	 case (SQLDBC_SENDSIZE                   ): { return "SENDSIZE"               ;}
	 case (SQLDBC_RECEIVESIZE                ): { return "RECEIVESIZE"            ;}
	 case (SQLDBC_CANCELCOUNT                ): { return "CANCELCOUNT"            ;}
	 case (SQLDBC_STATEMENTCOUNT             ): { return "STATEMENTCOUNT"         ;}
	 case (SQLDBC_PREPAREDSTATEMENTCOUNT     ): { return "PREPAREDSTATEMENTCOUNT" ;}
	 case (SQLDBC_NOTSELECTFETCHOPTIMIZED    ): { return "NOTSELECTFETCHOPTIMIZED";}
	 case (SQLDBC_DROPPARSEIDCOUNT           ): { return "DROPPARSEIDCOUNT"       ;}
	 case (SQLDBC_DROPCURSORCOUNT            ): { return "DROPCURSORCOUNT"        ;}
	 case (SQLDBC_DROPLONGDESCCOUNT          ): { return "DROPLONGDESCCOUNT"      ;}
	 case (SQLDBC_PREPARECOUNT               ): { return "PREPARECOUNT"           ;}
	 case (SQLDBC_PARSEINFOHITCOUNT          ): { return "PARSEINFOHITCOUNT"      ;}
	 case (SQLDBC_PARSINFOMISSCOUNT          ): { return "PARSINFOMISSCOUNT"      ;}
	 case (SQLDBC_PARSEAGAINCOUNT            ): { return "PARSEAGAINCOUNT"        ;}
	 case (SQLDBC_PARSEINFOSQUEEZECOUNT      ): { return "PARSEINFOSQUEEZECOUNT"  ;}
	 case (SQLDBC_EXECUTECOUNT               ): { return "EXECUTECOUNT"           ;}
	 case (SQLDBC_INSERTCOUNT                ): { return "INSERTCOUNT"            ;}
	 case (SQLDBC_UPDATECOUNT                ): { return "UPDATECOUNT"            ;}
	 case (SQLDBC_SELECTCOUNT                ): { return "SELECTCOUNT"            ;}
	 case (SQLDBC_CALLDBPROCEDURECOUNT       ): { return "CALLDBPROCEDURECOUNT"   ;}
	 case (SQLDBC_DELETECOUNT                ): { return "DELETECOUNT"            ;}
	 case (SQLDBC_FETCHCOUNT                 ): { return "FETCHCOUNT"             ;}
	 case (SQLDBC_SQLERRORCOUNT              ): { return "SQLERRORCOUNT"          ;}
	 case (SQLDBC_ROWNOTFOUNDCOUNT           ): { return "ROWNOTFOUNDCOUNT"       ;}
	 case (SQLDBC_FETCHROWCOUNT              ): { return "FETCHROWCOUNT"          ;}
	 case (SQLDBC_DATAREADSIZE               ): { return "DATAREADSIZE"           ;}
	 case (SQLDBC_DATAWRITESIZE              ): { return "DATAWRITESIZE"          ;}
	 case (SQLDBC_LONGDATAREADSIZE           ): { return "LONGDATAREADSIZE"       ;}
	 case (SQLDBC_LONGDATAWRITESIZE          ): { return "LONGDATAWRITESIZE"      ;}
  }
  return "UNKNOWN PROFILE COUNTER";
}

/*

*/
