*   *ID* N35GROU9 DBCMD    changed on 1992-09-28-13.26.21 by ADMIN     *
file connect ( kern test !
* Fehlerprotokoll 175 !
create table station ( bhf char (25))!
insert station set bhf = 'Adenauerplatz'!
select * from station where bhf in
(select bhf from station group by bhf having count(bhf) > 1)
order by bhf !
select * from station where bhf in
(select bhf from station group by bhf having count(bhf) >= 2)
order by bhf !
* Umgehung : !
declare erg1 cursor for 
select bhf, count(bhf) cbhf from station group by bhf !
select * from station where bhf in
(select bhf from erg1 where cbhf > 1)
order by bhf !
rollback release !
