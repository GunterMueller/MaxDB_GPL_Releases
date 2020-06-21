/*!
  module:       DBMTst_TpSimulator.cpp
  responsible:  BerndV
  description:  Simulator for tp executable of netweaver

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#include <stdio.h>
#include <string.h>

// tp pf=<profile> getlcconnectinfo <systemid> lc-name=<lcid> user-type=DBM
int main(int argc, char ** argv)
{
  // abuse the arguments
  // pf=<profile> to generate a error message
  // <systemid> for username and password

  if (argc >= 2) {
    if (strcmp(argv[1], "pf=error") == 0) {
      printf("A error occured\n");
    } else {
      if (argc >= 4) {
        printf("LC-INFO=%s\n", argv[3]);
      } else {
        printf("A error occured\n");
      } // end if
    } // end if
  } else {
    printf("A error occured\n");
  } // end if

} // end main