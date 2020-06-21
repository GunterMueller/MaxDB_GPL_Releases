*   *ID* A17SEL18 DBCMD    changed on 1992-11-11-17.01.54 by KERN      *
file connect ( kern test !
create table tab ( a fixed (3) key)!
insert tab set a =   1!
insert tab set a =   2!
insert tab set a =   3!
insert tab set a =   4!
insert tab set a =   5!
insert tab set a =   6!
insert tab set a =   7!
insert tab set a =   8!
insert tab set a =   9!
insert tab set a =  10!
insert tab set a =  11!
insert tab set a =  12!
select * from tab !
mfetch pos (1) !
10
mfetch pos (11) !
10
mfetch pos (9) !
10
mfetch pos (7) !
10
select * from tab !
mfetch pos (13) !
10
fetch1 prev !
fetch1 pos (11) !
fetch1 pos (7) !
 * Fehlerprotokoll 125 vom 25.7.90 !
select * from tab where a > 0!
fetch  first  into :v01!
fetch1 next   into :v01!
fetch1 next   into :v01!
fetch1 pos(3) into :v01!
fetch1 same   into :v01!
 * Problem des fetch prev nach 'b_no_next_record' !
fetch1 pos (12) into :v01!
fetch1 next     into :v01!
fetch1 prev     into :v01!
fetch1 pos (12) into :v01!
mfetch next  !
3
fetch1 prev     into :v01!
fetch1 pos (11) into :v01!
mfetch next  !
3
fetch1 prev     into :v01!
******** !
fetch1 first    into :v01!
fetch1 prev     into :v01!
fetch1 next     into :v01!
fetch1 first    into :v01!
mfetch prev  !
3
fetch1 next     into :v01!
fetch1 first    into :v01!
fetch1 next     into :v01!
mfetch prev  !
3
fetch1 next     into :v01!
declare erg cursor for 
select * from tab for reuse !
mfetch same erg !
2
mfetch pos (5) erg !
4
mfetch same erg !
4
mfetch last erg !
3
mfetch same erg !
4
select * from tab !
mfetch same !
2
mfetch pos (5) !
4
mfetch same !
4
mfetch last !
3
mfetch same !
4
rollback work release !
* Fehlerprotokoll 269 vom 30.10.92 !
file connect ( kern test !
create table t ( a char (8))!
insert t set a = 'aaa'!
create table t1 (a1 char (8))!
insert t1 set a1 = 'f'!
select * from t, t1 where a like 'aaa' and
(a = a1 or a1 = 'f')
order by a !
fetch1 pos(1) into :a, :b !
rollback !
* FETCH POS (x), x groesser als Ergebnismenge. Dann reagierten
* aufgebaute und nicht-aufgebaute Ergmengen unterschiedlich auf
* anschliessendes FETCH PREV !
create table t ( a fixed (8))!
insert t set a = 1 !
insert t set a = 2 !
insert t set a = 3 !
insert t set a = 4 !
declare erg1 cursor for 
select * from t for reuse !
fetch1 erg1 !
fetch1 erg1 !
fetch1 pos (6) erg1 !
fetch1 prev erg1 !
declare erg2 cursor for 
select * from t order by a for reuse !
fetch1 erg2 !
fetch1 erg2 !
fetch1 pos (6) erg2 !
fetch1 prev erg2 !
declare erg3 cursor for 
select * from t !
fetch1 erg3 !
fetch1 erg3 !
fetch1 pos (6) erg3 !
fetch1 prev erg3 !
declare erg4 cursor for 
select * from t order by a !
fetch1 erg4 !
fetch1 erg4 !
fetch1 pos (6) erg4 !
fetch1 prev erg4 !
rollback release !
 