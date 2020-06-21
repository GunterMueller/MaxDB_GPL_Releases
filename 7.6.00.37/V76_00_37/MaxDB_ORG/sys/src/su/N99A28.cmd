*   *ID* N99A28   DBCMD    changed on 1992-09-28-13.27.19 by ADMIN     *
*
* 1990-11-26 * interner Fehler: Order by in einer View wurde
*              ignoriert (op_output mit op_output_oflw ueberschrieben)
file connect ( kern test !
create table t1 (a1 char(5), a2 char(5)) !
insert t1 values ('2aaaa','2bbbb') !
insert t1 values ('1aaaa','1bbbb') !
insert t1 values ('4aaaa','4bbbb') !
insert t1 values ('3aaaa','3bbbb') !
insert t1 values ('5aaaa','5bbbb') !
insert t1 values ('7aaaa','7bbbb') !
insert t1 values ('6aaaa','6bbbb') !
insert t1 values ('9aaaa','9bbbb') !
insert t1 values ('8aaaa','8bbbb') !
create table t2 (b1 char(5), b2 char(5)) !
insert t2 values ('aaaaa','bbbbb') !
insert t2 values ('aaaaa','xxxxx') !
create view tv1 (a1, "a2") as select a1, a2 from t1 !
create view tv2 (b1, "b2") as select b1, b2 from t2 !
declare erg cursor for
select a1, b1, "b2" from <tv1, tv2> order by "a2" !
FETCH erg!
declare erg cursor for
select a1, b1, "b2" from <tv2, tv1> order by "a2" !
FETCH erg!
declare erg cursor for
select a1, b1, "b2" from <tv1, tv2> order by "a2" desc !
FETCH erg!
declare erg cursor for
select a1, b1, "b2" from <tv2, tv1> order by "a2" desc !
FETCH erg!
declare erg cursor for
select a1, "a2", b1, "b2" from <tv1, tv2> order by "a2" !
FETCH erg!
declare erg cursor for
select a1, "a2", b1, "b2" from <tv2, tv1> order by "a2" !
FETCH erg!
declare erg cursor for
select a1, "a2", b1, "b2" from <tv1, tv2> order by "a2" desc !
FETCH erg!
declare erg cursor for
select a1, "a2", b1, "b2" from <tv2, tv1> order by "a2" desc !
FETCH erg!
rollback work release !
