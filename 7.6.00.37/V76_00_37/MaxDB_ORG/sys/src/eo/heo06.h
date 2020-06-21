/*!
  @file           heo06.h
  @author         DanielD
  @special area   RTE | Client | Virtual File
  @brief          Handling of files and directories
  @see            http://p26326.bea.sap-ag.de/LZU/specs/VirtualFile.htm

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



#ifndef HEO06_H
#define HEO06_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp01.h"
#include "gsp05.h"
#include "hsp77.h"

/*!
   @brief          Most routines are provided both with Pascal and C "bindings":


   - Pascal routines end with 'p' 
   - C routines end with 'c' 
   - the length of a Pascal string is determined by searching for the last non blank character 
   - the length of a C string is determined by searching for the first zero byte 
   - calls to Pascal and C version can be intermixed, even on the same file handle
           All filenames may contain references to environment variables of the form
   - %varname% on NT
   - ${varname} on UNIX

 */


/*!
   @brief          

 */


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*! @name Reading and Writing */
 /*@(*/

/*!
   @brief          Opens a operating system file for reading or writing.
   @param          filename [in] The name of the file to be opened. If        the file is opened for reading, a name of "-" or "stdin" will use        standard input. If the file is opened for writing, a name of "-" or        "stdout" will use standard output, "stderr" will use standard error.
   @param          dataKind [in] 
                        * sp5vf_text: the file is interpreted as a sequence of text lines. End of line markers are stripped in read mode. In write mode, an end of line marker is inserted after every write operation.
                        * sp5vf_binary: the file is read and written without any interpretation.
   @param          fileMode [in] &lt;UL>        &lt;LI>
                        * sp5vf_read: the file is read starting with the first byte
                        * sp5vf_write: the file is truncated (or created), writing starts at the first byte
                        * sp5vf_append: writing starts at the end of the file,          the file is created if it does not exists.
   @param          buffering [in]
                        * sp5bk_buffered:
                        * sp5bk_unbuffered:
                        * sp5bk_disksynced: the runtime environment must guarantee that          the data is flushed to disk when sqlfopen returns
   @param          fileHandle [out] if the file could be opened, fhandle contains a        value to be used in subsequent calls to sqlfread, sqlfwrite,        sqlfseek, sqlfclose.
   @param          ferr [out] error information
   @return         none


       As far as possible with the particular operating system, the
       runtime environment should be implemented to guarantee the the same
       file cannot be opened simultaneously by different users for writing
       purposes.

 */


externC void
sqlfopenc (
    const char                  * rawFName,
    tsp05_RteDataKind_Param       dataKind,
    tsp05_RteFileMode_Param       fileMode,
    tsp05_RteBufferingKind_Param buffering,
    tsp00_Int4                  * fileHandle,
    tsp05_RteFileError          * ferr);
 
externC void
sqlfopenp (
    const tsp00_VFilename rawFName,
    tsp05_RteDataKind dataKind,
    tsp05_RteFileMode fileMode,
    tsp05_RteBufferingKind buffering,
    tsp00_Int4 *fileHandle,
    tsp05_RteFileError *ferr);
 

/*!
   @brief          Opens operating system file for reading or writing
      using a character encoding.
   @param          filename [in] The name of the file to be opened. If        the file is opened for reading, a name of "-" or "stdin" will use        standard input. If the file is opened for writing, a name of "-" or        "stdout" will use standard output, "stderr" will use standard error.
   @param          fileEncoding [in] - on read: the file is assumed to have this encoding. When fileEncoding is NULL, sqlfopenEncodedc will          try to detect the encoding from the file content. Console          input will be read according to the current locale.      - on write: data is written using this encoding. Console output          will be written according to the current locale.
   @param          clientEncoding [in] buffers specified by the client are assumed to      have this encoding
   @param          fileMode [in] 
                        * sp5vf_read: the file is read starting with the first byte 
                        * sp5vf_write: the file is truncated (or created), writing starts at the first byte
                        * sp5vf_append: writing starts at the end of the file,          the file is created if it does not exists.
   @param          buffering [in] 
                        * sp5bk_buffered:
                        * sp5bk_unbuffered
                        * sp5bk_disksynced: the runtime environment must guarantee that          the data is flushed to disk when sqlfopen returns
   @param          fileHandle [out] if the file could be opened, fhandle contains a        value to be used in subsequent calls to sqlfread, sqlfwrite,        sqlfseek, sqlfclose.
   @param          ferr [out] error information


       Remarks for sqlfopoen[pc] apply.

 */

