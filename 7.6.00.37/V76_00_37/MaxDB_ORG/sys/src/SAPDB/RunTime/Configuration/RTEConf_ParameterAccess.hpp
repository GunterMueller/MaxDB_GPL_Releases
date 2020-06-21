/*!---------------------------------------------------------------------
  @file           RTEConf_ParameterAccess.hpp
  @author         FrankH, JoergM, RobinW
  @brief          RTE configuration parameters: class for kernel configurable parameter access
  @see            example.html ...
  
  first created  2000-03-22  17:49

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
---------------------------------------------------------------------*/


#ifndef CONF_PARAMETER_ACCESS_HPP
#define CONF_PARAMETER_ACCESS_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Messages.hpp"

#include "RunTime/System/RTESys_IO.h"  // file access routines 

#include "Container/Container_List.hpp"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/* 6 is cryptic... found in gsp00.h */
#define RTECONF_CRYPT_ARRAY_SIZE 6                          //!< internal only

#define RTECONF_BYTECODE_FOR_INTEGER_       ((SAPDB_Byte)0x11)  //!< internal only
#define RTECONF_BYTECODE_FOR_REAL_          ((SAPDB_Byte)0x22)  //!< internal only
#define RTECONF_BYTECODE_FOR_STRING_        ((SAPDB_Byte)0x33)  //!< internal only
#define RTECONF_BYTECODE_FOR_CRYPTINFO_     ((SAPDB_Byte)0x44)  //!< internal only
#define RTECONF_BYTECODE_FOR_PARAMCLASS_    ((SAPDB_Byte)0x55)  //!< internal only

#define RTECONF_CURRENT_VERSION             ((SAPDB_Int4)0)     //!< this should be used if no version is to be specified

#define RTECONF_MAXSTRINGLENGTH             ((SAPDB_Int4)256)   //!< the maximum length of a string parameter
#define RTECONF_MAXNAMELENGTH               ((SAPDB_Int4) 32)   //!< the maximum length of a  parameter name


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define STRCMP_UTF8(string1,string2)            strcmp((SAPDB_Char *)(string1),(SAPDB_Char *)(string2))         //!< internal only
#define STRNCMP_UTF8(string1,string2,length)    strncmp((SAPDB_Char *)(string1),(SAPDB_Char *)(string2),length) //!< internal only
#define STRLEN_UTF8(string)                     strlen((SAPDB_Char *)(string))                                  //!< internal only
#define STRCPY_UTF8(string1,string2)            strcpy((SAPDB_Char *)(string1),(SAPDB_Char *)(string2))         //!< internal only
 
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
   @brief copy a string
   Two different functions are provided to avoid the need for casting
   @param urString [in] SAPDB_Char string
   @return copy of SAPDB_Char string
 */
SAPDB_Char          *RTEConf_MakeACopyOfString(const SAPDB_Char *urString);
/*!
   @brief copy a string
   Two different functions are provided to avoid the need for casting
   @param urString [in] UTF8 string
   @return copy of UTF8 string
 */
const SAPDB_UTF8    *RTEConf_MakeACopyOfString(const SAPDB_UTF8 *urString);

class RTEConf_ParameterIterator;
class RTEConf_ParameterRecord;

/*!
  @class                RTEConf_Parameter
  @brief                Parameter access class
                        This class allows to handle XParam-Files and its individual parameters.

 */
class RTEConf_Parameter
{
public:
    /*!
        @brief  enumeration type that represents the different types of parameters possible
     */
    typedef enum 
    { 
        t_Integer = RTECONF_BYTECODE_FOR_INTEGER_, 
        t_Real = RTECONF_BYTECODE_FOR_REAL_, 
        t_String = RTECONF_BYTECODE_FOR_STRING_, 
        t_CryptInfo = RTECONF_BYTECODE_FOR_CRYPTINFO_, 
        t_ParamClass = RTECONF_BYTECODE_FOR_PARAMCLASS_,
        t_Invalid
    } Type;

    /*!
        @brief  type definition for the name of a parameter
     */
    typedef SAPDB_UTF8 const    * Name;

    /*!
        @brief  type definition for the value of an integer parameter
     */
    typedef SAPDB_Int8            Integer;

    /*!
        @brief  type definition for the value of an real parameter
     */
    typedef SAPDB_Real8           Real;

    /*!
        @brief  type definition for an element of the value of a string parameter
     */
    typedef SAPDB_UTF8            StringElement;

    /*!
        @brief  type definition for the value of an string parameter
     */
    typedef StringElement const * String;

