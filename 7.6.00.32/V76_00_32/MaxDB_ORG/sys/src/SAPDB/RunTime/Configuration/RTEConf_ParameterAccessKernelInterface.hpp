/*!---------------------------------------------------------------------
  @file           RTEConf_ParameterAccessKernelInterface.hpp
  @author         FrankH, JoergM, RobinW
  @brief          RTE configuration parameters: class RTEConf_ParameterAccess for accessing parameters from kernel C++-code

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


#ifndef CONF_PARAMETER_ACCESS_KERNEL_INTERFACE_HPP
#define CONF_PARAMETER_ACCESS_KERNEL_INTERFACE_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Types.hpp"
#include "RunTime/RTE_ISystem.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Messages.hpp"

#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"

#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "SAPDB/Container/Container_List.hpp"

#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"

#include "geo002.h"

class Msg_List;
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



 /*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class          RTEConf_ParameterAccess
  @brief          class for Parameter access from kernel.


  Realized as a singleton as there is exactly one parameter set
  in the kernel context

 */

class RTEConf_ParameterAccess
{
public:
    /*!
       @brief          returns the one and only instance of the class


          Creates the instance if it does not exist
       @return value   the instance

     */

    static RTEConf_ParameterAccess *Instance(void);
	
    /*!
       @brief          returns the value of an integer parameter
       @param          name [in] parameter name
       @param          integerValue [out] value reference
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool GetInteger(RTEConf_Parameter::Name name,RTEConf_Parameter::Integer &integerValue,SAPDBErr_MessageList &err);
	
    /*!
       @brief          returns the value of an integer parameter
       @param          name [in] parameter name
       @param          updateOnline [out] indicates whether the parameter can be changed in online mode
       @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
       @param          integerValue [out] value reference
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool GetInteger(RTEConf_Parameter::Name name,RTEConf_Parameter::Integer &integerValue,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,SAPDBErr_MessageList &err);
	
    /*!
       @brief          returns the value of a real parameter
       @param          name [in] parameter name
       @param          realValue [out] value reference
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
	SAPDB_Bool GetReal(RTEConf_Parameter::Name name,RTEConf_Parameter::Real &realValue,SAPDBErr_MessageList &err);

    /*!
       @brief          returns the value of a real parameter
       @param          name [in] parameter name
       @param          realValue [out] value reference
       @param          updateOnline [out] indicates whether the parameter can be changed in online mode
       @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
	SAPDB_Bool GetReal(RTEConf_Parameter::Name name,RTEConf_Parameter::Real &realValue,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,SAPDBErr_MessageList &err);

    /*!
       @brief          returns the value of a string parameter
       @param          name [in] parameter name
       @param          stringValue [out] value reference
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
	SAPDB_Bool GetString(RTEConf_Parameter::Name name,RTEConf_Parameter::String &stringValue,SAPDBErr_MessageList &err);
	
    /*!
       @brief          returns the value of a string parameter
       @param          name [in] parameter name
       @param          stringValue [out] value reference
       @param          updateOnline [out] indicates whether the parameter can be changed in online mode
       @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
	SAPDB_Bool GetString(RTEConf_Parameter::Name name,RTEConf_Parameter::String &stringValue,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,SAPDBErr_MessageList &err);
	
    /*!
       @brief          returns the value of a cryptinfo parameter
       @param          name [in] parameter name
       @param          cryptInfo [out] value reference
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
	SAPDB_Bool GetCryptInfo(RTEConf_Parameter::Name name,RTEConf_Parameter::CryptInfo &cryptInfo,SAPDBErr_MessageList &err);
	
    /*!
       @brief          returns the value of a cryptinfo parameter
       @param          name [in] parameter name
       @param          cryptInfo [out] value reference
       @param          updateOnline [out] indicates whether the parameter can be changed in online mode
       @param          permanentChange [out] indicates whether a permanent change has been made to the parameter
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
	SAPDB_Bool GetCryptInfo(RTEConf_Parameter::Name name,RTEConf_Parameter::CryptInfo &cryptInfo,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,SAPDBErr_MessageList &err);
	
    /*!
       @brief          returns the value of the parameter RUNDIRECTORY as absolute path
       @param          stringValue [out] value reference
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
	SAPDB_Bool GetRundirectoryAbsolutePath(RTEConf_Parameter::String &stringValue,SAPDBErr_MessageList &err);

    /*!
       @brief          returns the HSS configuration: a flag that shows if there are
                       HSS nodes configured and, if so, the configured delay time for this node.
                       If the HSS configuration contains errors (for example if the local node
                       name cannot be found among the list of configured hss nodes or the list
                       of configured HSS nodes contains inconsistencies), the return value is 
                       false and information about what went wrong is stored in err.
       @param          hssNodesConfigured [out] true if any HSS node name configured
       @param          delayTime [out] if any HSS node name configured: the delay time 
                       configured for this node, 0 if none configured. If no HSS node name 
                       is configured, the value remains unchanged
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool GetHssConfiguration(
        SAPDB_Bool &hssNodesConfigured,
        RTEConf_Parameter::Integer &delayTime,
        SAPDBErr_MessageList &err);

    /*!
       @brief          add a node to the list of HSS nodes
       @param          nodeName [in] the name of the new node
       @param          indexOfElement [in] the index of the node
       @return value   true if successful, false if not.

     */
    SAPDB_Bool AddHssNode(SAPDB_Char *nodeName,SAPDB_UInt indexOfElement);