externC void
sqlfopenEncodedc (
    const char *filename,
    const tsp77encoding ** fileEncoding,
    const tsp77encoding * clientEncoding,
    tsp05_RteFileMode_Param fileMode,
    tsp05_RteBufferingKind_Param buffering,
    tsp00_Int4 *fileHandle,
    tsp05_RteFileError *ferr);

/*!
   @brief          Opens a database save for reading or writing.
   @param          filename [in] The name of the file to be opened.
   @param          fileMode [in] 
                        * sp5vf_read: the file is opened for reading, the "magic record" is skipped
                        * sp5vf_write: the file is opened for writing, a "magic record" is written
                        * sp5vf_append: the file is opened for reading and writing,          the "magic record" is skipped
   @param          buffersize [in/out] the assumed buffer size of the save. If the file is opened for        reading and this parameter does not match the buffer size inside the        "magic record", an error is returned and buffersize contains the size        read from the "magic record"
   @param          fileHandle [out] if the file could be opened, fhandle contains a        value to be used in subsequent calls to sqlfread, sqlfwrite,        sqlfseek, sqlfclose.
   @param          bufptr [out] all reading/writing on the filehandle must be        done through this buffer, as some environments have
   @param          ferr [out] error information
   @return         none


       The file is always opend binary and unbuffered. This is currently
           the only reliable way to work with tape files on NT.
       Although this routine can be used to open devspaces, handling of
           the magic record is not transparent. Seeking to
           position 0 and writing data will destroy the magic record. 

 */


externC void
sqlfsaveopenc (
    const char *rawFName,
    tsp05_RteFileMode_Param fileMode,
    tsp00_Int4* buffersize,
    tsp00_Int4 *fileHandle,
    void** bufptr,
    tsp05_RteFileError *ferr) ;

/*!
   @brief          Closes the specified file.
   @param          fileHandle [in] the handle to the file to be closed
   @param          option [in] 
                        * sp5vf_close_normal: no special processing
                        * sp5vf_close_rewind: tape files are rewound
                        * sp5vf_close_next_tape: tape files are rewound,          the tape is ejected and the next tape is loaded. If          the loading of the next tape fails (or the device does          not support autoloading),          err.sp5fe_warning is set to sp5vfw_no_next_tape.
   @param          err [out] error information


      Any associated ressources (buffers) are released. The runtime
      environment is free to reuse the value of fhandle for
      a subsequent sqlfopen. 

 */


externC void
sqlfclosec (
    tsp00_Int4 fileHandle,
    tsp05_RteCloseOption_Param option,
    tsp05_RteFileError *ferr);
 
externC void
sqlfclosep (
    tsp00_Int4 fileHandle,
    tsp05_RteCloseOption option,
    tsp05_RteFileError *ferr);
 
/*!
   @brief          Reads data from the specified file.
   @param          fhandle [in] the file to read from
   @param          buf [out] the data will be read into this buffer
   @param          bufsize [in] the size of buffer
   @param          outlen [out] the number of bytes actually read
   @param          err [out] error information


   When the file was opened as sp5vf_binary, 
      sqlfreadp and sqlfreadp read at most bufsize bytes info buf.
      Fewer bytes may be transferred if the end of
      the file is reached, but err.sp5fe_result will still be set to vf_ok.
   [sqlfopenc] When the file was opened as sp5vf_text, 
      sqlfreadc reads at most (bufsize - 1) characters into buf up to
      the next end of line marker and ends the data with a zero byte.
      The end of line marker is not transferred. If buf is to small to
      hold the line, then err.sp5fe_warning is set to sp5vwf_no_eol_found.
      The rest of the line can be read with a subsequent sqlfreadc.
   [sqlfopenp] When the file was opened as sp5vf_text, 
      sqlfreadp reads at most bufsize characters into buf up to the
      next end of line marker. The end of line
      marker is not transferred. If buf is to small to hold the line,
      then err.sp5fe_warning is set to sp5vwf_no_eol_found.
      The rest of the line can be read with a subsequent sqlfreadp.

 */