    /*!
        @brief  type definition for the value of a crypted parameter
     */
    typedef SAPDB_Int4            CryptInfo[RTECONF_CRYPT_ARRAY_SIZE];

    /*!
        @brief  type definition for the time stamp of a parameter file
     */
	typedef RTE_ISystem::DateTime TimeStamp;

    /*!
        @brief  type definition for call back function to be called when a parameter is changed
     */
    typedef SAPDB_Bool (*UpdateFuncType)(   RTEConf_Parameter::Name,
                                            RTEConf_Parameter::Type,
                                            void *,
                                            void *,
                                            SAPDBErr_MessageList &);
     /*!
       @brief   ctor
                Construct the object, but does not do any I/O. This is done in Read() member function.
       @param   dbname                  [in]    name of the database to construct the parameter set for
       @param   version                 [in]    for dbmsrv parameter file versioning
       @param   maxLengthOfName         [in]    maximum length of parameter names allowed
       @param   maxLengthOfStringValue  [in]    maximum length of string parameter allowed
     */
    RTEConf_Parameter(const SAPDB_Char *dbname, SAPDB_Int4 version,SAPDB_Int4 maxLengthOfName,SAPDB_Int4 maxLengthOfStringValue);

     /*!
       @brief   ctor
                Construct the object, but does not do any I/O. This is done in Read() member function.
                This constructor uses its own allocator to prevent early initialization of RTEMem_RteAllocator.
                It uses the same default value as RTEConf_ParameterAccess ctor does.
       @param   dbname  [in]    name of the database to construct the parameter set for
     */
    RTEConf_Parameter(const SAPDB_Char *dbname);

    /*!
       @brief   copy ctor
                Construct the object using another object as template.
                The copies use independent storage for parameters, so value change
                on template afterwards have no effect on the object.
                Does not do any I/O, and copies state of dirty flag from template!
       @param   templateParameter   [in]    the template parameter class

     */
    RTEConf_Parameter(const RTEConf_Parameter &templateParameter);

    /*!
       @brief           assignment operator
                        Copy all the content from template parameter object, but keep original filename.
                        Does not do any I/O, but sets dirty flag!
       @param           templateParameter [in] the template parameter class

     */
    RTEConf_Parameter & operator=(const RTEConf_Parameter &templateParameter);

    /*!
       @brief           dtor
                        Frees any memory allocated inside the object but does no I/O activity.
                        I/O must be enforced by a Write() call.
     */
    ~RTEConf_Parameter();

    /*!
       @brief           Read the Parameter file and convert into internal memory structure
                        This does read in the complete parameter set and builds up the internal memory image.
                        If no file was found, no error is returned, but dataRead flag is set to false.

       @param           dataRead [out] flag to indicate file was read (true) or does not exist (false)
       @param           err [out] error list filled, if false is returned
       @return value    true if fileNotFound is filled, false if err is filled

     */
    SAPDB_Bool Read(SAPDB_Bool &dataRead, SAPDBErr_MessageList & err);

    /*!
       @brief           Write the Parameter file from internal memory structure if needed
                        This does write out the complete parameter set from internal memory image. If the
                        dirty flag was never set, it does nothing. If the file does not exist it is created.
       @param err [out] message list
       @return true if write succeeded, false if write failed
     */
    SAPDB_Bool Write(SAPDBErr_MessageList & err);

    /*!
       @brief          Add a block of binary data (structured like a parameter file) to an
                       internal storage. 
                       This can be used to feed the parameter class with the contents 
                       of a parameter file that are not stored as a file in a file system
                       (but - for example - stored inside a backup).
                       AddBinaryData() can be called more than once, all blocks are stored
                       internally in a list. After all blocks are put into the calls by calls
                       to AddBinaryData(), InterpretBinaryData() can be called to fill the
                       internal parameter set of the class with the contents of the binary 
                       blocks. 

       @param          binaryData [in] pointer to a block of binary data
       @param          length [in] size of the binary data block
       @param          err [out] error list filled, if false is returned
       @return value   true if type was filled, false if err was filled

     */
    SAPDB_Bool AddBinaryData(SAPDB_Byte *binaryData,SAPDB_UInt4 length,SAPDBErr_MessageList & err);

