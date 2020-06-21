/*!**********************************************************************

  module: odbcreg

  -----------------------------------------------------------------------

  responsible:  Thomas Kötter

  special area: ODBC-Driver

  description:  Registry of the odbc driver (only Win)

  see also:     http://www.microsoft.com/data/odbc/

  -----------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (C) 2001 SAP AG

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

#ifndef WIN32
int main(int argc, char ** argv)
{
  printf("odbcreg not yet implemented.\n");
  return(-1);
}
#else

/* Invokes odbcreg in development tree (identified by environment
   INSTROOT. Serves only as proxy for the odbcreg in the development tree. */
int main(int argc, char ** argv)
{
  char exe[2048];

  sprintf (exe, "%s\\pgm\\odbcreg.exe", getenv ("INSTROOT"));

  _execvp (exe, argv);
  /* _execvp returns only, if an error has occured */
  printf ("Environment INSTROOT = %s\n", getenv ("INSTROOT"));
  printf ("Can't find %s\n", exe);
  return 0;
}
#endif
