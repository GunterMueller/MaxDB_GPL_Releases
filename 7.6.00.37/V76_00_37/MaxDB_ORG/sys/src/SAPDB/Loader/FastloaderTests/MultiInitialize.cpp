/*!================================================================

 module:        MultiInitialize.cpp

 -------------------------------------------------------------------

 responsible:   D025086

 special area:  SQL

 description:   Example: Simple Execute

 ------------------------------------------------------------------


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




 ===================================================================*/

#include <stdio.h>
#include "Interfaces/sapdbifr.h"
#include "SAPDB/Loader/Loader_Types.hpp"
#include "SAPDB/Loader/Loader_Loader.hpp"
#include "SAPDB/Loader/FastloaderTests/FastloaderTestUtils.h"


static const LOADER_Char globalCharBuffer[301] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuv"
                                                 "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuv"
                                                 "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuv";
static const LOADER_Char globalBlankBuffer[101] = "                                                                                                    ";


class MultiInitialize : public FastloaderTestCase
{
public:
  MultiInitialize ()
    : FastloaderTestCase ("MultiInitialize")
  {}

    virtual LOADER_Bool run ()
    {
        LOADER_Retcode rc = LOADER_OK;

        typedef struct paramstruct          // param_X immer 2 Byte groesser als benoetigt, das sp77sprintf noch null-bytes schreibt
        {
            LOADER_Char param_1[33];
            LOADER_Length paramlen_1;

            LOADER_Char param_2[33];
            LOADER_Length paramlen_2;
                
            LOADER_Char param_3[33];
            LOADER_Length paramlen_3;

            LOADER_Char param_4[33];
            LOADER_Length paramlen_4;

            LOADER_Char param_5[301];
            LOADER_Length paramlen_5;

            LOADER_Char param_6[33];
            LOADER_Length paramlen_6;
        }
        paramstruct;

        Loader_FastloadCommand* pFLCmd       = NULL;
        LOADER_Int4             lRecordCount = 10;

    
        if (LOADER_TRUE == execSQL("CREATE TABLE MULTIINITIALIZE1(A CHAR(32) ASCII, B CHAR(32) ASCII, C CHAR(32), D CHAR(32) ASCII, E CHAR(300) ASCII, F CHAR(32) ASCII)"))
        {
            if (LOADER_TRUE == execSQL("CREATE TABLE MULTIINITIALIZE2(A CHAR(32) ASCII, B CHAR(32) ASCII, C CHAR(32) ASCII, D CHAR(32) ASCII, E CHAR(300) ASCII, F CHAR(32) ASCII)"))
            {
                execSQL("COMMIT WORK");
            }
        }

        // Create a FastloadCommand object
        pFLCmd = FastloadCommand();

        // Initialize the Fastload command to load 1st table
        LOADER_Char _pszTabName[128];
        _pszTabName[0] = 0;

        sp77sprintfUnicode(sp77encodingUCS2Native, _pszTabName, 128, "%=.*S", sp77encodingAscii, 16, "MULTIINITIALIZE1");

        rc = pFLCmd->initialize(_pszTabName, NULL, 0, LOADER_STRINGENCODING_UCS2NATIVE);
        if (LOADER_OK != rc)
        {
            logerror("Initialization of FasloadCommand for table \"MULTIINITIALIZE1\" failed.");
            logerror (pFLCmd->getErrorText());
        }
        else
        {
            LOADER_UInt4 ARRAYSIZE      = 10;
            paramstruct* pmyValueArray  = new paramstruct[ARRAYSIZE];

            pFLCmd->setBindingType(sizeof(paramstruct), LOADER_TRUE);
            pFLCmd->SetBatchSize(ARRAYSIZE);

            LOADER_UInt4 k = 0;
            LOADER_UInt4 _pos = 0;

            // NON UNICODE
            for (k = 0; k < ARRAYSIZE; ++k)
            {
                _pos = sprintf(pmyValueArray[k].param_1, "%d_1", k);
                _pos = sprintf(pmyValueArray[k].param_1 + _pos, "%.*s", 32-_pos, globalCharBuffer);
                pmyValueArray[k].paramlen_1 = strlen(pmyValueArray[k].param_1);

                _pos = sprintf(pmyValueArray[k].param_2, "%d_2", k);
                _pos = sprintf(pmyValueArray[k].param_2 + _pos, "%.*s", 32-_pos, globalCharBuffer);
                pmyValueArray[k].paramlen_2 = strlen(pmyValueArray[k].param_2);

                _pos = sprintf(pmyValueArray[k].param_3, "%d_3", k);
                _pos = sprintf(pmyValueArray[k].param_3 + _pos, "%.*s", 32-_pos, globalCharBuffer);
                pmyValueArray[k].paramlen_3 = strlen(pmyValueArray[k].param_3);

                _pos = sprintf(pmyValueArray[k].param_4, "%d_4", k);
                _pos = sprintf(pmyValueArray[k].param_4 + _pos, "%.*s", 32-_pos, globalCharBuffer);
                pmyValueArray[k].paramlen_4 = strlen(pmyValueArray[k].param_4);

                _pos = sprintf(pmyValueArray[k].param_5, "%d_5", k);
                _pos = sprintf(pmyValueArray[k].param_5 + _pos, "%.*s", 300-_pos, globalCharBuffer);
                pmyValueArray[k].paramlen_5 = strlen(pmyValueArray[k].param_5);

                _pos = sprintf(pmyValueArray[k].param_6, "%d_6", k);
                _pos = sprintf(pmyValueArray[k].param_6 + _pos, "%.*s", 32-_pos, globalCharBuffer);
                pmyValueArray[k].paramlen_6 = strlen(pmyValueArray[k].param_6);
            }


            pFLCmd->bindParameter(1, LOADER_HostType_ASCII, &pmyValueArray[0].param_1, &pmyValueArray[0].paramlen_1, sizeof(pmyValueArray[0].param_1));
            pFLCmd->bindParameter(2, LOADER_HostType_ASCII, &pmyValueArray[0].param_2, &pmyValueArray[0].paramlen_2, sizeof(pmyValueArray[0].param_2));
            pFLCmd->bindParameter(3, LOADER_HostType_ASCII, &pmyValueArray[0].param_3, &pmyValueArray[0].paramlen_3, sizeof(pmyValueArray[0].param_3));
            pFLCmd->bindParameter(4, LOADER_HostType_ASCII, &pmyValueArray[0].param_4, &pmyValueArray[0].paramlen_4, sizeof(pmyValueArray[0].param_4));
            pFLCmd->bindParameter(5, LOADER_HostType_ASCII, &pmyValueArray[0].param_5, &pmyValueArray[0].paramlen_5, sizeof(pmyValueArray[0].param_5));
            pFLCmd->bindParameter(6, LOADER_HostType_ASCII, &pmyValueArray[0].param_6, &pmyValueArray[0].paramlen_6, sizeof(pmyValueArray[0].param_6));

            rc = pFLCmd->Execute();
            if (LOADER_OK != rc)
            {
                logerror("Execution of FasloadCommand for table \"MULTIINITIALIZE1\" failed.");
                logerror (pFLCmd->getErrorText());
                pFLCmd->End(LOADER_TRUE);
            }
            else
            {
                // Commit FastloadCommand for table MULTIINITIALIZE1
                pFLCmd->End();


                // Initialize Fastload command to load 2nd table
                sp77sprintfUnicode(sp77encodingUCS2Native, _pszTabName, 128, "%=.*S", sp77encodingAscii, 16, "MULTIINITIALIZE2");

                rc = pFLCmd->initialize(_pszTabName, NULL, 0, LOADER_STRINGENCODING_UCS2NATIVE);
                if (LOADER_OK != rc)
                {
                    logerror("Initialization of FasloadCommand for table \"MULTIINITIALIZE2\" failed.");
                    logerror (pFLCmd->getErrorText());
                }
                else
                {
                    pFLCmd->setBindingType(sizeof(paramstruct), LOADER_TRUE);
                    pFLCmd->SetBatchSize(ARRAYSIZE);

                    pFLCmd->bindParameter(1, LOADER_HostType_ASCII, &pmyValueArray[0].param_1, &pmyValueArray[0].paramlen_1, sizeof(pmyValueArray[0].param_1));
                    pFLCmd->bindParameter(2, LOADER_HostType_ASCII, &pmyValueArray[0].param_2, &pmyValueArray[0].paramlen_2, sizeof(pmyValueArray[0].param_2));
                    pFLCmd->bindParameter(3, LOADER_HostType_ASCII, &pmyValueArray[0].param_3, &pmyValueArray[0].paramlen_3, sizeof(pmyValueArray[0].param_3));
                    pFLCmd->bindParameter(4, LOADER_HostType_ASCII, &pmyValueArray[0].param_4, &pmyValueArray[0].paramlen_4, sizeof(pmyValueArray[0].param_4));
                    pFLCmd->bindParameter(5, LOADER_HostType_ASCII, &pmyValueArray[0].param_5, &pmyValueArray[0].paramlen_5, sizeof(pmyValueArray[0].param_5));
                    pFLCmd->bindParameter(6, LOADER_HostType_ASCII, &pmyValueArray[0].param_6, &pmyValueArray[0].paramlen_6, sizeof(pmyValueArray[0].param_6));

                    rc = pFLCmd->Execute();
                }

                if (LOADER_OK != rc)
                {
                    logerror("Execution of FasloadCommand for table \"MULTIINITIALIZE2\" failed.");
                    logerror (pFLCmd->getErrorText());
                    pFLCmd->End(LOADER_TRUE);
                }
                else
                {
                    pFLCmd->End();
                }
            }
        }

        // Release Fastload command
        releaseFLCommand();


        // Check result
        if (LOADER_OK == rc)
        {
            SQLDBC_Retcode rcFETCH = SQLDBC_OK;

            // Fetch values of column E in 2nd table completely
            // Those values must be right. They are of kernel internal length type very long variable long columns. This
            // test checks if the multiple initialization of the same FastloadCommand object for different tables initializes
            // internal counter variables. If not the values for column E are garbage.
            SQLDBC_Statement *pStmt = statement();
            pStmt->execute("SELECT E FROM MULTIINITIALIZE2");

            SQLDBC_ResultSet* rs        = pStmt->getResultSet();
            LOADER_Int4       _rowCount = rs->getResultCount();
            
            if (lRecordCount != _rowCount)
            {
                logerror("Expected %d rows in table \"MULTIINITIALIZE2\", found %d rows", lRecordCount, _rowCount);
            }
            else
            {
                // The right amount of data has been loaded. Check if it is the right data, too.
                rcFETCH = rs->next();
                if (SQLDBC_OK != rcFETCH)
                {
                    logerror("Fetching first row of data failed.");
                }
                else
                {
                    LOADER_Char _szOutput[301];
                    SQLDBC_Length _len = 0;
                    rcFETCH = rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, _szOutput, &_len, 301, SQLDBC_TRUE);
                    if (SQLDBC_OK == rcFETCH)
                    {
                        if (_len != 300)
                        {
                            logerror("False data in column E of table \"MULTIINITIALIZE2\"; data has length %d", _len);
                        }
                        else
                        {
                            if (0 != strcmp(_szOutput,
                                            "0_5abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrs"))
                            {
                                logerror("False data in column E of table \"MULTIINITIALIZE2\": %s", _szOutput);
                            }
                        }
                    }
                    else
                    {
                        logerror("Fetching first row of data failed.");
                    }
                }
            }   // end if (lRecordCount != _rowCount)
        }

        return m_success;
    }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  FastloaderTestFrame testframe (argc, argv);
  testframe.addTestCase (new MultiInitialize ());
  if (testframe.run ()) 
    return 0;

  return 1;
}