    /*!
       @brief          Fill the internal parameter set of the class with the data 
                       that has been stored by subsequent calls to AddBinaryData().
                       The internal storage that has been filled by the AddBinaryData()
                       calls is destroyed by InterpretBinaryData().

       @param          dataRead [out] true if at least one parameter could be read from the stored binary data
       @param          err [out] error list filled, if false is returned
       @return value   true if type was filled, false if err was filled

     */
    SAPDB_Bool InterpretBinaryData(SAPDB_Bool &dataRead,SAPDBErr_MessageList & err);

    /*!
       @brief          Get type of given parameter
          Search given parameter with its name and return the type found
       @param          name [in] parameter name
       @param          type [out] parameter type found if any
       @param          err [out] error list filled, if false is returned
       @return value   true if type was filled, false if err was filled

     */
    SAPDB_Bool GetType (Name name, Type & type,       SAPDBErr_MessageList & err);

    /*!
       @brief          Get a named and typed value
       @param          name [in] parameter name
       @param          value [out] typed value reference
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool GetValue(Name name, Integer & value,   SAPDBErr_MessageList & err);

    /*!
       @brief          Get a named and typed value
       @param          name [in] parameter name
       @param          value [out] typed value reference
       @param          updateOnline [out] indicates whether the parameter can be changed in online mode
       @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool GetValue(Name name,Integer & value, SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,   SAPDBErr_MessageList & err);

    /*!
       @brief          Get a named and typed value
       @param          name [in] parameter name
       @param          value [out] typed value reference. 
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool GetValue(Name name, Real & value,      SAPDBErr_MessageList & err);


    /*!
       @brief          Get a named and typed value
       @param          name [in] parameter name
       @param          value [out] typed value reference. 
       @param          updateOnline [out] indicates whether the parameter can be changed in online mode
       @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool GetValue(Name name,Real & value, SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,   SAPDBErr_MessageList & err);

    /*!
       @brief          Get a named and typed value
       @param          name [in] parameter name
       @param          value [out] typed value reference. The space for a 
                       string value has to be allocated by the caller!
                       To receive a reference to a string parameter
                       stored within the class, use the GetReference()
                       method!
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool GetValue(Name name, String  value,     SAPDBErr_MessageList & err);

    /*!
       @brief          Get a named and typed value
       @param          name [in] parameter name
       @param          value [out] typed value reference. The space for a 
                       string value has to be allocated by the caller!
                       To receive a reference to a string parameter
                       stored within the class, use the GetReference()
                       method!
       @param          updateOnline [out] indicates whether the parameter can be changed in online mode
       @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool GetValue(Name name,String value,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,   SAPDBErr_MessageList & err);

    /*!
       @brief          Get a named and typed value
       @param          name [in] parameter name
       @param          value [out] typed value reference. The space for a 
                       string value has to be allocated by the caller!
                       To receive a reference to a string parameter
                       stored within the class, use the GetReference()
                       method!
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */

    SAPDB_Bool GetValue(Name name, CryptInfo & value, SAPDBErr_MessageList & err);