externC void
sqlfreadc (
    tsp00_Int4 fileHandle,
    void* buf,
    tsp00_Longint bufSize,
    tsp00_Longint* outLen,
    tsp05_RteFileError *ferr);
 
externC void
sqlfreadp (
    tsp00_Int4 fileHandle,
    void* buf,
    tsp00_Longint bufSize,
    tsp00_Longint* outLen,
    tsp05_RteFileError *ferr);
 
/*!
   @brief          Write data from the specified file.
   @param          fhandle [in] the file to write to
   @param          buf [out] the data will be written from this buffer
   @param          inlen [in] the data len
   @param          err [out] error information


   When the file has been opened as sp5vf_binary 
      sqlfwritec and sqlfwritep write inlen bytes from buf to the file 
   When the file has been opened as sp5vf_text 
      sqlfwritec and sqlfwritep write inlen bytes from buf to the file
      followed by an end of line marker. If inlen
      is less than zero, then inlen is calculated using the C library
      function strlen. 

 */

externC void
sqlfwritec (
    tsp00_Int4 fileHandle,
    const void* buf,
    tsp00_Longint inLen,
    tsp05_RteFileError *ferr);
 
externC void
sqlfwritep (
    tsp00_Int4 fileHandle,
    const void* buf,
    tsp00_Longint inLen,
    tsp05_RteFileError *ferr);
 
/*!
   @brief          Moves the file pointer.
   @param          fileHandle [in] 
   @param          distance [in] 
   @param          whence [in]
                        * sp5vf_seek_begin:           sqlfseek positions the file pointer at byte distance.          The first byte is byte "0".
                        * sp5vf_seek_cur:           sqlfseek advances the file pointer by distance bytes.
                        * sp5vf_seek_end:           sqlfseek positions the file pointer at byte distance          relative to the end of the file.
   @param          err [out] error information


      A negative distance and reccount is allowed. 
      If the device is not capable of seeking, the runtime environment
          sets err.fe_result to vf_noseek. 
      There is no difference between the C (*c) and Pascal (*p) versions.

 */


externC void
sqlfseekc (
    tsp00_Int4 fileHandle,
    tsp00_Longint distance,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError *ferr);
 
externC void
sqlfseekp (
    tsp00_Int4 fileHandle,
    tsp00_Longint distance,
    tsp05_RteSeekKind whence,
    tsp05_RteFileError *ferr);
 
/*!
   @brief          Moves the file pointer (suitable for 64bit file systems).
   @param          fileHandle [in] 
   @param          distance [in] 
   @param          whence [in] 
                        * sp5vf_seek_begin: sqlflongseek positions the file pointer at byte (recsize * reccount). The first byte is byte "0".
                        * sp5vf_seek_cur:   sqlflongseek advances the file pointer by (recsize * reccount) bytes.
                        * sp5vf_seek_end:   sqlflongseek positions the file pointer at byte (recsize * reccount) relative to the end of the file.
   @param          err [out] error information


      A negative distance and reccount is allowed. 
      If the device is not capable of seeking, the runtime environment
          sets err.fe_result to vf_noseek. 
      There is no difference between the C (*c) and Pascal (*p) versions.

 */


externC void
sqlflongseekc (
    tsp00_Int4 fileHandle,
    tsp00_Longint distance,
    tsp00_Longint recordCount,
    tsp05_RteSeekKind_Param whence,
    tsp05_RteFileError *ferr);
 
externC void
sqlflongseekp (
    tsp00_Int4 fileHandle,
    tsp00_Longint distance,
    tsp00_Longint recordCount,
    tsp05_RteSeekKind whence,
    tsp05_RteFileError *ferr);
 
