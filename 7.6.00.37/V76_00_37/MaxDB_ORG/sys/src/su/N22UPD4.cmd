*   *ID* N22UPD4  DBCMD    changed on 1992-09-28-13.25.33 by ADMIN     *
file connect ( kern test !
update of tab set spalte3 = -1
   key spalte1 = 'KARIN', spalte2 = 256.54 !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 from tab !
fetch !
rollback work !
update of tab set spalte5 = 'BERLIN'
   key spalte1 = 'PAUL ', spalte2 =112.28  !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab
                            where spalte1 = 'PAUL' !
fetch !
rollback work !
update of tab set spalte3 = -5           ,
                  spalte8 = null         ,
                  spalte5 = user         ,
                  spalte4 = 34.3         ,
                  spalte6 = 'BERLIN'     ,
                  spalte7 = 'MUENCHEN'
   key spalte1 = 'CHRISTINE', spalte2 = - 0.02 !
select substr(spalte1,1,20) spalte1, spalte2, spalte3, spalte8, 
substr(spalte5,1,18) spalte5 from tab !
fetch !
select substr(spalte1,1,20) spalte1, spalte2, spalte4, spalte6, 
spalte7 from tab !
fetch !
rollback work !
update  tab set spalte4 = 50.04e12 , spalte5 = 'HAMBURG'
   key spalte1 = 'BERND', spalte2 = 48.12  !
select substr(spalte1,1,20) spalte1, spalte2, spalte4, 
substr(spalte5,1,18) spalte5 from tab
                                  where spalte1 = 'BERND' !
fetch !
rollback work !
update of tab set spalte3 = -0,  spalte6 = substr(USER, 1, 8)
   key spalte1 = 'PAUL ', spalte2 =112.28  !
select substr(spalte1,1,20) spalte1, spalte2, spalte3, spalte6 from tab
                           where spalte1 = 'PAUL' !
fetch !
rollback work !
update of tab set spalte4 = spalte8 , spalte8 = spalte4
   key spalte1 = 'PAUL' , spalte2 = 112.28 !
select substr(spalte1,1,20) spalte1, spalte2, spalte4, spalte8 from tab
                                  where spalte1 = 'PAUL' !
fetch !
rollback work !
update of tab set spalte3 =spalte4  ,
                  spalte4 = spalte2 ,
                  spalte5= spalte6  ,
                  spalte6 = spalte7 ,
                  spalte7 = spalte5
   key spalte1 = 'ELISABETH' , spalte2 = - 36 !
update of tab set spalte8 = spalte2  ,
                   spalte7=spalte5   ,
                  spalte5 = 'HAMELN' ,
                  spalte6 = spalte5
   key spalte1 = 'KLAUS', spalte2 = 3.00   !
update of tab set spalte5 = user , spalte6 = spalte5
   key spalte1 = 'KARIN', spalte2 = 33 !
update of tab set spalte6 = spalte5, spalte5 =USER
   key spalte1 = 'PABLO', spalte2 = 112.280 !
update of tab set spalte8 = -32.10e12
   key spalte1 = 'HANNELORE', spalte2 = 0.123e2 !
update of tab set spalte5 = 'HAMELN'
   key spalte2 = 0.12  , spalte1 = 'FRIEDA' !
select substr(spalte1,1,20) spalte1, spalte2 , 
substr(spalte5,1,18) spalte5 from tab !
fetch !
update tab set  spalte7 = 'HAMELN'
   key spalte1 = 'KARIN', spalte2 = 299.99  !
update of tab set spalte2 = 100
   key spalte1 = 'HANS '    , spalte2 = 123.17  !
update of tab set spalte9 = null
   key spalte1 = 'HILDE    ', spalte2 = -3      !
update of tab set spalte3 = null
   key spalte1 = 'HEINRICH' !
update of tab set spalte3 = 0 , spalte3 = 5
   key spalte1 = 'HANS'     , spalte2 = -58.76  !
update of tab set spalte2 = spalte3
   key spalte1 = 'KLAUS' ,    spalte2 = 3 !
rollback work release !
