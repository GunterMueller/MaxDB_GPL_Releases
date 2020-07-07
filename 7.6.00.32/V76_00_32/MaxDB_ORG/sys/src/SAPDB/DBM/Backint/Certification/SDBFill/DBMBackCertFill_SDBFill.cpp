/*!
    \file    DBMSrvBackCertFill_SDBFill.cpp
    \author  TiloH
    \ingroup sdbfill program
    \brief   implementing the main function of sdbfill

    ========== licence begin  GPL
    Copyright (c) 1998-2006 SAP AG

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

#include "DBM/Backint/Certification/SDBFill/DBMBackCertFill_Output.hpp"
#include "Interfaces/SQLDBC/SQLDBC.h"
#include "ToolsCommon/Parsers/Syntax/ToolsParsersSyntax_Syntax.hpp"
#include "ToolsCommon/Parsers/ToolsParsers_ProgramParameterParser.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined (WIN32)
#include<windows.h>
#else
#include <unistd.h>
#endif

enum ProgramMode
{
    None,
    Init,
    Create,
    Append,
    Check,
    Delete
};

using namespace SQLDBC;

#if defined (WIN32)
void sleep(int sec)
{
    DWORD MillSec=1000*sec;
    Sleep(MillSec);
}
#endif

double absolute(double x)
{
    if(x<0.0)
        return -x;
    else
        return x;
}

int equal(int i,int io, double h1, double h1o, double h2, double h2o, DBMBackCertFill_Output & output)
{
    int rc=1;

    if(i!=io)
    {
        char buf[1024];

        sprintf(buf, "\n%d!=%d %d (c1)", i, io, i-io);
        output.write(DBMBackCertFill_Output::Ascii, buf);
            
        rc=0;
    }

    if(absolute(h1-h1o)>0.001)
    {
        char buf[1024];
        
        sprintf(buf, "\n%f!=%f %.15f (c2)", h1, h1o, (h1-h1o));
        output.write(DBMBackCertFill_Output::Ascii, buf);
            
        rc=0;
    }

    if(absolute(h2-h2o)>0.001)
    {
        char buf[1024];
        
        sprintf(buf, "\n%f!=%f %.15f (c3)", h2, h2o, (h2-h2o));
        output.write(DBMBackCertFill_Output::Ascii, buf);

        rc=0;
    }

    if(0==rc)
    {
        char buf[1024];
        
        sprintf(buf, "\nerror at %d(%d) %f(%f) %f(%f)", i, io, h1, h1o, h2, h2o);
        output.write(DBMBackCertFill_Output::Ascii, buf);
    }

    return rc;
}

bool commitWork(int sleepBeforeCommit, SQLDBC_Statement * statement)
{
    if(sleepBeforeCommit>0)
        sleep(sleepBeforeCommit);

    return (SQLDBC_OK==statement->execute("commit work") ? true : false);
}

bool commitWork(int sleepBeforeCommit, SQLDBC_Connection * connection)
{
    SQLDBC_Statement * statement=connection->createStatement();
    bool rc=commitWork(sleepBeforeCommit, statement);

    connection->releaseStatement(statement);

    return  rc;
}

#define DBMBackCertFill_SDBFILL_MAX_MASSROWS 1000

int InsertIntoTable(int From, int To, int TransactionLength, int SleepToCommit, SQLDBC_Connection * connection, DBMBackCertFill_Output & output)
{
    int i;
    int Ai[1000];
    double Ah1[1000],
           Ah2[1000];

    SQLDBC_PreparedStatement * preparedStatement=connection->createPreparedStatement();

    preparedStatement->prepare("insert sdbfill (c1,c2,c3) values (?,?,?)");

    preparedStatement->bindParameter(1, SQLDBC_HOSTTYPE_INT4,    Ai, NULL, sizeof(int));
    preparedStatement->bindParameter(2, SQLDBC_HOSTTYPE_DOUBLE, Ah1, NULL, sizeof(double));
    preparedStatement->bindParameter(3, SQLDBC_HOSTTYPE_DOUBLE, Ah2, NULL, sizeof(double));

    int LastCommitAt=0,
        j=From,
        k,
        OutputIntervall=1000,
        rc=1;

    if(OutputIntervall>TransactionLength)
        OutputIntervall=TransactionLength;

    while(j!=To)
    {
        if((i=To-j)>DBMBackCertFill_SDBFILL_MAX_MASSROWS)
            i=DBMBackCertFill_SDBFILL_MAX_MASSROWS;

        if(TransactionLength<i)
            i=TransactionLength;

        for(k=0;k<i;k++)
        {
            Ai[k]=j;
            Ah1[k]=(double)j;
            Ah2[k]=((double)j++)/3.0;
        }

        preparedStatement->setBatchSize(i);

        if(SQLDBC_OK != preparedStatement->execute())
        {
            char buf[1024];

            sprintf(buf, "\rInsert (%d,%f,%f-%d,%f,%f) failed with SQL-Code %d.",Ai[0],Ah1[0],Ah2[0],Ai[i-1],Ah1[i-1],Ah2[i-1], (int)connection->error().getErrorCode());
            output.write(DBMBackCertFill_Output::Error, buf);
            j=To;
            rc=0;
        }
        else
        {
            if(j%OutputIntervall==0)
            {
                char intAsString[51];
                sprintf(intAsString, "%d", (int)j);

                output.write(DBMBackCertFill_Output::Ascii, "\r%s", intAsString);
            }

            if(j-LastCommitAt>=TransactionLength)
            {
                if(!commitWork(SleepToCommit, connection))
                {
                    char intAsString[51];
                    sprintf(intAsString, "%d", (int)j);

                    output.write(DBMBackCertFill_Output::Error, "\nCommit failed at %s.", intAsString);
                    j=To;
                    rc=0;
                }
                else
                {
                    LastCommitAt=j;

                    char intAsString1[51];
                    sprintf(intAsString1, "%d", (int)i);

                    char intAsString2[51];
                    sprintf(intAsString2, "%d", (int)LastCommitAt);

                    output.write(DBMBackCertFill_Output::Ascii, ", %s, last commit at %s", intAsString1, intAsString2);
                    output.write(DBMBackCertFill_Output::Binary,"c");
                }
            }
        }
    }

    if(LastCommitAt!=j-1)
    {
        if(!commitWork(SleepToCommit, connection))
        {
            rc=0;

            char intAsString[51];
            sprintf(intAsString, "%d", (int)j);

            output.write(DBMBackCertFill_Output::Ascii, "\nCommit failed at %s.", intAsString);
        }
        else
        {
            LastCommitAt=j-1;

            char intAsString1[51];
            sprintf(intAsString1, "%d", (int)j);

            char intAsString2[51];
            sprintf(intAsString2, "%d", (int)LastCommitAt);

            output.write(DBMBackCertFill_Output::Ascii, "\r%s, final commit at %s                           ", intAsString1, intAsString2);
        }
    }

    connection->releaseStatement(preparedStatement);

    return rc;
}

int checkTable(int From, int To, SQLDBC_Connection * connection, DBMBackCertFill_Output & output)
{
    int io,
        rc=1;

    double h1o,
           h2o;

    int i;
    double h1,h2;
    
    i=From;

    SQLDBC_Statement * statement=connection->createStatement();
    SQLDBC_ResultSet * result=0;

    char statementAsString[1024];

    sprintf(statementAsString, "select * from sdbfill where c1>=%d", (int)i);

    if(SQLDBC_OK!=statement->execute(statementAsString))
    {
        output.write(DBMBackCertFill_Output::Error, "\nCould not execute '%s'.", statementAsString);
        rc=0;
    }
    else
    {
        result=statement->getResultSet();
    
        if(!result)
        {
            rc=0;
            output.write(DBMBackCertFill_Output::Error, "SQL command '%s' did not return a result set %s", statementAsString, statement->error().getErrorText());
        }
        else
        {
            result->bindColumn(1, SQLDBC_HOSTTYPE_INT4,    &i, NULL, sizeof(int));
            result->bindColumn(2, SQLDBC_HOSTTYPE_DOUBLE, &h1, NULL, sizeof(double));
            result->bindColumn(3, SQLDBC_HOSTTYPE_DOUBLE, &h2, NULL, sizeof(double));
        }
    }
    
    if(rc)
    {
        if(SQLDBC_OK!=result->next() || SQLDBC_OK!=result->getRowSet()->fetch())
        {
            output.write(DBMBackCertFill_Output::Error, "Could not fetch first result row.\nSQLDBC: %s", result->error().getErrorText());
            rc=0;
        }
        else
            output.write(DBMBackCertFill_Output::Ascii, "\rFetched first");

        char buf[1024];
        sprintf(buf," (%d %f %f)\n", i, h1, h2);
        output.write(DBMBackCertFill_Output::Ascii, buf);

        if(From<i)
            rc=0;
    }
    
    if(rc)
    {
        while(i!=From && rc)
        {
            io=i;

            if(SQLDBC_OK!=result->next() || SQLDBC_OK!=result->getRowSet()->fetch())
            {
                char intAsString[51];
                sprintf(intAsString, "%d", (int)From);

                output.write(DBMBackCertFill_Output::Error, "\nCould not fetch next element (after row %s).", intAsString);

                rc=0;               
            }
            else
                if(i==io)
                {
                    char intAsString[51];
                    sprintf(intAsString, "%d", (int)From);

                    output.write(DBMBackCertFill_Output::Error, "\nerror could not reach %s", intAsString);
                        
                    rc=0;
                }
                else
                    if(0==i%1000)
                    {
                        char intAsString[51];
                        sprintf(intAsString, "%d", (int)i);

                        output.write(DBMBackCertFill_Output::Ascii, "\rfetched %s                    ", intAsString);
                    }
        }

        if(rc)
        {
            io=i;
            h1o=(double)io;
            h2o=(double)h1o/3.0;

            if(!equal(io, i, h1o, h1, h2o, h2, output))
            {
                io=To;
                rc=0;
            }

            for(io++; io<To; io++)
            {
                h1o=io;
                h2o=h1o/3.0;

                if(SQLDBC_OK!=result->next() || SQLDBC_OK!=result->getRowSet()->fetch())
                {
                    char intAsString[51];
                    sprintf(intAsString, "%d", (int)io);

                    output.write(DBMBackCertFill_Output::Error, "\nCould not fetch next element (row %s).", intAsString);

                    io=To;
                    rc=0;
                }
                else
                {
                    if(!equal(io,i,h1o,h1,h2o,h2, output))
                    {
                        io=To;
                        rc=0;
                    }

                    if(0==io%1000)
                    {
                        char intAsString[51];
                        sprintf(intAsString, "%d", (int)io);

                        output.write(DBMBackCertFill_Output::Ascii, "\r%s                    ", intAsString);
                    } 
                }
            }
        }

        if(!commitWork(0, statement))
            output.write(DBMBackCertFill_Output::Error, "\rCommit after check failed.");
    }

    connection->releaseStatement(statement);

    return rc;
}

int deleteFromMytable(int From,int To, int TransactionLength, int SleepBeforeCommit, SQLDBC_Connection * connection, DBMBackCertFill_Output & output)
{
    int rc=1,
        j=From,
        k,
        LastCommitAt=0;
    
    int Ai[1000];
    int i;

    SQLDBC_PreparedStatement * preparedStatement=connection->createPreparedStatement();

    preparedStatement->prepare("delete from sdbfill key c1=?");
    preparedStatement->bindParameter(1, SQLDBC_HOSTTYPE_INT4, Ai, NULL, sizeof(int));

    while(j!=To)
    {
        i=To-j;

        if(i>DBMBackCertFill_SDBFILL_MAX_MASSROWS)
            i=DBMBackCertFill_SDBFILL_MAX_MASSROWS;

        if(TransactionLength<i)
            i=TransactionLength;

        for(k=0;k<i;k++)
            Ai[k]=j++;

        preparedStatement->setBatchSize(i);

        if(SQLDBC_OK!=preparedStatement->execute())
        {
            char buf[1024];
            sprintf(buf, "\nDelete (%d,-%d) failed.", (int)Ai[0], int(Ai[i-1]));
            output.write(DBMBackCertFill_Output::Error, buf);

            rc=0;
            j=To;
        }
        else
        {
            if(0==j%1000)
            {
                char intAsString[51];
                sprintf(intAsString, "%d", (int)j);

                output.write(DBMBackCertFill_Output::Ascii, "\r%s", intAsString);
            }

            if(j-LastCommitAt>=TransactionLength)
            {
                char intAsString[51];
                sprintf(intAsString, "%d", (int)i);

                if(!commitWork(SleepBeforeCommit, connection))
                {
                    output.write(DBMBackCertFill_Output::Error, "\nCommit failed at %s.", intAsString);

                    rc=0;
                    j=To;
                }
                else
                {
                    LastCommitAt=j;

                    char intAsString2[51];
                    sprintf(intAsString2, "%d", (int)LastCommitAt);

                    output.write(DBMBackCertFill_Output::Ascii, "\r%s, last commit at %s", intAsString, intAsString2);
                    output.write(DBMBackCertFill_Output::Binary, "c");
                }
            }
        }
    }

    if(LastCommitAt!=i-1)
    {
        if(!commitWork(SleepBeforeCommit, connection))
            rc=0;
        else
        {
            LastCommitAt=i-1;

            char intAsString1[51];
            sprintf(intAsString1, "%d", (int)i);

            char intAsString2[51];
            sprintf(intAsString2, "%d", (int)LastCommitAt);

            output.write(DBMBackCertFill_Output::Ascii, "\r%s, finale commit at %s", intAsString1, intAsString2);
        }
    }

    connection->releaseStatement(preparedStatement);

    return rc;
}

int main(unsigned argc, char ** argv)
{
    int fromRow=0,
        toRow=0,
        transactionLength=1,
        sleepToCommit=0,
        retCode=1;

    ProgramMode mode=None;

    const char * nameDB="";
    const char * nameDBA="DBA";
    const char * passwordDBA="DBA";
	const char * node="localhost";

    DBMBackCertFill_Output output;

    char usageString[]="usage: sdbfill <DBName> (init|create|append|check|delete) [<From> <To>\n"
                       "               [<TransactionLength> [<SleepBeforeCommit> [ (q|a|b) \n"
                       "               [<DBAUser> <DBAPassword>]]]]] [<node>]\n";

    ToolsParsers_ProgramParameterParser parser(
        argc, argv,
        "<DBName> (init|create|append|check|delete) [<From>DEF=0 <To>DEF=0 ["
        "<TransactionLength>DEF=1 [<SleepBeforeCommit>DEF=0 [ (q|a|b) [<dba>DEF=DBA "
        "<dbapassword>DEF=DBA]]]]] [<node>DEF=localhost]");

    const ToolsParsers_ParseResult & parseResult=parser.parse();

    if(ToolsParsers_ParseResult::NoError!=parseResult.getResultCode())
    {
        switch(parseResult.getResultCode())
        {
            case ToolsParsers_ParseResult::ErrorOutOfMemory:
                output.write(DBMBackCertFill_Output::Error, "Could not allocate enough memory.");
                break;
            case ToolsParsers_ParseResult::ErrorSyntax:
                output.write(DBMBackCertFill_Output::Error, usageString);
                break;
            default:
                output.write(DBMBackCertFill_Output::Error, "Internal error during parameter parsing.");
        }

        return retCode;
    }
    
    nameDB=parseResult.getVariable("<DBName>").getValue();

    if(parseResult.getKeyword("create").isSet())
        mode=Create;

    if(parseResult.getKeyword("check").isSet())
        mode=Check;

    if(parseResult.getKeyword("append").isSet())
        mode=Append;

    if(parseResult.getKeyword("delete").isSet())
        mode=Delete;

    if(parseResult.getKeyword("init").isSet())
        mode=Init;

    fromRow=atoi(parseResult.getVariable("<From>").getValue());
    toRow=atoi(parseResult.getVariable("<To>").getValue());
                            
    if(fromRow>toRow) //exchange values
    {
        int exh=toRow;
        toRow=fromRow;
        fromRow=exh;   
    }

    transactionLength=atoi(parseResult.getVariable("<TransactionLength>").getValue());

	if(transactionLength <= 0 &&
       (mode==Create || mode== Append || mode==Delete))
    {
		output.write(DBMBackCertFill_Output::Error, "<TransactionLength> should be > 0.\n");
		return retCode;
	}

    sleepToCommit=atoi(parseResult.getVariable("<SleepBeforeCommit>").getValue());

    if(parseResult.getKeyword("q").isSet())
        output.setStyle(DBMBackCertFill_Output::NoOutput);

    if(parseResult.getKeyword("a").isSet())
        output.setStyle(DBMBackCertFill_Output::Ascii);

    if(parseResult.getKeyword("b").isSet())
        output.setStyle(DBMBackCertFill_Output::Binary);

    nameDBA=parseResult.getVariable("<dba>").getValue();
    passwordDBA=parseResult.getVariable("<dbapassword>").getValue();

	node=parseResult.getVariable("<node>").getValue();

    char errorText[200];

    SQLDBC_IRuntime * runtime = GetClientRuntime(errorText, sizeof(errorText));

    if (!runtime)
        output.write(DBMBackCertFill_Output::Error, "Getting instance of the ClientRuntime failed.\nSQLDBC: %s\n", errorText);
    else
    {
        output.write(DBMBackCertFill_Output::Ascii, "\rGot client runtime.");

        SQLDBC::SQLDBC_Environment env(runtime);

        SQLDBC_Connection * connection = env.createConnection();
        SQLDBC_Retcode rcSQL = connection->connect(node, nameDB, nameDBA, passwordDBA);

        if(SQLDBC_OK != rcSQL)
        {
            output.write(
                DBMBackCertFill_Output::Error,
                "\rCould not connect to %s with %s,%s.\n",
                nameDB,
                nameDBA,
                passwordDBA);

            char intAsString[51];
            sprintf(intAsString, "%d", (int)connection->error().getErrorCode());

            output.write(
                DBMBackCertFill_Output::Error,
                "SQL-Error: %s.\nSQLDBC: %s.\n",
                intAsString,
                connection->error().getErrorText());                        
        }
        else
        {
            if(Init==mode)
            {
				const char *h1="alter user \"";
				const char *h2="\" not exclusive";

				char * command=new char[strlen(h1)+strlen(nameDBA)+strlen(h2)+1];

                if(0==command)
                    output.write(DBMBackCertFill_Output::Error, "Memory allocation failed.\n");
                else
                {
                    sprintf(command, "%s%s%s", h1, nameDBA, h2);
                    output.write(DBMBackCertFill_Output::Ascii, "\rExecuting command '%s'.                       ", command);

                    SQLDBC_Statement * statement = connection->createStatement();
                    rcSQL = statement->execute(command);

                    if(SQLDBC_OK == rcSQL)
                    {
                        output.write(DBMBackCertFill_Output::Ascii, "\rCommand '%s' succeded.                      ", command);
                        retCode=0;
                    }
                    else
                    {
                        char intAsString[51];
                        sprintf(intAsString, "%d", (int)connection->error().getErrorCode());

                        output.write(DBMBackCertFill_Output::Error, "\rCommand '%s' failed.                         \n", command);
                        output.write(DBMBackCertFill_Output::Error, "SQL-Error: %s.\nSQLDBC: %s.\n",
                            intAsString,
                            connection->error().getErrorText());
                    }
                }
            }
            else
            {
                SQLDBC_Statement * statement = connection->createStatement();
                rcSQL = statement->execute("exists table sdbfill");
                int codeSQL=connection->error().getErrorCode();

                if( (Check==mode || Append==mode || Delete==mode) && SQLDBC_OK!=rcSQL)
                {
                    output.write(DBMBackCertFill_Output::Error, "\rTable 'sdbfill' does not exists, but is required in mode %s.\n", Check==mode?"check":Append==mode?"append":"delete");
                }
                else
                {
                    int TableOk=1;

                    switch(mode)
                    {
                        case Create:
                        {
                            switch(codeSQL)
                            {
                                case 0:
                                {
                                    rcSQL = statement->execute("drop table sdbfill");
                                    codeSQL=connection->error().getErrorCode();

                                    if(0==codeSQL)
                                    {
                                        output.write(DBMBackCertFill_Output::Ascii, "\rDropped table 'sdbfill'.         \n");

                                        if(!commitWork(sleepToCommit, statement))
										{
											output.write(DBMBackCertFill_Output::Error, "\rCommit failed.                   ");

											TableOk=0;
											break;
										}
                                    }
                                    else
                                    {
                                        output.write(DBMBackCertFill_Output::Error, "\rCould not drop table 'sdbfill'.                \n");
                                        TableOk=0;
                                        break;
                                    }
                                }

                                case -4004:
                                {
                                    rcSQL = statement->execute("create table sdbfill (c1 int key,c2 float,c3 float)");
                                    codeSQL=connection->error().getErrorCode();
                                    
                                    if(SQLDBC_OK==rcSQL)
                                        output.write(DBMBackCertFill_Output::Ascii, "\rCreated table 'sdbfill'.                     \n");
                                    else
                                    {
                                        output.write(DBMBackCertFill_Output::Error, "\rCould not create table 'sdbfill'.              \n");

                                        char intAsString[51];
                                        sprintf(intAsString, "%d", (int)connection->error().getErrorCode());

                                        output.write(DBMBackCertFill_Output::Error, "SQL-Error: %s.\nSQLDBC: %s.\n",
                                            intAsString,
                                            connection->error().getErrorText());

                                        TableOk=0;
                                    }
                                } break;

                                default:
                                {
                                    char intAsString[51];
                                    sprintf(intAsString, "%d", (int)connection->error().getErrorCode());

                                    output.write(DBMBackCertFill_Output::Error, "\rGot unexpected SQL-Code %s as answer on 'exists table sdbfill'.\n", intAsString);
                                    TableOk=0;
                                } break;
                            }
                        }

                        case Append:
                        {
                            if(TableOk)
                                if(InsertIntoTable(fromRow, toRow, transactionLength, sleepToCommit, connection, output))
                                {
                                    retCode=0;

                                    char intAsString[51];
                                    sprintf(intAsString, "%d", toRow-fromRow);
                                    output.write(DBMBackCertFill_Output::Ascii, "\n%s rows inserted successfully.                                        \n", intAsString);
                                }
                        } break;

                        case Check:
                        {
                            output.write(DBMBackCertFill_Output::Ascii, "\rChecking table 'sdbfill'.                                          \n");

                            if(checkTable(fromRow, toRow, connection, output))
                            {
                                output.write(DBMBackCertFill_Output::Ascii, "\rtable 'sdbfill' is OK.                                                     \n");
                                retCode=0;
                            }
                            else
                                output.write(DBMBackCertFill_Output::Ascii, "\rtable 'sdbfill' is not OK.                                                 \n");

                        } break;

                        case Delete:
                        {
                            if(deleteFromMytable(fromRow, toRow, transactionLength, sleepToCommit, connection, output))
                            {
                                output.write(DBMBackCertFill_Output::Ascii,"\rRows deleted successfully                                               \n");
                                retCode=0;
                            }
                        } break;

                        default:
                            output.write(DBMBackCertFill_Output::Error, "Unexpected program-mode.\n");
                    }
                }
            }

            connection->commit();
        }

        output.write(DBMBackCertFill_Output::Binary, "e");
    }

    return retCode;
}
