*   *ID* N22UPD1  DBCMD    changed on 1992-09-28-13.25.31 by ADMIN     *
file connect ( kern test !
create table tab
   (spalte1 char(64)          key                                       ,
    spalte2 fixed(5,2) key      range between -299.99 and +299.99       ,
    spalte3 fixed(1)            range in (-5,-4,-3,-2,-1,0,1,2,3,4,5)   ,
    spalte4 float(5)   not null                                         ,
    spalte5 char(64)                                                    ,
    spalte6 char(10)                                                    ,
    spalte7 char(12)            range in ('BERLIN','HAMBURG','MUENCHEN'),
    spalte8 float(3)            range between -1E30 and +1E30 ) !
insert tab ( spalte1 , spalte2 , spalte4 , spalte8 )
    values ( 'HANS'  , -123.17 , 22E5    ,    1    ) !
insert into tab set spalte1 = 'PAUL'     ,
                    spalte2 = 112.28     ,
                    spalte4 = - 0.22E-12 ,
                    spalte5 = 'BERLIN'   ,
                    spalte7 = 'HAMBURG'  ,
                    spalte8 = 9.9E8      !
insert  tab  set spalte1 = 'FRIEDA'   ,
                 spalte2 = .12        ,
                 spalte3 = -3         ,
                 spalte4 = 1.E13      ,
                 spalte7 = 'HAMBURG'  ,
                 spalte8 = -8.e-04    !
insert tab values (user,0,null,0,null,NULL,nuLL,null  ) !
insert tab values
    ('LOTTE',+299.99,3,123.4E42,'HAMBURG',null,'HAMBURG',-3E1) !
insert tab ( spalte1   , spalte2 , spalte3 , spalte8 , spalte4 )
    values( 'JUERGEN' ,- 88.09 ,  null   ,   -3    ,  29.2   ) !
insert into tab values
    ('JOACHIM',012.3,null,-00.8e-3,'MUENCHEN',null,'BERLIN',-.8e-3) !
insert tab (spalte1 ,spalte2,spalte3,spalte4 ,spalte5,spalte7   ,spalte8)
    values ('GISELA',-33.000,   5   ,+.99e+09, null  ,'MUENCHEN', 99e9 )!
insert into
       tab ( spalte2 , spalte8, spalte1 , spalte4 , spalte5, spalte7 )
    values ( 256.54  , 120e-1 , 'KARIN' , 3343e26 , null   ,  null   ) !
insert tab values ( user,7.070,null,+3,'BERLIN',null, null, .0050e+3 ) !
insert into tab set spalte3 = - 1      ,
                    spalte1 =  'HANS'  ,
                    spalte2 = 123.17   ,
                    spalte5 = null     ,
                    spalte4 = 29.2     ,
                    spalte7 = 'BERLIN' ,
                    spalte8 = 1E00     !
insert into tab set spalte8 = 12.3       ,
                    spalte1 = 'HANS'     ,
                    spalte2 = -58.76     ,
                    spalte4 = 0.0034e-26 ,
                    spalte5 = 'HAMBURG'  !
insert into tab set spalte1 = 'PABLO'  ,
                    spalte2 = 299.99   ,
                    spalte3 = 3        ,
                    spalte4 = -80e-05  ,
                    spalte8 = -300.E-2 !
insert tab set spalte2 = - .04       ,
               spalte3 = 4           ,
               spalte5 = 'BERLIN'    ,
               spalte1 = 'CHRISTINE' ,
               spalte4 = - 2.2e-13   ,
               spalte7 = 'BERLIN'    !
insert tab values
    ('BERND',+48.120,-3,12345.,'BERLIN',null,'HAMBURG',0.00292e04 ) !
insert tab values
    ( 'ELISABETH',-36,5,1.0,'MUENCHEN',null,'HAMBURG',88.8 ) !
insert tab ( spalte1     , spalte2 , spalte4 , spalte8 )
    values ( 'HANNELORE' , 12.30   , 58.e36  ,   -3    ) !
insert into tab ( spalte1 , spalte5 , spalte4 , spalte8 , spalte2 )
       values   ( 'RITA'  ,  user   , 34.e-30 , +1.230e3,    4    ) !
insert tab (spalte3,spalte4  ,spalte5,spalte2,spalte1 ,spalte7 ,spalte8)
    values (   5   ,0.099E+10, null  , -55.  ,'GISELA','BERLIN',12.300 )!
insert tab ( spalte7 , spalte5 , spalte4 , spalte3 , spalte2 , spalte1 )
    values ('HAMBURG','HAMBURG',1234E+41 ,    3    , 256.54  , 'KARL'  )!
insert into tab values
    ( 'ILSE',112.28,-3,+.22E+7,'MUENCHEN',null,'BERLIN',null ) !
insert into tab set spalte8 = 5          ,
                    spalte7 = 'MUENCHEN' ,
                    spalte5 = 'BERLIN'   ,
                    spalte4 = 3343E26    ,
                    spalte3 = -4         ,
                    spalte2 = 33         ,
                    spalte1 = 'KARIN'    !
insert into tab set spalte1 = 'HEINRICH' ,
                    spalte2 = -3         ,
                    spalte3 = -3         ,
                    spalte4 = -3         ,
                    spalte5 = null       ,
                    spalte7 = 'MUENCHEN' ,
                    spalte8 = 2.2E06     !
insert into tab ( spalte4 , spalte1 , spalte2 , spalte7 , spalte8 )
         values (  7E13   ,'GERHARD',    4    ,'BERLIN' ,    1 ) !
insert tab set spalte5 = USER     ,
               spalte1 = user     ,
               spalte2 = -  88.80 ,
               spalte4 = 220E4    ,
               spalte7 = 'BERLIN' !
insert tab values
     ( 'HILDE' , -3 , 5 , 4 , 'BERLIN' , null , 'BERLIN' , -3 ) !
insert tab ( spalte1  , spalte2 , spalte3 , spalte4 ,  spalte8  )
    values ( 'INGRID' ,  22.0   ,    5    ,  5E01   , + .012E+1 ) !
insert tab ( spalte1 , spalte2 , spalte3 , spalte4 , spalte8 )
    values ( 'KLAUS' ,    3    ,   -1    ,  112.28 ,    3    ) !
select substr(spalte1,1,20) spalte1, spalte2, spalte3, spalte4, 
substr(spalte5,1,18) spalte5 from tab
                                             order by spalte1 !
fetch !
select substr(spalte1,1,20) spalte1, spalte2, spalte6, 
spalte7, spalte8 from tab
                                             order by spalte1 !
fetch !
commit work release !
