/*!***************************************************************************

  module      : SQLMan_SchemaMap.hpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: SQL Manager
  description : active schema map
  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

#ifndef SQLMAN_CONSISTENTVIEWDIRECTORY_HPP
#define SQLMAN_CONSISTENTVIEWDIRECTORY_HPP

#include "Container/Container_AVLTree.hpp"
#include "SQLManager/SQLMan_Types.hpp"

class SAPDBMem_IRawAllocator;      /// allocator interface
class SQLMan_Context;              /// sql manager context
class SQLMan_SchemaMapTable;       /// implementation class, see cpp file
class SQLMan_OneMapInfoComparator; /// implementation class, see cpp file
struct SQLMan_OneMapInfo;          /// implementation class, see cpp file


/*!
   @brief provides a directory of schema names to be mapped.
*/
class SQLMan_SchemaMap
{
public :
    typedef tsp00_C24 MapGuid;
    /*!
       @brief destructor
    */
    ~SQLMan_SchemaMap();
     /*!
       @brief returns true, if any schema has been mapped
     */
    bool AnySchemaMapped() const;
    /*!
       @brief creates a map
     */
    static SQLMan_SchemaMap* Create(SQLMan_Context& Context);
    /*!
       @brief destucts and destorys the map
     */
    static void Destroy (SQLMan_SchemaMap* & pMap);
    /*!
       @brief enables or disables the map
     */
    void Enable (bool DoEnable);
    /*!
       @brief  stores a given schema name and the mapped name in the directory
       @param  SchemaName the schema name to be mapped
       @param  MappedSchemaName the corresponding mapped schema name
       @return true, if the map has been stored successfully; otherwise false (out of memory)  
    */
    bool Insert(const SQLMan_Identifier& SchemaName, const SQLMan_Identifier& MappedSchemaName);
    /*!
       @brief empties the schema map
     */
    void DeleteAll();
    /*!
       @brief maps the given schema name to the defined schema name. If no mapping ist defined, SchemaName will not be modified
      */
    bool MapSchema (SQLMan_Identifier& SchemaName, int Pos);
    /*!
       @brief reads a mapping from SYSLOADER.MAPSCHEMANAMES
     */
    bool ReadMap (MapGuid& Guid);
    
    void Rename ();

    /*!
       @brief resets the mapping table
     */
    void Reset();
    /*!
      @brief returns true, if the schema map contains at least one entry; false otherwise
     */
    bool SchemaMappingActive() const;
    /*!
      @brief sets the statement offset
     */
    void SetOffset(int offset);
private :
    /*!
       @brief constructor
       @param current sqlmanager context 
       @param alloc allocator to be used for memory allocation
    */
    SQLMan_SchemaMap (SQLMan_Context& Context, SAPDBMem_IRawAllocator& Alloc);
    struct TreeContent
    {
        SAPDB_UInt1       m_unmappedLength;
        SAPDB_UInt1       m_mappedLength;
        SQLMan_Identifier m_mappedSchemaName;
    };

    typedef Container_AVLNodeComparator<SQLMan_Identifier>                                 Comparator; //!< comparator for statement identifications
    typedef Container_AVLContentNode<SQLMan_Identifier,TreeContent,Comparator>       Node;       //!< node of the avl tree implementing the directory
    typedef Container_AVLContentTree <Node,SQLMan_Identifier,TreeContent,Comparator> Tree;       //!< avl tree holding the consistent views

    static Comparator      m_comparator;      //!< singleton instance of the comparator 
    SQLMan_Context&        m_context;         //!< sql manager context 
    Tree                   m_schemaMap;       //!< avl tree storing the schema map
    SQLMan_SchemaMapTable* m_mapTable;        //!< table of mapping information
    int                    m_offset;          //!< current statement offset
    bool                   m_anySchemaMapped; //!< true, if at least one schema has been mapped 
    bool                   m_enabled;         //!< true, if the map is enabled (active); otherwise false

    friend class SQLMan_SchemaMapTable;
    friend class SQLMan_OneMapInfoComparator;
    friend struct SQLMan_OneMapInfo;
};

#endif
