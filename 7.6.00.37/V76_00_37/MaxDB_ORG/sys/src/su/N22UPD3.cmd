*   *ID* N22UPD3  DBCMD    changed on 1992-09-28-13.25.32 by ADMIN     *
file connect ( kern test !
update of tab set spalte3 = 0.0, spalte5 = 'HAMELN' !
select substr(spalte1,1,20) spalte1, spalte2, spalte3, 
substr(spalte5,1,18) spalte5 from tab !
fetch !
rollback work !
update  tab set spalte8=72.e-28 , spalte7 = 'BERLIN' !
select substr(spalte1,1,20) spalte1, spalte2, spalte8, spalte7  from tab !
fetch !
rollback work !
update of  tab set spalte4 =  0.012345      ,
                   spalte6 = 'ESSEN'        ,
                   spalte3 = null           ,
                   spalte5 =  user          ,
                   spalte7 = 'BERLIN'       ,
                   spalte8 =1               !
select substr(spalte1,1,20) spalte1, spalte2, spalte4, 
spalte6, spalte3 from tab !
fetch !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5, spalte7, spalte8 from tab !
fetch !
rollback work !
update of tab set spalte5 = spalte7, spalte7 = spalte6 !
update of tab set spalte5 = spalte7, spalte7 = spalte5 !
update tab set spalte3 = 5, spalte7 =null !
select substr(spalte1,1,20) spalte1, spalte2, spalte3, spalte7 from tab !
fetch !
update tab set spalte5 = 'BERLIN'  , spalte4 = null !
update   tab  set spalte8=5,spalte2 = - 100 !
rollback work release !
