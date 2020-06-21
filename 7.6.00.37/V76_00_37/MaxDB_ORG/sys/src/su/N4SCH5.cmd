* NIST SQL Test Suite, V4.0, Schema Definition, schema5.std !
* 59-byte ID !
* TEd Version # !
* date_time print !
* ******************************************************************* !
* ****** THIS FILE SHOULD BE RUN UNDER AUTHORIZATION ID FLATER ****** !
* ******************************************************************* !
* This is a standard schema definition. !
CREATE SCHEMA AUTHORIZATION FLATER !
   CREATE TABLE USIG (C1 INT, C_1 INT) !
   CREATE TABLE U_SIG (C1 INT, C_1 INT) !
   CREATE VIEW DV1 AS 
     SELECT DISTINCT HOURS FROM HU.WORKS !
 * This small one-column table is used to generate an 
 * indicator overflow data exception for SQLSTATE testing. 
 * If the table cannot be created, the test is assumed passed. 
 * Save the error message and then use TEd to delete the next line. 
 * Test number 0491 in programs DML082 may also need to be deleted. !
   CREATE TABLE TINY (C1 CHAR(33000)) !
 * For generation of "with check option violation" SQLSTATE. !
   CREATE TABLE BASE_WCOV (C1 INT) !
   CREATE VIEW WCOV AS SELECT * FROM BASE_WCOV WHERE 
     C1 > 0 WITH CHECK OPTION !
   CREATE TABLE BASE_VS1 (C1 INT, C2 INT) !
   CREATE VIEW VS1 AS SELECT * FROM BASE_VS1 WHERE C1 = 0 !
   CREATE VIEW VS2 AS 
     SELECT A.C1 FROM BASE_VS1 A WHERE EXISTS 
       (SELECT B.* FROM BASE_VS1 B WHERE B.C2 = A.C1) !
   CREATE VIEW VS2 AS 
     SELECT A.C1 FROM BASE_VS1 A WHERE EXISTS 
       (SELECT * FROM BASE_VS1 B WHERE B.C2 = A.C1) !
   CREATE VIEW VS3 AS 
     SELECT A.C2 FROM BASE_VS1 A WHERE A.C2 IN 
       (SELECT B.C1 FROM BASE_VS1 B WHERE B.C1 < A.C2) !
   CREATE VIEW VS4 AS 
     SELECT A.C1 FROM BASE_VS1 A WHERE A.C1 < ALL 
       (SELECT B.C2 FROM BASE_VS1 B) !
   CREATE VIEW VS5 AS 
     SELECT A.C1 FROM BASE_VS1 A WHERE A.C1 < SOME 
       (SELECT B.C2 FROM BASE_VS1 B) !
   CREATE VIEW VS6 AS 
     SELECT A.C1 FROM BASE_VS1 A WHERE A.C1 < ANY 
       (SELECT B.C2 FROM BASE_VS1 B) !
   GRANT ALL PRIVILEGES ON TINY TO SCHANZLE !
   GRANT ALL ON BASE_WCOV TO SCHANZLE !
   GRANT ALL ON WCOV TO SCHANZLE !
   GRANT ALL ON VS1 TO SCHANZLE !
 * Test granting of privileges that we do not have to start with. 
 * We have GRANT OPTION, but we should not be able to grant unrestricted
 * update on STAFF3 since our own update is restricted to two columns. 
 * Do not change SCHEMA1 to grant unrestricted update. 
 * * expect error message * !
   GRANT SELECT, UPDATE ON HU.STAFF3 TO SCHANZLE !
 * Same thing for views. 
 * * expect error message * !
   GRANT SELECT, UPDATE ON HU.VSTAFF3 TO SCHANZLE !
 * See whether GRANT ALL PRIVILEGES gives you GRANT OPTION. 
 * It should not.  GRANT OPTION is not technically a privilege.
 * * expect error message * !
   GRANT SELECT ON CUGINI.BADG1 TO SCHANZLE !
 * See whether GRANT OPTION on a view gives you GRANT OPTION 
 * on the base table. 
 * * expect error message * !
   GRANT SELECT ON CUGINI.BADG2 TO SCHANZLE !
 * Delimited identifiers. !
   CREATE VIEW "SULLI.SEL" ("sulli.sel") AS 
     SELECT C1 FROM BASE_VS1 !
   GRANT ALL PRIVILEGES ON "SULLI.SEL" TO SCHANZLE !
 * ************* End of Schema ************* !
