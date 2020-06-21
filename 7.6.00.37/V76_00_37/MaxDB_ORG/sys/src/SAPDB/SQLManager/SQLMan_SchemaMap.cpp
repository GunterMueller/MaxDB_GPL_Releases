/*!***************************************************************************

  module      : SQLMan_SchemaMap.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: SQL Manager
  description : directory of currently active sql statements in session.

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

#include "SQLManager/SQLMan_SchemaMap.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp" 
#include "SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp" 
#include "SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h"
#include "SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp"
#include "Container/Container_Vector.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_Comparator.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_QuickSort.hpp"
#include "SAPDB/SQLManager/SQLMan_Tracing.hpp"

#include "hak05.h"

struct SQLMan_OneMapInfo
{
    SQLMan_OneMapInfo()
    {
    }
    SQLMan_OneMapInfo (const SQLMan_SchemaMap::Node* pNode, int Pos)
        : m_pNode(pNode)
        , m_pos(Pos)
    {
    }
    const SQLMan_SchemaMap::Node* m_pNode;
    int                           m_pos;
};

//---------------------------------------------------------------------------------------------

SQLMan_SchemaMap::Comparator SQLMan_SchemaMap::m_comparator;

class SQLMan_OneMapInfoComparator : public SAPDBAlgo_IComparator<SQLMan_OneMapInfo>
{
public :
    virtual bool IsEqual( SQLMan_OneMapInfo const& A, SQLMan_OneMapInfo const& B ) 
    {
        return A.m_pos == B.m_pos;
    }
    virtual bool IsPredecessor( SQLMan_OneMapInfo const& A, SQLMan_OneMapInfo const& B ) 
    {
        return A.m_pos < B.m_pos;
    }
};

//---------------------------------------------------------------------------------------------

class SQLMan_SchemaMapTable
{
public :
    SQLMan_SchemaMapTable(SAPDBMem_IRawAllocator& Alloc)
        : m_mapTable(Alloc)
    {
        m_mapTable.Reserve(16);
    }

    bool Append (const SQLMan_OneMapInfo& Info)
    {
        return m_mapTable.InsertEnd(Info);
    }
    void Sort()
    {
        SQLMan_OneMapInfoComparator cmp;
        SAPDBAlgo_QuickSort(&m_mapTable[0], m_mapTable.GetSize(), cmp);
    }

    const SQLMan_OneMapInfo& operator[](int Index)
    {
        return m_mapTable[Index];
    }
    int GetSize() const
    {
        return m_mapTable.GetSize();
    }
private :
    Container_Vector<SQLMan_OneMapInfo> m_mapTable;
};

//---------------------------------------------------------------------------------------------

SQLMan_SchemaMap::SQLMan_SchemaMap(SQLMan_Context& Context, SAPDBMem_IRawAllocator& Alloc)
: m_context(Context)
, m_schemaMap(&m_comparator, &Alloc)
, m_mapTable(0)
, m_offset(0)
, m_anySchemaMapped(false)
, m_enabled(true)
{
}

//---------------------------------------------------------------------------------------------
    
SQLMan_SchemaMap::~SQLMan_SchemaMap()
{
    destroy (m_mapTable, m_context.GetAllocator());
}

//---------------------------------------------------------------------------------------------

bool SQLMan_SchemaMap::AnySchemaMapped() const
{
    return m_anySchemaMapped;
}

//---------------------------------------------------------------------------------------------

SQLMan_SchemaMap* SQLMan_SchemaMap::Create(SQLMan_Context& Context)
{
    return new(Context.GetAllocator()) SQLMan_SchemaMap (Context, Context.GetAllocator());
}

//---------------------------------------------------------------------------------------------

void SQLMan_SchemaMap::Destroy (SQLMan_SchemaMap* & pMap)
{
    if (0 != pMap)
    {
        destroy (pMap, pMap->m_context.GetAllocator());
    }
}

//---------------------------------------------------------------------------------------------

 void SQLMan_SchemaMap::Enable (bool DoEnable)
 {
     SAPDBTRACE_WRITELN( SQLMan_Trace, 1, "SQLMan_SchemaMap::Enable " << DoEnable);
     m_enabled = DoEnable;
 }

//---------------------------------------------------------------------------------------------

 bool SQLMan_SchemaMap::Insert(const SQLMan_Identifier& SchemaName, const SQLMan_Identifier& MappedSchemaName)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLMan_SchemaMap::Register", SQLMan_Trace, 1);
    SAPDBTRACE_WRITELN( SQLMan_Trace, 5, "SchemaName " << SchemaName << ", mapped SchemaName " << MappedSchemaName);
    Container_AVLTreeError rc;
    TreeContent content;
    content.m_unmappedLength   = SchemaName.GetLength();
    content.m_mappedLength     = MappedSchemaName.GetLength();
    content.m_mappedSchemaName = MappedSchemaName;
    m_schemaMap.Insert(SchemaName, content, rc);
    return (rc != AVLTree_OutOfMemory);  // duplicates are ok
}

//---------------------------------------------------------------------------------------------
    
void SQLMan_SchemaMap::DeleteAll()
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLMan_SchemaMap::DeleteAll", SQLMan_Trace, 1);
    m_schemaMap.DeleteAll();
}

//---------------------------------------------------------------------------------------------

bool SQLMan_SchemaMap::MapSchema (SQLMan_Identifier& SchemaName, int Pos)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLMan_SchemaMap::MapSchema", SQLMan_Trace, 1);
    if (!m_enabled)
    {
        return false;
    }
    const Node* pNode = m_schemaMap.Find( SchemaName );
    if (pNode)
    {
        const TreeContent* pContent = pNode->GetContent();
        if (pContent)
        {
            SAPDBTRACE_WRITELN( SQLMan_Trace, 5, "map at " << Pos << " offset " << m_offset << " " << SchemaName << " to " << pContent->m_mappedSchemaName);
            SQLMAN_TRACE_BUFFER(SQLMan_Trace, 5, "rename ", &m_context.a_cmd_part->sp1p_buf()[Pos-1], SchemaName.GetLength());
            SchemaName = pContent->m_mappedSchemaName;
            if (0 == m_mapTable)
            {
                SAPDBMem_IRawAllocator& alloc = m_context.GetAllocator();
                m_mapTable = new (alloc) SQLMan_SchemaMapTable(alloc);
                if (!m_mapTable)
                {
                     m_context.SetNoMoreMemoryError (Msg_List(Msg_List::Error, SDBMSG_SQLMAN_NO_MEMORY/*, Msg_Arg("MEMREQUEST",?)*/));
                     return false;
                }
            }
            if (!m_mapTable->Append(SQLMan_OneMapInfo(pNode, m_offset + Pos)))
            {
                m_context.SetNoMoreMemoryError (Msg_List(Msg_List::Error, SDBMSG_SQLMAN_NO_MEMORY/*, Msg_Arg("MEMREQUEST",?)*/));
                return false;
            }
            m_anySchemaMapped = true;
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------------

