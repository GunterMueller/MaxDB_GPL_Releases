*   *ID* N45INIT  DBCMD    changed on 1992-09-28-13.26.35 by ADMIN     *
file connect ( kern test !
create table tab1 ( spalte1 char(10) key,
                    spalte2 char(10) ascii,
                    spalte3 char(10),
                    spalte4 char(10) ) !
insert tab1 values ( 'Hans'   , 'Paul'    , 'Moelln'    , 'Bonn'      ) !
insert tab1 values ( 'Karin'  , null      , 'Paderborn' , 'Bonn'      ) !
insert tab1 values ( 'Paul'   , 'Ilse'    , 'Hamburg'   , 'Berlin'    ) !
insert tab1 values ( 'Otto'   , null      , null        , 'Hamburg'   ) !
insert tab1 values ( 'Bernd'  , null      , null        , null        ) !
insert tab1 values ( 'Lisa'   , null      , 'Moelln'    , null        ) !
insert tab1 values ( 'Holger' , 'Paul'    , 'Moelln'    , 'Bonn'      ) !
insert tab1 values ( 'Karl'   , 'Otto'    , 'Bonn'      , 'Karlsruhe' ) !
insert tab1 values ( 'Heinz'  , 'Paul'    , 'Hannover'  , 'Helmstedt' ) !
insert tab1 values ( 'Ilse'   , 'Juergen' , 'Braunlage' , 'Koeln'     ) !
insert tab1 values ( 'Paula'  , 'Otto'    , 'Bonn'      , 'Essen'     ) !
select * from tab1 !
fetch !
commit work !
