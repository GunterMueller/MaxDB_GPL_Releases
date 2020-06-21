/*!
  \file    KernelDBE_CallbackHandler.hpp
  \author  MarcW
  \ingroup Database Events Management, parameter handling
  \brief   

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


*/

#if !defined(KernelDBE_CallbackHandler_hpp)
#define KernelDBE_CallbackHandler_hpp

#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "vsp0031.h"

/*!
    @brief implements the interface of the world of kernel parameters to the database event managment

    The database events management is implemented through online changeable kernel parameters. Each event
    category is represented by one kernel parameter in the kernel parameter class 'EVENTS'.

    This class provides the the initialization and callback funtions to be registered for online parameter changes
    and parameter initialization at database startup.
*/
class KernelDBE_CallbackHandler {

public:
    /*!
        @brief category initializer

        This method is to be called at database startup for each event category, when the parameters
        are read from the parameter file. From here, the initial event configuration for this event
        category is read and can be published to the "other parts" of the kernel.
        Currently it is called in extern "C" void g011get_conf_param(tgg00_BasisError *b_err).
        @param eventCategoryName [IN] name of the event category (kernel parameter) as defined in SAPDBDBE_KernelParameterNames.hpp
        @param paramType [IN] type of parameter (e.g. in case of a complext event category it is t_String)
        @param eventCategoryIndex [IN] index of the event category
        @param errOut [OUT] message list filled in case of error, cleared initially.
        @return true if this event category could be updated successfully, false otherwise
    */
    static SAPDB_Bool initializeEventCategory(
            RTEConf_Parameter::Name eventCategoryName,
            RTEConf_Parameter::Type paramType,
            tsp31_event_ident_Enum eventCategoryIndex,
            Msg_List &errOut);

    /*!
        @brief callback function for event category update

        This method is to be registered at the singleton instance of RTEConf_ParameterAccess as callback
        function for each kernel parameter that represents an event category.
        @param eventCategoryName [IN] name of the event category (kernel parameter) as defined in SAPDBDBE_KernelParameterNames.hpp
        @param paramType [IN] type of parameter (e.g. in case of a complext event category it is t_String)
        @param newValue [IN] new value of this parameter (modified valid string representing an event category)
        @param context [IN] pointer to an object of class KernelDBE_CallbackHandler::KernelDBE_ContextProvider, that
               allows to easily find the old value of this event category
        @param errOut [OUT] message list filled in case of error, cleared initially
        @return true if this event category could be updated successfully, false otherwise
        @see RTEConf_Parameter::Type
    */
    static SAPDB_Bool eventCategoryUpdated(
            RTEConf_Parameter::Name eventCategoryName,
            RTEConf_Parameter::Type paramType,
            void *newValue,
            void *context,
            Msg_List &errOut);

    /*!
        @brief union capable of taking parameter values of all possible types
    */
    union ParamValueUnionType {
        /*!
            @brief assignment for an integer parameter

            @param aInteger [IN] the integer value to assign
        */
        void operator=( const RTEConf_Parameter::Integer& aInteger );

        /*!
            @brief assignment for a float parameter

            @param aReal [IN] the float value to assign
        */
        void operator=( const RTEConf_Parameter::Real& aReal );

        /*!
            @brief assignment for a string parameter

            @param aString [IN] the string to assign
        */
        void operator=( const RTEConf_Parameter::String aString );

        /*!
            @brief assignment for a crypt parameter

            @param aCrypt [IN] the crypt value to assign
        */
        void operator=( const RTEConf_Parameter::CryptInfo aCrypt );

        /*!
            @brief assignment for a parameter with a type

            @param aType [IN] type of the value to assign (type of *aNewValue)
            @param aNewValue [IN] pointer to the new value to assign
            @return true, if the new value was successfully assigned (type is known and pointer is valid),
                    false otherwise

        */
        SAPDB_Bool assign( const RTEConf_Parameter::Type aType, const void* aNewValue );
        
        /*! @brief the integer "aspect" */
        RTEConf_Parameter::Integer       IntVal;
        /*! @brief the real "aspect" */
        RTEConf_Parameter::Real          RealVal;
        /*! @brief the string "aspect" */
        RTEConf_Parameter::StringElement StringVal[RTECONF_MAXSTRINGLENGTH];
        /*! @brief the cryptinfo "aspect" */
        RTEConf_Parameter::CryptInfo     CryptVal;
    };

private:
    /*!
        @brief provides the context in a callbackfunction

        @see KernelDBE_CallbackHandler::eventCategoryUpdated
    */
    class KernelDBE_ContextProvider {

        /*!
        @brief grant the KernelDBE_CallbackHandler private access
        */
        friend class KernelDBE_CallbackHandler;

    public:
        
        /*!
            @brief constructor

            @param aContext [IN] the index of the contex that this object will represent
        */
        KernelDBE_ContextProvider(tsp31_event_ident_Enum aContext)
                : m_TheContext(aContext) {}
    private:
        tsp31_event_ident_Enum getContext() { return m_TheContext;}
        tsp31_event_ident_Enum m_TheContext;
    };
    
    static ParamValueUnionType
            m_ActiveValues[sp31ei_last_but_not_used];

static SAPDB_Bool updateCategoryDBFillingAboveLimit(
        Msg_List& errOut,
        RTEConf_Parameter::String newValue,
        RTEConf_Parameter::String oldValue);

static SAPDB_Bool updateCategoryDBFillingBelowLimit(
        Msg_List& errOut,
        RTEConf_Parameter::String newValue,
        RTEConf_Parameter::String oldValue);

static SAPDB_Bool updateCategoryLogFillingAboveLimit(
        Msg_List& errOut,
        RTEConf_Parameter::String newValue,
        RTEConf_Parameter::String oldValue);

public:
    /*! @brief array of context providers */
    static KernelDBE_ContextProvider
        contextProviders[sp31ei_last_but_not_used];
};

#endif //KernelDBE_CallbackHandler_hpp