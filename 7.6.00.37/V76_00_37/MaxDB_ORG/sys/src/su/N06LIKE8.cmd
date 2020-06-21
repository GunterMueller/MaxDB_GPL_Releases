*   *ID* N06LIKE8 DBCMD    changed on 1992-09-28-13.25.11 by ADMIN     *
 *** 'LIKE' wird getestet.
 %%% Falsche 'LIKE'. !
create table datei     (name    CHAR(10)   ASCII key,
                        vorname CHAR(10)   ASCII  ,
                        ort     CHAR(8)           ,
                        gehalt  FIXED(6,2)        ,
                        code    CHAR(4)  BYTE     ,
                        zahl    FLOAT(12))      !
insert into datei set name    = 'Mueller',
                      vorname = 'Armin'  ,
                      ort     = 'Berlin' ,
                      gehalt  = 2125.75  ,
                      code    = X'C1F1F0C6' ,
                      zahl    = 7.8E+3  !
commit work !
select name   from datei where name   like 'Armin'    !
fetch !
select ort    from datei where ort    like 'Frankfurt'!
fetch !
select ort    from datei where ort    like  Berlin    !
fetch !
select gehalt from datei where gehalt like '2125.75'  !
fetch !
select gehalt from datei where gehalt like  2125.75   !
fetch !
select code   from datei where code   like X'C1F1F0C6' !
fetch !
select zahl   from datei where zahl   like '7.8E+3'   !
fetch !