/*!
   @brief          Gets the current file position.
   @param          fileHandle [in] 
   @param          distance [out] the current file position from the beginning        of the file
   @param          err [out] error information


       This position may be used in a subsequent call to sqlfseek.
       If the value cannot be represented by an tsp00_Longint,
       sp5vf_notok is returned in err. 

 */


externC void
sqlftellc (
    tsp00_Int4 fileHandle,
    tsp00_Longint *pos,
    tsp05_RteFileError *ferr);
 
externC void
sqlftellp (
    tsp00_Int4 fileHandle,
    tsp00_Longint *pos,
    tsp05_RteFileError *ferr);
 
/*!
   @brief          Truncate the given file, but do not modify current write position.
   @param          fileHandle [in] 
   @param          newSize [in] New size of file
   @param          err [out] error information


       If the file does not exists, err.sp5fe_result is set to
       vf_notok. If the file exists, but cannot be truncated,
       err.sp5fe_result is set to vf_notok.
       If newSize is larger than current size, file is expanded, but
       the new space is left uninitialized. Otherwise it is truncated and additional
       space is freed.
       NOTE On NT after this call the current write position is reset to write position before
       this call ! This means the write position is not modified by this call! 
       Do not expect, that next write action is done at beginning of file or at the new end of
       the file! If you must ensure this, do so by an explicit sqlfseek[pc]() call in advance.
       Such behaviour is implemented identical for NT and UNIX ( it actually implements the
       SingleUNIX ftruncate() specification). 
       Again... sqlftruncate() does not modify current write position.

 */


externC void
sqlftruncatec (
    tsp00_Int4 fileHandle,
    tsp00_Longint newSize,
    tsp05_RteFileError *ferr);

externC void
sqlftruncatep (
    tsp00_Int4 fileHandle,
    tsp00_Longint newSize,
    tsp05_RteFileError *ferr);

/*@)*/

/*! @name File Functions */
 /*@(*/

/*!
   @brief          Erases the given file.
   @param          filename [in] 
   @param          err [out] error information


       If the file does not exist or if the file exists, but cannot be erased,
           err.sp5fe_result is set to vf_notok. 

 */


externC void
sqlferasec (
    const char *rawFName,
    tsp05_RteFileError *ferr);
 
externC void
sqlferasep (
    const tsp00_VFilename rawFName,
    tsp05_RteFileError *ferr);
 
/*!
   @brief          Returns information about the given file.
   @param          fname [in] 
   @param          finfo [out] a struct containing file information
   @param          err [out] error information


      If the file does not exists,
          finfo.sp5fi_exist is set to false and err.sp5fe_result is set to vf_ok.
          The runtime environment may set err.sp5fe_result to vf_notok to
          signal other errors (e.g. device not accessible), but
          finfo.sp5fi_exists must be set to false.
      finfo.fi_size and finfo.fi_size_x are set such that the file size
          can be calculated as
          (finfo.fi_size_x * csp_maxlongint) + finfo.fi_size (to support 64 bit file systems).

 */


externC void
sqlfinfoc (
    const char *rawFName,
    tsp05_RteFileInfo *finfo,
    tsp05_RteFileError *ferr);
 
externC void
sqlfinfop (
    const tsp00_VFilename rawFName,
    tsp05_RteFileInfo *finfo,
    tsp05_RteFileError *ferr);
 
/*!
   @brief          Copies a file from Src to Dst.
   @param          Src [in] the name of the source file
   @param          Dst [in] the name of the destination file
   @param          RteError [out] error information
   @returns        true if the file could be copied, false otherwise


  new 28.07.98

 */


externC boolean
sqlfilecopyc( char           *Src,
              char           *Dst,
              tsp01_RteError *RteError );

#if !defined(_WIN32)
/*!
   @brief          Create a named pipe (UNIX only)
   @param          rawFName [in] The name of the named pipe to be created
   @param          ferr [out] error information
   @return         none


       While NT uses a specific naming convention, named pipes in UNIX file systems
       are not identified by their name. So a special function call is needed to create
       a named pipe in an UNIX environment.

 */


externC void
sqlmkfifoc (
    const char *rawFName,
    tsp05_RteFileError *ferr);
 
externC void
sqlmkfifop (
    const tsp00_VFilename rawFName,
    tsp05_RteFileError *ferr);