    /*!
       @brief          Get a named and typed value
       @param          name [in] parameter name
       @param          value [out] typed value reference. The space for a 
                       string value has to be allocated by the caller!
                       To receive a reference to a string parameter
                       stored within the class, use the GetReference()
                       method!
       @param          updateOnline [out] indicates whether the parameter can be changed in online mode
       @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool GetValue(Name name,CryptInfo & value,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,   SAPDBErr_MessageList & err);

    /*!
       @brief          Get a named and typed value
       @param          name [in] parameter name
       @param          intValue [out] typed value reference. 
       @param          realValue [out] typed value reference. 
       @param          stringValue [out] typed value reference. The space for a 
                       string value has to be allocated by the caller!
                       To receive a reference to a string parameter
                       stored within the class, use the GetReference()
                       method!
       @param          cryptValue [out] typed value reference.
       @param          type [out] the type of the parameter
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool GetTypeAndValueDirect(Name name, Type & type, Integer & intValue, Real & realValue, String  stringValue, CryptInfo & cryptValue,  SAPDBErr_MessageList & err);

    /*!
       @brief          Get a named and typed value

       @param          name [in] parameter name
       @param          reference [out] typed value reference. This method exists for
                       string parameters only as there would be no
                       difference to the GetValue() method for other
                       types of parameters
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool GetReference(Name name, String & reference, SAPDBErr_MessageList & err);

    /*!
       @brief          Get a named and typed value

       @param          name [in] parameter name
       @param          reference [out] typed value reference. This method exists for
                       string parameters only as there would be no
                       difference to the GetValue() method for other
                       types of parameters
       @param          updateOnline [out] indicates whether the parameter can be changed in online mode
       @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool GetReference(Name name,String & reference, SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,   SAPDBErr_MessageList & err);

    /*!
       @brief          Get the RUNDIRECRTORY parameter. If it is stored as a relative path it is preceeded by the independend data path.

       @param          value [out] typed value reference. The space for a 
                       string value has to be allocated by the caller!
       @param          err [out] error list filled, if false is returned
       @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool GetRundirectoryAbsolutePath(String value, SAPDBErr_MessageList & err);

    /*!
        @brief          (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          updateOnlinePossible [out] indicates whether the parameter can be changed in online mode
        @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
        @param          err [out] error list filled, if false is returned
        @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool SetValue(Name name, const Integer & value, SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange, SAPDBErr_MessageList & err);
    /*!
        @brief          DEPRECATED!!!
        
          (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          updateOnlinePossible [out] indicates whether the parameter can be changed in online mode
        @param          updateByUser [in] update by user
        @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
        @param          err [out] error list filled, if false is returned
        @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool SetValue(Name name, const Integer & value, SAPDB_Bool updateOnlinePossible,SAPDB_Bool updateByUser,SAPDB_Bool permanentChange, SAPDBErr_MessageList & err)
    {
        return SetValue(name,value,updateOnlinePossible,permanentChange,err);
    }
    /*!
        @brief          (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          updateOnlinePossible [out] indicates whether the parameter can be changed in online mode
        @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
        @param          err [out] error list filled, if false is returned
        @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool SetValue(Name name, const Real & value, SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange,SAPDBErr_MessageList & err);
    /*!
        @brief          DEPRECATED!!!     
        (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          updateOnlinePossible [out] indicates whether the parameter can be changed in online mode
        @param          updateByUser [in] update by user
        @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
        @param          err [out] error list filled, if false is returned
        @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool SetValue(Name name, const Real & value, SAPDB_Bool updateOnlinePossible,SAPDB_Bool updateByUser,SAPDB_Bool permanentChange,SAPDBErr_MessageList & err)
    {
        return SetValue(name,value,updateOnlinePossible,permanentChange,err);
    }
    /*!
        @brief          (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          updateOnlinePossible [out] indicates whether the parameter can be changed in online mode
        @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
        @param          err [out] error list filled, if false is returned
        @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool SetValue(Name name, const String & value, SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange, SAPDBErr_MessageList & err);
    /*!
        @brief          DEPRECATED!!!     
        (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          updateOnlinePossible [out] indicates whether the parameter can be changed in online mode
        @param          updateByUser [in] update by user
        @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
        @param          err [out] error list filled, if false is returned
        @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool SetValue(Name name, const String & value, SAPDB_Bool updateOnlinePossible,SAPDB_Bool updateByUser,SAPDB_Bool permanentChange, SAPDBErr_MessageList & err)
    {
        return SetValue(name,value,updateOnlinePossible,permanentChange,err);
    }
    /*!
        @brief          (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          updateOnlinePossible [in] indicates whether the parameter can be changed in online mode
        @param          permanentChange [in] indicates whether a permanent change has been made to the parameter
        @param          err [out] error list filled, if false is returned
        @return         true if value was filled, false if err was filled

     */
    SAPDB_Bool SetValue(Name name, const CryptInfo & value,SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange, SAPDBErr_MessageList & err);
    /*!
        @brief          DEPRECATED!!! 
        
                        (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          updateOnlinePossible [in] indicates whether the parameter can be changed in online mode
        @param          updateByUser [in] update by user
        @param          permanentChange [in] indicates whether a permanent change has been made to the parameter
        @param          err [out] error list filled, if false is returned
        @return         true if value was filled, false if err was filled

     */
    SAPDB_Bool SetValue(Name name, const CryptInfo & value,SAPDB_Bool updateOnlinePossible,SAPDB_Bool updateByUser,SAPDB_Bool permanentChange, SAPDBErr_MessageList & err)
    {
        return SetValue(name,value,updateOnlinePossible,permanentChange,err);
    }
    /*!
        @brief          (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          err [out] error list filled, if false is returned
        @return         true if value was filled, false if err was filled

     */

    SAPDB_Bool SetValue(Name name, const Integer & value, SAPDBErr_MessageList & err)
    {
        return SetValue(name,value,false,true/*,(SAPDB_Char *)NULL*/,err);
    }

