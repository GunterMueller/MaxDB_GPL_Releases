/*!**********************************************************************

  module: StudioUtil_Functions.hpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: StudioUtil

  description:  Utility functions for SQLStudio

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG




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



*/

#include "SQLStudio/Util/StudioUtil_Functions.hpp"

/*===========================================================================*/
/*                                                                           */
/*   Function name:     asciiToHexSegmented                                  */
/*                                                                           */
/*   Prototype:         int asciiToHexSegmented   (char *sString  ,          */
/*                                                 int   iSegmentSize)       */
/*                                                                           */
/*   Return value:      Number of bytes that were converted.                 */
/*   Parameters:        conv          Input data for conversions             */
/*   Description:       Converts the string given in conv->sInString from    */
/*                      ASCII to hexadecimal representation. Only            */
/*                      conv->iSegmentSize bytes of the string are converted */
/*                      during one call of the function, starting at         */
/*                      conv->iCurrentPos.                                   */
/*                                                                           *
/*                                                                           */
/*===========================================================================*/
int __stdcall asciiToHexSegmented (CONV_DATA *conv)
{
  /*===== Local Variables ===================================================*/
    char     *pChar;                // Pointer to the current character
    int       i;                    // Counter for the loop
    char      buffer[16];           // Buffer for the converted character
  /*=========================================================================*/

  /*
  +---------------------------------------------------------------------------+
  |  Check if either the input string or the ouput string is NULL. In that    |
  |  case return -1 to signal an error.                                       |
  +---------------------------------------------------------------------------+
  */
  if ((conv->sInString == NULL) || (conv->sOutString == NULL))
  {
    return -1;
  }

  /*
  +---------------------------------------------------------------------------+
  |  Reset the output string.                                                 |
  +---------------------------------------------------------------------------+
  */
  *conv->sOutString = '\0';

  /*
  +---------------------------------------------------------------------------+
  |  Convert each character of the input string to its hexadecimal represen-  |
  |  tation until either the number of characters per segement or the end of  |
  |  the input string has been reached.                                       |
  +---------------------------------------------------------------------------+
  */
  for (pChar = conv->sInString + conv->iCurrentPos, i = 0;
       (i < conv->iSegmentSize) && (conv->iCurrentPos < conv->iLength);
       pChar++, i++, conv->iCurrentPos++)
  {
    sprintf (buffer, "%.2X ", (int)*pChar);

    if (strlen (buffer) > 3)
    {
      strncat (conv->sOutString, &buffer[6], 3);
    }
    else
    {
      strncat (conv->sOutString, buffer, 3);
    }
  }


	return i;
}