#endif /* !_WIN32 */ 

/*@)*/

/*! @name Directory Functions */
 /*@(*/

/*!
   @brief          Opens a directory for iterating over the filenames.
   @param          dirname [in] the directory name
   @param          dirHandleP [out] the resulting handle to be used in subsequent calls        to sqlclosedirc
   @param          firstname [out] the name of the first file
   @param          ferr [out] error information


       The first filename is read.
       Wildcards in parameter dirname are not permitted. 

 */


externC void
sqlfopendirc (
    tsp00_C256           *dirname,
    void              **dirHandleP,
    tsp00_C256           *firstname,
    tsp05_RteFileError *ferr);

/*!
   @brief          Closes the given directory handle.
   @param          dirHandle [in] the directory handle
   @param          ferr [out] error information

 */


externC void
sqlfclosedirc (
    void               *dirHandle,
    tsp05_RteFileError *ferr);

/*!
   @brief          Reads the next name from a directory
   @param          dirHandle [in] the directory handle
   @param          fname [out] the resulting filename
   @param          ferr [out] error information

 */


externC void
sqlfreaddirc (
    void               *dirHandle,
    tsp00_C256           *fname,
    tsp05_RteFileError *ferr);

/*!
   @brief          Retrieves the name of the current directory.
   @param          dirbuf [out] the directory name is writtin into this buffer
   @param          buflen [in] the size of dirbuf
   @param          ferr [out] error information
   @returns        true if the directory name could be copied, false otherwise.


  new 28.07.98

 */


externC boolean
sqlgetcwdc (
    char           * dirbuf,
    int              buflen,
    tsp01_RteError * RteError);

/*!
   @brief          Changes the current directory.
   @param          dirname [out] the directory name
   @param          RteError [out] error information
   @returns        true if the directory name could be changed, false otherwise.


  new 28.07.98

 */


externC boolean
sqlchdirc (
    const char     * dirname,
    tsp01_RteError * RteError);

/*!
   @brief          Concatenates two path names to a new pathname.
   @param          target [out] the resulting path name
   @param          part1 [in] the first path (e.g. directory)
   @param          part2 [in] the second path
   @param          targetSize [in] the size of target
   @returns        a pointer to target if target contains the result, NULL otherwise.


  new 28.07.98


       When the second part is an absolute path, the first part is ignored.

 */


externC const char *
sqljoindirc (
    char           * target,
    const char     * part1,
    const char     * part2,
    unsigned int     targetSize);

/*!
   @brief          Creates a new directory.
   @param          PathToCreate [in] the directory name
   @param          RteError [out] error information
   @returns        true if the directory could be created, false otherwise.


  new 28.07.98

 */


externC boolean
sqlcreate_dirc ( char           *PathToCreate,
                 tsp01_RteError *RteError ) ;

/*!
   @brief          Deletes a empty directory.
   @param          PathToRemove [in] the directory name
   @param          RteError [out] error information
   @returns        true if the directory could be deleted, false otherwise.


  new 07.01.2000

 */

externC boolean
sqlremove_empty_dirc ( const char     *PathToRemove,
                       tsp01_RteError *RteError );

/*!
   @brief          Creates a new enumeration to iterate over special devices.
   @param          devKind [in]
                        * sdk_tapes_sp05: tape devices
                        * sdk_drives_sp05: drives (suitable only on NT)
   @param          longInfo [in] whether the enumeration shows only the        item names (False) or additional information (True)
   @param          additionalInfo [in] some enumerations require/allow additional infomation        (e.g. a filename pattern for UNIX tapes)
   @param          listHandle [out] a handle to be used in subsequent calls to        sqlspecialnext and sqlspecialclose
   @param          header [out] will contain the field names
   @param          ferr [out] error informationnew 28.07.98

 */


externC void
sqlspecialenum (
    tsp05_SpecialdevKind_Param   devKind,
    boolean                     longInfo,
    const char                * additionalInfo,
    tsp05_SpecialEnumeration ** listHandle,
    char                      * header,
    int                         headerSize,
    tsp05_RteFileError        * ferr);

