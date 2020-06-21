/*!*****************************************************************************

  module      : IspcVersion.h

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of interface description code
  description : LC version

  ----------------------------------------------------------------------------

    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
    ========== licence end

*******************************************************************************/


#ifndef ISPCVERSION_H
#define ISPCVERSION_H


#define PROD_NAME_ISPCV               "SAP DB"
#define COMPANY_NAME_ISPCV            "SAP AG"
#define COPYRIGHT_RC_ISPCV            "Copyright \251 2000 by SAP AG" /* Resource Copyright Strings */
#define COPYRIGHT_ISPCV               "Copyright 2000 by SAP AG"
#define DATE_ISPCV                    "Date 2002-06-17"

#  define MAJOR_VERSION_NO_ISPCV         7
#  define MAJOR_VERSION_NO_STR_ISPCV    "7"

#  define MINOR_VERSION_NO_ISPCV         4
#  define MINOR_VERSION_NO_STR_ISPCV    "4"

#  define CORRECTION_LEVEL_ISPCV         3
#  define CORRECTION_LEVEL_STR_ISPCV    "3"

#  define BUILD_PREFIX_ISPCV             2
#  define BUILD_PREFIX_STR_ISPCV        "002"

 /*
 // --- Resource Version Strings
 */
# if defined(_WIN32)
#   define VERSION_MM_STR_ISPCV         MAJOR_VERSION_NO_STR_ISPCV "."  \
                                        MINOR_VERSION_NO_STR_ISPCV

#   define VERSION_MMC_STR_ISPCV        MAJOR_VERSION_NO_STR_ISPCV "."  \
                                        MINOR_VERSION_NO_STR_ISPCV "."  \
                                        CORRECTION_LEVEL_STR_ISPCV
 
#   define VERSION_MM_ISPCV             MAJOR_VERSION_NO_ISPCV,         \
                                        MINOR_VERSION_NO_ISPCV

#   define VERSION_MMC_ISPCV            MAJOR_VERSION_NO_ISPCV,         \
                                        MINOR_VERSION_NO_ISPCV,         \
                                        CORRECTION_LEVEL_ISPCV
# endif

#  define VERSION_STR_ISPCV             "%-9.9s %-8.8s Build %-3.3s-%-11.11s" 


#endif /* ISPCVERSION_H */

