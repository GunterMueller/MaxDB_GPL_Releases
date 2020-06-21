void	verify_fdir_index_info ();
void	verify_fdir_table_info ();
void	movep (int, int, const char *, ...);
void	prot (const char *, ...);
/*------------------------------------------------------------------------*/
void verify_fdir_index_info ()
/*
	compares the file-directory counter for indexes with the content of INDEXSTATISTICS
	if there are any differences, there will be a protocol entry and a sqldump will be forced
*/
{
    typedef char	c100 [101] ;

	c100 tablename={0};
    c100 indexname={0};
    c100 description={0};
    char stmtText[501]={0};
    int numeric_value=0;
	int counter=0;

    int secKeyCount=0;
    int refSecKeyCount=0;
	int indexPageCount=0;
    int refIndexPageCount=0;
	int indexLeafCount=0;
    int refIndexLeafCount=0;

	stmt   = conn->createStatement();
	pstmt  = conn->createPreparedStatement();
	pstmt2 = conn->createPreparedStatement();
	pstmt3 = conn->createPreparedStatement();

    movep (line, col, "===== VERIFY INDEX COUNTERS") ;
	
	returncode = pstmt->prepare("SELECT TABLENAME, INDEXNAME FROM INDEXES "
                                "WHERE OWNER = USER AND FILESTATE = 'OK'");
    CE("verify_fdir_index_info: prepare1", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

    returncode = pstmt2->prepare(
        "Select f.ENTRYCOUNT, f.TREEINDEXSIZE, f.TREELEAVESSIZE"
        " from FILES f, INDEXES i"
        " where i.OWNER = user and i.TABLENAME = ? and i.INDEXNAME = ?"
        " and i.FILEID = f.FILEID");
    CE("verify_fdir_index_info: prepare2", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

    returncode = pstmt3->prepare(
        "Select DESCRIPTION, NUMERIC_VALUE from INDEXSTATISTICS"
        " where OWNER = user and TABLENAME = ? and INDEXNAME = ?"
        " and (DESCRIPTION = 'Secondary keys (index lists)' or"
        " DESCRIPTION = 'Index pages' or"
        " DESCRIPTION = 'Leaf  pages')");
    CE("verify_fdir_index_info: prepare3", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);

    returncode = pstmt->execute();
	CE("verify_fdir_index_info: SELECT TABLENAME ...", 1, conn->ERRORCODE, conn->ERRORTEXT);

    if ( pstmt->ERRORCODE == 0 )
    {
        rs = pstmt->getResultSet();
        while (rs->next() == SQLDBC_OK)
        {
			SQLDBC_Length l;

            returncode = rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, tablename, &l, sizeof(tablename), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_index_info (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
            returncode = rs->getObject(2, SQLDBC_HOSTTYPE_ASCII, indexname, &l, sizeof(indexname), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_index_info (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);

            sprintf(stmtText, "LOCK TABLE %s IN SHARE MODE", tablename);
            returncode = stmt->execute(stmtText);
            CE("verify_fdir_index_info: LOCK TABLE", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

            if ( (stmt->ERRORCODE == 500) || (stmt->ERRORCODE == 400)) // Lock Request Timeout or Lock Collision
            {
                continue;
            }

            counter++;

            returncode = pstmt2->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, tablename, NULL, sizeof (tablename), true);
			if (returncode != SQLDBC_OK) CE("error with bindParameter in verify_fdir_index_info (col 1)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
            returncode = pstmt2->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, indexname, NULL, sizeof (indexname), true);
			if (returncode != SQLDBC_OK) CE("error with bindParameter in verify_fdir_index_info (col 2)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

            returncode = pstmt2->execute();

			if (returncode == SQLDBC_NO_DATA_FOUND)
			{
				commit();
				continue;
			}

			if (returncode != SQLDBC_OK)
            {
                prot("error during select on FILES: %d = %s (rc = %d)\n", pstmt2->ERRORCODE, pstmt2->ERRORTEXT, returncode);
            }

            if (returncode == SQLDBC_OK)
            {
                rs2 = pstmt2->getResultSet();

                if (rs2->next() == SQLDBC_OK)
                {
                    returncode = rs2->getObject(1, SQLDBC_HOSTTYPE_INT4, &secKeyCount, &l, sizeof(secKeyCount), true);
					if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_index_info (col 3)", 1, rs2->ERRORCODE, rs2->ERRORTEXT);
                    returncode = rs2->getObject(2, SQLDBC_HOSTTYPE_INT4, &indexPageCount, &l, sizeof(indexPageCount), true);
					if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_index_info (col 4)", 1, rs2->ERRORCODE, rs2->ERRORTEXT);
                    returncode = rs2->getObject(3, SQLDBC_HOSTTYPE_INT4, &indexLeafCount, &l, sizeof(indexPageCount), true);
					if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_index_info (col 5)", 1, rs2->ERRORCODE, rs2->ERRORTEXT);

                    rs2->close();
                }
            }

            if (returncode != SQLDBC_OK)
            {
                prot("error during get info from fdir\n");
            }

            if (returncode == SQLDBC_OK)
            {
                returncode = pstmt3->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, tablename, NULL, sizeof (tablename), true);
				if (returncode != SQLDBC_OK) CE("error with bindParameter in verify_fdir_index_info (col 3)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);
                returncode = pstmt3->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, indexname, NULL, sizeof (indexname), true);
				if (returncode != SQLDBC_OK) CE("error with bindParameter in verify_fdir_index_info (col 4)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);

                returncode = pstmt3->execute();

                if (returncode == SQLDBC_OK)
                {
                    rs2 = pstmt3->getResultSet();
                }
                else
                {
                    prot("error during execute indexstatistics, %d = %s (rc = %d)\n", pstmt3->ERRORCODE, pstmt3->ERRORTEXT, returncode);
                }

            }

            if (returncode == SQLDBC_OK)
            {
                while (rs2->next() == SQLDBC_OK)
                {
                    returncode = rs2->getObject(1, SQLDBC_HOSTTYPE_ASCII, description, &l, sizeof(description), true);
					if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_index_info (col 6)", 1, rs2->ERRORCODE, rs2->ERRORTEXT);
                    returncode = rs2->getObject(2, SQLDBC_HOSTTYPE_INT4, &numeric_value, &l, sizeof(numeric_value), true);
					if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_index_info (col 7)", 1, rs2->ERRORCODE, rs2->ERRORTEXT);

                    if (returncode == SQLDBC_OK)
                    {
                        if (strcmp(description, "Secondary keys (index lists)" ) == 0)
                            refSecKeyCount = numeric_value;
                        else if (strcmp(description,"Index pages") == 0)
                            refIndexPageCount = numeric_value*8;
                        else if (strcmp(description,"Leaf  pages") == 0)
                            refIndexLeafCount = numeric_value*8;
                        else
                        {
                            prot("error with unknown description\n", rs2->ERRORCODE, description, returncode);
                        }
                    }
                }
            }

            if (returncode == SQLDBC_OK)
            {
                rs2->close();

                if ( (secKeyCount != refSecKeyCount)
                     || (indexPageCount != refIndexPageCount)
                     || (indexLeafCount != refIndexLeafCount) )
                {
					prot("mismatching counters - checking index %s of table %s (FILES, INDEXSTATISTICS): "
						"entryCount (%d, %d); "
						"indexPageCount (%d, %d); "
                        "leafPageCount (%d, %d)\n",
						 indexname, tablename,
                         secKeyCount, refSecKeyCount,
                         indexPageCount, refIndexPageCount,
                         indexLeafCount, refIndexLeafCount);

                    stmt->execute("force savepoint"); // make sure index is not bad on restart
                    print_f ("** calling sqldump **", 0, tablename) ;
                    Sqldump ("verify_fdir_index_info()", 0, "Error with force savepoint!", conn) ;
                }
            }

            if (returncode != SQLDBC_OK)
            {
                    prot("error comparing counters\n", returncode, NULL, 0);
                    print_f ("** calling sqldump **", 0, indexname) ;
                    Sqldump ("verify_fdir_index_info()", 0, "Error comparing counters in verify_fdir_index_info()!", conn) ;
            }

            commit () ;
        }
    }

	prot("verify_fdir_index_info checks %d combinations of indices and tables\n", counter);

	conn->releaseStatement(pstmt3);
	pstmt3 = 0;
	conn->releaseStatement(pstmt2);
	pstmt2 = 0;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void verify_fdir_table_info ()
/*
	compares the file-directory counter for tables with the content of TABLESTATISTICS
	if there are any differences, there will be a protocol entry and a sqldump will be forced
*/
{
    typedef char	c100 [101] ;

    c100 tablename={0};
    c100 description={0};
    char stmtText[501]={0};
    int numeric_value=0;
	int counter=0;

    int entryCount=0;
    int refEntryCount=0;
	int indexPageCount=0;
    int refIndexPageCount=0;
	int indexLeafCount=0;
    int refIndexLeafCount=0;
	unsigned int lobPageCount=0;
    unsigned int refLobPageCount=0;

	stmt   = conn->createStatement();
	pstmt  = conn->createPreparedStatement();
	pstmt2 = conn->createPreparedStatement();
	pstmt3 = conn->createPreparedStatement();

    movep (line, col, "===== VERIFY TABLE COUNTERS") ;
	
	returncode = pstmt->prepare("SELECT TABLENAME FROM TABLES "
                                " WHERE OWNER = USER"
                                " AND TABLETYPE = 'TABLE'");
    CE("verify_fdir_table_info: prepare1", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	returncode = pstmt2->prepare(
        "Select f.ENTRYCOUNT, f.TREEINDEXSIZE, f.TREELEAVESSIZE, f.LOBSIZE"
        " from FILES f, TABLES t"
        " where t.OWNER = user and t.TABLENAME = ?"
        " and t.TABLEID = f.FILEID");
    CE("verify_fdir_table_info: prepare2", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

    returncode = pstmt3->prepare(
        "Select DESCRIPTION, NUMERIC_VALUE from TABLESTATISTICS"
        " where OWNER = user and TABLENAME = ?"
        " and (DESCRIPTION = 'Rows' or"
        " DESCRIPTION = 'Index pages' or"
        " DESCRIPTION = 'Leaf  pages' or"
        " DESCRIPTION = 'LOB column pages' or"
        " DESCRIPTION = 'LONG column pages')");
	CE("verify_fdir_table_info: prepare3", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

    returncode = pstmt->execute();
	CE("verify_fdir_table_info: SELECT TABLENAME ...", 1, conn->ERRORCODE, conn->ERRORTEXT);

    if ( pstmt->ERRORCODE == 0 )
    {
        rs = pstmt->getResultSet();
        while (rs->next() == SQLDBC_OK)
        {
			SQLDBC_Length l;

            returncode = rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, tablename, &l, sizeof(tablename), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_table_info (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);

            sprintf(stmtText, "LOCK TABLE \"%s\" IN SHARE MODE", tablename);
            returncode = stmt->execute(stmtText);
            CE("verify_fdir_table_info: LOCK TABLE", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

            if ( (stmt->ERRORCODE == 500) || (stmt->ERRORCODE == 400)) // Lock Request Timeout or Lock Collision
            {
                continue;
            }

            counter++;

            returncode = pstmt2->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, tablename, NULL, sizeof(tablename), true);
			if (returncode != SQLDBC_OK) CE("error with bindParameter in verify_fdir_table_info (col 1)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
            
			returncode = pstmt2->execute();

            if (returncode != SQLDBC_OK)
            {
                prot("error during select on FILES: %d = %s (rc = %d)\n", pstmt2->ERRORCODE, pstmt2->ERRORTEXT, returncode);
            }

            if (returncode == SQLDBC_OK)
            {
                rs2 = pstmt2->getResultSet();

                if (rs2->next() == SQLDBC_OK)
                {
                    returncode = rs2->getObject(1, SQLDBC_HOSTTYPE_INT4, &entryCount, &l, sizeof(entryCount), true);
					if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_table_info (col 2)", 1, rs2->ERRORCODE, rs2->ERRORTEXT);
                    returncode = rs2->getObject(2, SQLDBC_HOSTTYPE_INT4, &indexPageCount, &l, sizeof(indexPageCount), true);
					if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_table_info (col 3)", 1, rs2->ERRORCODE, rs2->ERRORTEXT);
                    returncode = rs2->getObject(3, SQLDBC_HOSTTYPE_INT4, &indexLeafCount, &l, sizeof(indexPageCount), true);
					if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_table_info (col 4)", 1, rs2->ERRORCODE, rs2->ERRORTEXT);
                    returncode = rs2->getObject(4, SQLDBC_HOSTTYPE_UINT4, &lobPageCount, &l, sizeof(lobPageCount), true);
					if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_table_info (col 5)", 1, rs2->ERRORCODE, rs2->ERRORTEXT);
                    
                    rs2->close();
                }
            }

            if (returncode != SQLDBC_OK)
            {
                prot("error during get info from fdir\n");
            }

            if (returncode == SQLDBC_OK)
            {
                returncode = pstmt3->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, tablename, NULL, sizeof (tablename), true);
				if (returncode != SQLDBC_OK) CE("error with bindParameter in verify_fdir_table_info (col 2)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);

                returncode = pstmt3->execute();

                if (returncode == SQLDBC_OK)
                {
                    rs2 = pstmt3->getResultSet();
                }
                else
                {
                    prot("error during execute tablestatistics, %d = %s (rc = %d)\n", pstmt3->ERRORCODE, pstmt3->ERRORTEXT, returncode);
                }
            }

            if (returncode == SQLDBC_OK)
            {
                while (rs2->next() == SQLDBC_OK)
                {
                    returncode = rs2->getObject(1, SQLDBC_HOSTTYPE_ASCII, description, &l, sizeof(description), true);
					if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_table_info (col 6)", 1, rs2->ERRORCODE, rs2->ERRORTEXT);
					returncode = rs2->getObject(2, SQLDBC_HOSTTYPE_INT4, &numeric_value, &l, sizeof(numeric_value), true);
                    if (returncode != SQLDBC_OK) CE("error with getObject in verify_fdir_table_info (col 7)", 1, rs2->ERRORCODE, rs2->ERRORTEXT);

                    if (returncode == SQLDBC_OK)
                    {
                        if (strcmp(description, "Rows" ) == 0)
                            refEntryCount = numeric_value;
                        else if (strcmp(description,"Index pages") == 0)
                            refIndexPageCount = numeric_value*8;
                        else if (strcmp(description,"Leaf  pages") == 0)
                            refIndexLeafCount = numeric_value*8;
                        else if (strcmp(description,"LOB column pages") == 0)
                            refLobPageCount = numeric_value * 8;
                        else if (strcmp(description,"LONG column pages") == 0)
                            refLobPageCount = numeric_value * 8;
                        else
                        {
                            prot("error with unknown description\n", rs2->ERRORCODE, description, returncode);
                        }
                    }
                }
            }

            if (returncode == SQLDBC_OK)
            {
                rs2->close();

                if ( (entryCount != refEntryCount)
                     || (indexPageCount != refIndexPageCount)
                     || (indexLeafCount != refIndexLeafCount)
                     || (lobPageCount != refLobPageCount) )
                {
                    prot("mismatching counters for table %s (FILES, TABLESTATISTICS): "
						"entryCount (%d, %d); "
						"indexPageCount (%d, %d); "
						"leafPageCount (%d, %d); "
                        "lobPageCount (%d, %d)\n",
                         tablename,
                         entryCount, refEntryCount,
                         indexPageCount, refIndexPageCount,
                         indexLeafCount, refIndexLeafCount,
                         lobPageCount, refLobPageCount);
                    stmt->execute("force savepoint");
                    print_f ("** calling sqldump **", 0, tablename) ;
                    Sqldump ("verify_fdir_table_info()", 0, "Error with force savepoint!", conn) ;
                }
            }

            if (returncode != SQLDBC_OK)
            {
                    prot("error comparing counters\n", returncode, NULL, 0);
                    print_f ("** calling sqldump **", 0, tablename) ;
                    Sqldump ("verify_fdir_table_info()", 0, "Error comparing counters in verify_fdir_table_info()!", conn) ;
            }

            commit () ;
        }
    }

	prot("verify_fdir_table_info checks %d tables\n", counter);

	conn->releaseStatement(pstmt3);
	pstmt3 = 0;
	conn->releaseStatement(pstmt2);
	pstmt2 = 0;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
