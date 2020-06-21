*   *ID* N06LIKE7 DBCMD    changed on 1992-09-28-13.25.11 by ADMIN     *
 *** 'LIKE' wird getestet.
 %%% Falsche 'LIKE' wegen NULL-Wert. !
create table person  ( nachname char(4) key,
                         vorname  char(4) ,
                         nul      char(4) ) !
insert into person set nachname = 'NULL' ,
                      vorname  =  NULL    !
commit work !
select nachname, vorname, nul  from person
                      where nachname like  NULL  !
fetch !
select nachname, vorname, nul  from person
                      where vorname  like  NULL  !
fetch !
select nachname, vorname, nul  from person
                      where vorname  like 'NULL' !
fetch !
select nachname, vorname, nul  from person
                      where nul      like  NULL  !
fetch !
