*   *ID* N21DEL2  DBCMD    changed on 1992-09-28-13.21.44 by ADMIN     *
 *** 'DELETE' wird getestet.
 *** Vor diesem File ist 'N21DEL1 VDNMAC' aufzurufen.
 *** Dieses File dient zur Kontrolle von 'N21DEL1 VDNMAC'. !
file connect ( kern test !
select * from tab !
fetch !
select substr(spalte1,1,20) spalte1, spalte2, 
        substr(spalte1,1,20) spalte1_1 from tab order by spalte1 !
fetch !
select substr(spalte1,1,20) spalte1, spalte2 
        from tab order by spalte2 !
fetch !
select substr(spalte1,1,20) spalte1, spalte2, spalte3 
        from tab order by spalte3 !
fetch !
select substr(spalte1,1,20) spalte1, spalte2, spalte4 
        from tab order by spalte4 !
fetch !
select substr(spalte1,1,20) spalte1, spalte2, 
        substr(spalte5,1,18) spalte5 from tab order by spalte5 !
fetch !
select substr(spalte1,1,20) spalte1, spalte2, spalte7 
        from tab order by spalte7 !
fetch !
select substr(spalte1,1,20) spalte1, spalte2, spalte8 
        from tab order by spalte8 !
fetch !
rollback work release !
