*   *ID* N55VIEW5 DBCMD    changed on 1995-11-15-14.48.00 by Heinz     *
file pnxdiff!
file connect ( kern test !
create table t ( a char (8))!
create view v1(va, vb, vc, vd, ve) as select
 date, time, user, a, usergroup from t !
create view v2 (va, vb) as select
 a, datediff('19901231', date) from t !
 * datediff mit falschem datum in viewdefinition !
create view v3 (va, vb) as select
 a, datediff('19903112', date) from t !
insert t set a = 'userview' !
insert v1 set vd = 'secondrow' !
select va, vb, vc, vd, ve, 'abcdef' from v1
where va > '19900131' and vc = 'KERN' and vb >= time !
fetch !
select * from v2 !
 *++ Differenz durch Zeitabhaengigkeit!
fetch!
select * from v3 !
drop table t !
commit release !
