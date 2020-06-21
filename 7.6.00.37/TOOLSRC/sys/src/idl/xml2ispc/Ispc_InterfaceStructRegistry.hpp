/*!*****************************************************************************

  module      : Ispc_InterfaceStructRegistry.hpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of interface description code
  description : Registry for C-structs

  ----------------------------------------------------------------------------

    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
    ========== licence end

*******************************************************************************/


#ifndef ISPC_IFSTRUCTREGISTRY_HPP
#define ISPC_IFSTRUCTREGISTRY_HPP


#include <string>
#include <stdio.h>


class Ispc_InterfaceStruct;


const int nameLen = 64;


class Ispc_Synonym /* PTS 1134734 */
{
public:
  Ispc_Synonym(const char * name);
  const char *   getName() const {return m_name;};
  Ispc_Synonym * getNext() const {return m_next;};
  void           setNext(Ispc_Synonym * ifSynonym);
private:
  char           m_name[nameLen];
  Ispc_Synonym * m_next;
};


class Ispc_Member
{
public:
  Ispc_Member(const char * name);
  const char *           getName() const {return m_name;};
  Ispc_InterfaceStruct * getTypeStruct() const {return m_typeStruct;};
  bool                   setTypeStruct(Ispc_InterfaceStruct * ifStruct); /* PTS 1134734 */
  bool                   setTypeScalar();                                /* PTS 1134734 */
  const bool             typeIsRegistered() const;                       /* PTS 1134734 */
  Ispc_Member *          getNext() const {return m_next;};
  void                   setNext(Ispc_Member * next);
  void                   addToEnum( std::string & colEnum, const char * entry , unsigned int & colNo );
private:
  char                   m_name[nameLen];
  Ispc_InterfaceStruct * m_typeStruct;
  bool                   m_typeScalar; /* PTS 1134734 */
  Ispc_Member *          m_next;
};


class Ispc_InterfaceStruct
{
public:
  Ispc_InterfaceStruct(const char * name);
  ~Ispc_InterfaceStruct();
  const char *           getName() const {return m_name;};
  Ispc_InterfaceStruct * getNext() const {return m_next;};
  void                   setNext(Ispc_InterfaceStruct * ifStruct);
  void                   addSynonym(const char * name);      /* PTS 1134734 */
  bool                   isSynonym(const char * name) const; /* PTS 1134734 */
  Ispc_Member *          getFirstMember() const {return m_firstMember;};
  void                   addMember(Ispc_Member * member);
  const bool             allMemberTypesRegistered() const; /* PTS 1134734 */
  void                   genColEnum(std::string & colEnum);
  void                   addMembersToEnum( std::string & colEnum, const char * entry, unsigned int & colNo );
private:
  char                   m_name[nameLen];
  Ispc_Synonym *         m_synonym; /* PTS 1134734 */
  Ispc_Member *          m_firstMember;
  Ispc_Member *          m_lastMember;
  Ispc_InterfaceStruct * m_next;
};


class Ispc_InterfaceStructRegistry
{
public:
  Ispc_InterfaceStructRegistry();
  ~Ispc_InterfaceStructRegistry();
  void                   registerStruct(Ispc_InterfaceStruct * ifStruct);
  Ispc_InterfaceStruct * findStruct(const char * name) const;
private:
  Ispc_InterfaceStruct * m_first;
  Ispc_InterfaceStruct * m_last;
};


#endif /* ISPC_IFSTRUCTREGISTRY_HPP */

