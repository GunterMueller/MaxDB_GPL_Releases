/*!
  @file           droptables.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Drop the table from a testrun to reduce the space for 
  the next run.
  @keywords       

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include <string.h>

class DropTables : public SQLDBC_TestCase
{
public:
  DropTables ()
    : SQLDBC_TestCase ("DropTables")
  {}

  virtual SQLDBC_Bool run ()
  {
    dropTable("BATCHINSERT_TEST");
    dropTable("BIGINTEGER_TEST");
    dropTable("BLOBTST001");
    dropTable("CHARASCII_TEST");
    dropTable("DATECHAR_TEST");
    dropTable("DATEDATE_TEST");
    dropTable("DEX_CHAR_TEST");
    dropTable("DEX_INT4_TEST");
    dropTable("DEX_LONGDATA_TEST");
    dropTable("DEX_PARSEAGAIN");
    dropTable("DEX_SEQ_TEST");
    dropTable("FETCHPREVIOUS");
    dropTable("FETCHTYPES");
    dropTable("FIXEDCHAR_INSERTTEST");
    dropTable("FIXEDCHAR_TEST");
    dropTable("FLOATCHAR_TEST");
    dropTable("FLOATFLOAT_TEST10");
    dropTable("FLOATFLOAT_TEST38");
    dropTable("FLOATNAN_TEST");
    dropTable("INTEGER_TEST");
    dropTable("MASSNAVIGATION");
    dropTable("MULTITHREAD_TEST");
    dropTable("PACKET4");
    dropTable("SQLNUM_TEST");
    dropTable("TEST_NAVIGATION");
    dropTable("TEST_TABLEDEF");
    dropTable("TIMECHAR_TEST");
    dropTable("TIMETIME_TEST");
    dropTable("UPDDELETE");
    dropTable("UPDINSERT");
    dropTable("UPDUPDATE");
    execSQL("COMMIT WORK");
    return m_success;
  }

};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new DropTables());
  if (testframe.run ()) 
    return 0;

  return 1;
}


