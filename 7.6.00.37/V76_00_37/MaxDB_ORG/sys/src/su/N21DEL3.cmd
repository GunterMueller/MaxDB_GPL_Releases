*   *ID* N21DEL3  DBCMD    changed on 1992-09-28-13.21.45 by ADMIN     *
 *** 'DELETE' wird getestet.
 *** Vor diesem File ist 'N21DEL1 VDNMAC' aufzurufen.
 ***********************************************************
 *** <between predicate> ***********************************
 ***********************************************************!
file connect ( kern test !
delete tab where spalte1 between 'I' and 'Z' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte2 between  0  and 299.99 !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete tab where spalte2 between -.021e+4 and 15768E-2 !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete from tab where spalte3 between -3  and  3  !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work !
delete from tab where spalte3 between -0.3E1 and +0.3e1 !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work !
delete  from  tab  where  spalte4  between  0  and 1E20 !
select substr(spalte1,1,20) spalte1, spalte2, spalte4 
from tab order by spalte4 !
fetch !
rollback work !
delete tab where spalte5 between 'HAMBURG' and 'HAMBURG' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete tab where spalte1 between 'X' and 'Z' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte6 between 'a' and 'z' !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete tab where spalte1 between ' ' and ' ' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte1 between '          ' and 'M' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte1 between '           ' and  'M' !
rollback work !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte1 between 'M' and '          ' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte1 between 'M' and '           ' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte1  between ' G'  and 'M' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte1 between 'G          ' and 'M' !
rollback work !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte1 between 'G' and 'HANS-JOACHIM' !
rollback work !
delete tab where spalte1 between 'M' and 'B' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte1 between 'OTTO' and 'SUPERDBAU' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte2 between 299.99 and 500.00 !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete tab where spalte2 between   300.00  and 500.00 !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete tab where spalte2 between  -500 and  +500 !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete tab where spalte2 between 200.001 and 290 !
delete tab where spalte2 between   1 and -1  !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete tab where spalte3 between  1  and  9  !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work !
delete tab where spalte3 between  4  and -4  !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work !
delete tab where spalte3 between  1  and 2.4 !
delete tab where spalte4 between 0.123456E5 and 2.3E12 !
delete tab where spalte4 between  1  and 1E64 !
select substr(spalte1,1,20) spalte1, spalte2, spalte4 
from tab order by spalte4 !
fetch !
rollback work !
delete tab where spalte4 between 'BERLIN' and 5E16 !
delete tab where spalte5 between 'BERLIN' and HAMBURG !
delete tab where spalte5 between null and 'BERLIN' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete tab where spalte7 between 'AUGSBURG' and 'ESSEN' !
select substr(spalte1,1,20) spalte1, spalte2, spalte7 
from tab order by spalte7 !
fetch !
rollback work !
delete tab where spalte5 between user and 'HANNOVER' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete tab where spalte5 between 'HANNOVER' and user !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete tab where spalte8 between '0' and 1E20 !
delete tab where spalte8 between  0  and 1E50 !
select substr(spalte1,1,20) spalte1, spalte2, spalte8 
from tab order by spalte8 !
fetch !
rollback work !
delete tab where spalte8 between  0  and NULL !
select substr(spalte1,1,20) spalte1, spalte2, spalte8 
from tab order by spalte8 !
fetch !
rollback work !
delete tab where spalte2 not between -200 and +200 !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete from tab where spalte6 not between  'a' and  'z' !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
 ***********************************************************
 *** <comparison predicate> ********************************
 ***********************************************************!
delete from tab where spalte1 = 'CHRISTINE'  !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work  !
delete  tab where spalte1     = 'GISELA'     !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work  !
delete from tab where spalte2=    48.12      !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work  !
delete from tab where spalte2=    4812.e-02 !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work  !
delete from tab where spalte3 =-3.0        !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work  !
delete from tab where spalte3 = -0.0000300e5 !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work  !
delete from tab where spalte4 =11.228E1    !
select substr(spalte1,1,20) spalte1, spalte2, spalte4 
from tab order by spalte4 !
fetch !
rollback work  !
delete from tab where spalte4 =  112.28    !
select substr(spalte1,1,20) spalte1, spalte2, spalte4 
from tab order by spalte4 !
fetch !
rollback work  !
delete from tab where spalte1 = spalte1 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete  tab where spalte5 =   spalte5  !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete from tab where spalte6 =spalte6 !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete from tab where spalte5= spalte7 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5, spalte7 from tab
                              order by spalte5, spalte7 !
fetch !
rollback work !
delete from tab where spalte2  = spalte3 !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 from tab
                              order by spalte2, spalte3 !
fetch !
rollback work !
delete   tab    where spalte4=spalte3 !
select substr(spalte1,1,20) spalte1, spalte2, spalte4, spalte3 
from tab
                              order by spalte4, spalte3 !
fetch !
rollback work !
delete from tab where spalte3 = spalte8 !
select substr(spalte1,1,20) spalte1, spalte2, spalte3, spalte8 from tab
                              order by spalte3, spalte8 !
fetch !
rollback work !
delete from tab  where  spalte4=  spalte8 !
select substr(spalte1,1,20) spalte1, spalte2, spalte4, spalte8 from tab
                              order by spalte4, spalte8 !
fetch !
rollback work !
delete from tab where spalte5= spalte6 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5, spalte6 from tab
                              order by spalte5, spalte6 !
fetch !
rollback work !
delete  tab where spalte7 = spalte5 !
select substr(spalte1,1,20) spalte1, spalte2, 
spalte7, substr(spalte5,1,18) spalte5 from tab
                              order by spalte7, spalte5 !
fetch !
rollback work !
delete  from tab where  spalte1 = 'HANS-JOACHIM' !
delete  from tab where  spalte1 = 'GISELA     '  !
rollback work !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete  from tab where  spalte1 =   ' GISELA'      !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete  tab where  spalte1 = spalte5 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1, substr(spalte5,1,18) spalte5 from tab
                               order by spalte1, spalte5 !
fetch !
rollback work !
delete  from tab where  spalte6 =  NULL !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete  from tab where  spalte8 =      1e50      !
select substr(spalte1,1,20) spalte1, spalte2, spalte8 
from tab order by spalte8 !
fetch !
rollback work !
delete     tab where spalte1   <> 'HANS'   !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work  !
delete from tab where spalte3 <>-05         !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work  !
delete from tab where spalte6<> 'VDN'       !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work  !
delete from tab where spalte2 <> spalte2 !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete from tab where spalte5<>spalte5 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete  tab  where    spalte6 <> spalte6 !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete from tab where spalte2 <> spalte3 !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 from tab
                            order by spalte2,spalte3  !
fetch !
rollback work !
delete from tab where spalte4  <>  spalte2 !
select substr(spalte1,1,20) spalte1, spalte2, spalte4 from tab
                          order by spalte4, spalte2 !
fetch !
rollback work !
delete    tab where spalte4 <>  spalte8 !
select substr(spalte1,1,20) spalte1, spalte2, spalte4, spalte8 from tab
                            order by spalte4, spalte8 !
fetch !
rollback work !
delete tab where spalte8  <> spalte3 !
select substr(spalte1,1,20) spalte1, spalte2, spalte8, spalte3 from tab
                             order by spalte8, spalte3 !
fetch !
rollback work !
delete from tab  where spalte5 <> spalte7 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5, spalte7 from tab
                            order by spalte5, spalte7 !
fetch !
rollback work !
delete from tab where spalte1 <>spalte6 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1, spalte6 from tab
                            order by spalte1, spalte6 !
fetch !
rollback work !
delete from tab where spalte5 <>   18   !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete from tab where spalte6 <> null   !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete from tab where spalte5 <> USER   !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete  tab    where spalte2<=  -33         !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work  !
delete from tab where spalte3  <=-5          !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work  !
delete from tab where   spalte6  <=  'z'    !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work  !
delete from tab where spalte2 <= spalte2 !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete from tab where spalte6<= spalte6 !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete from tab where spalte3 <= spalte8 !
select substr(spalte1,1,20) spalte1, spalte2, spalte3, spalte8 from tab
                            order by spalte3, spalte8 !
fetch !
rollback work !
delete    tab   where spalte3<=spalte2 !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 from tab
                            order by spalte3, spalte2 !
fetch !
rollback work !
delete from tab where  spalte4  <=  spalte8 !
select substr(spalte1,1,20) spalte1, spalte2, spalte4, spalte8 from tab
                            order by spalte4, spalte8 !
fetch !
rollback work !
delete tab  where spalte5 <= spalte6  !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5, spalte6 from tab
                            order by spalte5, spalte6 !
fetch !
rollback work !
delete from tab where spalte6  <= spalte5 !
select substr(spalte1,1,20) spalte1, spalte2, 
spalte6, substr(spalte5,1,18) spalte5 from tab
                            order by spalte6, spalte5 !
fetch !
rollback work !
delete from tab where spalte1  <= user    !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete   tab   where  spalte1 >= 'KARIN'     !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work  !
delete from tab where spalte4 >= 0.0         !
select substr(spalte1,1,20) spalte1, spalte2, spalte4 
from tab order by spalte4 !
fetch !
rollback work  !
delete from tab where spalte6 >= 'aaa'       !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work  !
delete from tab where spalte7 >= spalte5 !
select substr(spalte1,1,20) spalte1, spalte2, 
spalte7, substr(spalte5,1,18) spalte5 from tab
                            order by spalte7, spalte5 !
fetch !
rollback work !
delete tab where spalte1>=spalte7 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1, spalte7 from tab
                            order by spalte1, spalte7 !
fetch !
rollback work !
delete from tab  where  spalte2  >=  spalte8 !
select substr(spalte1,1,20) spalte1, spalte2, spalte8 from tab
                            order by spalte2, spalte8 !
fetch !
rollback work !
delete  tab where spalte6 >=spalte6  !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete  tab where spalte3 >= 5       !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work !
delete from tab where  spalte2  < -33.0      !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work  !
delete tab      where spalte5 <  'BERLIN'    !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work  !
delete from tab where   spalte6<'zzz'        !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work  !
delete from tab where spalte2 < spalte2 !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete from tab  where spalte6<spalte6 !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete from tab where spalte4 < spalte8 !
select substr(spalte1,1,20) spalte1, spalte2, spalte4, spalte8 from tab
                               order by spalte4, spalte8 !
fetch !
rollback work !
delete  tab  where spalte7  <  spalte5  !
select substr(spalte1,1,20) spalte1, spalte2, 
spalte7, substr(spalte5,1,18) spalte5 from tab
                               order by spalte7, spalte5 !
fetch !
rollback work !
delete  tab  where spalte1  <  user  !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete  tab  where spalte7  <  'AUGSBURG' !
select substr(spalte1,1,20) spalte1, spalte2, spalte7 
from tab order by spalte7 !
fetch !
rollback work !
delete  tab  where spalte7  >  'AUGSBURG' !
select substr(spalte1,1,20) spalte1, spalte2, spalte7 
from tab order by spalte7 !
fetch !
rollback work !
delete   tab    where spalte1 > 'MANFRED'    !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work  !
delete from tab where spalte5  >  'HAMBURG'  !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work  !
delete from tab where   spalte5>'MUENCHE'    !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work  !
delete  tab where   spalte6 >   'aaa'    !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work  !
delete  tab where   spalte3 >  5         !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work  !
delete from tab where spalte3 > spalte3 !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work !
delete  tab where spalte2>spalte3 !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 from tab
                     order by spalte2, spalte3 !
fetch !
rollback work !
delete    tab where   spalte4 >spalte3 !
select substr(spalte1,1,20) spalte1, spalte2, spalte4, spalte3 from tab
                          order by spalte4, spalte3 !
fetch !
rollback work !
delete from tab where spalte5>spalte6 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5, spalte6 from tab
                          order by spalte5, spalte6 !
fetch !
rollback work !
delete   tab  where  spalte5  >  spalte7 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5, spalte7 from tab
                          order by spalte5, spalte7 !
fetch !
rollback work !
 *****************************************************
 *** <in predicate> **********************************
 *****************************************************!
delete from tab where spalte1 in ( 'ELISABETH','GISELA') !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte2 in ( 0.12 , -88, +12.3, -00.27 ) !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete tab where spalte2
              in ( 12E-2, -88, 00.0001230E05, -.27e00 ) !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete from tab where spalte3 in (-5,-4,-3,-2,-1,0,1,2,3,4,5) !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work !
delete from tab where spalte3
              in (-3e0,-0.2E1,0e10,200e-02, + 300.E-02 ) !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work !
delete from tab where spalte5  in      ( 'KOELN' ) !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete   tab   where  spalte6 in ( 'aaa', 'bbb' ) !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete from tab where spalte8 in ( 9.9E8 ,  1  ) !
select substr(spalte1,1,20) spalte1, spalte2, spalte8 
from tab order by spalte8 !
fetch !
rollback work !
delete from tab where spalte2 in ( 299.99 , 300 ) !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete from tab where spalte3 in (     6       ) !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work !
delete from tab where spalte3 in ( 4.5 ) !
delete from tab where spalte3 in ( 'BERLIN' ) !
delete from tab where spalte3 in ( null )   !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work !
delete from tab where spalte6 in ( 8 ) !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete from tab where spalte7 in ( 'KOELN' ) !
select substr(spalte1,1,20) spalte1, spalte2, spalte7 
from tab order by spalte7 !
fetch !
rollback work !
delete from tab where spalte7 in ( null ) !
select substr(spalte1,1,20) spalte1, spalte2, spalte7 
from tab order by spalte7 !
fetch !
rollback work !
delete from tab where spalte7 in (   USER      ) !
select substr(spalte1,1,20) spalte1, spalte2, spalte7 
from tab order by spalte7 !
fetch !
rollback work !
delete from tab where spalte1 in ( 'OTTO' , user ) !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete from tab where spalte1 in ( user , 'PAUL' ) !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete from tab
    where spalte2 not in ( -123.17, -88.09,-37,-36, -33 ) !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete from tab
    where spalte5 not in ('BERLIN','HAMBURG','MUENCHEN') !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete  tab
     where spalte6  not  in  ( 'aaa' , '1234567890' ) !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
 ***********************************************************
 *** <like predicate> **************************************
 ***********************************************************!
delete from tab where spalte1  like  '%RI%' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete  tab where spalte5 like 'B%RLI_' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete from tab where spalte5 like '%K%' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete from tab where spalte6 like   '%'  !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete from tab where spalte1 like  5     !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete from tab where spalte1 not like '%A%' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete   tab    where  spalte6  not  like  'A%' !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
 ***********************************************************
 *** <null predicate> **************************************
 ***********************************************************!
delete    tab where spalte2  is  null !
select substr(spalte1,1,20) spalte1, spalte2 
from tab order by spalte2 !
fetch !
rollback work !
delete from tab where spalte4 is null !
select substr(spalte1,1,20) spalte1, spalte2, spalte4 
from tab order by spalte4 !
fetch !
rollback work !
delete  tab   where  spalte5  is null !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete from tab where spalte6 is null !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
delete  tab    where  spalte8 is null !
select substr(spalte1,1,20) spalte1, spalte2, spalte8 
from tab order by spalte8 !
fetch !
rollback work !
delete  tab    where  spalte5 is 6    !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete  tab    where  spalte5 is 'BERLIN' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete  tab    where  spalte5 is spalte7 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete from tab where spalte1 is not null !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete from tab where spalte3 is not null !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
from tab order by spalte3 !
fetch !
rollback work !
delete from tab where spalte4 is not null !
select substr(spalte1,1,20) spalte1, spalte2, spalte4 
from tab order by spalte4 !
fetch !
rollback work !
delete from tab where spalte6 is not null !
select substr(spalte1,1,20) spalte1, spalte2, spalte6 
from tab order by spalte6 !
fetch !
rollback work !
 ***********************************************************
 *** <sounds predicate> ************************************
 ***********************************************************!
delete from tab where spalte1 sounds 'GSL' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete  tab  where  spalte1   sounds   'GIS' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete  tab  where  spalte5 sounds 'BRL' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work !
delete  tab  where  spalte1 sounds  5  !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete  tab  where  spalte1 sounds spalte1 !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
rollback work !
delete tab where spalte5 not sounds 'HAMBURG' !
select substr(spalte1,1,20) spalte1, spalte2, 
substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
rollback work release !
