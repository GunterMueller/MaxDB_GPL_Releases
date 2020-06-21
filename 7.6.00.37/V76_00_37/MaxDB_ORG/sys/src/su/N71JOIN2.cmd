*   *ID* N71JOIN2 DBCMD    changed on 1992-09-28-13.27.07 by ADMIN     *
file CONNECT ( KERN TEST !
CREATE TABLE T1 ( A1 CHAR (8)) !
INSERT T1 SET A1 = 'aaa' !
CREATE TABLE T2 ( A2 CHAR (8)) !
INSERT T2 SET A2 = 'aaa' !
CREATE INDEX "T1.A1"
	on T1( A1 ) !
CREATE INDEX "T2.A2"
	on T2( A2 ) !
PARS_THEN_EX !
DATA !
SELECT t1.syskey, t1.syskey, t1.syskey FROM T1, T2
  WHERE T1.A1 = T2.A2 AND T1.A1 IN (:A, :B) AND T2.A2 IN (:C, :D) !
c 8 'www' c 8 'aaa' c 8 'aaa' c 8 'eee'

NODATA !
PARS_EXECUTE !
rollback work release !
