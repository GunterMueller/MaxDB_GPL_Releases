*   *ID* N99A27   DBCMD    changed on 1992-09-28-13.27.19 by ADMIN     *
*
 * 1990-10-25 * Kern-Fehlerprotokoll # 154  (2.407) !
*
file connect ( kern test !
create table t1 (a1 char(5) key, a2 char(5) key) !
insert t1 values ('aaaaa','bbbbb') !
insert t1 values ('bbbbb','xxxxx') !
insert t1 values ('ccccc','zzzzz') !
declare erg cursor for
select * from t1 for reuse !
explain select * from erg,
t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a1 = erg.a1 and r2.a1 = erg.a1 and r3.a1 = erg.a1 and
r4.a1 = erg.a1 and r5.a1 = erg.a1 and r6.a1 = erg.a1 and r7.a1 = erg.a1
and  r1.a2 = 'xxxxx' and r2.a2 = 'xxxxx' and r3.a2 = 'xxxxx' and
r4.a2 = 'xxxxx' and r5.a2 = 'xxxxx' and r6.a2 = 'xxxxx' and
r7.a2 = 'xxxxx' and erg.a2 = 'xxxxx' !
select * from erg,
t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a1 = erg.a1 and r2.a1 = erg.a1 and r3.a1 = erg.a1 and
r4.a1 = erg.a1 and r5.a1 = erg.a1 and r6.a1 = erg.a1 and r7.a1 = erg.a1
and  r1.a2 = 'xxxxx' and r2.a2 = 'xxxxx' and r3.a2 = 'xxxxx' and
r4.a2 = 'xxxxx' and r5.a2 = 'xxxxx' and r6.a2 = 'xxxxx' and
r7.a2 = 'xxxxx' and erg.a2 = 'xxxxx' !
fetch !
explain select * from erg,
t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a1 = erg.a1 and r2.a1 = erg.a1 and r3.a1 = erg.a1 and
r4.a1 = erg.a1 and r5.a1 = erg.a1 and r6.a1 = erg.a1 and r7.a1 = erg.a1
and  r1.a2 = 'xxxxx' and r2.a1 = 'xxxxx' and r3.a1 = 'xxxxx' and
r4.a1 = 'xxxxx' and r5.a1 = 'xxxxx' and r6.a1 = 'xxxxx' and
r7.a1 = 'xxxxx' !
select * from erg,
t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a1 = erg.a1 and r2.a1 = erg.a1 and r3.a1 = erg.a1 and
r4.a1 = erg.a1 and r5.a1 = erg.a1 and r6.a1 = erg.a1 and r7.a1 = erg.a1
and  r1.a2 = 'xxxxx' and r2.a1 = 'xxxxx' and r3.a1 = 'xxxxx' and
r4.a1 = 'xxxxx' and r5.a1 = 'xxxxx' and r6.a1 = 'xxxxx' and
r7.a1 = 'xxxxx' !
fetch !
rollback work !
create table t1 (a1 char(5) key, a2 char(5) key) !
insert t1 values ('aaaaa','bbbbb') !
insert t1 values ('bbbbb','xxxxx') !
insert t1 values ('ccccc','zzzzz') !
declare erg cursor for
select * from t1 for reuse !
select * from erg, t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a1 = erg.a1 and r2.a1 = erg.a1 and r3.a1 = erg.a1 and
r4.a1 = erg.a1 and r5.a1 = erg.a1 and r6.a1 = erg.a1 and r7.a1 = erg.a1 !
fetch !
rollback work !
create table t1 (a1 char(5) key, a2 char(5) key) !
insert t1 values ('aaaaa','bbbbb') !
insert t1 values ('bbbbb','xxxxx') !
insert t1 values ('ccccc','zzzzz') !
declare erg cursor for
select * from t1 for reuse !
explain select * from erg,
t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a2 = erg.a2 and r2.a2 = erg.a2 and r3.a2 = erg.a2 and
r4.a2 = erg.a2 and r5.a2 = erg.a2 and r6.a2 = erg.a2 and r7.a2 = erg.a2
and  r1.a1 = 'bbbbb' and r2.a1 = 'bbbbb' and r3.a1 = 'bbbbb' and
r4.a1 = 'bbbbb' and r5.a1 = 'bbbbb' and r6.a1 = 'bbbbb' and
r7.a1 = 'bbbbb' !
select * from erg, t1 r1, t1 r2, t1 r3, t1 r4, t1 r5, t1 r6, t1 r7
where r1.a2 = erg.a2 and r2.a2 = erg.a2 and r3.a2 = erg.a2 and
r4.a2 = erg.a2 and r5.a2 = erg.a2 and r6.a2 = erg.a2 and r7.a2 = erg.a2
and  r1.a1 = 'bbbbb' and r2.a1 = 'bbbbb' and r3.a1 = 'bbbbb' and
r4.a1 = 'bbbbb' and r5.a1 = 'bbbbb' and r6.a1 = 'bbbbb' and
r7.a1 = 'bbbbb' !
fetch !
rollback work release !
