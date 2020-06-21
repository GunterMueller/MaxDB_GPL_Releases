*   *ID* E04BET   DBCMD    changed on 1992-09-28-13.23.43 by ADMIN     *
 ************************ !
 * stack optimization   * !
 * for single indices   * !
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
insert into t set k1 = '0' , k2 = '2', i1 = '0'                     !
insert into t set k1 = '0' , k2 = '3',           i2 = '0'           !
insert into t set k1 = '0' , k2 = '4',                     i3 = '0' !
insert into t set k1 = '1' , k2 = '1', i1 = '1', i2 = '1', i3 = '1' !
insert into t set k1 = '1' , k2 = '2', i1 = '1', i2 = '1', i3 = '2' !
insert into t set k1 = '1' , k2 = '3', i1 = '1', i2 = '2', i3 = '1' !
insert into t set k1 = '1' , k2 = '4', i1 = '1', i2 = '2', i3 = '2' !
insert into t set k1 = '2' , k2 = '1', i1 = '2', i2 = '1', i3 = '1' !
insert into t set k1 = '2' , k2 = '2', i1 = '2', i2 = '1', i3 = '2' !
insert into t set k1 = '2' , k2 = '3', i1 = '2', i2 = '2', i3 = '1' !
insert into t set k1 = '2' , k2 = '4', i1 = '2', i2 = '2', i3 = '2' !
insert into t set k1 = '3' , k2 = '1'                               !
insert into t set k1 = '3' , k2 = '2', i1 = '3'                     !
insert into t set k1 = '3' , k2 = '3',           i2 = '3'           !
insert into t set k1 = '3' , k2 = '4',                     i3 = '3' !
&display
***********************************************************
declare erg cursor for 
select * from t where i2 = '2' for reuse !
fetch erg !
declare erg cursor for 
select * from t where i2 >= '2' for reuse !
fetch erg !
declare erg cursor for 
select * from t where i2 <= '2' for reuse !
fetch erg !
declare erg cursor for 
select * from t where i2 between '1' and '2' for reuse !
fetch erg !
declare erg cursor for 
select * from t where i2 >= '2' and i2 <= '3' for reuse !
fetch erg !
declare erg cursor for 
select * from t where i2 <= '2' and i2 <= '3'for reuse !
fetch erg !
declare erg cursor for 
select * from t where i2 like '2%' for reuse !
fetch erg !
declare erg cursor for 
select * from t where i2 between '2' and '4' and i2 <= '3'
for reuse !
fetch erg !
declare erg cursor for select * from t where i2 is null
for reuse !
fetch erg !
rollback release !
