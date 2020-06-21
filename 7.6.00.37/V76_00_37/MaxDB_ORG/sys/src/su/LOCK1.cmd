*   *ID* LOCK1    DBCMD    changed on 1992-09-28-13.24.32 by ADMIN     *
connect kern identified by test isolation level  &1 !
&2 !
file &3 (ROLLBACK !
insert t set a = 'fourth', b = 'b4'!
file &3 (ROLLBACK !
insert t1 select b,a from t where a > 'm' !
file &3 (ROLLBACK !
update t set b = 'new' key a = 'second' !
file &3 (ROLLBACK !
update t set b = 'new' key a = 'fourth' !
file &3 (ROLLBACK !
update t set b = 'new' where a < 't' !
file &3 (ROLLBACK !
delete t key a = 'first' !
file &3 (ROLLBACK !
delete t key a = 'fourth' !
file &3 (ROLLBACK !
delete t where b >= 'b2' !
file &3 (ROLLBACK !
file &3 (COMMIT !
file &3 (COMMIT !
file &3 (COMMIT !
file &3 (COMMIT !
file &3 (COMMIT !
file &3 (COMMIT !
file &3 (COMMIT !
file &3 (COMMIT !
declare erg cursor for select * from t !
file &3 (COMMIT !
fetch1 erg into :a, :b !
file &3 (COMMIT !
fetch first erg into :a, :b !
file &3 (COMMIT !
mfetch first erg into :a, :b !
10
file &3 (COMMIT !
declare erg cursor for select * from t for reuse !
file &3 (COMMIT !
fetch1 erg into :a, :b !
file &3 (COMMIT !
fetch first erg into :a, :b !
file &3 (COMMIT !
mfetch first erg into :a, :b !
10
file &3 (COMMIT !
declare erg cursor for select * from t with lock isolation level 20 !
file &3 (COMMIT !
fetch1 erg into :a, :b !
file &3 (COMMIT !
fetch first erg into :a, :b !
file &3 (COMMIT !
mfetch first erg into :a, :b !
10
file &3 (COMMIT !
declare erg cursor for select * from t with lock isolation level 20 for reuse!
file &3 (COMMIT !
fetch1 erg into :a, :b !
file &3 (COMMIT !
fetch first erg into :a, :b !
file &3 (COMMIT !
mfetch first erg into :a, :b !
10
file &3 (COMMIT !
declare erg cursor for select * from t with lock exclusive !
file &3 (COMMIT !
fetch1 erg into :a, :b !
file &3 (COMMIT !
fetch first erg into :a, :b !
file &3 (COMMIT !
mfetch first erg into :a, :b !
10
file &3 (COMMIT !
declare erg cursor for select * from t with lock exclusive for reuse !
file &3 (COMMIT !
fetch1 erg into :a, :b !
file &3 (COMMIT !
fetch first erg into :a, :b !
file &3 (COMMIT !
mfetch first erg into :a, :b !
10
file &3 (COMMIT !
declare erg1 cursor for select * from erg !
file &3 (COMMIT !
fetch erg1 into :a, :b !
file &3 (COMMIT !
mfetch first erg1 into :a, :b !
10
file &3 (COMMIT !
declare erg1 cursor for select * from erg with lock isolation level 20 !
file &3 (COMMIT !
fetch erg1 into :a, :b !
file &3 (COMMIT !
mfetch first erg1 into :a, :b !
10
file &3 (COMMIT !
declare erg1 cursor for select * from erg with lock exclusive !
file &3 (COMMIT !
fetch erg1 into :a, :b !
file &3 (COMMIT !
mfetch first erg1 into :a, :b !
10
file &3 (COMMIT !
declare erg1 cursor for select * from t, t1 where a < a1 !
file &3 (COMMIT !
fetch erg1 into :a, :b, :c, :d !
file &3 (COMMIT !
mfetch first erg1 into :a, :b, :c, :d !
10
file &3 (COMMIT !
insert t1 select b,a from t where a < 't' !
commit !
declare erg1 cursor for select * from t, t1 where b <> a1 !
file &3 (COMMIT !
fetch erg1 into :a, :b, :c, :d !
file &3 (COMMIT !
mfetch first erg1 into :a, :b, :c, :d !
10
file &3 (COMMIT !
declare erg1 cursor for select * from t, t1 
where a = a1 with lock isolation level 20 !
file &3 (COMMIT !
fetch erg1 into :a, :b, :c, :d !
file &3 (COMMIT !
mfetch first erg1 into :a, :b, :c, :d !
10
file &3 (COMMIT !
declare erg1 cursor for select * from t, t1 
where a = a1 with lock exclusive !
file &3 (COMMIT !
fetch erg1 into :a, :b, :c, :d !
file &3 (COMMIT !
mfetch first erg1 into :a, :b, :c, :d !
10
file &3 (COMMIT !
pars_execute !
delete t1 !
commit release !
