*   *ID* E04MI1   DBCMD    changed on 1992-09-28-13.23.45 by ADMIN     *
 ************************ !
 * stack optimization   * !
 * for multiple indices * !
 ************************ !
file connect ( kern test !
create table t (k1  char(20) key,
                k2  char(20) key,
                mi1 char(20) ,
                mi2 char(20) ,
                mi3 char(20) ) !
 **** inserts !
&nodisplay
insert into t set k1 = '0' , k2 = '1'                                  !
insert into t set k1 = '0' , k2 = '2', mi1 = '1'                       !
insert into t set k1 = '0' , k2 = '3',             mi2 = '1'           !
insert into t set k1 = '0' , k2 = '4',                       mi3 = '1' !
insert into t set k1 = '1' , k2 = '1', mi1 = '1', mi2 = '1', mi3 = '1' !
insert into t set k1 = '1' , k2 = '2', mi1 = '1', mi2 = '1', mi3 = '2' !
insert into t set k1 = '1' , k2 = '3', mi1 = '1', mi2 = '2', mi3 = '1' !
insert into t set k1 = '1' , k2 = '4', mi1 = '1', mi2 = '2', mi3 = '2' !
insert into t set k1 = '2' , k2 = '1', mi1 = '2', mi2 = '1', mi3 = '1' !
insert into t set k1 = '2' , k2 = '2', mi1 = '2', mi2 = '1', mi3 = '2' !
insert into t set k1 = '2' , k2 = '3', mi1 = '2', mi2 = '2', mi3 = '1' !
insert into t set k1 = '2' , k2 = '4', mi1 = '2', mi2 = '2', mi3 = '2' !
insert into t set k1 = '3' , k2 = '1'                                  !
insert into t set k1 = '3' , k2 = '2', mi1 = '2'                       !
insert into t set k1 = '3' , k2 = '3',            mi2 = '2'            !
insert into t set k1 = '3' , k2 = '4',                       mi3 = '2' !
create index mia on t(mi1, mi2, mi3) !
&display
declare erg cursor for select * from t where mi1 > '1' for reuse !
fetch erg !
declare erg cursor for select * from t where mi1 < '3' for reuse !
fetch erg !
declare erg cursor for 
select * from t where mi1 > '1' and mi1 < '4' for reuse !
fetch erg !
declare erg cursor for 
select * from t where mi1 < '3' and mi1 < '4'for reuse !
fetch erg !
declare erg cursor for 
select * from t where mi1 between '2' and '4' and mi1 < '4'
for reuse !
fetch erg !
*********************************************************** !
declare erg cursor for select * from t where mi1 = '2' and mi2 > '1'
for reuse !
fetch erg !
declare erg cursor for select * from t where mi1 = '2' and mi2 < '3'
for reuse !
fetch erg !
declare erg cursor for 
select * from t where mi1 = '2' and mi2 between '2' and '3'
for reuse !
fetch erg !
declare erg cursor for 
select * from t where mi1 = '2' and mi2 > '1' and mi2 < '4'
for reuse !
fetch erg !
declare erg cursor for 
select * from t where mi1 = '2' and mi2 like '2%' and mi2 < '4'
for reuse !
fetch erg !
rollback release !
