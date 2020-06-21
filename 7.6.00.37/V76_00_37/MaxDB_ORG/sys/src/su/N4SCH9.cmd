*  NIST SQL Test Suite, V4.0, Schema Definition, schema9.std !
*  59-byte ID !
*  date_time print !
*  *************************************************************** !
*  ****** THIS FILE SHOULD BE RUN UNDER AUTHORIZATION ID SULLIVAN !
*  *************************************************************** !
*  This file defines base tables used in the CDR tests. !
  CREATE SCHEMA
    AUTHORIZATION SULLIVAN!
  CREATE TABLE WORKS_P
   (EMPNUM   CHAR(3) REFERENCES SUN.STAFF_P(EMPNUM),
    PNUM     CHAR(3),
    HOURS    DECIMAL(5),
    FOREIGN KEY (PNUM)
    REFERENCES SUN.PROJ_P(PNUM))!
  CREATE TABLE TTT(P1 DECIMAL(4) NOT NULL,
                 constraint c1_ttt UNIQUE(p1),
                 P2 CHAR(4))!
 *  ************* grant statements follow ************* !
   GRANT SELECT ON TTT TO SCHANZLE!
   GRANT REFERENCES ON SUN.ACR_SCH_P TO SCHANZLE!
 *   Test GRANT without grant permission below *  expect error message
 *   "WITH GRANT OPTION" purposefully omitted from SUN''s GRANT.
 *   Do not change file SCHEMA8 to allow "WITH GRANT OPTION" on STAFF_P. !
   GRANT REFERENCES ON SUN.STAFF_P TO SCHANZLE!
 *  ************* End of Schema ************* !