    /*!
       @brief          returns the verified flag
       @return value   true if verified, false if not.

     */
	SAPDB_Bool IsVerified( )
	{
      RTEConf_Parameter::TimeStamp oTimeStamp;
      return m_Conf_Parameter->GetLastVerifyTime(oTimeStamp);
	};

    /*!
       @brief          returns the crash flag
       @return value   true if crashed or online/admin, false if not.

     */
	SAPDB_Bool IsCrashed( )
	{
      return m_Conf_Parameter->GetOriginalCrashFlag();
	};

    /*!
       @brief          sets the crash flag
       @return value   true if successfull, false if not.

     */
    SAPDB_Bool MarkCrashed(SAPDB_Int8 state, SAPDBErr_MessageList &err)
    {
        if (state == 0)
            return m_Conf_Parameter->MarkStoppedCorrectly(err);
        else
            return m_Conf_Parameter->SetCrashFlag(err);
    };

    /*!
       @brief          returns information about the file format
       @return value   true if file is in old format, false if file is in new format
     */
    SAPDB_Bool FileInOldFormat( )
    {
        return m_Conf_Parameter->FileInOldFormat();
    };

    /*!
       @brief          sets the value of the parameter identified by name.
       @param          name [in] parameter name reference
       @param          value [in] parameter value reference
       @param          permanentChange [in] indicate whether the change made is permanent
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool SetInteger(RTEConf_Parameter::Name name, RTEConf_Parameter::Integer  value,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err);

    /*!
       @brief          sets the value of the parameter identified by name.
       @param          name [in] parameter name reference
       @param          value [in] parameter value reference
       @param          permanentChange [in] indicate whether the change made is permanent
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool SetReal(RTEConf_Parameter::Name name, RTEConf_Parameter::Real  value,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err);

    /*!
       @brief          sets the value of the parameter identified by name.
       @param          name [in] parameter name reference
       @param          value [in] parameter value reference
       @param          permanentChange [in] indicate whether the change made is permanent
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool SetString(RTEConf_Parameter::Name name, RTEConf_Parameter::String  value,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err);

    /*!
       @brief          add a new parameter.
       @param          name [in] parameter name reference
       @param          value [in] parameter value reference
       @param          changeOnline [in] indicate whether the parameter can be changed in online mode
       @param          permanentChange [in] indicate whether the change made is permanent
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool AddInteger(RTEConf_Parameter::Name name, RTEConf_Parameter::Integer  value,SAPDB_Bool changeOnline,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err);

    /*!
       @brief          add a new parameter.
       @param          name [in] parameter name reference
       @param          value [in] parameter value reference
       @param          changeOnline [in] indicate whether the parameter can be changed in online mode
       @param          permanentChange [in] indicate whether the change made is permanent
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool AddReal(RTEConf_Parameter::Name name, RTEConf_Parameter::Real  value,SAPDB_Bool changeOnline,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err);

    /*!
       @brief          add a new parameter.
       @param          name [in] parameter name reference
       @param          value [in] parameter value reference
       @param          changeOnline [in] indicate whether the parameter can be changed in online mode
       @param          permanentChange [in] indicate whether the change made is permanent
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool AddString(RTEConf_Parameter::Name name, RTEConf_Parameter::String  value,SAPDB_Bool changeOnline,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err);

    /*!
       @brief          remove a parameter.
       @param          name [in] parameter name reference
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool Remove(RTEConf_Parameter::Name name, SAPDBErr_MessageList &err);

    //! hexadecimal representation of a crypt info
    //! CryptInfo is an array of RTECONF_CRYPT_ARRAY_SIZE(six) 4-byte-integers. As this cannot be typed in, 
    //! a representation as a string of hexadecimal values is being used
    typedef SAPDB_Char const RTEConf_CryptInfoAsString[RTECONF_CRYPT_ARRAY_SIZE * sizeof(SAPDB_Int4) * 2]; 


    /*!
       @brief          sets the value of the parameter identified by name.

       @param          name [in] parameter name reference
       @param          value [in] parameter value reference
       @param          permanentChange [in] indicate whether the change made is permanent
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool SetCryptInfo(RTEConf_Parameter::Name name,RTEConf_Parameter::CryptInfo &value,SAPDB_Bool permanentChange,SAPDBErr_MessageList &err);
    /*!
       @brief          Marks the configuration as verified
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not. 
              In this case, the error message can be retrieved by calling GetLastErrorMessage

     */
    SAPDB_Bool MarkAsVerified(SAPDBErr_MessageList &err)
    {
        return m_Conf_Parameter->MarkAsVerified(err);
    };
    
