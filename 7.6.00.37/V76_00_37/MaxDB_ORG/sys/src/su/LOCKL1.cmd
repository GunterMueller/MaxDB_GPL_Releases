*   *ID* LOCKL1   DBCMD    changed on 1993-08-11-13.24.32 by ELKE      *
connect kern identified by test isolation level  &1 !
&2 !
file &3 (ROLLBACK !
pars_then_ex !
data !
insert t set a = 'fourth', b = 'b4', l = :l!
l 400 'fourth Text-String. '

nodata !
&2 !
file &3 (ROLLBACK !
insert t1 select b,a from t where a > 'm' !
file &3 (ROLLBACK !
pars_then_ex !
data !
update t set b = 'new', l = :l key a = 'second' !
l 400 'new-second String.  '

nodata !
&2 !
file &3 (ROLLBACK !
pars_then_ex !
data !
update t set b = 'new', l = :l key a = 'fourth' !
l 400 'new-fourth String.  '

nodata !
&2 !
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
fetch1 erg into :a, :b, :l !
file &3 (COMMIT !
fetch first erg into :a, :b, :l !
file &3 (COMMIT !
mfetch first erg into :a, :b, :l !
10
file &3 (COMMIT !
declare erg cursor for select * from t for reuse !
file &3 (COMMIT !
fetch1 erg into :a, :b, :l !
file &3 (COMMIT !
fetch first erg into :a, :b, :l !
file &3 (COMMIT !
mfetch first erg into :a, :b, :l !
10
file &3 (COMMIT !
declare erg cursor for select * from t with lock isolation level 20 !
file &3 (COMMIT !
fetch1 erg into :a, :b, :l !
file &3 (COMMIT !
fetch first erg into :a, :b, :l !
file &3 (COMMIT !
mfetch first erg into :a, :b, :l !
10
file &3 (COMMIT !
declare erg cursor for select * from t 
with lock isolation level 20 for reuse !
file &3 (COMMIT !
fetch1 erg into :a, :b, :l !
file &3 (COMMIT !
fetch first erg into :a, :b, :l !
file &3 (COMMIT !
mfetch first erg into :a, :b, :l !
10
file &3 (COMMIT !
declare erg cursor for select * from t with lock exclusive !
file &3 (COMMIT !
fetch1 erg into :a, :b, :l !
file &3 (COMMIT !
fetch first erg into :a, :b, :l !
file &3 (COMMIT !
mfetch first erg into :a, :b, :l !
10
file &3 (COMMIT !
declare erg cursor for select * from t with lock exclusive for reuse !
file &3 (COMMIT !
fetch1 erg into :a, :b, :l !
file &3 (COMMIT !
fetch first erg into :a, :b, :l !
file &3 (COMMIT !
mfetch first erg into :a, :b, :l !
10
file &3 (COMMIT !
declare erg1 cursor for select * from erg !
file &3 (COMMIT !
fetch erg1 into :a, :b, :l !
file &3 (COMMIT !
mfetch first erg1 into :a, :b, :l !
10
file &3 (COMMIT !
declare erg1 cursor for select * from erg with lock isolation level 20 !
file &3 (COMMIT !
fetch erg1 into :a, :b, :l !
file &3 (COMMIT !
mfetch first erg1 into :a, :b, :l !
10
file &3 (COMMIT !
declare erg1 cursor for select * from erg with lock exclusive !
file &3 (COMMIT !
fetch erg1 into :a, :b, :l !
file &3 (COMMIT !
mfetch first erg1 into :a, :b, :l !
10
file &3 (COMMIT !
declare erg1 cursor for select * from t, t1 where a < a1 !
file &3 (COMMIT !
fetch erg1 into :a, :b, :l, :c, :d !
file &3 (COMMIT !
mfetch first erg1 into :a, :b, :l, :c, :d !
10
file &3 (COMMIT !
insert t1 select b,a from t where a < 't' !
commit !
declare erg1 cursor for select * from t, t1 where b <> a1 !
file &3 (COMMIT !
fetch erg1 into :a, :b, :l, :c, :d !
file &3 (COMMIT !
mfetch first erg1 into :a, :b, :l, :c, :d !
10
file &3 (COMMIT !
declare erg1 cursor for select * from t, t1 where a = a1 
with lock isolation level 20 !
file &3 (COMMIT !
fetch erg1 into :a, :b, :l, :c, :d !
file &3 (COMMIT !
mfetch first erg1 into :a, :b, :l, :c, :d !
10
file &3 (COMMIT !
declare erg1 cursor for select * from t, t1 where a = a1 
with lock exclusive !
file &3 (COMMIT !
fetch erg1 into :a, :b, :l, :c, :d !
file &3 (COMMIT !
mfetch first erg1 into :a, :b, :l, :c, :d !
10
file &3 (COMMIT !
pars_execute !
delete t1 !
commit release !
