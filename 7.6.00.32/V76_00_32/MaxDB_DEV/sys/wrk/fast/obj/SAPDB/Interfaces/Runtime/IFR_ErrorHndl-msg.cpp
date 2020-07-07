++
++@file           SQLDBCM.en
++@author         DL SAP DB INTERFACES
++@brief          System messages of the SAP Database Connectivity
++========== licence begin LGPL
++  Copyright 2000-2008 by SAP AG
++
++  This library is free software; you can redistribute it and/or
++  modify it under the terms of the GNU Lesser General Public
++  License as published by the Free Software Foundation; either
++  version 2.1 of the License, or (at your option) any later version.
++  
++  This library is distributed in the hope that it will be useful,
++  but WITHOUT ANY WARRANTY; without even the implied warranty of
++  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
++  Lesser General Public License for more details.
++
++  You should have received a copy of the GNU Lesser General Public
++  License along with this library; if not, write to the Free Software
++  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
++  ========== licence end
++ =========================================================
++ @(#) SQLDBCM.en(s) 7.6.00.32 Build 000-000-000 2008-05-09
++ =========================================================
++------------------ERRORS-----------------------------------------------
ENG -99999 SQLDBC ENGMSG 7.6.00.32 Build 000-000-000
ENG -10899 Unknown error
ENG -10803 Allocation of shared memory failed \"%s\" of size %d failed
ENG -10803 Allocation of shared memory failed (%s)
ENG -10808 Feature (%s) not implemented yet in module: %s line: %d
ENG -10001 System error: %s.
ENG -10003 Feature not supported for kernel version %d
ENG -10810 Session already in use
ENG -10807 Connection down: [%d] %.*s
ENG -10857 Missing database name for connection
ENG -10715 Missing user name for connection
ENG -10715 Missing password for connection
ENG -10709 Connection failed (%s:%s)
ENG -10100 Invalid value for option \'%s\' of SQLDBC_ConnectProperties
ENG -10101 Invalid value for ISOLATIONLEVEL (%s)
ENG -10101 Invalid value for ISOLATIONLEVEL (%d)
ENG -10102 Invalid value for PACKETCOUNT (%s)
ENG -10103 Invalid value for STATEMENTCACHESIZE (%s)
ENG -10104 Invalid value for KEY
ENG -10106 Invalid client application id (%s)
ENG -10757 XUSER error (%s)
ENG -10200 Function sequence error (data for execution expected)
ENG -10201 Function sequence error (no data for execution expected)
ENG -10202 Function sequence error (operation was aborted by disconnect)
ENG -10210 Invalid command state (No prepared SQL command)
ENG -10220 Calling %s is not allowed for %s.
ENG -10221 Command info to long [length is %d maximum length %d]
ENG -10300 Invalid parameter index
ENG -10300 Invalid parameter index %d
ENG -10300 Invalid parameter index 0
ENG -10304 Parameter/Column (%d) not bound.
ENG -10305 Parameter (%d) contains null pointer
ENG -10306 Negative buffer length for parameter (%d) not allowed (found %d)
ENG -10307 Invalid length or indicator value for parameter (%d)
ENG -10308 Non-character and non-binary data for parameter (%d) for piece wise insert not allowed
ENG -10310 Invalid column index (%d)
ENG -10312 Attempt to concatenate to a NULL or DEFAULT value for parameter (%d)
ENG -10313 Requested parameter (%d) was already processed
ENG -10318 Invalid LOB object
ENG -10319 Could not read LOB for parameter/column (%d)
ENG -10319 Could not write write LOB for parameter/column (%d)
ENG -10320 LOB already closed LOB for parameter/column (%d)
ENG -10861 Invalid use of null pointer for stream handle in parameter (%d)
ENG -10324 Stream with id (%d) not found
ENG -10325 Stream parameter (%d) is not an input parameter
ENG -10325 Stream parameter (%d) is not an input parameter
ENG -10862 Stream parameter (%d), error in read procedure
ENG -10862 Stream parameter (%d), error in write procedure
ENG -10326 No data found for stream (%d), but data was expected
ENG -10329 Invalid serial value requested, must be first or last value.
ENG -10330 Multiple stream output sequence error, unexpected '%s' part.
ENG -10332 No callback function supplied for stream parameter %d.
ENG -10333 Unsupported database type in parameter/column (%d).
ENG -10400 Conversion looses significant digits for parameter/column (%d)
ENG -10401 Conversion of parameter/column (%d) would truncate data
ENG -10811 Numeric overflow for parameter/column (%d)
ENG -10802 Invalid numeric value for parameter/column (%d)
ENG -10404 Conversion not supported
ENG -10404 Conversion not supported for parameter/column (%d)
ENG -10406 Invalid data length for UCS2 data for parameter/column (%d)
ENG -10410 Date/Time format is not supported for parameter (%d)
ENG -10411 Illegal DATE value for parameter/column (%d)
ENG -10412 Illegal TIME value for parameter/column (%d)
ENG -10413 Illegal TIMESTAMP value for parameter/column (%d)
ENG -10418 Buffer for DECIMAL too short for parameter/column (%d)
ENG -10419 Character value contains non-ASCII characters (%d)
ENG -10421 Could not convert too large LONG data for parameter/column (%d)
ENG -10422 Appending data to binary output not supported for parameter/column (%d)
ENG -10424 Conversion from %s data to binary would truncate data for parameter/column (%d)
ENG 100 Row not found
ENG -10500 Result set is closed
ENG -10501 Invalid position in resultset: before first row
ENG -10502 Invalid position in resultset: after last row
ENG -10503 The operation is not allowed for result set type FORWARD_ONLY
ENG -10504 Invalid position (%d) for rowset
ENG -10505 Result set type is UPDATABLE but row set type is READ_ONLY.
ENG -10506 Result set type is READ_ONLY but row set type is UPDATABLE
ENG -10507 Internal error: No metadata available for rowset operation
ENG -10508 Invalid encoding of column name for column %d for rowset operation
ENG -10509 The statement was not executed with updatable cursor concurrency
ENG -10856 Invalid row array size 0
ENG -10510 Degree of derived table does not match column list
ENG -10821 Session not connected
ENG -10601 SQL command is the empty string
ENG -10602 SQL command contains UNICODE character
ENG -10603 SQL command would generate a result set
ENG -10604 SQL command contains output parameters
ENG -10706 Statement too long
ENG -10605 Illegal schema modification, parameters/columns have changed type/number incompatibly
ENG -10901 No space left in request packet
ENG -10902 Invalid request packet structure
ENG -10903 Internal Error: LONGDATA part expected
ENG -10904 Internal error: Expected result count
ENG -10905 Invalid reply packet structure
ENG -10906 Internal Error: Entry for valindex %d not found
ENG -10907 Internal Error: Long descriptor has error flag set
