/*!*****************************************************************************
  module:       hcn384.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  Backint for SAP DB functions for handling the pipe2file
                converter programs 
  last change:  2000-11-29 11:46
  version:      7.2.5
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 2000-2005 SAP AG


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



*******************************************************************************/

#if !defined HCN384_H
#define HCN384_H

#include "hcn38.h"
#include "hcn381.h"
#include "hcn386.h"

int cn384_StartPipeToFileConverters(int &                            StartedProcesses,
                                    ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                    tcn381_APipeToFileConverter      Converter[MAX_STAGING_AREAS_CN38],
                                    const char * const               StagingAreaName[MAX_STAGING_AREAS_CN38],
                                    const teo00_Uint8                StagingAreaSize[MAX_STAGING_AREAS_CN38],
                                    tcn386_BackintOut              & Output);

int cn384_StartNeededPipeToFileConverters(int &                            StartedProcesses,
                                          ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                          tcn381_APipeToFileConverter      Converter[MAX_STAGING_AREAS_CN38],
                                          const char * const               StagingAreaName[MAX_STAGING_AREAS_CN38],
                                          tcn386_BackintOut              & Output);

int cn384_WaitForAnswersFromPipeToFileConverters(int                         & NumberOfAnswersAvailable,
                                                 int                         & NumberOfRunningProcesses,
                                                 int                         & NumberOfFilesPerBackintCall,
                                                 int                         & NumberOfFilesOfNextBackintCall,
                                                 int                           StartedProcesses,
                                                 tcn381_APipeToFileConverter   Converter[MAX_STAGING_AREAS_CN38],
                                                 unsigned                      MaxDelayOfBackintCall,
                                                 tcn386_BackintOut           & Output);

int cn384_WaitForAnswersFromPipeToFileConverters(int                            & NumberOfAnswersAvailable,
                                                 int                              TotalyNeededConverters,
                                                 int                              StartedConverters,
                                                 int                            & ConvertersFinishedWork,
                                                 int                            & EndedConverters,
                                                 int                            & MaxNumberOfFilesPerBackintCall,
                                                 int                            & NumberOfFilesOfNextBackintCall,
                                                 ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                                 unsigned                         MaxDelayOfBackintCall,
                                                 tcn386_BackintOut              & Output);

int cn384_RemoveFilesAndTellConverters(int                         StartedProcesses,
                                       tcn381_APipeToFileConverter Converter[MAX_STAGING_AREAS_CN38]);

int cn384_TellRunningConverters(int                              StartedProcesses,
                                ni31_List<tcn381_InputEntry *> & TheInputFileList);

int cn384_CleanUpConverters(int                              StartedProcesses,
                            ni31_List<tcn381_InputEntry *> & TheInputFileList);

#endif
