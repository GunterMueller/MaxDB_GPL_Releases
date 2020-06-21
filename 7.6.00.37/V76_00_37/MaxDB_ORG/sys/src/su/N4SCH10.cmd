* NIST SQL Test Suite, V4.0, Schema Definition, schem10.std !
* 59-byte ID !
* TEd Version # !
* date_time print !
* *************************************************************** !
* ****** THIS FILE SHOULD BE RUN UNDER AUTHORIZATION ID SCHANZLE  !
* *************************************************************** !
* This file defines base tables used in the CDR tests. !
* This is a standard schema definition. !
  CREATE SCHEMA  
  AUTHORIZATION SCHANZLE !
  CREATE TABLE ACR_SCH_F(F1 DECIMAL(4),
         F2 CHAR(4),
         FOREIGN KEY (F1) 
         REFERENCES SUN.ACR_SCH_P(P1)) !
 * Test GRANT REFERENCES without grant permission below -- expect error message
 * "WITH GRANT OPTION" purposefully omitted from SUN''s GRANT on STAFF_P 
 * Do not change file SCHEMA8 to allow "WITH GRANT OPTION" !
   CREATE TABLE TAB5(F15 CHAR(3),
          F5 CHAR(4),
          FOREIGN KEY (F15) 
          REFERENCES SUN.STAFF_P(EMPNUM)) !
 * Test REFERENCES without reference permission below -- expect error message
 * GRANT SELECT by SULLIVAN does not imply GRANT REFERENCES. 
 * Do not change file SCHEMA9 to allow "GRANT REFERENCES" on TTT. !
   CREATE TABLE TAB6(F16 DECIMAL(4),
          F6 CHAR(4),
          FOREIGN KEY (F16) 
          REFERENCES SULLIVAN.TTT(P1)) !
 * Constraints:  column vs. column !
   CREATE TABLE ret_cat ( 
     VENDOR_ID INT,
     prod_id INT,
     WHOLESALE NUMERIC (10,2),
     RETAIL NUMERIC (10,2),
     MARKUP NUMERIC (10,2),
     export_c CHAR(2),
     exp_l_dat CHAR(20),
     CHECK (exp_l_dat IS NULL OR ( 
       export_c = 'F1' OR 
       export_c = 'F2' OR 
       export_c = 'F3'                  )),
     CHECK (export_c <> 'F2' OR WHOLESALE > 10000.00),
     CHECK (RETAIL >= WHOLESALE),
     CHECK (RETAIL = WHOLESALE + MARKUP)) !
   CREATE VIEW sale_item AS 
     SELECT * FROM ret_cat 
     WHERE MARKUP < WHOLESALE / 10.0 
     WITH CHECK OPTION !
   CREATE TABLE RASTER 
      (C1 INT NOT NULL,
       FOREIGN KEY (C1) 
       REFERENCES SUN.COMMODITY (C_NUM)) !
 * Test REFERENCES without reference permission below -- expect error message
 * GRANT REFERENCES (C_NUM) on COMMODITY by SUN does not imply 
 *      GRANT REFERENCES (C_NAME). 
 * Do not change file SCHEMA8 to allow "GRANT REFERENCES" on column C_NAME. !
   CREATE TABLE REFRESH 
      (C1 CHAR (7),
       FOREIGN KEY (C1) 
       REFERENCES SUN.COMMODITY (C_NAME)) !
   CREATE TABLE CPBASE 
      (KC INT,
       JUNK1 CHAR (10),
       PRIMARY KEY (KC)) !
   CREATE TABLE CPREF 
      (KCR INT,
       JUNK2 CHAR (10),
       FOREIGN KEY (KCR) REFERENCES CPBASE) !
   CREATE TABLE four_typs 
      (T_INT     INTEGER,
       T_CHAR    CHAR(10),
       T_DECIMAL DECIMAL(10,2),
       T_REAL    REAL) !
 * ************* End of Schema ************* !
