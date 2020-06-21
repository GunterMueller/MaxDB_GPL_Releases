*  NIST SQL Test Suite, V4.0, Schema Definition, schema7.std !
*  59-byte ID !
*  date_time print !
*  ************************************************************** !
*  ***** THIS FILE SHOULD BE RUN UNDER AUTHORIZATION ID ********* !
*  ***** CANWEPARSELENGTH18                             ********* !
*  ************************************************************** !
 *  used in program SDL026 !
CREATE SCHEMA AUTHORIZATION CANWEPARSELENGTH18!
CREATE TABLE CHARACTER18TABLE18 (CHARS18NAME18CHARS CHAR(4))!
CREATE VIEW  CHARACTERS18VIEW18 (LONGNAME18LONGNAME)
             AS SELECT CHARS18NAME18CHARS
                  FROM CHARACTER18TABLE18
                 WHERE CHARS18NAME18CHARS <> 'long'!
 *  ************* End of Schema ************* !
