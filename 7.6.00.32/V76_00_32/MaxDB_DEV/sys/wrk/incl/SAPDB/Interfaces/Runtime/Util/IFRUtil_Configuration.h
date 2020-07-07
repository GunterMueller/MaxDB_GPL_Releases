/*!
  @file           IFRUtil_Configuration.h
  @author         D039759
  @ingroup        IFR_Common
  @brief          
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
#ifndef IFRUTIL_CONFIGURATION_H
#define IFRUTIL_CONFIGURATION_H

#include "Interfaces/Runtime/IFR_Types.h"

/**
 * @ingroup IFR_Common 
 * Common configuration, for trace and similar optional things.
 */
class IFRUtil_Configuration
{
public:
    static IFR_Retcode getTraceFlags(const char *user, 
                                     char *traceflags, 
                                     IFR_size_t length,
                                     char *errortext,
                                     IFR_size_t errortextlength
        );
    static IFR_Retcode setTraceFlags(const char *user, char *traceflags,
                                     char *errortext,
                                     IFR_size_t errortextlength
        );

    static IFR_Retcode getTraceFileName(const char *user, char *filename, IFR_size_t length,
                                        char *errortext,
                                        IFR_size_t errortextlength
        );
    static IFR_Retcode setTraceFileName(const char *user, char *filename,
                                        char *errortext,
                                        IFR_size_t errortextlength
        );
    static IFR_Retcode getFullTraceFileName(const char *user, char *filename, IFR_size_t length,
                                            char *errortext,
                                            IFR_size_t errortextlength
        );
    static IFR_Retcode getProfileFileName(const char *user, char *filename, IFR_size_t length,
                                        char *errortext,
                                        IFR_size_t errortextlength
        );
    static IFR_Retcode setProfileFileName(const char *user, char *filename,
                                        char *errortext,
                                        IFR_size_t errortextlength
        );
    static IFR_Retcode getFullProfileFileName(const char *user, char *filename, IFR_size_t length,
                                            char *errortext,
                                            IFR_size_t errortextlength
        );

    static IFR_Retcode getFullFileName(const char *user, char *inputfilename, char *filename, IFR_size_t length,
                                       char *errortext,
                                       IFR_size_t errortextlength
        );
    
    static IFR_Retcode getSharedMemoryName(const char *user, char *sharedmemoryname, IFR_size_t length,
                                           char *errortext,
                                           IFR_size_t errortextlength
        );
    static IFR_Retcode getFullSharedMemoryName(const char *user, char *sharedmemoryname, IFR_size_t length,
                                               char *errortext,
                                               IFR_size_t errortextlength
        );
    
};


#endif 
 
