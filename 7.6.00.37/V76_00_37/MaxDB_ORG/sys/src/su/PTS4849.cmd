* PTS4849  ( 6.1.(PTS4848))
connect kern test !

CREATE TABLE "WAGGONS"            (
"WAGGON_NUMMER" CHAR(20) ASCII ,
"HOLZNR" INTEGER 
DEFAULT 0,
"BAHNHOFAB" TIMESTAMP ,
"VERMESSEN" TIMESTAMP ,
"STEINHORST" TIMESTAMP ,
"BOX" TIMESTAMP ,
"BHFADELEBSENAN" TIMESTAMP ,
"WEGENER" TIMESTAMP ,
"BEMERKUNG" CHAR(50) ASCII ,
"BAHNHOFSNR" INTEGER 
DEFAULT 99999,
"CAFM" FLOAT(8) 
DEFAULT 0.0000000E+00,
"ERLEDIGT" SMALLINT 
DEFAULT 0,
"FUHRMANN_NR" INTEGER 
DEFAULT 99999,
"BAHNHOF" CHAR(40) ASCII ,
"WAGGONSNR" CHAR(12) ASCII ,
"LOESCHZEIT" TIMESTAMP ,
"LKW" FIXED(5,0) 
DEFAULT 0
, PRIMARY KEY ("WAGGONSNR"
)) !

insert into waggons set waggon_nummer = '1', waggonsnr = '1' !
insert into waggons set waggon_nummer = '2', waggonsnr = '2' !
insert into waggons set waggon_nummer = '3', waggonsnr = '3' !

CREATE TABLE "PUFFER"             (
"STUECK" SMALLINT 
DEFAULT 0,
"LOESCHZEIT" TIMESTAMP ,
"AUFTRAGSNR" CHAR(12) ASCII ,
"WAGGONSNR" CHAR(12) ASCII ,
"PUFFERNR" CHAR(12) ASCII 
, PRIMARY KEY ("PUFFERNR"
)) !

insert into puffer set Puffernr = '2', waggonsnr = '2' !
insert into puffer set Puffernr = '3' !
insert into puffer set Puffernr = '4' !

commit !

explain select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<waggons,puffer> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !

select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<waggons, puffer> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !
fetch !

explain select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<puffer, waggons> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !

select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<puffer, waggons> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !
fetch !

CREATE INDEX        "WAGGONSNR"          ON "PUFFER"            
("WAGGONSNR" ASC ) !

explain select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<waggons,puffer> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !

select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<waggons, puffer> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !
fetch !

explain select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<puffer, waggons> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !

select waggons.WAGGONSNR,PUFFER.WAGGONSNR,
PUFFER.PUFFERNR from 
<puffer, waggons> where
waggons.WAGGONSNR = PUFFER.WAGGONSNR(+)
AND puffer.WAGGONSNR is null !
fetch !

drop table waggons !
drop table puffer !
commit work release !
