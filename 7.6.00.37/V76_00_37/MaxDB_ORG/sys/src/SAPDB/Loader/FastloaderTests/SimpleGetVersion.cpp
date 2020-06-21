/*!================================================================

 module:        SimpleGetVersion.cpp

 -------------------------------------------------------------------

 responsible:   D019882

 special area:  SQL

 description:   Gets the version from Loader API

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
#include "SAPDB/Loader/Loader_Loader.hpp"
#include "SAPDB/Loader/FastloaderTests/FastloaderTestUtils.h"


class SimpleGetVersion : public FastloaderTestCase
{
    public:
        SimpleGetVersion() : FastloaderTestCase("SimpleGetVersion")
        {}

    virtual LOADER_Bool run ()
    {
        LOADER_Retcode rc = LOADER_OK;

        Loader myLoader;
        const char* pszVersion = myLoader.getLoaderVersion();

        return m_success;
    }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  FastloaderTestFrame testframe (argc, argv);
  testframe.addTestCase (new SimpleGetVersion());
  if (testframe.run ()) 
    return 0;

  return 1;
}
