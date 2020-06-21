*   *ID* VIEWCHK1 DBCMD    changed on 1992-09-28-13.28.56 by ADMIN     *
*CS*
insert v1 set s =    5 !
insert v1 set s =   55 !
insert v1 set s =  555 !
insert v1 set s =  666 !
insert v1 set s =  999 !
insert v1 set s = 5555 !
*
insert v2 set s =    5 !
insert v2 set s =   55 !
insert v2 set s =  555 !
insert v2 set s = 5555 !
*
insert v3 set s =    5 !
insert v3 set s =   55 !
insert v3 set s =  555 !
insert v3 set s = 5555 !
*
select * from tab !
fetch !
*
select * from v1 !
fetch !
*
select * from v2 !
fetch !
*
select * from v3 !
fetch !
*
update v1 set s =   88 where s =  555 !
update v2 set s =  777 where s =    5 !
update v2 set s = 4444 where s =  999 !
update v3 set s =  333 where s =   88 !
update v3 set s = 2222 where s =  666 !
update v3 set s = 4444 where s = 5555 !
*
select * from tab !
fetch !
* views wieder loeschen !
rollback work !
