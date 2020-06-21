*   *ID* N28SEL1  DBCMD    changed on 1992-09-28-13.25.53 by ADMIN     *
 *** <select expression> wird getestet. !
file connect ( kern test !
create table kern.tab1 ( spalte11 char(10) key ,
                           spalte12 fixed(5,2),
                           spalte13 float(5) ) !
insert tab1 values ( 'HANS'    , 22.52 , null ) !
insert tab1 values ( 'PAUL'    ,   0   ,  3E2 ) !
insert tab1 values ( 'JOACHIM' , 17.1  ,   0  ) !
 ***********************************************************!
create table kern.tab2 ( spalte21 fixed(4) not null,
                           spalte22 char(8) ,
                           spalte23 float(4) ) !
insert tab2 values ( 300   , 'HANS'   , null ) !
insert tab2 values ( 22.00 , 'FRIEDA' , 0    ) !
insert tab2 values ( 12    , 'JOACHIM', 0    ) !
 ***********************************************************!
create table kern.tab3 ( spalte31 fixed(3),
                           spalte32 char(10) ,
                           spalte33 fixed(3) ) !
insert tab3 values ( 22  , 'HANS'    , null ) !
insert tab3 values ( 300 , 'ANDREAS' , 0    ) !
insert tab3 values ( 300 , 'ANDREAS' , 0    ) !
insert tab3 values ( null, 'JOACHIM' , 300  ) !
commit work !
 ***********************************************************!
declare erg1 cursor for select * from tab1 !
fetch erg1 !
declare erg2 cursor for select * from tab2 !
fetch erg2 !
declare erg3 cursor for select * from tab3 !
fetch erg3 !
commit work release !
