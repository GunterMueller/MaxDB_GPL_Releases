*   *ID* A17SEL14 DBCMD    changed on 1992-09-28-13.22.42 by ADMIN     *
file connect ( kern test !
CREATE TABLE RVP006 (OBJ_NR FIXED(6) KEY,
BEN_NAME CHAR(8) KEY,
BK_BEZ CHAR(60)  KEY)!
CREATE TABLE RVP011 (AUFG_BEZ_OBJ CHAR(60) KEY,
OBJ_NR FIXED (6) NOT NULL,
AUFG_ART CHAR(16) NOT NULL)!
COMMIT !
PARS_THEN_EX !
DATA !
DECLARE DUMMY2 CURSOR FOR 
SELECT DISTINCT RVP011.AUFG_BEZ_OBJ, AUFG_ART
FROM RVP006, RVP011
WHERE RVP006.OBJ_NR = RVP011.OBJ_NR
AND RVP006.BEN_NAME = :A
ORDER BY RVP011.AUFG_ART, AUFG_BEZ_OBJ !
c 8 '0d1196'

nodata !
fetch dummy2 into :a, :b!
pars_execute!
drop table rvp006 !
drop table rvp011 !
commit work release !
