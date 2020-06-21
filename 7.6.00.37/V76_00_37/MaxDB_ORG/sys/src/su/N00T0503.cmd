*   *ID* N00T0503 DBCMD    changed on 1992-09-28-13.24.51 by ADMIN     *
 *** 5.3  <delete statement> !
file connect ( kern test !
create table t8
   ( t8_c1 char(64) key ,
     t8_c2 char(10) byte key,
     t8_c3 fixed( 5,2 ) ,
     t8_c4 float ( 3 ) ) !
insert t8 values ( 'Hans', x'c2c5d9d3c0d5', 33.2 , 72.30 ) !
insert t8 values ( 'Otto', x'C8C1D4C2E4D9C7', 33.2, 0 ) !
insert t8 values ( 'Paul', x'c2d9c1e4d5d3c1c7c5',-0.27,null ) !
insert t8 values ( user , x'e2e4d7c5d9c4c2c1', +38, null ) !
insert t8 values ( 'Marianne', x'd2d6c5d3d5', NULL,-.17) !
insert t8 values ('Karin', x'C8C1D4D4', 512.41, 56 ) !
delete t8 where t8.t8_c3 = -.27 and t8.t8_c4 is null !
delete from t8 key t8_c1= 'KERN', t8_c2 = x'e2e4d7c5d9c4c2c1'!
delete from t8 where t8_c3 = 33.2 !
delete t8 key t8_c1 = 'Marianne', t8_c2= x'd2d6c5d3d5'
   where t8.t8_c3 is null !
select substr(t8_c1,1,20) t8_c1, t8_c2, t8_c3, t8_c4 from t8 !
fetch !
delete t8 !
insert t8 values ( 'Hans', x'c2c5d9d3c9d5', 33.2 , 72.30 ) !
insert t8 values ( 'Otto', x'C8C1D4C2E4D9C7', 33.2, 0 ) !
create synonym syn_t8 for t8 !
delete syn_t8 key t8_c1 = 'Hans', t8_c2 = x'c2c5d9d3c9d5' !
delete from syn_t8 !
insert t8 values ( 'Hans', x'c2c5d9d3c9d5', 33.2 , 72.30 ) !
insert t8 values ( 'Otto', x'C8C1D4C2E4D9C7', 33.2, 0 ) !
insert t8 values ('Karin', x'c8c1d4d4', 512.41, 56 ) !
create view view_t8 as select * from t8 where t8_c1 < 'M'
   with check option !
delete view_t8 where t8_c2=x'c8c1d4d4' and view_t8.t8_c3 > 500 !
delete view_t8 !
select substr(t8_c1,1,20) t8_c1, t8_c2, t8_c3, t8_c4 from syn_t8 !
fetch !
delete from syn_t8 where 100 > syn_t8.t8_c3 * 3 !
drop table t8 !
commit work release !
