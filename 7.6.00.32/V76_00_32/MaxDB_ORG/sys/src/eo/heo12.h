/*!
  @file           heo12.h
  @author         JoergM
  @special area   getopt Wrapper
  @brief          Needed to hide POSIXLY INCORRECT GNU getopt implementation
  @see            example.html ...

\if EMIT_LICENCE

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



\endif
*/



#ifndef HEO12_H
#define HEO12_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00_0.h"
#include "gsp00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
   @brief          This function warppes to support POSIX CORRECT getopt implementation
   @param          argc [in] argument count
   @param          argv [in] argument vector
   @param          optstring [in] option parsing input  extern int opterr Set this to 0 to suppress error messages of getopt
   @return         parsed option character
  &lt;UL>
      &lt;LI>EOF: End of options
      &lt;LI>'?':   Unknown option or missing option argument found
      &lt;LI>extern char *optarg points to the start of the option argument
      &lt;LI>extern int   optind is the index into argv of the next argument to process
      &lt;LI>extern int   optopt is the value of the error producing option character
&lt;/UL>


     See getopt() man pages for detailed information. This wrapper was needed to
     support LINUX or other ports that use GNU getopt() implementation

 */

externC int sqlgetopt(int argc, char * const argv[], const char *optString);

/*!
   @brief          This function return the current process id
   @param          pPid [out] Pointer of process id to be returned


   Dont mix this up with the SAPDB kernel idea of PROCESS_ID, which is 
   a tsp00_TaskId. 

 */

externC void sqlgetpid ( tsp00_Int4 *pid );

/*!
   @Description    Set the encoding type for command line options


  Only the programmer of the main routine knows the command line option
  encoding. This call allows him to tell his setting to other routines.
  This function should be called once before option parsing via sqlarg3.
  It influences the conversion of options especially for username and
  password.
  If not called before sqlarg3, sqlarg3 must assume arguments are passed as ASCII8
   @param          isUTF8encoded [in] true if arguments are UTF8 encoded, false if ASCII8

 */

externC void sqlSetOptionEncodingUTF8(bool isUTF8encoded);

/*!
   @Description    Get the encoding type for command line options


  If sqlSetOptionEncodingUTF8 was never called before,
  the default false (meaning ASCII8) is returned.
   @Return value   true (UTF8) or false (ASCII8).

 */

externC bool sqlIsOptionEncodingUTF8();

#endif  /* HEO12_H */
