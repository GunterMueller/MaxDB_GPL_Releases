/***************************************************************************

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



*/
/* sphsapdbObj.cpp : Implementation of Csphsapdb                            */
/****************************************************************************/

#ifdef HPUX
#define NEED_TYPEINFO NEED_TYPEINFO
#endif

#if defined(WIN32)
#include <string>
#endif
#include "Oms/OMS_Defines.h" // base types
#include "livecachetypes.h" //nocheck, generated from vco03.idl
#include "bw/sphsapdb.h"
#include "bw/sphsapdbObj.h"

#ifdef NEED_TYPEINFO
#include <typeinfo>
#endif

class cChar2
{
public:
	char buf[2];
	cChar2 (){};
	cChar2 (char c[2]){ memcpy(buf,&c[0],sizeof(buf));};
	operator SqlCol () { return SqlCol (buf, SqlCol::VCHAR, sizeof(buf));};
};
/****************************************************************************/
/* Method Register is the very first Method called after the COM-Objekt     */
/* has been created.                                                        */
/* The CALLBACK Interface (IKernelSink) has been activated and may be used  */
/* (within the "constructor" of the COM-object IKernelSink hasn't been      */
/*  activated yet !)                                                        */

STDMETHODIMP Csphsapdb::Register()
{
  HRESULT hr	= S_OK;
  try {

    co90CheckVersion ((IUnknown**)&this->m_pIUnknown,
                      ILIVECACHESINK_VERSION,
                      LIVECACHETYPES_VERSION, LIVECACHEOMS_VERSION,
                      LIVECACHESQL_VERSION );

/* *** TODO ***                                                            */
/* Register all persistent objects used by DB-Procedures here              */
/* This Routine is called, before the first DB-Procedure gets control.     */
  }
  catch (DbpError e) {
    omsExceptionHandler (e);
  }
	return hr;
}

/****************************************************************************/
/* Isphsapdb method implementation  (to be inserted here)                   */

STDMETHODIMP Csphsapdb::test(int a)
{

	try
	{
		sql(" create table t_string2 ( str1 fixed, str2 fixed)")();
	}
	catch (DbpError e)
    {
       sqlExceptionHandler( e );
    }
	return S_OK;
}