    /*!
       @brief           Register a call back function to be called when the
                        given parameter is changed
       @param           name [in] parameter name
       @param           updateFunc [in] the function to be registered
       @param           context [in] context that is passed to the function
       @param           err [out] message list that is filled when the function fails
       @return value    NULL if no call back function has been registered
     */
    SAPDB_Bool RegisterUpdateFunction(RTEConf_Parameter::Name name, RTEConf_Parameter::UpdateFuncType updateFunc, void *context, SAPDBErr_MessageList & err)
    {
        return m_Conf_Parameter->RegisterUpdateFunction(name,updateFunc,context,err);
    }

    /*!
       @brief           Check if all parameters that are marked as "may be changed in online mode"
                        have a callback routine registered and if all parameters that have a callback
                        routine registered are marked as "may be changed in online mode"
       @param           messages [out] contains a message for every parameter that does not fulfill
                        the condition described above
       @return value    false if messages is empty, true if not.
     */
    SAPDB_Bool CheckUpdateFunctions(SAPDBErr_MessageList & messages);
        
        
    /*!
       @brief       uses RTEMem_RteAllocator to allocate size bytes.
                    If allocation fails, RTECrash() is called
       @param       size [in] number of bytes to allocate

     */
    void *operator new(size_t size);

    /*!
      @brief        indicate if the singleton has been already initialized
      @return value true if the singleton has been already initialized, false if not
    */
    static SAPDB_Bool InstanceExists(void)
    {
        return (NULL != m_Instance);
    };

#ifdef AIX
    static void Reset(void)
    {
        m_Instance = NULL;
    }
#endif

private:
    RTEConf_ParameterAccess(SAPDB_Char *dbName);

    SAPDB_Char *BuildIndexedParamName(
        SAPDB_Char *baseParamName, 
        SAPDB_UInt indexDigits, 
        SAPDB_UInt indexToUse);

    void RegisterUpdateFunctions(void);
    RTEConf_Parameter *m_Conf_Parameter;
    static RTEConf_ParameterAccess *m_Instance;