/*!
   @brief          Copies information about the next item into buf.
   @param          listHandle [in] the handle to the enumeration
   @param          buf [out] the buffer containing the result
   @param          bufSize [in] the size of buf
   @param          ferr [in] error information    new 28.07.98

 */


externC void
sqlspecialnext (
    tsp05_SpecialEnumeration  * listHandle,
    char                      * buf,
    int                         bufSize,
    tsp05_RteFileError        * ferr);
   
/*!
   @brief          Closes the enumeration handle.
   @param          listHandle [in] the handle to the enumeration    new 28.07.98

 */


externC void
sqlspecialclose (
    tsp05_SpecialEnumeration  * listHandle);

/*@)*/

/*! @name Misc. */
 /*@(*/

/*!
   @brief          Closes all open file handles.

 */


externC void
sqlffinishc (void);
 
externC void
sqlffinishp (void);

/*!
   @brief          Erases the given file.
   @param          filename [in] 
   @param          err [out] error information
   @deprecated     

   use sqlferase instead


       If the file does not exists, err.sp5fe_result is set to
           vf_ok. If the file exists, but cannot be erased,
           err.sp5fe_result is set to vf_notok. 

 */


externC void
sqlfunlockc (
    const char *rawFName,
    tsp05_RteFileError *ferr);
 
externC void
sqlfunlockp (
    const tsp00_VFilename rawFName,
    tsp05_RteFileError *ferr);
 
/*!
   @brief          Reads password from console input
   @param          Prompt [in] Prompt to be written before password is read
   @param          Password [out] password read
   @param          err [out] error information
   @returns        true if a password was read, false otherwise.


       The password is prompted and read without echo. The result is NUL terminated C string
       stripped from Linefeed and CarriageReturn.
       If the password was read, err.sp5fe_result is set to vf_ok. If the password cannot be
       read err.sp5fe_result is set to vf_notok. 

 */


externC tsp00_Bool
sqlgetpass(const char *                     Prompt, 
           tsp00_Namec        VAR_ARRAY_REF Password, 
           tsp05_RteFileError VAR_VALUE_REF err);

/*@)*/

/*! @name Examples */
 /*@(*/

/*!
   @brief          sqlfopenc, sqlfreadc, sqlfclosec
   @code           
    
    include <stdio.h>
    include "heo06.h"
    
    int
    main (
        int argc,
        char * argv [])
    {
        tsp00_Int4            fhandle;
        char                buf [4096];
        tsp00_Longint         outLen;
        tsp00_Longint         outMax;
        tsp05_RteFileError  ferr;
    
        sqlfopenc (argv [1], sp5vf_text, sp5vf_read, sp5bk_buffered,
                   &fhandle, &ferr);
        while (ferr.sp5fe_result == vf_ok) {
            sqlfreadc (fhandle, buf, sizeof (buf), &outLen, &ferr);
            if (ferr.sp5fe_result == vf_ok) {
                fprintf (stdout, "'%s' ", buf);
                if (ferr.sp5fe_warning == sp5vfw_no_eol_found) {
                    fprintf (stdout, "(incomplete)");
                }
                fprintf (stdout, "\n");
            }
        }
        sqlfclosec (fhandle, sp5vf_close_normal, &ferr);
        return 0;
    }
    
    
    @endcode        

       Show how a file is opened, read until an error or end
       of file occurs and closed

 */


/*!
   @brief          sqlspecialenum, sqlspecialnext, sqlspecialclose
   @code           
    
    include <stdio.h>
    include "heo06.h"
    
    int
    main ()
    {
        tsp05_RteFileError ferr;
        tsp05_SpecialEnumeration * enumHandle;
        
        sqlspecialenum (sdk_tapes_sp05, 1, NULL, &enumHandle, buf, sizeof (buf), &ferr);
        while (ferr.sp5fe_result == vf_ok) {
            fprintf (stdout, "%s", buf);
            sqlspecialnext (enumHandle, buf, sizeof (buf), &ferr);
        }
        sqlspecialclose (enumHandle);
        return 0;
    }
    
    @endcode        
 */


/*@)*/

#endif  /* HEO06_H */