    /*!
        @brief          (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          err [out] error list filled, if false is returned
        @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool SetValue(Name name, const Real & value,      SAPDBErr_MessageList & err)
    {
        return SetValue(name,value,false,true/*,(SAPDB_Char *)NULL*/,err);
    }

    /*!
        @brief          (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          err [out] error list filled, if false is returned
        @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool SetValue(Name name, const String & value,    SAPDBErr_MessageList & err)
    {
        return SetValue(name,value,false,true/*,(SAPDB_Char *)NULL*/,err);
    }

    /*!
        @brief          (Client only ???)Set a named and typed value in memory image
                        This call does not produce any I/O. The given values are stored internally only.
                        Any string references to 'old value' are invalid after setting a string.
                        Setting an value to a different type is an error. It must be deleted first.
                        Setting a value with a unknown name, creates that value.
                        Any modification on internal memory image sets the 'dirty' flag

        @param          name [in] parameter name
        @param          value [in] typed value reference
        @param          err [out] error list filled, if false is returned
        @return value   true if value was filled, false if err was filled

     */
    SAPDB_Bool SetValue(Name name, const CryptInfo & value, SAPDBErr_MessageList & err)
    {
        return SetValue(name,value,false,true/*,(SAPDB_Char *)NULL*/,err);
    }

    /*!
       @brief           (Client only ???)Set the update online attribute for a given parameter in memory image.
                        The parameter must exist in the memory image, either read from the parameter file or created 
                        by the SetValue() method.

                        This call does not produce any I/O. The given attribute is stored internally only.
       @param           name [in] parameter name
       @param           updateOnlinePossible [in] true if the paramater can be updated in online mode, false if not
       @param           err [out] error list filled, if false is returned
       @return value    true if attribute was set, false if err was filled

     */

    SAPDB_Bool SetUpdateOnline(Name name, SAPDB_Bool updateOnlinePossible,   SAPDBErr_MessageList & err);

    /*!
       @brief           Return the update-online-possible attribute for a given parameter

       @param           name    [in] parameter name

       @return value    true if the parameter can be updated in online mode, false if not
                        If the parameter is not yet included in the parameter set, true
                        is returned.

     */
    SAPDB_Bool UpdateOnlinePossible(Name name);
    /*!
       @brief           DEPRECATED
       
       Return the update-online-possible attribute for a given parameter

       @param           name   [in] parameter name
       @param           sysDBA [in] is adminiatrator 

       @return value    true if the parameter can be updated in online mode, false if not
                        If the parameter is not yet included in the parameter set, true
                        is returned.

     */
    SAPDB_Bool UpdateOnlinePossible(Name name,SAPDB_Bool sysDBA)
    {
        return UpdateOnlinePossible(name);
    }
    /*!
       @brief           Register a call back function to be called

      (Kernel only) callback called when the given parameter is changed

       @param           name [in] parameter name
       @param           updateFunc [in] the function to be registered
       @param           context [in] context of function
       @param           err [out] error list filled, if false is returned
       @return          false if no call back function has been registered
     */
    SAPDB_Bool RegisterUpdateFunction(Name name, UpdateFuncType updateFunc,  void *context, SAPDBErr_MessageList & err);

    /*!
       @brief           Set the update online attribute for a given parameter in memory image.

       (Client only ???) The parameter must exist in the memory image, either read from the parameter file or created 
                        by the SetValue() method.

                        This call does not produce any I/O. The given attribute is stored internally only.
       @param           name [in] parameter name
       @param           permanentChange [in] true if the paramater can be updated in online mode, false if not
       @param           err [out] error list filled, if false is returned
       @return value    true if attribute was set, false if err was filled

     */

    SAPDB_Bool SetPermanentChange(Name name, SAPDB_Bool permanentChange,   SAPDBErr_MessageList & err);
    
    /*!
       @brief           Get the call back function that has been registered

       (Kernel only)  for changes on the given parameter

       @param           name [in] parameter name
       @param           context [out] context of function call
       @return value    NULL if no call back function has been registered
     */
    RTEConf_Parameter::UpdateFuncType GetUpdateFunc(Name name,void *&context);

    /*!
       @brief           (Client only)Request if any parameter modification was done
                        If the parameter set was modified after initial read or by 'assignment', but before any
                        write action the internal dirty flag is set.
       @return value    true if internal dirty flag is set, false if no I/O is needed.

     */

    SAPDB_Bool IsDirty()
	{
		return m_DirtyFlag;
	}

    /*!
       @brief           (Client only)Request to force parameter set write
                        With this call the internal dirty flag is set, which is used in Write() call to decide,
                        if any I/O is needed.

     */

    void MarkAsDirty()
	{
		m_DirtyFlag = true;
	};

    /*!
       @brief           (Client only)Delete a named value in memory image
                        This call does not produce any I/O. The given name is searched and if found
                        the reference in memory is removed.
                        Any string references to 'old value' are invalid after deleting a string.

       @param           name [in] parameter name
       @param           err [out] error list filled, if false is returned
       @return value    true if parameter was deleted, false if not found or other problem

     */

    SAPDB_Bool Delete(Name name, SAPDBErr_MessageList & err);

    /*!
       @brief           (Client only)Get the timestamp of last verify action
                        If the parameter set was verified if the SAPDB Kernel was able to reach ONLINE state with it.
                        This is marked by SAPDB by setting a time stamp inside the parameter file. This timestamp
                        is read here from memory image.

       @param           timeStamp [out] verification time
       @return value    true if the parameter has not been changed since the time indicated by timeStamp, 
                        false if changed have been made in the meantime

     */
    SAPDB_Bool GetLastVerifyTime(TimeStamp & timeStamp);

    /*!
       @brief           (SAPDB kernel only)Mark the parameter file as verified
                        If the parameter set was verified if the SAPDB Kernel was able to reach ONLINE state with it.
                        This function is called by SAPDB and sets the timestamp in the memory image and does the I/O
                        to the parameter file.

       @param           err [out] error list filled, if false is returned
       @return value    true if marking timestamp succeeded, false if I/O error occured

     */

    SAPDB_Bool MarkAsVerified(SAPDBErr_MessageList & err);

    /*!
       @brief           (SAPDB kernel only)Get the flag that indicates kernel crash
                        If SAPDB Kernel crashed before resetting parameter file crash flag, this call returns true.
                        This call should be able to return the old state, even after SetCrashFlag() was called...

       @return value    true if previous kernel crashed, false if previous kernel reached OFFLINE state correctly

     */

    SAPDB_Bool GetOriginalCrashFlag()
	{
		return m_CrashFlag;
	};

    /*!
       @brief           (SAPDB kernel only)Mark the parameter file crash flag
                        If the kernel starts, it sets this flag to make the next kernel able to detect a crash. This is
                        possible, since SAPDB kernel uses MarkStoppedCorrectly() to reset the flag. Since the function
                        has to survive any crash, it must do I/O directly, without update of 'original' crash flag.
                        This call does not influence the internal 'dirty' state!

       @param           err [out] error list filled, if false is returned
       @return value    true if marking crash flag succeeded, false if I/O error occured

     */

    SAPDB_Bool SetCrashFlag(SAPDBErr_MessageList & err);

    /*!
       @brief           (SAPDB kernel only)Reset the parameter file crash flag
                        Just before the SAPDB kernel reached OFFLINE state, it calls this function to reset the crash flag.
                        This call must do I/O directly, without update of 'original' crash flag.
                        This call does not influence the internal 'dirty' state!

       @param           err [out] error list filled, if false is returned
       @return value    true if resetting crash flag succeeded, false if I/O error occured

     */

    SAPDB_Bool MarkStoppedCorrectly(SAPDBErr_MessageList & err);

    /*!
       @brief           indicate if the parameter file is in old format

       @return value    true if a parameter file in old format has been read.

     */

    SAPDB_Bool FileInOldFormat()
    {
        return m_FileInOldFormat;
    };

    /*!
       @brief           Get an iterator objected used to scan parameters
                        The iterator is used primarily to scan through a complete parameter set, without knowledge of
                        its content. An iterator does not get invalid, if a Delete() was done on the parameterset. But
                        mixing up deletes with iterations can be very confusing...

       @param           beginningOfParameterName [in] if != NULL, the iterator contains only those
                        parameters of which the name begins with the string beginningOfParameterName points to.
                        If == NULL, the iterator contains all parameters of the parameter set. This is the
                        default, if this parameter is omitted.
       @return value    An iterator useful to scan parameter set content

     */

    RTEConf_ParameterIterator GetIterator(SAPDB_UTF8 *beginningOfParameterName = NULL);

