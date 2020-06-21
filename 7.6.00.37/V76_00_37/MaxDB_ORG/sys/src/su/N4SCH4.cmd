* NIST SQL Test Suite, V4.0, Schema Definition, schema4.std !
* 59-byte ID !
* TEd Version # !
* date_time print !
* ****************************************************************** !
* ****** THIS FILE SHOULD BE RUN UNDER AUTHORIZATION ID SULLIVAN1 !
* ****************************************************************** !
* This is a standard schema definition. !
   CREATE SCHEMA AUTHORIZATION SULLIVAN1 !
   CREATE TABLE auth_tab (FIRST1 INTEGER, SECOND2 CHAR) !
   CREATE VIEW MUL_SCH  
       AS SELECT EMPNUM, SECOND2 
          FROM HU.STAFF, auth_tab 
          WHERE GRADE = FIRST1 !
    GRANT ALL PRIVILEGES ON auth_tab TO HU !
    GRANT SELECT ON MUL_SCH TO HU !
 * The following tables are used to run concurrency program pairs
 *    e.g MPA001 and MPB001 use the tables with prefix MP1_  !
CREATE TABLE MP1_MM2 (NUMTEST NUMERIC(10)) !
CREATE TABLE MP1_NN (NUMTEST NUMERIC(9)) !
CREATE TABLE MP1nextk (KEYNUM INTEGER, AUTHOR CHAR(1), 
                           DOLLARS INTEGER) !
CREATE TABLE MP2_MM2 (NUMTEST NUMERIC(10)) !
CREATE TABLE MP2_NN (NUMTEST NUMERIC(9)) !
CREATE TABLE MP2nextk (KEYNUM INTEGER, AUTHOR CHAR(1), 
                           DOLLARS INTEGER) !
CREATE TABLE MP3_MM2 (NUMTEST NUMERIC(10)) !
CREATE TABLE MP3_NN (NUMTEST NUMERIC(9)) !
CREATE TABLE MP3nextk (KEYNUM INTEGER, AUTHOR CHAR(1), 
                           DOLLARS INTEGER) !
CREATE TABLE MP4_MM2 (NUMTEST NUMERIC(10)) !
CREATE TABLE MP4_NN (NUMTEST NUMERIC(9)) !
CREATE TABLE MP4nextk (KEYNUM INTEGER, AUTHOR CHAR(1), 
                           DOLLARS INTEGER) !
CREATE TABLE MP5_AA (ANUM NUMERIC(4)) !
CREATE TABLE mp5_a_ind (ANUM NUMERIC(4) NOT NULL,
    constraint c1_mp5_a_ind UNIQUE(anum)) !
CREATE TABLE MP5_TT (TESTTYPE CHAR(3), KOUNT DECIMAL(4)) !
 * The following tables are used to run interactive concurrency program pairs. !
CREATE TABLE TTT (ANUM NUMERIC(4) NOT NULL, AUTHOR CHAR(1),
    constraint c1_ttt unique(anum)) !
CREATE TABLE TT (DOLLARS NUMERIC(4), ANUM NUMERIC(4)) !
CREATE TABLE AA (ANUM NUMERIC(4)) !
CREATE TABLE BB (BNUM NUMERIC(4)) !
 *  Test GRANT UPDATE for additional columns beyond those WITH GRANT OPTION. 
 *  expect error message 
 *  In SCHEMA1 for USER HU is the following grant: 
 *         GRANT SELECT,UPDATE(EMPNUM,EMPNAME) ON STAFF3  
 *          TO ..., SULLIVAN1 WITH GRANT OPTION 
 * If the following self-grant by SULLIVAN1 will not compile/execute, 
 *                 delete it. !
   GRANT SELECT,UPDATE ON HU.STAFF3 
           TO SULLIVAN1 
           WITH GRANT OPTION !
CREATE TABLE MP6_MM2 (NUMTEST NUMERIC(10)) !
CREATE TABLE MP6_NN (NUMTEST NUMERIC(9)) !
CREATE TABLE MP6nextk (KEYNUM INTEGER, AUTHOR CHAR(1), 
                           DOLLARS INTEGER) !
CREATE TABLE MP6_AA (ANUM NUMERIC(4)) !
CREATE TABLE MP6_BB (BNUM NUMERIC(4)) !
CREATE TABLE MP7_NN (NUMTEST NUMERIC(9)) !
CREATE TABLE MP7nextk (KEYNUM INTEGER, AUTHOR CHAR(1), 
                           DOLLARS INTEGER) !
CREATE TABLE MP7_AA (ANUM NUMERIC(4)) !
CREATE TABLE MP7_BB (BNUM NUMERIC(4)) !
CREATE TABLE MP8_NN (NUMTEST NUMERIC(9)) !
CREATE TABLE MP8nextk (KEYNUM INTEGER, AUTHOR CHAR(1), 
                           DOLLARS INTEGER) !
CREATE TABLE MP8_AA (ANUM NUMERIC(4) NOT NULL, 
    AUTHOR CHAR(1), constraint c1_mp8_aa UNIQUE (ANUM)) !
CREATE TABLE MP8_BB (NUMTEST NUMERIC(9)) !
 * ************* End of Schema ************* !