STDMETHODIMP Csphsapdb::sphn(TABLENAME* hier_tab,
							 TABLENAME* tmp_tab,
							 OmsTypeInt4 act_level,
							 OmsTypeInt4 end_level,
							 SVER* sver_in,
							 OmsTypeInt4 hiesid_in,
							 OmsTypeInt4 start_succ,
							 OmsTypeInt4 start_is_pred)
{
	OmsTypeInt4	succ_out;
	OmsTypeInt4	factor_out;
	OmsTypeInt4	seq_nr;
	short		i;
	short		count;
	char		sql_stmt[1024];
	char		sver[2];
	char		hiesid[20];
	char*		tmptab;
	char*		hiertab;
	SQL			ins_tmp1, ins_tmp2, ins_result, ins_resconst,
				fetch_stmt, fetch_C_1, fetch_C_2,
				sel_subtmp1, sel_subtmp2,
				upd_result;
	SP			sp;

//	OmsHandle::dbpDebugBreak();
	tmptab = (char *) tmp_tab;
	hiertab = (char *) hier_tab;
	seq_nr = 0;
	i = 30;
	while ( (tmptab[i] == ' ' || tmptab[i] == '\0') && i > 0)
	{
		i--;
	}
	tmptab[i+1] = '\0';
	i = 30;
	while ( (hiertab[i] == ' ' || hiertab[i] == '\0') && i > 0)
	{
		i--;
	}
	hiertab[i+1] = '\0';

	sp = SPHN;
	strcpy(sver, (const char*) sver_in);
	sprintf(hiesid, "%d", hiesid_in);
	initSqlStmt(sp,
				hiertab,
				tmptab,
				hiesid,
				sver,
				&ins_tmp1,
				&ins_tmp2,
				&ins_result,
				&ins_resconst,
				&sel_subtmp1,
				&sel_subtmp2,
				&upd_result);


	try
	{
		strcpy(sql_stmt, "DROP TABLE TEMP.\"");
		strcat(sql_stmt, tmptab);
		strcat(sql_stmt, "_1\"");
		sql(sql_stmt)();

	}
	catch (DbpError e)
	{
		switch(e.dbpError())
		{
			case -4004:
				break;
			default:
				sqlExceptionHandler( e );
		}

	}
	try
	{
		strcpy(sql_stmt, "CREATE TABLE TEMP.\"");
		strcat(sql_stmt, tmptab);
		strcat(sql_stmt, "_1\" ( SUCC FIXED(10) )");
		sql(sql_stmt)();

		ins_tmp1 << start_succ;
		ins_tmp1();

		for (i = act_level; i <= end_level; i = i + 2)
		{
			act_level++;
			if (act_level > end_level)
			{
				break;
			}
			strcpy(sql_stmt, "SELECT COUNT(*) FROM TEMP.\"");
			strcat(sql_stmt,  tmptab);
			strcat(sql_stmt, "_1\"");
			sql(sql_stmt)();
			fetch_stmt = sql("FETCH");
			fetch_stmt << count;
			fetch_stmt();
			if (count == 0)
			{
				break;
			}
			try
			{
				strcpy(sql_stmt, "DROP TABLE TEMP.\"");
				strcat(sql_stmt, tmptab);
				strcat(sql_stmt, "_2\"");
				sql(sql_stmt)();

			}
			catch (DbpError e)
			{
				switch(e.dbpError())
				{
					case -4004:
						break;
					default:
						sqlExceptionHandler( e );
				}
			}
			strcpy(sql_stmt, "CREATE TABLE TEMP.\"");
			strcat(sql_stmt, tmptab);
			strcat(sql_stmt, "_2\" ( SUCC FIXED(10) )");
			sql(sql_stmt)();

			sel_subtmp1();
			fetch_C_1 = sql("FETCH C_1");
			fetch_C_1 << succ_out << factor_out;
			while (fetch_C_1())
			{

				if (((succ_out >= 0) && (act_level < end_level)) ||
					(act_level == end_level))
				{
					try
					{

						if ((start_is_pred == 1)  && (start_succ != succ_out))
						{
							ins_result << succ_out << start_succ << seq_nr << factor_out;
							ins_result();
						}
						else
						{
//							succ_out >= 0
							ins_resconst << succ_out << succ_out << seq_nr;
							ins_resconst();
						}
					}
					catch (DbpError e)
					{
						switch(e.dbpError())
						{
							case 200:
								if ((start_is_pred == 1)  && (start_succ != succ_out))
								{
									upd_result << factor_out << succ_out << start_succ << seq_nr;
							        upd_result();
								}

								break;
							default:
								sqlExceptionHandler( e );
						}
					}

				}
				else
				{
					ins_tmp2 << succ_out;
					ins_tmp2();
				}

			} /* while (fetch_stmt()) */
			act_level++;
			if (act_level > end_level)
			{
				break;
			}
			try
			{
				strcpy(sql_stmt, "DROP TABLE TEMP.\"");
				strcat(sql_stmt, tmptab);
				strcat(sql_stmt, "_1\"");
				sql(sql_stmt)();

			}
			catch (DbpError e)
			{
				switch(e.dbpError())
				{
					case -4004:
						break;
					default:
						sqlExceptionHandler( e );
				}
			}
			strcpy(sql_stmt, "CREATE TABLE TEMP.\"");
			strcat(sql_stmt, tmptab);
			strcat(sql_stmt, "_1\" ( SUCC FIXED(10) )");
			sql(sql_stmt)();

			sel_subtmp2();
			fetch_C_2 = sql("FETCH C_2");
			fetch_C_2 << succ_out << factor_out;
			while (fetch_C_2())
			{
				if (((succ_out >= 0) && (act_level < end_level)) ||
					(act_level == end_level))
				{
					try
					{
						if ((start_is_pred == 1)  && (start_succ != succ_out))
						{
							ins_result << succ_out << start_succ << seq_nr << factor_out;
							ins_result();
						}
						else
						{
							ins_resconst << succ_out << succ_out << seq_nr;
							ins_resconst();
						}

					}
					catch (DbpError e)
					{
						switch(e.dbpError())
						{
							case 200:
								if ((start_is_pred == 1)  && (start_succ != succ_out))
								{
									upd_result << factor_out << succ_out << start_succ << seq_nr;
							        upd_result();
								}

								break;
							default:
								sqlExceptionHandler( e );
						}
					}

				}
				else
				{
					ins_tmp1 << succ_out;
					ins_tmp1();
				}

			} /* while (fetch_stmt()) */


		} /* for (i = act_level; i <= end_level; i = i + 2) */
	}
	catch (DbpError e)
	{
		sqlExceptionHandler( e );
	}


	return S_OK;
}