private:

    friend class RTEConf_ParameterIterator;
    friend class RTEConf_ParameterRecord;
    friend class CharacterBuffer;

    SAPDB_Int4 CalcIndex(SAPDB_Char *Identifier);


    static SAPDB_Bool m_IntelByteOrder;

    static void ByteOrderSwapInt(SAPDB_Int4 *pTheInt4);
    static void ByteOrderSwapInt(SAPDB_Int8 *pTheInt8);
    static void ByteOrderSwapReal(SAPDB_Real8 *pTheReal8);

    RTEConf_ParameterRecord *LookupRecordByName(Name name);

    void CheckIfStringValueTooLong(const SAPDB_UTF8 *stringValue,SAPDB_Bool &valueTooLong);
    void InsertRecord(Name name,const SAPDB_Char *paramClass,SAPDB_Bool updateOnlinePossible,SAPDB_Bool permanentChange,RTEConf_Parameter::Type type,SAPDB_Byte *pcValue,SAPDB_Bool &nameTooLong,SAPDB_Bool &valueTooLong);

    void ReadOldStyleXParamRecord(Name name,Type & Type,SAPDB_Byte *Value,tRTESys_IOHandle fileHandle=NULL);
    SAPDB_Bool ReadNewStyleXParamRecord(SAPDB_Int4 version,Name name,SAPDB_Bool &updateOnlinePossible,SAPDB_Byte &paramClassIndex,Type & Type,SAPDB_Byte *Value,SAPDB_Bool *pfEndReachedCorrectly,tRTESys_IOHandle fileHandle = NULL);
    void ReadFromBinaryData ( SAPDB_Byte *recordBuffer,SAPDB_Int4 bytesToRead,SAPDB_Int4 &bytesRead,SAPDB_Bool &returnStatus );
    void ReadStringFromBinaryData(String string,SAPDB_UInt4 maxLength,SAPDB_Bool *endMarkerRead);
    void ReadStringFromFile(tRTESys_IOHandle fileHandle,String string,SAPDB_UInt4 maxLength,SAPDB_Bool *endMarkerRead);

    SAPDB_Bool WriteCrashFlag(SAPDB_Byte value,SAPDBErr_MessageList & err);

    SAPDB_Int2 ReadInt2FromByteArray(SAPDB_Byte *ByteArray);
    void       WriteInt2ToByteArray (SAPDB_Byte *ByteArray, SAPDB_Int2 nNum);

    SAPDB_Bool WriteInt2ToFile(SAPDB_Int2 theInt,tRTESys_IOHandle fileHandle,SAPDBErr_MessageList & err);

    void CopyRecords(const RTEConf_Parameter &templateParameter);
    void DeleteRecords();

    SAPDB_Bool BuildFileNameIfNecessary(SAPDBErr_MessageList & err);

    SAPDB_Char *TypeName(RTEConf_Parameter::Type type);

    SAPDB_Byte GetClassIndex(const SAPDB_Char *className);

    const SAPDB_Char* GetClassName(SAPDB_Byte classIndex);

    void AddNewParamClass(const SAPDB_Char* className,SAPDB_Byte index);

    const SAPDB_Char* GetClassName(SAPDB_Int4 classIndex);

    SAPDB_UInt8 CalcChecksum(void);

