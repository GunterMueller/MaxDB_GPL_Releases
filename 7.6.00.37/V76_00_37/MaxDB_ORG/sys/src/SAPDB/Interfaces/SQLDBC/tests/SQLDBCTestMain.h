/*!================================================================
 module:    SQLDBC_TestMain.h

 responsible: D025086

 special area:
 description: Special main for all tests to suppress the upcomming
              messagebox on Windows in case of an exception 

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
#ifndef SQLDBC_TESTMAIN_H
#define SQLDBC_TESTMAIN_H
#ifdef IPT_MAIN
#include "SAPDB/Interfaces/Benchmark/IPT/IPT_Main.hpp" /* no check */
#else
#if !defined (SQLDBC_NO_MAIN_FUNC) && defined (WIN32)
int mainproc(int argc, char **argv);
int main(int argc, char **argv);

void CPCexception(char *szStr);

int main(int argc, char **argv)
#define main mainproc
{
#ifndef CATCH_ALL
  __try {
#endif
    
    return(mainproc(argc, argv));
    
#ifndef CATCH_ALL
  }__except(CPCexception(argv[0]), 1)
    {
      exit(1);
    }
#endif
}

#ifndef CATCH_ALL
void CPCexception(char *szStr)
{
  fprintf(stderr, "ERROR: An unhandled exception was produced from %s.", szStr);
}
#endif
#endif // SQLDBC_NO_MAIN_FUNC
#endif // IPT_MAIN
#endif // SQLDBC_TESTMAIN_H
