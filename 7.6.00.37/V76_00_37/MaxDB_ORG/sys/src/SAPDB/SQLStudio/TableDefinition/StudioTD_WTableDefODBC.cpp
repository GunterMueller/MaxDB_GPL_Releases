/*!===========================================================================

  module:       StudioTD_WTableDefODBC.cpp

  ----------------------------------------------------------------------------

  responsible:  MarkusS

  special area: SQL Studio

  description:  Implementation of the ODBC enabled table definition class StudioTD_WTableDefODBC.

  created:      2000-05-29

  updated:      2000-05-29

  ----------------------------------------------------------------------------

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




============================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SQLStudio/TableDefinition/StudioTD_WTableDefODBC.h"


/*===========================================================================*
 *     StudioTD_WTableDefODBC::StudioTD_WTableDefODBC                                  *
 *===========================================================================*/
StudioTD_WTableDefODBC::StudioTD_WTableDefODBC ()
{
  setType (ClassTableDefODBC);
  
  mResult = NULL;
  mEnv    = NULL;
  mDBC    = NULL;
}

/*===========================================================================*
 *     StudioTD_WTableDefODBC::StudioTD_WTableDefODBC                                  *
 *===========================================================================*/
StudioTD_WTableDefODBC::StudioTD_WTableDefODBC (const StudioTD_WTableDefODBC & table)
    : StudioTD_TableDef (table)
{
  setType (ClassTableDefODBC);
  
  /*-------------------------------------------------------------------------*
   * The connection data must not be copied, because the connections are     *
   * managed by the connection pool of the web agent.                        *
   *-------------------------------------------------------------------------*/
  mResult = NULL;
  mEnv    = NULL;
  mDBC    = NULL;
}

/*===========================================================================*
 *     StudioTD_WTableDefODBC::~StudioTD_WTableDefODBC                                 *
 *===========================================================================*/
StudioTD_WTableDefODBC::~StudioTD_WTableDefODBC()
{
  if (mResult)
    delete mResult;
}


/*===========================================================================*
 *     StudioTD_WTableDefODBC::setConnection                                      *
 *===========================================================================*/
SAPDB_Bool StudioTD_WTableDefODBC::setConnection ( SQLHANDLE hdbc, SQLHENV henv)
{
  if (hdbc && henv)
  {
    mResult = new StudioOAL_WResult (hdbc, henv);
  
    if (!mResult)
      return false;
  }
  else
    mResult = NULL;
  
  mEnv = henv;
  mDBC = hdbc;
  
  return true;
}

  
/*===========================================================================*
 *     StudioTD_WTableDefODBC::executeSQL                                         *
 *===========================================================================*/
SAPDB_Bool StudioTD_WTableDefODBC::executeSQL (StudioTD_String& statement)
{
  return mResult->openResult (statement);
}


/*===========================================================================*
 *     StudioTD_WTableDefODBC::readResult                                         *
 *===========================================================================*/
SAPDB_Bool StudioTD_WTableDefODBC::readResult (StudioTD_String *record,SAPDB_Bool* bIsNull)
{
  SAPDB_Int        i;
  StudioTD_String  s;
  
  /*-------------------------------------------------------------------------*
   * Fetch the next record.                                                  *
   *-------------------------------------------------------------------------*/
  if (!mResult->fetch())
  {
    mResult->closeResult();
    return false;
  }
  
  /*-------------------------------------------------------------------------*
   * If there are no more records, reading the result is finished.           *
   *-------------------------------------------------------------------------*/
  if (mResult->noRows())
  {
    mResult->closeResult();
    return false;
  }
  
  /*-------------------------------------------------------------------------*
   * Read all columns of the fetched record.                                 *
   *-------------------------------------------------------------------------*/
  for (i = 0; i < mResult->getColCount(); i++)
  {	
    mResult->getColumnDataAsString (i + 1, s);
    record[i] = s;

	//check if column is null
    mResult->isColNull(i+1,*(bIsNull+i));	

  }
  
  return true;
}
