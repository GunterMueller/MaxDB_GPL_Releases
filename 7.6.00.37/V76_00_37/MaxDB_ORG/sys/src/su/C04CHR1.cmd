*   *ID* C04CHR1  DBCMD    changed on 1992-09-28-13.23.09 by ADMIN     *
*------- es war moeglich: create table t (col char(n)), insert,
*                         update t set col = 3+5 .
*                         inhalt war dann 'C1 80 00...', also die Zahl 8!
create table c ( col char(10) )!
insert c values ('aaaaaaaaaa')!
select * from c !
fetch!
*
update c set col = 3+5 !
select * from c !
fetch!
*
update c set col = '>' & 3 + 5 & '<' !
select * from c !
fetch!
*
update c set col = 3-5 !
select * from c !
fetch!
*
update c set col = 3 * 5 !
select * from c !
fetch!
*
update c set col = 3 / 5 !
select * from c !
fetch!
