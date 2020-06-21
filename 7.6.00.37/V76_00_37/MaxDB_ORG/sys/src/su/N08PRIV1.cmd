*   *ID* N08PRIV1 DBCMD    changed on 1992-09-28-13.25.14 by ADMIN     *
 *** GRANT und REVOKE wird getestet.
 *** Getestet wird die Privilegienvergabe von Benutzern
 *** mit Status DBA und Status RESOURCE
 *** mit Ausnahme des OWNER-Privilegs.
 *** Die zugrundeliegenden Tabellen sind Basis-Tabellen.!
 ****************************************************
 ******************* CREATE USER ********************
 ****************************************************!
file connect (sut sut !
CREATE USER d1 PASSWORD d1_w1 DBA !
CREATE USER d2 PASSWORD d2_w1 DBA !
CREATE USER d3 PASSWORD d3_w1 DBA !
CREATE USER r1 PASSWORD r1_w1 RESOURCE !
CREATE USER r2 PASSWORD r2_w1 RESOURCE !
CREATE USER r3 PASSWORD r3_w1 RESOURCE !
CREATE USER p1 PASSWORD p1_w1 !
CREATE USER p2 PASSWORD p2_w1 !
COMMIT WORK RELEASE !
 ****************************************************
 ******************* CREATE TABLE *******************
 ****************************************************!
file connect (sut sut !
CREATE TABLE sut.tab ( tab_c1 CHAR(3)) !
COMMIT WORK RELEASE !
file connect (d1 d1_w1 !
CREATE TABLE        d1_pt1 ( d1_pt1_c1 CHAR(3) ) !
CREATE TABLE        d1_pt2 ( d1_pt2_c1 CHAR(3) ) !
CREATE TABLE        d1_t1  ( d1_t1_c1  CHAR(3) ) !
CREATE TABLE        d1_t2  ( d1_t2_c1  CHAR(3) ) !
COMMIT WORK RELEASE !
file connect (d2 d2_w1 !
CREATE TABLE        d2_t1  ( d2_t1_c1  CHAR(3) ) !
COMMIT WORK RELEASE !
file connect (r1 r1_w1 !
CREATE TABLE        r1_t1 ( r1_t1_c1 CHAR(3) ) !
CREATE TABLE        r1_t2 ( r1_t2_c1 CHAR(3) ) !
COMMIT WORK RELEASE !
file connect (r2 r2_w1 !
CREATE TABLE        r2_t1  ( r2_t1_c1  CHAR(3) ) !
COMMIT WORK RELEASE !
 ****************************************************
 ************* fehlerhaftes GRANT *******************
 ****************************************************!
file connect (sut sut !
GRANT INSERT ON     d1.d1_pt1 TO d1 !
GRANT INSERT ON     d1.d1_t1  TO d1 !
GRANT INSERT ON     d1.d1_t1  TO d2 !
GRANT INSERT ON     d1.d1_t1  TO r1 !
GRANT INSERT ON     d1.d1_t1  TO p1 !
GRANT DELETE ON     r1.r1_t1  TO d1 !
GRANT DELETE ON     r1.r1_t1  TO r1 !
GRANT DELETE ON     r1.r1_t1  TO r2 !
GRANT DELETE ON     r1.r1_t1  TO p1 !
ROLLBACK WORK RELEASE !
file connect (d2 d2_w1 !
GRANT INSERT       ON sut.tab TO sut !
GRANT INSERT       ON sut.tab TO d3    !
GRANT INSERT       ON        tab TO d3    !
GRANT INSERT       ON sut.tab TO r1    !
GRANT INSERT       ON sut.tab TO p1    !
GRANT DELETE       ON sut.d1_pt1   TO sut !
GRANT DELETE       ON sut.d1_pt1   TO d3    !
GRANT DELETE       ON sut.d1_pt1   TO d3    !
GRANT DELETE       ON sut.d1_pt1   TO r1    !
GRANT DELETE       ON sut.d1_pt1   TO p1    !
GRANT ALL          ON  d1.d1_t1   TO sut !
GRANT ALL          ON  d1.d1_t1   TO d3    !
GRANT ALL          ON     d1_t1   TO d3    !
GRANT ALL          ON  d1.d1_t1   TO r1    !
GRANT ALL          ON  d1.d1_t1   TO p1    !
GRANT SELECT       ON  r2.r2_t1    TO sut !
GRANT SELECT       ON  r2.r2_t1    TO d3    !
GRANT SELECT       ON     r2_t1    TO d3    !
GRANT SELECT       ON  r2.r2_t1    TO r1    !
GRANT SELECT       ON  r2.r2_t1    TO p1    !
COMMIT WORK RELEASE !
file connect (r1 r1_w1 !
GRANT DELETE       ON sut.tab TO sut !
GRANT DELETE       ON sut.tab TO d3    !
GRANT DELETE       ON        tab TO d3    !
GRANT DELETE       ON sut.tab TO r2    !
GRANT DELETE       ON sut.tab TO p1    !
GRANT UPDATE       ON sut.d1_pt1   TO sut !
GRANT UPDATE       ON sut.d1_pt1   TO d3    !
GRANT UPDATE       ON        d1_pt1   TO d3    !
GRANT UPDATE       ON sut.d1_pt1   TO r2    !
GRANT UPDATE       ON sut.d1_pt1   TO p1    !
GRANT INSERT       ON  d2.d2_t1    TO sut !
GRANT INSERT       ON  d2.d2_t1    TO d3    !
GRANT INSERT       ON     d2_t1    TO d3    !
GRANT INSERT       ON  d2.d2_t1    TO r2    !
GRANT INSERT       ON  d2.d2_t1    TO p1    !
GRANT SELUPD ON  r2.r2_t1    TO sut !
GRANT SELUPD ON  r2.r2_t1    TO d3    !
GRANT SELUPD ON     r2_t1    TO d3    !
GRANT SELUPD ON  r2.r2_t1    TO r2    !
GRANT SELUPD ON  r2.r2_t1    TO p1    !
COMMIT WORK RELEASE !
file connect (p1 p1_w1 !
GRANT SELECT       ON sut.tab TO sut !
GRANT SELECT       ON sut.tab TO d3    !
GRANT SELECT       ON        tab TO d3    !
GRANT SELECT       ON sut.tab TO r2    !
GRANT SELECT       ON sut.tab TO p2    !
GRANT INSERT       ON  d1.d1_pt1   TO sut !
GRANT INSERT       ON  d1.d1_pt1   TO d3    !
GRANT INSERT       ON     d1_pt1   TO d3    !
GRANT INSERT       ON  d1.d1_pt1   TO r2    !
GRANT INSERT       ON  d1.d1_pt1   TO p2    !
GRANT DELETE       ON  d2.d2_t1    TO sut !
GRANT DELETE       ON  d2.d2_t1    TO d3    !
GRANT DELETE       ON     d2_t1    TO d3    !
GRANT DELETE       ON  d2.d2_t1    TO r2    !
GRANT DELETE       ON  d2.d2_t1    TO p2    !
GRANT UPDATE       ON  r1.r1_t1   TO sut !
GRANT UPDATE       ON  r1.r1_t1   TO d3    !
GRANT UPDATE       ON     r1_t1   TO d3    !
GRANT UPDATE       ON  r1.r1_t1   TO r2    !
GRANT UPDATE       ON  r1.r1_t1   TO p2    !
COMMIT WORK RELEASE !
 ****************************************************
 ************* fehlerfreies GRANT *******************
 ****************************************************!
file connect (d1 d1_w1 !
GRANT SELECT  ON  d1.d1_pt1   TO d2 !
GRANT SELECT  ON      d1_t1       TO d2 !
GRANT SELECT  ON d1.d1_pt1,
      SELECT  ON d1.d1_pt2  TO r1 !
GRANT SELECT  ON d1.d1_pt1,
      SELECT  ON        d1_t1   TO p1 !
GRANT SELECT  ON d1.d1_pt1,
      SELECT  ON d1.d1_pt2,
      SELECT  ON    d1_t1,
      SELECT  ON    d1_t2 TO p2 !
GRANT INSERT, DELETE ON d1.d1_pt1     TO d2 !
GRANT INSERT, UPDATE ON d1_pt2,
                 ALL ON d1_t2 TO d3, r3, p1 !
file n08privs !
COMMIT WORK RELEASE !
file connect (d2 d2_w1 !
file n08privs !
ROLLBACK WORK RELEASE !
file connect (sut sut !
file n08privs !
ROLLBACK WORK RELEASE !
file connect (r1 r1_w1 !
GRANT  INSERT, DELETE ON r1_t1,
 SELUPD, DELETE ON r1_t2 TO d2, r2, p2 !
GRANT INSERT, DELETE, SELECT, UPDATE ON r1_t1
                                     TO p1 !
COMMIT WORK RELEASE !
file connect (sut sut !
file n08privs !
ROLLBACK WORK RELEASE !
 ****************************************************
 ************* fehlerhaftes REVOKE ******************
 ****************************************************!
file connect (sut sut !
REVOKE SELECT  ON d1.d1_t1 FROM d2 !
REVOKE SELECT  ON d1.d1_t1 FROM p1 !
REVOKE SELECT  ON r1.r1_t1 FROM p1 !
ROLLBACK WORK RELEASE !
file connect ( d2 d2_w1 !
REVOKE INSERT  ON     d1.d1_pt1 FROM d2 !
REVOKE SELECT  ON     d1.d1_t1  FROM d2 !
REVOKE INSERT  ON     d1.d1_t2  FROM d3 !
REVOKE DELETE  ON     r1.r1_t1  FROM r2 !
REVOKE SELECT  ON     d1.d1_pt1 FROM p1 !
REVOKE SELECT  ON     d1.d1_t1  FROM p1 !
REVOKE SELECT  ON     r1.r1_t1  FROM p1 !
ROLLBACK WORK RELEASE !
file connect (p1 p1_w1 !
REVOKE SELECT  ON     d1.d1_pt1 FROM d2 !
REVOKE SELECT  ON     r1.r1_t2  FROM d2 !
REVOKE SELECT  ON     d1.d1_t1  FROM p1 !
REVOKE SELECT  ON     d1.d1_t1  FROM p2 !
REVOKE SELECT  ON     r1.r1_t1  FROM r2 !
ROLLBACK WORK RELEASE !
file connect (r2 r2_w1 !
REVOKE UPDATE  ON     d1.d1_t2  FROM d3 !
REVOKE DELETE  ON     r1.r1_t1  FROM r2 !
REVOKE INSERT  ON     d1.d1_pt2 FROM r3 !
REVOKE INSERT  ON     r1.r1_t1  FROM p2 !
ROLLBACK WORK RELEASE !
 ****************************************************
 ************* fehlerfreies REVOKE ******************
 ****************************************************!
file connect (r1 r1_w1 !
REVOKE ALL PRIVILEGES ON r1_t1
                      FROM d2, r2, p2 !
REVOKE SELECT, INSERT, DELETE, UPDATE ON r1_t1
                      FROM p1 !
COMMIT WORK RELEASE !
file connect (sut sut !
file n08privs !
ROLLBACK WORK RELEASE !
file connect (r1 r1_w1 !
REVOKE SELECT, UPDATE ON r1_t2 FROM d2, r2 !
COMMIT WORK RELEASE !
file connect (sut sut !
file n08privs !
ROLLBACK WORK RELEASE !
file connect (r1 r1_w1 !
REVOKE SELUPD ON r1_t2 FROM p2 !
COMMIT WORK RELEASE !
file connect (sut sut !
file n08privs !
ROLLBACK WORK RELEASE !
file connect (d1 d1_w1 !
REVOKE ALL ON d1.d1_pt1, ALL ON d1_t1 FROM d2 !
REVOKE SELECT ON d1.d1_pt1,
  ALL PRIVILEGES ON d1.d1_pt2 FROM r1 !
COMMIT WORK RELEASE !
file connect (sut sut !
file n08privs !
ROLLBACK WORK RELEASE !
file connect (d1 d1_w1 !
REVOKE SELECT ON d1.d1_pt1,
       SELECT ON d1.d1_pt2,
       SELECT ON d1_t1,
       SELECT ON d1_t2  FROM p2 !
REVOKE INSERT, UPDATE ON d1.d1_pt2,
  SELUPD, INSERT, DELETE ON d1_t2
  FROM d3, r3, p1 !
COMMIT WORK RELEASE !
file connect (sut sut !
file n08privs !
ROLLBACK WORK RELEASE !
 ****************************************************
 ************ Anfangszustand herstellen *************
 ****************************************************!
file connect (sut sut !
DROP USER d1 !
DROP USER d2 !
DROP USER d3 !
DROP USER r1 !
DROP USER r2 !
DROP USER r3 !
DROP USER p1 !
DROP USER p2 !
COMMIT WORK RELEASE !
