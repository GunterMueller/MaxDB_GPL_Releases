/*!---------------------------------------------------------------------
  @file           RTEConf_BuildParamFileName.h
  @author         RobinW
  @brief          RTE Configuration: BuildParamFileName
  @see            

\if EMIT_LICENCE

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



\endif
---------------------------------------------------------------------*/


/*!
   @brief     get the complete path to the parameter file including the current
              version of the parameter file


   @param     DBNameC               [in] the name of the database
   @param     ParamVersion          [in] current version of the parameter file
   @param     ParamFileNameWithPath [in] pointer to memory area to which the path is written
   @param     RteError              [in] pointer to tsp01_RteError structure to which an error 
                                    message is written if the routine fails
   @return value   true if call suceeded, false if call failed

 --------------------------------------------------------------------*/

externC SAPDB_Bool RTEConf_BuildParamFileNameWithPath ( SAPDB_Char      *DBNameC,
                                                        SAPDB_Int4      ParamVersion,
                                                        SAPDB_Char      *ParamFileNameWithPath,
                                                        tsp01_RteError  *RteError);
