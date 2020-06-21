/*!
  @file           closehandlesonexit.cpp
  @author         MarcoP
  @ingroup        Tests
  @brief          tests the profile counters of SQLDBC
  @keywords       double linked list




\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

#define maxVal 3

class MyAllocator : public SAPDBMem_IRawAllocator
{
public: 
    MyAllocator(SAPDBMem_IRawAllocator *allocator)
    :m_alloc_count(0),
     m_dealloc_count(0),
     m_alloc_size(0),
     m_dealloc_size(0),
     m_wrapped_instance(allocator)
    {}

    virtual void *Allocate(SAPDB_ULong ByteCount)
    {
    	  ByteCount+=8;
        m_alloc_count++;
        m_alloc_size += ByteCount;
        void * erg = m_wrapped_instance->Allocate(ByteCount);
        memcpy (erg, &ByteCount, 8);
        erg = ((char*)erg)+8;
        return erg; 
    }
    
    virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint)
    {
    	  ByteCount+=8;
        m_alloc_count++;
        m_alloc_size += ByteCount;
        void * erg = m_wrapped_instance->Allocate(ByteCount, Hint);
        memcpy (erg, &ByteCount, 8);
        erg = ((char*)erg)+8;
        return erg; 
    }

    virtual void Deallocate(void* p)
    {   
      	m_dealloc_count++;
        p = ((char*)p)-8;
      	m_dealloc_size  += *((SAPDB_ULong*)p);
        m_wrapped_instance->Deallocate(p);
    }

    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &,SAPDB_ULong &) const
    {
        return;
    }

    virtual void GetCallStatistics(SAPDB_ULong &,SAPDB_ULong &) const
    {
        return;
    }
    
    SQLDBC_Int4 getAllocCount()
    {
        return m_alloc_count;
    }
    SQLDBC_Int4 getDeAllocCount()
    {
        return m_dealloc_count;
    }
    SQLDBC_Int4 getAllocBytes()
    {
        return m_alloc_size;
    }
    SQLDBC_Int4 getDeAllocBytes()
    {
        return m_dealloc_size;
    }

private:
    SQLDBC_Int4 m_alloc_size;
    SQLDBC_Int4 m_dealloc_size;
    SQLDBC_Int4 m_alloc_count;
    SQLDBC_Int4 m_dealloc_count;
    SAPDBMem_IRawAllocator *m_wrapped_instance;
};

class testcase1 : public SQLDBC_TestCase
{
private:
  SQLDBC_IRuntime* m_rt;
  SQLDBC_Connection *myCon[maxVal*maxVal];
  SQLDBC_Statement *myStmt[maxVal*maxVal*maxVal];
  SQLDBC_PreparedStatement *myPStmt[maxVal*maxVal*maxVal];
  SQLDBC_Environment *myEnv[maxVal];
  MyAllocator *myAllocator;


public:
	testcase1 ()
		: SQLDBC_TestCase ("testcase1"),
		m_rt(0),
		myAllocator(0)
	{
		char errorText[100];
    m_rt = SQLDBC::GetClientRuntime(errorText, sizeof(errorText) );
    myAllocator = new  MyAllocator(&(m_rt->getGlobalAllocator()));
  }

	~testcase1 ()
	{
  }

	virtual SQLDBC_Bool prepare ()
	{

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		return m_success;
	}


#define CONINDEX (j*maxVal+k)
#define STMTINDEX (j*maxVal+k*maxVal+i)
	virtual SQLDBC_Bool run ()
	{
 
   	for (int j=0; j< maxVal ;j++){
       myEnv[j] = new SQLDBC_Environment(m_rt);
       if(!myEnv[j]) {
         logerror("SQLDBC_Environment_createEnvironment() failed");
         return SQLDBC_FALSE;
       }
	   	for (int k=0; k< maxVal ;k++){
	
	  			myCon[CONINDEX]= myEnv[j]->createConnection(*myAllocator);
	        tc_errorcheck(myCon[CONINDEX])->connect(m_frame->hostname(),
	                                       SQLDBC_NTS,
	                                       m_frame->database(),
	                                       SQLDBC_NTS,
	                                       m_frame->username(),
	                                       SQLDBC_NTS,
	                                       m_frame->password(),
	                                       SQLDBC_NTS,
	                                       SQLDBC_StringEncodingAscii,
	                                       connectproperties());
	  
	   	    for (int i=0; i< maxVal ;i++){
	          myPStmt[STMTINDEX] = myCon[CONINDEX]->createPreparedStatement();
	          if(!myPStmt[STMTINDEX]) {
	              logerror("createPreparedStatement() failed");
	              return SQLDBC_FALSE;
	          }
	  
	          tc_errorcheck(myPStmt[STMTINDEX])->prepare("SELECT *   FROM DUAL", SQLDBC_StringEncodingAscii);
	          tc_errorcheck(myPStmt[STMTINDEX])->execute();
	          tc_errorcheck(myPStmt[STMTINDEX])->getResultSet();
	  
	          myStmt[STMTINDEX] = myCon[CONINDEX]->createStatement();
	          if(!myStmt[STMTINDEX]) {
	              logerror("createStatement() failed");
	              return SQLDBC_FALSE;
	          }
	          tc_errorcheck(myStmt[STMTINDEX])->execute("SELECT * FROM DUAL", SQLDBC_StringEncodingAscii);
	          tc_errorcheck(myPStmt[STMTINDEX])->getResultSet();
          }
	  	}
    } 
    /*release some SQLDBC handles*/
		for (int i=0; i< maxVal ;i++){
           delete myEnv[i];
    }
    checkMemory();
		return m_success;
	}
	
	SQLDBC_Bool checkMemory(){
	  if (myAllocator->getAllocCount() != myAllocator->getDeAllocCount() 
	   || myAllocator->getAllocBytes() != myAllocator->getDeAllocBytes()){
		  logerror("Allocated %d (%d Bytes) - Deallocated %d (%d Bytes)",
		       myAllocator->getAllocCount(),
		       myAllocator->getAllocBytes(), 
		       myAllocator->getDeAllocCount(),
		       myAllocator->getDeAllocBytes()
		  );
		  logerror("Warning Memoryleaks found %d (%d Bytes)",
		       myAllocator->getAllocCount()-myAllocator->getDeAllocCount(),
		       myAllocator->getAllocBytes() - myAllocator->getDeAllocBytes()
		  );
	  }
    return m_success;
	}
};



int main (int argc, char **argv)
{
	// Initalize from command line params
	SQLDBC_TestFrame testframe (argc, argv);

	/* TODO:
	 *
	 * add all test cases that you have described above
	 *
	 */
	testframe.addTestCase (new testcase1());
//	testframe.addTestCase (new testcase2());
//	testframe.addTestCase (new testcase3());

	if (testframe.run ())
		return 0;

	return 1;
}


