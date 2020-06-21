file connect ( kern test !
create table dbtypes(
dbtype        char(8) not null,
data_type     fixed (4) not null,
type_name     varchar(30) not null,
dbprecision   fixed (4),
dbextralength fixed (4),
dbscale       fixed (4),
dbradix       fixed (4)) !
insert into dbtypes values 
( 'CHAR',    1, 'CHAR',          NULL, 0, NULL, NULL) !
insert into dbtypes values 
( 'DATE',    9, 'DATE',            10, 2, NULL, NULL) !
insert into dbtypes values 
( 'FIXED',   3, 'DECIMAL',       NULL, 2,    0,   10)!
insert into dbtypes values 
( 'FLOAT',   6, 'FLOAT',           15, 0, NULL,   10)!
insert into dbtypes values 
( 'LONG',    -1, 'LONG VARCHAR', NULL, 0, NULL, NULL)!
insert into dbtypes values 
( 'TIME',    10, 'TIME',            8, 3, NULL, NULL)!
insert into dbtypes values 
( 'TIMESTMP',11, 'TIMESTAMP',      26, 6,    6, NULL)!
insert into dbtypes values 
( 'VARCHAR', 12, 'VARCHAR',      NULL, 0, NULL, NULL)!
create table columns ( owner         char(20),
                      tablename     char(20),
                      columnname    char(20),
                      len           fixed (6),
                      xxx           fixed (6),
                      mode          char (3),
                      datatype      char(8) not null,
                      ccomment      char (20))!         
insert into columns values
('SQLhugo', 'tab', 'col', 5, 3, 'OPT', 'CHAR', 'ohne worte') !
* das ueberfluessige (1=1) fuehrte in diesem 2/3-stufigen Befehl 
* abwechselnd zu 100 und kb stack type illegal, weil das Ende
* dieser Bedingung nicht erkannt, zwei Bedingungen zusammengefaszt
* wurden und dann hinten ein 'AND' uber war    !
select  null,
        col.owner, col.tablename,
        dbty.data_type, dbty.type_name,
        value (dbty.dbprecision, col.len),
        dbty.dbscale + col.xxx,
        dbty.dbradix,
        decode (col.mode, 'KEY', 0,
                          'OPT', 1,
                          'MAN', 0),
        col.ccomment

from    columns col,
        dbtypes dbty

where   (1=1)
and     col.OWNER like 'SQ%'
and     col.tablename like '%'
and     col.columnname like '%'
and     dbty.dbtype = col.datatype

order by col.owner, col.tablename !
fetch!
drop table dbtypes !
drop table columns !
commit release !
