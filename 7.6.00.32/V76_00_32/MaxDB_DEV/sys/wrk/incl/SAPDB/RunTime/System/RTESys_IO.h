/*!
  @file           RTESys_IO.h
  @author         RobinW, JoergM
  @brief          platform-independent file I/O routines
    
              Contains simple routines for opening files, reading from them etc..
              The routines work on all platforms. They were extracted from veo15.cpp
              in order to make them available outside the kernel, too.
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
*/



#ifndef RTESYS_IO_H
#define RTESYS_IO_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h" /* for tsp00_VfReturn_Param */
#include "SAPDBCommon/SAPDB_C_Cplusplus.h"
#include "RunTime/RTE_Types.h"

/*! @brief type definition for file handle */
typedef RTE_FileHandle tRTESys_IOHandle;
/*! @brief type definition for offset within a file */
typedef RTE_FileOffset tRTESys_IOPosition;
/*! @brief the size of an error message in bytes */
#define RTESYS_ERROR_MESSAGE_SIZE 256
/*! @brief type definition for system error message */
typedef SAPDB_Char RTESys_ErrorMessage[RTESYS_ERROR_MESSAGE_SIZE];

#if defined(_WIN32)

/*! @brief invalid handle (platform dependend)*/
# define INVALID_HANDLE_RTESYS_IO   ((RTE_FileHandle)INVALID_HANDLE_VALUE)
/*! @brief type definition for security attribute of a file (platform dependend)*/
    typedef LPSECURITY_ATTRIBUTES tRTESys_IOSecurityAttrib;
/*! @brief default security attribute (platform dependend)*/
# define DEFAULT_SECURITY_RTESYS_IO NULL

# define DEFAULT_ATTRIBUTE_RTESYS_IO  -1
# define READONLY_ATTRIBUTE_RTESYS_IO FILE_ATTRIBUTE_READONLY

#else

/*! @brief invalid handle (platform dependend)*/
# define INVALID_HANDLE_RTESYS_IO   ((RTE_FileHandle)-1)
/*! @brief type definition for security attribute of a file (platform dependend)*/
    typedef int tRTESys_IOSecurityAttrib;
/*! @brief default security attribute (platform dependend)*/
# define DEFAULT_SECURITY_RTESYS_IO ((tRTESys_IOSecurityAttrib)0)

# define DEFAULT_ATTRIBUTE_RTESYS_IO  -1
# define READONLY_ATTRIBUTE_RTESYS_IO 0440

#endif

/*! @brief modes that can be supplied in an open call */
typedef enum { RTESys_IOReadWrite,
               RTESys_IOReadOnly, 
               RTESys_IOWriteOnly,
               RTESys_IOReadWriteShared,
               RTESys_IOReadOnlyShared,
               RTESys_IOWriteOnlyShared,
               RTESys_IOReadWriteExclusive,
               RTESys_IOReadOnlyExclusive,
               RTESys_IOWriteOnlyExclusive } tRTESys_IOOpenMode;

