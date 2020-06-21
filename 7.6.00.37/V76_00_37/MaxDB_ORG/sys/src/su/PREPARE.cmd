*   *ID* PREPARE  DBCMD    changed on 1992-11-11-17.02.09 by KERN      *
file connect ( kern test !
create table t1 (s1 int) !
insert t1 values (1) !
parsing !
parse select * from t1 for reuse !
executing !
select !
1,'RES'
parsing noinit !
parse select * from res !
pars_then_ex !
CLOSE RES !
executing !
select !
1,'RES1'
SELECT !
2,'RES2'
pars_execute !
rollback !
CREATE TABLE T ( A CHAR (8))!
INSERT T SET A = 'aaa'!
select a||a||a||a from t !
PARSing !
parse SELECT * FROM T !
parse fetch into ? !
executing !
select !
1
fetch into :a !
2
pars_execute!
rollback release !