STDMETHODIMP Csphsapdb::sphl(TABLENAME* hier_tab,
							 TABLENAME* tmp_tab,
							 OmsTypeInt4 act_level,
							 OmsTypeInt4 end_level1,
							 OmsTypeInt4 end_level2,
							 SVER* sver_in,
							 OmsTypeInt4 hiesid_in,
							 OmsTypeInt4 start_succ)
{
	OmsTypeInt4	succ_out;
	OmsTypeInt4	factor_out;
	OmsTypeInt4	seq_nr;
	short		i;
	short		count;
	char		sql_stmt[1024];
	char		sver[2];
	char		hiesid[20];
	char*		tmptab;
	char*		hiertab;
	SQL			ins_tmp3, ins_tmp4, ins_result, ins_resconst,
				fetch_stmt, fetch_C_3, fetch_C_4,
				sel_subtmp3, sel_subtmp4,
				upd_result;
	SP			sp;


//	OmsHandle::dbpDebugBreak();
	tmptab = (char *) tmp_tab;
	hiertab = (char *) hier_tab;
	seq_nr = 0;
	i = 30;
	while ( (tmptab[i] == ' ' || tmptab[i] == '\0') && i > 0)
	{
		i--;
	}
	tmptab[i+1] = '\0';
	i = 30;
	while ( (hiertab[i] == ' ' || hiertab[i] == '\0') && i > 0)
	{
		i--;
	}
	hiertab[i+1] = '\0';

	sp = SPHL;
	strcpy(sver, (const char*) sver_in);
	sprintf(hiesid, "%d", hiesid_in);
	initSqlStmt(sp,
				hiertab,
				tmptab,
				hiesid,
				sver,
				&ins_tmp3,
				&ins_tmp4,
				&ins_result,
				&ins_resconst,
				&sel_subtmp3,
				&sel_subtmp4,
				&upd_result);


	try
	{

		strcpy(sql_stmt, "DROP TABLE TEMP.\"");
		strcat(sql_stmt, tmptab);
		strcat(sql_stmt, "_3\"");
		sql(sql_stmt)();

	}
	catch (DbpError e)
	{
		switch(e.dbpError())
		{
			case -4004:
				break;
			default:
				sqlExceptionHandler( e );
		}

	}
	try
	{
		strcpy(sql_stmt, "CREATE TABLE TEMP.\"");
		strcat(sql_stmt, tmptab);
		strcat(sql_stmt, "_3\" ( SUCC FIXED(10) )");
		sql(sql_stmt)();

		ins_tmp3 << start_succ;
		ins_tmp3();

		for (i = act_level; i <= end_level1; i = i + 2)
		{
			act_level++;
			if (act_level > end_level1)
			{
				break;
			}
			strcpy(sql_stmt, "SELECT COUNT(*) FROM TEMP.\"");
			strcat(sql_stmt,  tmptab);
			strcat(sql_stmt, "_3\"");
			sql(sql_stmt)();
			fetch_stmt = sql("FETCH");
			fetch_stmt << count;
			fetch_stmt();
			if (count == 0)
			{
				break;
			}
			try
			{
				strcpy(sql_stmt, "DROP TABLE TEMP.\"");
				strcat(sql_stmt, tmptab);
				strcat(sql_stmt, "_4\"");
				sql(sql_stmt)();

			}
			catch (DbpError e)
			{
				switch(e.dbpError())
				{
					case -4004:
						break;
					default:
						sqlExceptionHandler( e );
				}
			}
			strcpy(sql_stmt, "CREATE TABLE TEMP.\"");
			strcat(sql_stmt, tmptab);
			strcat(sql_stmt, "_4\" ( SUCC FIXED(10) )");
			sql(sql_stmt)();
			sel_subtmp3();
			fetch_C_3 = sql("FETCH C_3");
			fetch_C_3 << succ_out << factor_out;
			while (fetch_C_3())
			{
				if (((succ_out >= 0) && (act_level < end_level1)) ||
					((succ_out >= 0) && (act_level == end_level1)))
				{
					ins_resconst << succ_out << succ_out << seq_nr;
					try
					{
						ins_resconst();
					}
					catch (DbpError e)
					{
						switch(e.dbpError())
						{
							case 200:
								break;

							default:
								sqlExceptionHandler( e );
						}
					}
				}
				else
				{
					if  (( succ_out < 0 ) && ( act_level == end_level1 ))
					{
						sphn(hier_tab,
						     tmp_tab,
						     0,
						     end_level2,
						     sver_in,
						     hiesid_in,
						     succ_out,
							 1);
					}
					else
					{
						ins_tmp4 << succ_out;
						ins_tmp4();
					}
				}
			} /* while (fetch_stmt()) */
			act_level++;
			if (act_level > end_level1)
			{
				break;
			}
			try
			{
				strcpy(sql_stmt, "DROP TABLE TEMP.\"");
				strcat(sql_stmt, tmptab);
				strcat(sql_stmt, "_3\"");
				sql(sql_stmt)();

			}
			catch (DbpError e)
			{
				switch(e.dbpError())
				{
					case -4004:
						break;
					default:
						sqlExceptionHandler( e );
				}
			}
			strcpy(sql_stmt, "CREATE TABLE TEMP.\"");
			strcat(sql_stmt, tmptab);
			strcat(sql_stmt, "_3\" ( SUCC FIXED(10) )");
			sql(sql_stmt)();
			sel_subtmp4();
			fetch_C_4 = sql("FETCH C_4");
			fetch_C_4 << succ_out << factor_out;
			while (fetch_C_4())
			{
				if (((succ_out >= 0) && (act_level < end_level1)) ||
					((succ_out >= 0) && (act_level == end_level1)))
				{
					ins_resconst << succ_out << succ_out << seq_nr;
					try
					{
						ins_resconst();
					}
					catch (DbpError e)
					{
						switch(e.dbpError())
						{
							case 200:
								break;

							default:
								sqlExceptionHandler( e );
						}
					}
				}
				else
				{
					if (( succ_out < 0 ) && ( act_level == end_level1 ))
					{
						sphn(hier_tab,
						     tmp_tab,
						     0,
						     end_level2,
						     sver_in,
						     hiesid_in,
						     succ_out,
							 1);
					}
					else
					{
						ins_tmp3 << succ_out;
						ins_tmp3();
					}
				}

			} /* while (fetch_stmt()) */


		} /* for (i = act_level; i <= end_level; i = i + 2) */
	}
	catch (DbpError e)
	{
		sqlExceptionHandler( e );
	}


	return S_OK;
}