/*! @brief modes that can be supplied in a seek call */
typedef enum { RTESys_IOSeekSet,
               RTESys_IOSeekEnd, 
               RTESys_IOSeekCurrent } tRTESys_IOSeekMode;

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*!
  @brief get last system error code
  @return system error code (maybe input for RTESys_StrError()
 */
externC RTE_SystemRc RTESys_GetLastError();

/*!
  @brief convert system error code into string
  @param errCode [in] system error code
  @param spaceToFill [in] space to write the system error message to
  @return pointer to system error message for given system error code
 */
externC SAPDB_Char *RTESys_StrError( RTE_SystemRc errCode, 
                                     RTESys_ErrorMessage VAR_ARRAY_REF spaceToFill);

/*! @name Simple Kernel File Interface */
 /*@{*/

externC void RTESys_IOCheckExist( const SAPDB_Char * Filename,
                                  SAPDB_Bool  *DoesExist,
                                  SAPDB_Bool  *IsDirectory,
                                  tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );
/*!
   @brief          Open a file
   @param          FileHandle [out] The file handle returned by open
   @param          Path [in] The file name and path information
   @param          FileMode [in] The mode how to open the file (readonly,readwrite,writeonly)
   @param          DoCreate [in] Flag showing to return an error file exists
   @param          Security [inout] The scerurity descrptior on NT ( on UNIX the access mask )
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok if failed )
   @return         none

 */

externC void RTESys_IOOpen ( RTE_FileHandle  VAR_VALUE_REF FileHandle,
                             const SAPDB_Char           *Path,
                             const tRTESys_IOOpenMode   FileMode,
                             const SAPDB_Bool                 DoCreate,
                             tRTESys_IOSecurityAttrib   Security,
                             tsp00_VfReturn_Param       VAR_VALUE_REF ReturnStatus );

/*!
   @brief          Close a file
   @param          FileHandle [in] The file handle returned by a previous call to RTESys_IOOpen
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok if failed )
   @return         none

 */

externC void RTESys_IOClose ( const RTE_FileHandle FileHandle,
                              tsp00_VfReturn_Param   VAR_VALUE_REF ReturnStatus );

/*!
   @brief          Write a fixed size buffer to a file
   @param          FileHandle [in] The file handle returned by a previous call to RTESys_IOOpen
   @param          Buffer [in] The buffer containing the information to write
   @param          WantedSizeInBytes [in] Number of bytes to write
   @param          WrittenSizeInBytes [out] Number of Bytes written
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok if failed )
   @return         none

 */

externC void RTESys_IOWrite ( const RTE_FileHandle   FileHandle,
                              const void               *Buffer,
                              const RTE_FileOffset WantedSizeInBytes,
                              RTE_FileOffset       VAR_VALUE_REF WrittenSizeInBytes,
                              tsp00_VfReturn_Param     VAR_VALUE_REF ReturnStatus );

/*!
   @brief          Read a fixed amount of data into a buffer from a file
   @param          FileHandle [in] The file handle returned by a previous call to RTESys_IOOpen
   @param          Buffer [out] The buffer containing the information read if sucessfull
   @param          WantedSizeInBytes [in] Maximum number of bytes to read
   @param          ReceivedSizeInBytes [out] Number of Bytes read
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok if failed )
   @return         none

 */

externC void RTESys_IORead ( const RTE_FileHandle   FileHandle,
                             void                     *Buffer,
                             const RTE_FileOffset WantedSizeInBytes,
                             RTE_FileOffset       VAR_VALUE_REF ReceivedSizeInBytes,
                             tsp00_VfReturn_Param     VAR_VALUE_REF ReturnStatus );

/*!
   @brief          Seek to the given position in file
   @param          FileHandle [in] The file handle returned by a previous call to RTESys_IOOpen
   @param          Position [in] The position where to seek
   @param          SeekMode [in] How to seek (Absolut,From End of File, Relativ to current position)
   @param          NewPosition [out] New absolut position after seek
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_noseek if failed )
   @return         none

 */

externC void RTESys_IOSeek ( const RTE_FileHandle   FileHandle,
                             const RTE_FileOffset Position,
                             const tRTESys_IOSeekMode SeekMode,
                             RTE_FileOffset       VAR_VALUE_REF NewPosition,
                             tsp00_VfReturn_Param     VAR_VALUE_REF ReturnStatus );

/*!
   @brief          Make a copy of a given file
   @param          Original [in] The path to the original file
   @param          Destination [in] The path to the destination
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok or vf_noseek if failed )
   @param          traceOutputFunc [in] function to call for trace output. May be NULL
   @return         none


     If copy file does not exist, it is created. If copy file already exists, it is overwritten!

 */

typedef void (*RTESys_IOTraceOutputFuncFuncType)(   const tsp00_Char *    Routine,
                                                    const tsp00_Char *    Filename );


externC void RTESys_IOCopyFile ( const SAPDB_Char     *Original,
                                 const SAPDB_Char     *Destination,
                                 tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus,
                                 RTESys_IOTraceOutputFuncFuncType traceOutputFunc );

/*!
   @brief          Make a copy of a given file
   @param          Original [in] The path to the original file
   @param          Destination [in] The path to the destination
   @param          DestinationAttribute [in] attribute to be given to the destination file. Up to now, 
                   the only supported values are DEFAULT_ATTRIBUTE_RTESYS_IO and READONLY_ATTRIBUTE_RTESYS_IO 
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok or vf_noseek if failed )
   @param          traceOutputFunc [in] function to call for trace output. May be NULL
   @return         none


     If copy file does not exist, it is created. If copy file already exists, it is overwritten!

 */

externC void RTESys_IOCopyFileWithDestinationAtribute
                              ( const tsp00_Char                   *Original,
                                const tsp00_Char                   *Destination,
                                SAPDB_Int                           DestinationAttribute,
                                tsp00_VfReturn_Param               *ReturnStatus,
                                RTESys_IOTraceOutputFuncFuncType    traceOutputFunc );

/*!
   @brief          Unlink a file
   @param          FileName [in] The path to the file to remove
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok if failed )
   @return         none


     If file is still open, vf_notok is returned.
     If file does exist and is successfull removed, vf_ok is returned.

 */

externC void RTESys_IOUnlink ( const SAPDB_Char     *FileName,
                               tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );

/*!
   @brief          Get path to directory where message files are stored
   @param          WorkingDirectory [out] The path to the root of all relativ specified message files
   @param          ReturnStatus [out] Status enumeration ( vf_ok if ok, vf_notok if failed )
   @return         none


     This returns the path to the directory that is used for all relativ specified message files.
     It should be identical to the WorkingDirectory of the database.

 */

externC void RTESys_IOGetWorkingDirectory( tsp00_Pathc          VAR_ARRAY_REF WorkingDirectory,
                                           tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );


/*!
   @brief          Check if Filename is a valid string
   @param          Filename [in] The path to the root of all relativ specified message files
   @return         true if Filename is a valid string, false if not


     This function checks if the parameter Filename is a valid string, that means if it is not NULL and does not
     point to an empty string.

 */

externC SAPDB_Bool RTESys_IOCheckFilename(const SAPDB_Char *Filename);

/*!
   @brief          Rename given file
   @param          OldFilename [in] The path to the root of all relativ specified message files
   @param          NewFilename [in] The new filename
   @param          ReturnStatus [out] The result of the rename action (vf_ok if renamed, vf_notok else)


     This function renames the given file

 */

externC void RTESys_IORename(const SAPDB_Char *OldFilename,
                             const SAPDB_Char *NewFilename,
                             tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus);

/*!
   @brief          Change access mode of given file
   @param          filePath [in] The path to the file to change
   @param          newMode [in] The requested mode (RTESys_IOReadOnly,RTESys_IOReadWrite or RTESys_IOWriteOnly )
   @param          ReturnStatus [out] The result of the rename action (vf_ok if now in that mode, vf_notok else)


     This function changes access mode of the given file

 */

externC void RTESys_IOChangeMode ( const SAPDB_Char     *filePath,
                                   tRTESys_IOOpenMode  newMode,
                                   tsp00_VfReturn_Param VAR_VALUE_REF ReturnStatus );
/*@}*/
#endif  /* RTESYS_IO_H */