    struct RTE_HssNode
    {
        RTE_Nodename name;
        SAPDB_Int    index;
    };
    Container_List < RTE_HssNode > m_hssNodeList;
    SAPDB_Bool m_hssNodeListConsistent;
    SAPDB_UInt m_localHSSIndex;
    SAPDB_Bool m_defaultNodeIsPresent;
    
    RTESync_SpinlockPool m_pool;
    RTESync_IRWRegion *m_pRWRegion;

    SAPDBErr_MessageList m_errorInFileRead;

    RTE_Path    m_runDirectory;
friend class RTEConf_ParameterReader;
};

/*!
  @class RTEConf_ParameterReader
  @brief Helper class to iterate through parameters
*/
class RTEConf_ParameterReader
{
public:
    RTEConf_ParameterReader()
    {
        m_Iterator = NULL;
        m_NumberBuffer = NULL;
    };

    /*!
       @brief          start an iteration through all parameters.

     */
	void StartIteration(void);

    /*!
       @brief          end the iteration. Frees the memory that has been allocated for the iterator
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
	SAPDB_Bool EndIteration(SAPDBErr_MessageList &err);

    /*!
       @brief          returns the next parameter. integer and real parameters are given as strings.
                       For CryptInfo parameters, an empty string is returned
       @param          name             [out] parameter name reference
       @param          valueString      [out] value reference as string
       @param          updateOnline     [out] true if parameter value can be changed when kernel is running
       @param          permanentChange  [out] true if the the parameter has been changed permanently
       @param          err              [out] contains an error message if unsuccessful
       @return value   true if successful, false if not. 
                       In this case, the error message can be retrieved by calling GetLastErrorMessage

     */
	SAPDB_Bool Next(RTEConf_Parameter::Name &name,SAPDB_Char *&valueString,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,SAPDBErr_MessageList &err);
private:
    SAPDB_Bool Next(RTEConf_Parameter::Name &name,SAPDB_Char *&valueString,const SAPDB_Char *&paramClass,SAPDB_Bool &updateOnline,SAPDB_Bool &permanentChange,SAPDBErr_MessageList &err);
public:
    /*!
       @brief          returns the name and type of the next parameter.
       @param          parameterName [out] parameter name reference
       @param          parameterType [out] parameter type reference
       @param          err [out] contains an error message if unsuccessful
       @return value   true if successful, false if not.

     */
    SAPDB_Bool Next(RTEConf_Parameter::Name &parameterName,RTEConf_Parameter::Type &parameterType,SAPDBErr_MessageList &err);

private:
    RTEConf_ParameterIterator *m_Iterator;
    SAPDB_Char *m_NumberBuffer;

};

/*!
   @brief           Read the contents of a parameter file and put it into a given XParam structure according to 
                    internal tables. Error messages are reported via RTE_Message() !
   @param           ServerDB            [in]    Name of the database
   @param           XParam              [in]    Pointer of the XParam structure to put the parameters read into
   @param           err                 [out]   message list filled with error messages in case the function does
                                                not succeed
   @return value    true if reading and analyzing parameters succeeded, false if not
 */
externC SAPDB_Bool RTEConf_ReadAndAnalyzeParam ( tsp00_DbNamec VAR_ARRAY_REF    ServerDB,
                                                 RTE_XPARAM_REC   *XParam,
                                                 Msg_List         &err);

/*!
   @brief           Check if given names has duplicate in devspacesArray
   @param           devspacesArray      [in]    Array of devspace description arrays
   @param           szDevspaceName      [in]    Devspacename to check
   @param           errList             [out]   message list filled with error messages in case the function does
                                                not succeed
   @return value    true is no duplicate, false if duplicate
 */
externC SAPDB_Bool RTEConf_CheckDuplicateDevSpaceName( DEVSPACES_DESCR *devspacesArray,
                                                       SAPDB_Char *szDevspaceName,
                                                       Msg_List   &errList);

#endif // CONF_PARAMETER_ACCESS_KERNEL_INTERFACE_HPP
