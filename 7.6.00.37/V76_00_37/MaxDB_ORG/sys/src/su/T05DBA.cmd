*   *ID* T05DBA   DBCMD    changed on 1992-09-28-13.28.39 by ADMIN     *
 ***************************
 ***   T 0 5 D B A       ***
 ***************************!
CREATE TABLE P2     ( K1 CHAR (5) KEY,
                      f1 FIXED (4,2) NOT NULL )  !
CREATE TABLE P1     ( K1 CHAR (3) KEY,
                      f1 CHAR (1) ) !
COMMIT WORK !
file sel_tab_owner ( KERN !
ALTER TABLE p1
  ADD  f4 CHAR (7)   !
COMMIT WORK !
file sel_cols_1tab ( KERN P1 !
ALTER TABLE kern.p2
  ADD  f3 CHAR (5)     !
COMMIT WORK !
file sel_cols_1tab ( KERN P2 !
ALTER TABLE kern.p2
  DROP f1  !
COMMIT WORK !
file sel_cols_1tab ( KERN P2 !
CREATE VIEW kern.p2_teil AS SELECT *
                       FROM kern.p2
                       WHERE ( k1 <= 'ghijk' )  !
COMMIT WORK !
file sel_view1 ( KERN P2_TEIL !
DROP VIEW kern.p2_teil !
COMMIT WORK !
file sel_view1 ( KERN P2_TEIL !
DROP TABLE kern.p1 !
DROP TABLE kern.p2 !
COMMIT WORK !
file sel_tab_owner ( KERN !