bool SQLMan_SchemaMap::ReadMap (MapGuid& Guid)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLMan_SchemaMap::ReadMap", SQLMan_Trace, 1);
    KSQL_PreparedStatement::SQLCode rc = 0;
    KSQL_Connection con;
    if (con.isConnected())
    {
          m_context.SetInDDLTrigger();
          KSQL_PreparedStatement stmt = con.createPreparedStatement();
          stmt.prepare("SELECT SOURCESCHEMANAME, TARGETSCHEMANAME FROM SYSLOADER.MAPSCHEMANAMES WHERE PACKAGEGUID = ?");
          stmt.bind(1, &Guid, sizeof(Guid));
          rc = stmt.execute();
          if (0 == rc)
          {
              const int SQL_ROW_NOT_FOUND = 100;
              KSQL_ResultSet rs = stmt.getResultSet();
              do
              {
                  rc = rs.next();
                  if (0 != rc)
                  {
                      rs.close();
                      stmt.release();
                      if (SQL_ROW_NOT_FOUND == rc) 
                      {
                           rc = 0;
                      }
                      break;
                  }
                  SQLMan_Identifier schemaName;
                  rs.getIdentifier(1, schemaName);
                  SQLMan_Identifier mappedSchemaName;
                  rs.getIdentifier(2, mappedSchemaName);
                  if (!Insert(schemaName, mappedSchemaName))
                  {
                      m_context.SetNoMoreMemoryError (Msg_List(Msg_List::Error, SDBMSG_SQLMAN_NO_MEMORY/*, Msg_Arg("MEMREQUEST",?)*/));
                      return false;
                  }
              }
              while (true);
          }
    }
    return 0 == rc;
}

//---------------------------------------------------------------------------------------------

void SQLMan_SchemaMap::Rename ()
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLMan_SchemaMap::Rename", SQLMan_Trace, 1);
    if (m_mapTable && m_enabled && m_context.IsOk())
    {
        m_mapTable->Sort();
        int size  = m_mapTable->GetSize();
        int delta = 0;
        for (int ix = 0; ix < size; ++ix)
        {
            const Node* pNode = (*m_mapTable)[ix].m_pNode;
            int pos = (*m_mapTable)[ix].m_pos;
            const TreeContent& content = *pNode->GetContent();
            SAPDBTRACE_WRITELN( SQLMan_Trace, 5, "rename at " << pos);
            SAPDBTRACE_WRITELN( SQLMan_Trace, 5, "Source " << *pNode->GetKey());
            SAPDBTRACE_WRITELN( SQLMan_Trace, 5, "Mapped " << content.m_mappedSchemaName);
            int currDelta;
            pos += delta;
            a05RenameIdentifierInPacket (m_context, content.m_mappedSchemaName, pos, currDelta);
            delta += currDelta;
        }
      
    }
    this->Reset();
}

//---------------------------------------------------------------------------------------------

void SQLMan_SchemaMap::Reset()
{
   if (m_mapTable)
   {
       m_anySchemaMapped = false;
       destroy (m_mapTable, m_context.GetAllocator());
   }
   m_offset = 0;
}

//---------------------------------------------------------------------------------------------

bool SQLMan_SchemaMap::SchemaMappingActive() const
{
    return m_schemaMap.GetSize() > 0;
}

//---------------------------------------------------------------------------------------------

void SQLMan_SchemaMap::SetOffset(int offset)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLMan_SchemaMap::ReadMap", SQLMan_Trace, 1);
    SAPDBTRACE_WRITELN( SQLMan_Trace, 5, "offset " << offset);
    m_offset = offset;
}



