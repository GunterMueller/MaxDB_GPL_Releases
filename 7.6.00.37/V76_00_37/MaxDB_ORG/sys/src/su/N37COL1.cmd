*   *ID* N37COL1  DBCMD    changed on 1992-09-28-13.26.26 by ADMIN     *
file connect ( kern test !
create table tab ( spalte1  char(5)  ,
                   spalte2  char(6)  ,
                   spalte3  char(65) ,
                   spalte4  char(5)  ascii,
                   spalte5  char(6)  ascii,
                   spalte6  char(65) ascii,
                   spalte7  char(5)  byte,
                   spalte8  char(6)  byte,
                   spalte9  char(65) byte,
                   spalte14 date,
                   spalte15 time,
                   spalte16 fixed(10),
                   spalte17 float(10),
                   spalte18 integer,
                   spalte19 smallint,
                   spalte20 decimal(5,2),
                   spalte21 float,
                   spalte22 varchar(8) ) !
file sel_cols_1tab ( KERN TAB !
commit work !
insert tab set spalte1  = date !
insert tab set spalte2  = date !
insert tab set spalte3  = date !
insert tab set spalte4  = date !
insert tab set spalte5  = date !
insert tab set spalte6  = date !
insert tab set spalte7  = date !
insert tab set spalte8  = date !
insert tab set spalte9  = date !
insert tab set spalte14 = date !
insert tab set spalte15 = date !
insert tab set spalte16 = date !
insert tab set spalte17 = date !
insert tab set spalte1  = time !
insert tab set spalte2  = time !
insert tab set spalte3  = time !
insert tab set spalte4  = time !
insert tab set spalte5  = time !
insert tab set spalte6  = time !
insert tab set spalte7  = time !
insert tab set spalte8  = time !
insert tab set spalte9  = time !
insert tab set spalte14 = time !
insert tab set spalte15 = time !
insert tab set spalte16 = time !
insert tab set spalte17 = time !
insert tab set spalte1  = user !
insert tab set spalte2  = user !
insert tab set spalte3  = user !
insert tab set spalte4  = user !
insert tab set spalte5  = user !
insert tab set spalte6  = user !
insert tab set spalte7  = user !
insert tab set spalte8  = user !
insert tab set spalte9  = user !
insert tab set spalte14 = user !
insert tab set spalte15 = user !
insert tab set spalte16 = user !
insert tab set spalte17 = user !
insert tab set spalte22 = user !
insert tab set spalte1  = '121011' !
insert tab set spalte2  = '121011' !
insert tab set spalte3  = '121011' !
insert tab set spalte4  = '121011' !
insert tab set spalte5  = '121011' !
insert tab set spalte6  = '121011' !
insert tab set spalte7  = '121011' !
insert tab set spalte8  = '121011' !
insert tab set spalte9  = '121011' !
insert tab set spalte14 = '19121011' !
insert tab set spalte15 = '121011' !
insert tab set spalte16 = '121011' !
insert tab set spalte17 = '121011' !
insert tab set spalte22 = '121011' !
insert tab set spalte1  = X'4E4E4E4E4E4E' !
insert tab set spalte2  = X'4E4E4E4E4E4E' !
insert tab set spalte3  = X'4E4E4E4E4E4E' !
insert tab set spalte4  = X'4E4E4E4E4E4E' !
insert tab set spalte5  = X'4E4E4E4E4E4E' !
insert tab set spalte6  = X'4E4E4E4E4E4E' !
insert tab set spalte7  = X'4E4E4E4E4E4E' !
insert tab set spalte8  = X'4E4E4E4E4E4E' !
insert tab set spalte9  = X'4E4E4E4E4E4E' !
insert tab set spalte14 = X'4E4E4E4E4E4E4E4E' !
insert tab set spalte15 = X'4E4E4E4E4E4E' !
insert tab set spalte16 = X'4E4E4E4E4E4E' !
insert tab set spalte17 = X'4E4E4E4E4E4E' !
insert tab set spalte22 = X'4E4E4E4E4E4E' !
insert tab set spalte1  = 22 !
insert tab set spalte6  = 22 !
insert tab set spalte9  = 22 !
insert tab set spalte14 = 22 !
insert tab set spalte15 = 22 !
insert tab set spalte16 = 22 !
insert tab set spalte17 = 22 !
insert tab set spalte22 = 22 !
insert tab set spalte14 = '19651132' !
insert tab set spalte14 = '99871318' !
insert tab set spalte14 = '00011218' !
insert tab set spalte14 = '19230018' !
insert tab set spalte14 = '19450700' !
insert tab set spalte15 = '250728' !
insert tab set spalte15 = '240728' !
insert tab set spalte15 = '230761' !
insert tab set spalte15 = '230760' !
insert tab set spalte15 = '000000' !
insert tab set spalte15 = '056123' !
insert tab set spalte15 = '156059' !
create table testtab (a dbyte (8))!
IF &rc = 0 THEN BEGIN
drop table testtab !
insert tab set spalte1  = G'4E4E4E4E4E4E' !
insert tab set spalte2  = G'4E4E4E4E4E4E' !
insert tab set spalte3  = G'4E4E4E4E4E4E' !
insert tab set spalte4  = G'4E4E4E4E4E4E' !
insert tab set spalte5  = G'4E4E4E4E4E4E' !
insert tab set spalte6  = G'4E4E4E4E4E4E' !
insert tab set spalte7  = G'4E4E4E4E4E4E' !
insert tab set spalte8  = G'4E4E4E4E4E4E' !
insert tab set spalte9  = G'4E4E4E4E4E4E' !
insert tab set spalte14 = G'4E4E4E4E4E4E' !
insert tab set spalte15 = G'4E4E4E4E4E4E' !
insert tab set spalte16 = G'4E4E4E4E4E4E' !
insert tab set spalte17 = G'4E4E4E4E4E4E' !
insert tab set spalte22 = G'4E4E4E4E4E4E' !
END
commit work !
select * from tab !
fetch !
commit work !