void Csphsapdb::initSqlStmt(SP		sp,
							char*	hiertab,
					    	char*	tmptab,
							char*	hiesid,
							char*	sver,
							SQL*	ins_tmp1,
							SQL*	ins_tmp2,
							SQL*	ins_result,
							SQL*    ins_resconst,
							SQL*	sel_subtmp1,
							SQL*	sel_subtmp2,
							SQL*	upd_result)
{
	char*		sql_stmt;
	char		tab1_post[3];
	char		tab2_post[3];


	if (sp == SPHN)
	{
		strcpy(tab1_post, "_1");
		strcpy(tab2_post, "_2");
	}
	else
	{
		strcpy(tab1_post, "_3");
		strcpy(tab2_post, "_4");
	}

	sql_stmt = new char[1024];

	strcpy(sql_stmt, "INSERT INTO TEMP.\"");
	strcat(sql_stmt, tmptab);
	strcat(sql_stmt, tab1_post);
	strcat(sql_stmt, "\" VALUES ( ? ) ");
	*ins_tmp1 = sql(sql_stmt);

	strcpy(sql_stmt, "INSERT INTO TEMP.\"");
	strcat(sql_stmt, tmptab);
	strcat(sql_stmt, tab2_post);
	strcat(sql_stmt, "\" VALUES ( ? ) ");
	*ins_tmp2 = sql(sql_stmt);

	strcpy(sql_stmt, "INSERT INTO \"");
	strcat(sql_stmt, tmptab);
	strcat(sql_stmt,
		"\" VALUES (?, ?, ?, ?)");
	*ins_result = sql(sql_stmt);

	strcpy(sql_stmt, "INSERT INTO \"");
	strcat(sql_stmt, tmptab);
	strcat(sql_stmt,
		"\" VALUES (?, ?, ?, 1)");
	*ins_resconst = sql(sql_stmt);

	strcpy(sql_stmt, "SELECT C");
	strcat(sql_stmt, tab1_post);
	strcat(sql_stmt, "( SUCC, FACTOR ) FROM \"");
	strcat(sql_stmt, hiertab);
	strcat(sql_stmt, "\" WHERE HIESID = ");
	strcat(sql_stmt, hiesid);
	strcat(sql_stmt, " AND SVER = '");
	strcat(sql_stmt, sver);
	strcat(sql_stmt, "' AND PRED IN ");
	strcat(sql_stmt, "( SELECT SUCC FROM TEMP.\"");
	strcat(sql_stmt, tmptab);
	strcat(sql_stmt, tab1_post);
	strcat(sql_stmt, "\")");
	*sel_subtmp1 = sql(sql_stmt);

	strcpy(sql_stmt, "SELECT C");
	strcat(sql_stmt, tab2_post);
	strcat(sql_stmt, "( SUCC, FACTOR ) FROM \"");
	strcat(sql_stmt, hiertab);
	strcat(sql_stmt, "\" WHERE HIESID = ");
	strcat(sql_stmt, hiesid);
	strcat(sql_stmt, " AND SVER = '");
	strcat(sql_stmt, sver);
	strcat(sql_stmt, "' AND PRED IN ");
	strcat(sql_stmt, "( SELECT SUCC FROM TEMP.\"");
	strcat(sql_stmt, tmptab);
	strcat(sql_stmt, tab2_post);
	strcat(sql_stmt, "\")");
	*sel_subtmp2 = sql(sql_stmt);

	strcpy(sql_stmt, "UPDATE \"");
	strcat(sql_stmt, tmptab);
	strcat(sql_stmt,
		"\" SET FACTOR = FACTOR + ? KEY SUCC = ?, PRED = ?, SEQ_NR = ?");
	*upd_result = sql(sql_stmt);

	delete sql_stmt;
}
