/****************************************************************************

  module      : RTE_Version.h

  -------------------------------------------------------------------------

  responsible : Robin

  special area: RunTime and Kernel versions
  description : contains functions that return RunTime and Kernel versions strings

  last changed: 2004-11-16  17:52
  see also    : 

  -------------------------------------------------------------------------

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




*****************************************************************************/

/*!
  @fn SAPDB_Char *RTE_GetRTEVersion(void)

  @brief return the RunTime Version string of the running kernel

  This function is not completely thread safe as it uses an internal buffer. This buffer is filled
  only during the first call to the function, so it should be made sure that the first call to this
  function can be made by one thread only.


  @return  runtime version string
 */
externC SAPDB_Char *RTE_GetRTEVersion(void);

/*!
  @fn SAPDB_Char *RTE_GetKernelVersion(void)

  @brief return the Kernel Version string of the running kernel

  This function is not completely thread safe as it uses an internal buffer. This buffer is filled
  only during the first call to the function, so it should be made sure that the first call to this
  function can be made by one thread only.


  @return  kernel version string
 */
externC SAPDB_Char *RTE_GetKernelVersion(void);