/* number of lists for the characters from a to z */
#define NUMBER_OF_CHARS_FROM_A_TO_Z_    ('Z'-'A'+1)  //!< internal only
/* number of lists for the characters from 0 to 9 */
#define NUMBER_OF_CHARS_FROM_0_TO_9_    ('9'-'0'+1)  //!< internal only

/* the total number of lists: one for names shorter than KEY_INDEX_, one for each character from A to Z, one for each character
   from 0 to 9 and one list for the underscore */
#define TOTAL_NUMBER_OF_LISTS_ (1 + NUMBER_OF_CHARS_FROM_A_TO_Z_ + NUMBER_OF_CHARS_FROM_0_TO_9_ + 1)  //!< internal only

    RTEConf_ParameterRecord *m_ParameterRecords[TOTAL_NUMBER_OF_LISTS_];

    RTE_Path m_FileName;

    TimeStamp m_VerifyTimeStamp;

    SAPDB_Bool m_DirtyFlag;

    SAPDB_Bool m_CrashFlag;

    SAPDB_UInt4 m_maxLengthOfName;
    SAPDB_UInt4 m_maxLengthOfStringValue;

    tsp00_DbNamec	m_dbnamec;
    SAPDB_Int4      m_version;    

    SAPDB_Bool MakeSureParamFileIsInNewFormat(tRTESys_IOHandle &fileHandle,SAPDBErr_MessageList & err );
    SAPDB_Bool m_FileHasBeenRead;
    SAPDB_Bool m_FileInOldFormat;

    struct BinaryDataBlock
    {
        SAPDB_Byte *data;
        SAPDB_UInt4 length;
    };

    Container_List < BinaryDataBlock > m_binaryData;

    SAPDB_Int4 m_positionInBinaryData;

    struct ClassListEntry;

    ClassListEntry *m_ClassList;
    SAPDB_Byte      m_maxClassIndex;

    SAPDB_UInt8     m_checkSumFromHeader;

    SAPDBErr_MessageList    m_errorInConstructor;
};


