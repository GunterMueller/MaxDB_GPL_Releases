/*!
  @file           Catalog_DBProcObject.hpp
  @author         ThomasA
  @brief          memory representation of a db-procedure  
  @ingoup         Catalog

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


#ifndef CATALOG_DBPROCOBJECT_HPP
#define CATALOG_DBPROCOBJECT_HPP
#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"

/*!
  @brief encapsulation of pascal type tak_methodrecord. Describes a db-procedure
*/

class Catalog_DBProcObject : public Catalog_MethodObjectPascal
{
public :
    /*!
       @brief progamming language used for db-procedure implementation 
     */
    enum Language { PL, Cpp, Java, System, Unknown };  
    /*!
       @brief key of a db-procedure object
    */
    class Catalog_Key : public Catalog_Object::Catalog_Key
    {
    public :
        /*!
           @brief constructor
        */
        Catalog_Key(const SQLMan_Surrogate& procId);
    };
    /*!
       @brief iterator over the parameter descriptions of a db-procedure
    */
    class ParameterIterator
    {
    public :
        ~ParameterIterator();                                   //!< destructor
        operator bool() const;                                  //!< true, if iterator didn't reached the end yet
        void operator++();                                      //!< steps to the next parameter
        SQLMan_DataType ParamDataType() const;                  //!< returns the datatype of the current parameter
        int             Fraction() const;                       //!< returns the number of digits to the left of the decimal point
        int             DataLength() const;                     //!< returns the dataLength of the current parameter
        int             InOutLength() const;                    //!< returns the pysical length of the current parameter
        bool            IsInputParameter() const;               //!< returns true, if current parameter is an IN parameter
        bool            IsOutputParameter() const;              //!< returns true, if current parameter is an OUT parameter
        int             ParameterNo() const { return m_index; } //!< returns the current parameter number. Counting starts with 0
    private :
        ParameterIterator(const Catalog_DBProcObject& dbproc); //!< constructor, stores Catalog_DBProcObject
        const Catalog_DBProcObject& m_dbproc;         //!< DBProc object containing the parameter information
        int                         m_index;          //!< current parameter index
        Catalog_ParameterInfo*      m_currParameter;  //!< pointer to current parameter description
        friend class Catalog_DBProcObject;
    };
    /*!
        @brief returns the number of parameters (IN and OUT) of the db-procedure
    */
    int ParamCount() const;

    /*!
        @brief returns an iterator pointing to the first parameter description
    */
    ParameterIterator FirstParameter() const;
    /*!
       @brief returns the identification of the db-procedure
     */
    const Catalog_ObjectId& GetId() const;
    /*!
       @brief returns the programming language implementing the procedure
     */
    Language GetLanguage() const;
    /*!
       @brief returns the programming language implementing the procedure as 0 terminated string
     */
    const char* GetLanguageString() const;
    /*!
        @brief returns an iterator pointing to the i-th parameter description
    */
    ParameterIterator GetParameter(int idx) const;

    friend class Catalog_DBProcObject::ParameterIterator;
};

#endif