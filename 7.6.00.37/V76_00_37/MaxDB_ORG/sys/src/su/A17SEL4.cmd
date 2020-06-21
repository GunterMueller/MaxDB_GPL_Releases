*   *ID* A17SEL4  DBCMD    changed on 1992-09-28-13.22.46 by ADMIN     *
file connect ( kern test !
CREATE TABLE T4 ( A CHAR (4) KEY, B FIXED (5,2)) !
INSERT T4 VALUES ('AAA', 11)!
INSERT T4 VALUES ('BAA', 11)!
INSERT T4 VALUES ('CAA', 13)!
COMMIT WORK RELEASE !
file connect ( kern test !
SELECT * FROM T4 !
FETCH !
parsing !
declare :a cursor for
select a from t4 !
executing!
data!
select!
1
c 18 'ABCDEFGH'

nodata!
pars_then_ex!
fetch abcdefgh into :a !
pars_execute!
drop table t4 !
COMMIT WORK RELEASE !