/*!
  @class                RTEConf_ParameterIterator
  @brief                Parameter iterator class
                        This class allows to iterate over all members of an instance of class RTEConf_Parameter

 */
class RTEConf_ParameterIterator
{
public:
    /*!
       @brief           Get the next name and type combination is parameter set
                        This call returns the name and type information for the next parameter found. At end of list
                        the corresponding flag is set. Ignoring this flag leads to an error for the next call.

       @param           name [out] name of next parameter
       @param           type [out] type of next parameter
       @param           endOfList [out] flag to indicate end of list reached
       @param           err [out] error list filled, if false is returned
       @return value    true if name, type and endOfList are filled, false if err is filled

     */
    SAPDB_Bool Next(RTEConf_Parameter::Name & name, RTEConf_Parameter::Type & type, SAPDB_Bool &endOfList, SAPDBErr_MessageList & err )
    {
        SAPDB_Byte paramClassIndex;
        return Next(name,type,paramClassIndex,endOfList,err);
    }

    /*!
       @brief           Get the next name and type combination is parameter set
                        This call returns the name and type information for the next parameter found. At end of list
                        the corresponding flag is set. Ignoring this flag leads to an error for the next call.

       @param           name [out] name of next parameter
       @param           type [out] type of next parameter
       @param           paramClassIndex [out] parameter class index
       @param           endOfList [out] flag to indicate end of list reached
       @param           err [out] error list filled, if false is returned
       @return value    true if name, type and endOfList are filled, false if err is filled

     */
    SAPDB_Bool Next(RTEConf_Parameter::Name & name, RTEConf_Parameter::Type & type, SAPDB_Byte & paramClassIndex, SAPDB_Bool &endOfList, SAPDBErr_MessageList & err );
    /*!
       @brief           ctor
                        Construct an iterator object, making a snapshot of the parameter set

       @param           param [in] the parameter class to make a snapshot of
       @param           beginningOfParameterName [in] if != NULL, the iterator contains only those
                        parameters of which the name begins with the string beginningOfParameterName points to.
                        If == NULL, the iterator contains all parameters of the parameter set. This is the
                        default, if this parameter is omitted.

     */

    RTEConf_ParameterIterator(RTEConf_Parameter &param,SAPDB_UTF8 *beginningOfParameterName=NULL);

    /*!
       @brief           copy ctor
                        Construct the object using another object as template.
                        The copies use independent storage for parameters, so value change
                        on template afterwards have no effect on the object.

       @param           templateIterator [in] the template parameter iterator class

     */

    RTEConf_ParameterIterator(const RTEConf_ParameterIterator &templateIterator);

    /*!
       @brief           assignment operator
                        Copy all the content from template parameter iterator object

       @param           templateIterator [in] the template parameter iterator class

     */

    RTEConf_ParameterIterator & operator=(const RTEConf_ParameterIterator &templateIterator);

    /*!
       @brief           dtor
                        Frees any memory allocated inside the object

     */

    ~RTEConf_ParameterIterator();

    /*!
       @brief           operator new
                        uses RTEMem_RteAllocator to allocate size bytes.
                        If allocation fails, RTECrash() is called

       @param           size [in] number of bytes to allocate

     */

    void *operator new(size_t size);

    /*!
       @brief           operator delete
                        uses RTEMem_RteAllocator to deallocate the memory block at address.

       @param           address [in] base address of the memory block to deallocate

     */

    void operator delete(void *address);
private:

    RTEConf_ParameterRecord *m_FirstRecord;
    RTEConf_ParameterRecord *m_LastVisitedRecord;

    void IterateThroughOneList(RTEConf_Parameter &param,SAPDB_Int4 listToIterate,SAPDB_UTF8 *beginningOfParameterName,SAPDB_Int4 lengthToCompare);
    void DeleteIteratorRecords();
    void CopyIteratorRecords(const RTEConf_ParameterIterator &templateIterator);
    

};

/*! 
  @brief parameter type to string conversion 
  @param type [in] parameter type
  @return SAPDB_ToString 
 */
SAPDB_ToStringClass SAPDB_ToString( RTEConf_Parameter::Type type);

#endif // CONF_PARAMETER_ACCESS_HPP
