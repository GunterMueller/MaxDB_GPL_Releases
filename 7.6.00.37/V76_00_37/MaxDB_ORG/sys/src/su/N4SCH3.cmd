*   *ID* SCHEMA3  DBCMD    changed on 1992-09-28-13.28.01 by ADMIN     *
*  NIST SQL Test Suite, V4.0, Schema Definition, schema3.smi
*  59-byte ID
*  date_time print
*  ***************************************************************
*  ****** THIS FILE SHOULD BE RUN UNDER AUTHORIZATION ID MCGINN **
*  ***************************************************************!
     CREATE SCHEMA AUTHORIZATION MCGINN !
 *  This file contains extensions to the SQL standard.
 *  Failure to create this schema is not a deficiency.
 *  This file tests extensions to SQL to allow names longer than 18.
 *  If a create table or create view statement causes a syntax error,
 *       delete that create table or create view statement.!
CREATE TABLE TABLEFGHIJKLMNOPQ19
  (COL2  INTEGER)!
CREATE TABLE SHORTTAB
  (COLUMN123456789IS19  INTEGER)!
CREATE TABLE BASETAB1 (COL1  INTEGER)!
CREATE VIEW VIEWABCDEFGHIKLMN19 (COL3)
  AS SELECT COL1 FROM BASETAB1!
 *  ************* End of Schema ************* !
