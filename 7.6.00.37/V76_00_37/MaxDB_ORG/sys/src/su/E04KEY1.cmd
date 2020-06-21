*   *ID* E04KEY1  DBCMD    changed on 1992-09-28-13.23.44 by ADMIN     *
 ************************ !
 * stack optimization   * !
 * for key conditions   * !
 ************************ !
file connect ( kern test !
create table t (k1 char(20) key,
                k2 char(20) key,
                i1 char(20) ,
                i2 char(20) ,
                i3 char(20) ) !
create index "t.i1"
	on t( i1 ) !
create index "t.i2"
	on t( i2 ) !
create index "t.i3"
	on t( i3 ) !
 **** inserts !
&nodisplay
insert into t set k1 = '0' , k2 = '1'                               !
insert into t set k1 = '0' , k2 = '2', i1 = '1'                     !
insert into t set k1 = '0' , k2 = '3',           i2 = '1'           !
insert into t set k1 = '0' , k2 = '4',                     i3 = '1' !
insert into t set k1 = '1' , k2 = '1', i1 = '1', i2 = '1', i3 = '1' !
insert into t set k1 = '1' , k2 = '2', i1 = '1', i2 = '1', i3 = '2' !
insert into t set k1 = '1' , k2 = '3', i1 = '1', i2 = '2', i3 = '1' !
insert into t set k1 = '1' , k2 = '4', i1 = '1', i2 = '2', i3 = '2' !
insert into t set k1 = '2' , k2 = '1', i1 = '2', i2 = '1', i3 = '1' !
insert into t set k1 = '2' , k2 = '2', i1 = '2', i2 = '1', i3 = '2' !
insert into t set k1 = '2' , k2 = '3', i1 = '2', i2 = '2', i3 = '1' !
insert into t set k1 = '2' , k2 = '4', i1 = '2', i2 = '2', i3 = '2' !
insert into t set k1 = '3' , k2 = '1'                               !
insert into t set k1 = '3' , k2 = '2', i1 = '1'                     !
insert into t set k1 = '3' , k2 = '3',           i2 = '1'           !
insert into t set k1 = '3' , k2 = '4',                     i3 = '1' !
&display
declare erg cursor for select * from t where k1 > '1' for reuse !
fetch erg !
declare erg cursor for select * from t where k1 < '3' for reuse !
fetch erg !
declare erg cursor for 
select * from t where k1 > '1' and k1 < '4' for reuse !
fetch erg !
declare erg cursor for 
select * from t where k1 < '3' and k1 < '4'for reuse !
fetch erg !
declare erg cursor for 
select * from t where k1 between '2' and '4' and k1 < '4'
for reuse !
fetch erg !
*********************************************************** !
declare erg cursor for select * from t where k1 = '2' and k2 > '1'
for reuse !
fetch erg !
declare erg cursor for select * from t where k1 = '2' and k2 < '3'
for reuse !
fetch erg !
declare erg cursor for 
select * from t where k1 = '2' and k2 > '1' and k2 < '4'
for reuse !
fetch erg !
declare erg cursor for 
select * from t where k1 = '2' and k2 like '2%' and k2 < '4'
for reuse !
fetch erg !
rollback release !
