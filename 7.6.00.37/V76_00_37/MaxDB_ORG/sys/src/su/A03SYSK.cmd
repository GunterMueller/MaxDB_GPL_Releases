*   *ID* A03SYSK  DBCMD    changed on 1992-09-28-13.22.22 by ADMIN     *
file connect ( kern test !
 * die Angabe von syskey war eine zeitlang erlaubt gewesen.
 * der zugehoerige Wert wurde aber einfach weggeworfen !
create table t ( a char (8) byte, b char (7))!
insert t values (stamp, stamp, 'bbb')!
insert t (syskey, a, b ) values (stamp, stamp, 'bbb')!
insert t set syskey = x'0000000000000001', a = x'0000000000000001',
              b = 'bbb'!
insert t set  a = x'0000000000000001', b = 'bbb'!
select * from t !
fetch !
drop table t !
CREATE TABLE T ( A FIXED (10) KEY, B CHAR (8) BYTE KEY, C CHAR (18))!
PARS_THEN_EX !
INSERT T VALUES (123, STAMP, '    ')!
PARS_EXECUTE !
SELECT * INTO :A, :B, :C FROM T !
DELETE T !
INSERT T VALUES (123, STAMP, '    ')!
SELECT * INTO :A, :B, :C FROM T !
DROP TABLE T !
commit release !
