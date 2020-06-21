/*!================================================================
 module:    updinsert.cpp

 responsible: TTK

 special area: updatable rowset
 description:  insert by updatable rowset

 see:
 
 change history:


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




 ===================================================================*/

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include <string.h>

#define MAXLINES 10000

long id;
char text1[25], text2[225];

class sqlinitbench : public SQLDBC_TestCase
{
public:
  sqlinitbench ()
    : SQLDBC_TestCase ("sqlinitbench")
  {}

  virtual SQLDBC_Bool run ()
  {

    connection()->setAutoCommit(SQLDBC_FALSE);

    dropTable ("phpbench");
    execSQL("create table phpbench (id int, text1 char(20), text2 varchar(200))");

    SQLDBC_PreparedStatement *pStmt;

    pStmt = tc_errorcheck(connection())->createPreparedStatement ();

    tc_errorcheck(pStmt)->prepare("insert into phpbench values (?,?,?)");

    tc_errorcheck(pStmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &id, NULL, 0);
    tc_errorcheck(pStmt)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, text1, NULL, sizeof(text1));
    tc_errorcheck(pStmt)->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, text2, NULL, sizeof(text2));

    for (int i=0; i<MAXLINES; i++) {
      id = i;
      sprintf (text1, "%d. Zeile", i);
      sprintf (text2, "%s %s %s %s %s %s und ein langer Rattenschwanz %s",
               text1, text1, text1, text1, text1, text1, text1);  
      tc_errorcheck(pStmt)->execute();
    }
    
    if(m_success)
      tc_errorcheck(connection())->commit();

    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new sqlinitbench ());
  if (testframe.run ()) 
    return 0;

  return 1;
}
