/*!================================================================

 module:        StmtExecute.cpp

 -------------------------------------------------------------------

 responsible:   D025086

 special area:  SQL

 description:   Example: Simple Execute

 ------------------------------------------------------------------


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




 ===================================================================*/

#include <stdio.h>
#include "Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"

class SelDual : public IFR_TestCase
{
public:
  SelDual ()
    : IFR_TestCase ("SelDual")
  {}

  virtual IFR_Bool run ()
  {
    IFR_Retcode rc;

    IFR_StringEncoding encoding;

    char *ld_path = getenv("LD_LIBRARY_PATH");
    if (ld_path)
      loginfo("LD_LIBRARY_PATH=%s\n", ld_path);
    else
      loginfo("LD_LIBRARY_PATH=IS NULL\n");

    static char *Cmd = (char *) "SELECT * FROM DUAL";
    unsigned short buffer_w[100];
    char *buffer = (char*)buffer_w;

    if (connection()->isUnicodeDatabase()) {
        convertToUCS2(buffer_w, sizeof(buffer_w), Cmd);
      encoding = IFR_StringEncodingUCS2Native;
    }
    else {
      strcpy (buffer, Cmd);
      encoding = IFR_StringEncodingAscii;
    }
    
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    
    rc = tc_errorcheck (stmt)->execute (buffer, encoding);
    if (rc == IFR_OK) {
      printf("Statement successfull executed\n");
    }
    else {
      printf("error : execution failed\n");
      logerror (stmt->error().getErrorText());      
    }
    IFR_ResultSet *resultset = tc_errorcheck (stmt)->getResultSet();
    if (resultset != NULL) {
      printf ("Resultset exists.\n");
    } else {
      printf ("No resultset exists.\n");
    }
    execSQL ("COMMIT WORK");
    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new SelDual ());
  if (testframe.run ()) 
    return 0;

  return 1;
}




