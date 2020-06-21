*   *ID* N22UPD2  DBCMD    changed on 1992-09-28-13.25.32 by ADMIN     *
file connect ( kern test !
update of tab set spalte1 = 'MARLIS'   !
select substr(spalte1,1,20) spalte1, spalte2 , 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
update of tab set spalte1 = null !
update  of  tab set  spalte1 = user !
update  tab set spalte2 = 5.5 !
rollback work !
update of tab set spalte2 = NULL !
update of  tab  set  spalte2  =  USER !
update of tab set spalte3 = 1.0 !
select substr(spalte1,1,20) spalte1, spalte2  , spalte3 from tab !
fetch !
rollback work !
update  of  tab  set  spalte3  =  3.   !
select substr(spalte1,1,20) spalte1, spalte2  , spalte3 from tab !
fetch !
rollback work !
update   tab set spalte3 = 3.5 !
select * from tab where rowno <= 3 !
fetch!
rollback !
update of tab set spalte3= 6   !
update of tab set spalte3 =  spalte3 !
select substr(spalte1,1,20) spalte1, spalte2  , spalte3 
from tab order  by spalte3  !
fetch !
rollback work !
update of tab set spalte3 =  -spalte3 !
select substr(spalte1,1,20) spalte1, spalte2  , spalte3 
from tab order  by spalte3  !
fetch !
rollback work !
update of tab set spalte3 =spalte2 !
update tab set spalte3=spalte4  !
update  tab    set spalte3 =  spalte5 !
update of tab set spalte3 = null !
select substr(spalte1,1,20) spalte1, spalte2  , spalte3 
from tab order  by spalte3  !
fetch !
rollback work !
update of tab set spalte3 = user!
update of tab set spalte4 = - 77.e-19 !
select substr(spalte1,1,20) spalte1, spalte2  , spalte4 from tab  !
fetch !
rollback work !
update    tab   set spalte4 =1E-65 !
select substr(spalte1,1,20) spalte1, spalte2  , spalte4 
from tab order  by spalte4  !
fetch !
rollback work !
update of tab set   spalte4='BERLIN' !
select substr(spalte1,1,20) spalte1, spalte2  , spalte4 
from tab order  by spalte4  !
fetch !
rollback work !
update  tab set spalte4 =   spalte8 !
update of tab   set spalte4=spalte2 !
select substr(spalte1,1,20) spalte1,  spalte2 , spalte4 
from tab order by spalte2 !
fetch !
update of tab set spalte4 = null     !
update of tab set spalte4 = USER !
update  tab set spalte5 = 82.01 !
update  tab set spalte5 = +spalte5 !
select substr(spalte1,1,20) spalte1, spalte2  , 
substr(spalte5,1,18) spalte5 from tab order  by spalte5  !
fetch !
rollback work !
update of tab set spalte5 = 'KOELN' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab !
fetch !
rollback work !
update of tab set spalte5 =   'BERLIN'  !
select substr(spalte1,1,20) spalte1, spalte2  , 
substr(spalte5,1,18) spalte5 from tab order  by spalte5  !
fetch !
rollback work !
update of tab set spalte5 =
'GARMISCH PARTENKIRCHEN ist sehr ein bekannter Ort, in dem Wintersport'!
update    tab set  spalte5 = '  HAMELN'  !
select substr(spalte1,1,20) spalte1, spalte2  , 
substr(spalte5,1,18) spalte5 from tab order  by spalte5  !
fetch !
rollback work !
update of tab  set spalte5 = 
'                                                           HAMELN'  !
select substr(spalte1,1,20) spalte1, spalte2  , 
substr(spalte5,1,18) spalte5 from tab order  by spalte5  !
fetch !
rollback work !
update of tab set spalte5 =
'                                                          HAMELN '  !
select substr(spalte1,1,20) spalte1, spalte2  , 
substr(spalte5,1,18) spalte5 from tab order  by spalte5  !
fetch !
rollback work !
update of tab set spalte5 =null !
select substr(spalte1,1,20) spalte1, spalte2  , 
substr(spalte5,1,18) spalte5 from tab order  by spalte5  !
fetch !
rollback work !
update of tab set spalte5=   user        !
select substr(spalte1,1,20) spalte1, spalte2  , 
substr(spalte5,1,18) spalte5 from tab !
fetch !
rollback work !
update of tab set spalte6 = 'FRANKREICH'  !
select substr(spalte1,1,20) spalte1, spalte2  , spalte6 from tab !
fetch !
rollback work !
update of tab set spalte6 =spalte1  !
select substr(spalte1,1,20) spalte1, spalte6 from tab !
fetch !
update  tab set spalte6 = substr(user, 1, 9) !
select substr(spalte1,1,20) spalte1, spalte2  , 
spalte6 from tab order  by spalte6  !
fetch !
rollback work !
update of tab set spalte7 ='KOELN' !
update of tab set spalte7 ='MUENCHEN' !
select substr(spalte1,1,20) spalte1, spalte2  , spalte7 from tab  !
fetch !
rollback work !
update of tab set spalte7 =  spalte5 !
select substr(spalte1,1,20) spalte1, spalte2, spalte7, 
substr(spalte5,1,18) spalte5 from tab !
fetch !
update of tab set spalte7=spalte6 !
update of tab set spalte7 =  spalte8 !
update  tab  set  spalte7 =  NULL !
select substr(spalte1,1,20) spalte1, spalte2 , spalte7 from tab  !
fetch !
rollback work !
update of tab set spalte7 = substr(USER,1 ,9) !
update of tab set spalte8 = 1E30 !
select substr(spalte1,1,20) spalte1, spalte2  , spalte8 from tab  !
fetch !
rollback work !
update tab set spalte8 = - 1E30  !
select substr(spalte1,1,20) spalte1, spalte2  , spalte8 from tab !
fetch !
rollback work !
update of tab set spalte8  = 2e30 !
update of tab set spalte8 = spalte7  !
update   tab set spalte8 =spalte4 !
rollback work release !
