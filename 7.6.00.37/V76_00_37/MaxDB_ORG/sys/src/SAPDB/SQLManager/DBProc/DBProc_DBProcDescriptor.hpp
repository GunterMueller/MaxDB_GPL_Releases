/*!
  @file           DBProc_DBProcDescriptor.hpp
  @author         ThomasA
  @ingroup        DBProc
  @brief          a descriptor for a db-procedure  

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

#ifndef DBProc_DBProcDescriptor_HPP
#define DBProc_DBProcDescriptor_HPP
#include "SQLManager/Catalog/Catalog_DBProcObject.hpp"

class Catalog_ISessionCache;
class Catalog_DBProcFileObject;
class Catalog_DBProcInfoObject;
/*!
   @brief implements an descriptor which yields information about a db-procedure 
*/
class DBProc_DBProcDescriptor
{
public  :
    typedef Catalog_DBProcObject::ParameterIterator ParameterIterator; //!< iterator over all dbproc parameters
    typedef Catalog_DBProcObject::Language          Language;          //!< possible predifined implementation languages
   /*!
      @brief constructor
      @param sessionCache [in,out] the sessionCache caching the db-procedure catalog inforamtion
      @param procId [in] the internal identification of the db-procedure
    */
   DBProc_DBProcDescriptor(Catalog_ISessionCache& sessionCache, const SQLMan_Surrogate& procId);

   /*!
        @brief returns an iterator pointing to the first parameter description
    */
   inline ParameterIterator FirstParameter() const;
   /*!
       @brief returns the external information as 0 terminated string. 
              create dbproc 'name' ... as language java name 'external information'
              The caller doesn't have to take care about memory management
    */
    const char* GetExternalInfo();
     /*!
        @brief returns the number of parameters (IN and OUT) of the db-procedure
    */
    inline int ParamCount() const;
    /*!
       @brief returns a pointer to a 0-terminated string containing a concatenation of the create data and time
     */
    const char* GetCreateDateTime();
    /*!
       @brief returns the programming language implementing the procedure
     */
    inline Language GetLanguage() const;
    /*!
       @brief returns the programming language implementing the procedure as 0 terminated string
     */
    const char* GetLanguageString();
    /*!
       @brief returns the error code of the last operation
     */
    inline SAPDB_Int2 GetLastError() const;

    /*!
    @brief returns a pointer to the db-procedure name (0-terminated)
    */
    const char* GetName();
    /*!
        @brief returns an iterator pointing to the i-th parameter description
    */
    inline ParameterIterator GetParameter(int idx) const;
    /*!
       @brief checks whether descriptor is valid
     */
    inline bool IsValid() const;
private :
   typedef char NullTerminatedIdentifier[33];      //<! null terminated identifier  
   void LoadProcInfo();                            //<! load a catalog information into the session cache
   void CopyIdentifier(const SQLMan_Identifier&, 
                       NullTerminatedIdentifier&); //<! copies an identifier into a null terminated string
   Catalog_ISessionCache&    m_sessionCache;       //<! the session cache to be used
   Catalog_DBProcObject*     m_pDBproc;            //<! pointer to the Catalog_DBProcObject of the db-procedure
   Catalog_DBProcFileObject* m_pDBProcFile;        //<! pointer to the Catalog_DBProcFileObject of the db-procedure
   Catalog_DBProcInfoObject* m_pDBProcInfo;        //<! pointer to the Catalog_DBProcInfoObject of the db-procedure
   SQLMan_Surrogate          m_procId;             //<! the internal 'name' of the db-procedure
   int                       m_error;              //<! last error
   NullTerminatedIdentifier  m_dbprocName;         //<! the name of the procedure (0 terminated)
   NullTerminatedIdentifier  m_language  ;         //<! the implementation language (0 terminated)
   char                      m_createDateTime[17]; //<! concatenation of create date and time in internal representation (0-terminated)
};

//-----------------------------------------------------------------------------------

inline DBProc_DBProcDescriptor::ParameterIterator DBProc_DBProcDescriptor::FirstParameter() const
{
    return m_pDBproc->FirstParameter();
}

//-----------------------------------------------------------------------------------

inline int DBProc_DBProcDescriptor::ParamCount() const
{
    return m_pDBproc->ParamCount();
}

//-----------------------------------------------------------------------------------

inline Catalog_DBProcObject::Language DBProc_DBProcDescriptor::GetLanguage() const
{
    return m_pDBproc->GetLanguage();
}

//-----------------------------------------------------------------------------------

inline DBProc_DBProcDescriptor::ParameterIterator DBProc_DBProcDescriptor::GetParameter(int idx) const
{
    return m_pDBproc->GetParameter(idx);
}

//-----------------------------------------------------------------------------------

inline bool DBProc_DBProcDescriptor::IsValid() const
{
    return (0 == m_error);
}

//-----------------------------------------------------------------------------------

inline SAPDB_Int2 DBProc_DBProcDescriptor::GetLastError() const
{
    return m_error;
}

#endif

