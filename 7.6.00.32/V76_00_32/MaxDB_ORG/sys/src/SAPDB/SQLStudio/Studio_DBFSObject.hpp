/*
    Copyright (c) 2000-2005 SAP AG

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



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

/*!**********************************************************************

  module: Studio_DBFSObject.hpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: WebQuery

  description:  class for keeping and showing db result

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/
 
#ifndef STUDIO_DBFSOBJECT_HPP
#define STUDIO_DBFSOBJECT_HPP

//some functions
#include "hwd101dbfs.h"

#include "SAPDB/SQLStudio/Studio_Connection.hpp"

class Studio_DBFSObject
{

public:

		/*!-------------------------------------------------------------------------
		  function:     StudioWeb_Result
		  description:  Constructor
		  arguments:	[in] poConnection, pointer to connection object
					    [in] sFSPrefix, prefix for database file system
						
      
		  return value: / 
      --------------------------------------------------------------------------*/
	Studio_DBFSObject(Studio_Connection* poConnection,
					  SAPDB_UInt2				nFSPrefix);

	~Studio_DBFSObject();

	/*!-------------------------------------------------------------------------
		function:      readFile
		description:   read a file from dbfs at once; open/close of file not needed
		arguments:     [in] sFilePath, complete path and filename
					   [in] nMaxFileSize, max size th file will have or be read					   
					   [out] sContent, content of file
			
		return value:  bytes read from file

	 --------------------------------------------------------------------------*/
	 SAPDB_Int4 readFile(const Tools_DynamicUTF8String& sFilePath, 
						 SAPDB_UInt4			  nMaxFileSize,
						 Tools_DynamicUTF8String& sContent);

	/*!-------------------------------------------------------------------------
		function:      readFileBytes
		description:   read a file from dbfs in chunks into a byte buffer
		arguments:     [in] fileDescriptor, numerical file descriptor
					   [in] buf, the buffer					   
					   [in] bufSize, the size of the buffer
					   [out] readBytes, the number of bytes read into the buffer					   
					   [out] sError, errorstring
			
		return value:  true, or false if an error occurred

	 --------------------------------------------------------------------------*/
     SAPDB_Bool readFileBytes(SAPDB_Int2                      fileDescriptor, 
								char                            buf [],
                                const SAPDB_ULong               bufSize,
								SAPDB_ULong                   & readBytes,
                                Tools_DynamicUTF8String       & sError);
				
    /*!-------------------------------------------------------------------------
		function:      getDBFSError
		description:   send a dbfs error 
		arguments:     [in] sResponse, container for HTTP response
			
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool getDBFSError(Tools_DynamicUTF8String &sOut);

	  /*!-------------------------------------------------------------------------
		function:      Status
		description:   get information about a file,folder, link
		arguments:     [in] sPath, full path (+filename) 
					   [out] Info, info to be populated
					   [out] sError, error text, if one occured
			
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool  status(Tools_DynamicUTF8String &sPath,st_db_stat &Info,Tools_DynamicUTF8String &sError);

	  /*!-------------------------------------------------------------------------
		function:      Status
		description:   get information about a file,folder, link
		arguments:     [in] sPath, full path 
					   [out] sError, error text, if one occured
			
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Int2  openDir(const Tools_DynamicUTF8String &sPath,Tools_DynamicUTF8String &sError);

	  /*!-------------------------------------------------------------------------
		function:      closeDir
		description:   close a directory
		arguments:     [in] lfno, file number
					   [out] sError, error text, if one occured
					   
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool  closeDir(SAPDB_Int2 lfno,Tools_DynamicUTF8String &sError);

	  /*!-------------------------------------------------------------------------
		function:      getDirEntry
		description:   get conent of directory
		arguments:     [in] lfno, file number
					   [out] sOut, content of directory or error text, if one occured
					   
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool  getDirEntry(SAPDB_Int2 lfno,Tools_DynamicUTF8String &sOut);

	  /*!-------------------------------------------------------------------------
		function:      createFolder
		description:   create a new folder
		arguments:     [in] sCompletePath, complete path of new folder
					   [out] sError, error text, if one occured
					   
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool createFolder(const Tools_DynamicUTF8String &sCompletePath,Tools_DynamicUTF8String &sError);


	  /*!-------------------------------------------------------------------------
		function:      deleteFolder
		description:   delete a folder
		arguments:     [in] sCompletePath, complete path of folder
					   [out] sError, error text, if one occured
					   
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool deleteFolder(const Tools_DynamicUTF8String &sCompletePath,Tools_DynamicUTF8String &sError);

	  
	  /*!-------------------------------------------------------------------------
		function:      deleteFile
		description:   delete a file
		arguments:     [in] sCompletePath, complete path of file
					   [out] sError, error text, if one occured
					   
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool deleteFile(const Tools_DynamicUTF8String &sCompletePath,Tools_DynamicUTF8String &sError);


	  /*!-------------------------------------------------------------------------
		function:      renameItem
		description:   rename a file or folder
		arguments:     [in] sCompleteOldPath, complete old path of item
					   [in] sCompleteNewPath, complete new path of item
					   [out] sError, error text, if one occured
					   
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool renameItem(const Tools_DynamicUTF8String &sCompleteOldPath,
						    const Tools_DynamicUTF8String &sCompleteNewPath,
						    Tools_DynamicUTF8String &sError);

	  /*!-------------------------------------------------------------------------
		function:      moveItem
		description:   move a file or folder
		arguments:     [in] sCompleteOldPath, complete old path of item
					   [in] sCompleteNewPath, complete new path of item
					   [out] sError, error text, if one occured
					   
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool moveItem(const Tools_DynamicUTF8String &sCompleteOldPath,
						 const Tools_DynamicUTF8String &sCompleteNewPath,
						 Tools_DynamicUTF8String &sError);

	  /*!-------------------------------------------------------------------------
		function:      copyItem
		description:   copy a file or folder
		arguments:     [in] sCompleteOldPath, complete old path of item
					   [in] sCompleteNewPath, complete new path of item
					   [out] sError, error text, if one occured
					   
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool copyItem(const Tools_DynamicUTF8String &sCompleteOldPath,
						  const Tools_DynamicUTF8String &sCompleteNewPath,
						  Tools_DynamicUTF8String &sError);


	  /*!-------------------------------------------------------------------------
		function:      createFile
		description:   create a new file
		arguments:     [in] sCompletePath, complete path of new folder
					   [out] sError, error text, if one occured
					   
		return value:  a numerical file descriptor

	  --------------------------------------------------------------------------*/
	  SAPDB_Int2 createFile(const Tools_DynamicUTF8String &sCompletePath,Tools_DynamicUTF8String &sError);

	  /*!-------------------------------------------------------------------------
		function:      openFile
		description:   open a file for writing
		arguments:     [in] sCompletePath, complete path of new folder
					   [out] sError, error text, if one occured
					   
					   
		return value:  a numerical file descriptor

	  --------------------------------------------------------------------------*/
	  SAPDB_Int2 openFile(const Tools_DynamicUTF8String &sCompletePath,						  
						  Tools_DynamicUTF8String &sError);


	  /*!-------------------------------------------------------------------------
		function:      openFile
		description:   open a file
		arguments:     [in] sCompletePath, complete path of new folder
					   [out] sError, error text, if one occured
					   [in] accessMode, either access_read or access_write
					   
					   
		return value:  a numerical file descriptor

	  --------------------------------------------------------------------------*/
	  SAPDB_Int2 openFile(const Tools_DynamicUTF8String &sCompletePath,						  
						  Tools_DynamicUTF8String &sError,
                          twd101OpenFlag accessMode);


	  /*!-------------------------------------------------------------------------
		function:      writeFile
		description:   writem data to a file
		arguments:     [in] lfno, file number
					   [in] buffer,buffer to write
					   [in] nBufferSize, size of buffer
					   [out] sError, error text, if one occured
					   
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Int4 writeFile(SAPDB_Int2   lfno,
						   void         *buffer,
						   SAPDB_Int4   nBufferSize,
						   Tools_DynamicUTF8String &sError);

	  /*!-------------------------------------------------------------------------
		function:      closeFile
		description:   close a file
		arguments:     [in] lfno, file number
					   [out] sError, error text, if one occured
					   
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool closeFile(SAPDB_Int2 lfno,Tools_DynamicUTF8String &sError);

	  /*!-------------------------------------------------------------------------
		function:      getConnection
		description:   get connection pointer of dbfs object
		arguments:     /
					   
		return value:  m_poConnection, connection pointer of dbfs object

	  --------------------------------------------------------------------------*/
	  const Studio_Connection* getConnection() const {return m_poConnection;}

private:

	/*!-------------------------------------------------------------------------
		function:      getValidDBFS
		description:   get a database file system
		arguments:     [in] sResponse, container for HTTP response
			
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool getValidDBFS(Tools_DynamicUTF8String &sResponse);

	//variables

	//database file system
   twd101DBFSP					m_pDBFS;   
   SAPDB_UInt2					m_nFSPrefix;

   Studio_Connection* m_poConnection;

};


#endif
