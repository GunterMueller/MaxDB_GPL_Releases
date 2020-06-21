*   *ID* N24SEL2  DBCMD    changed on 1992-09-28-13.25.42 by ADMIN     *
 *** <single select statement> wird getestet.
 *** SELECT /<distinct spec>/ <column expression>, ...
 *** INTO :a1,:a2, ...
 *** FROM tab WHERE ....
 *** Es existiert genau eine Ergebniszeile. !
file connect ( kern test !
select  spalte1
  into  :a1
  from  tab
  where (spalte1, spalte5) in ((user, user)) !
select  spalte5, spalte4
  into  :a1, :a2
  from  tab
  where spalte4 in ( -2, -1, 1, 2 ) !
select  spalte4  , spalte4
  into  :a1, :a2
  from  tab
  where spalte4 in ( -2, -1, 1, 2 ) !
select  spalte1, spalte2, spalte3, spalte4,
        spalte5, spalte6, spalte7, spalte8
  into  :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8
  from  tab
  where spalte3 < - 3 !
select  spalte1, spalte2, spalte3, spalte4,
        spalte5  ,spalte6,spalte7, spalte8, spalte3
  into  :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9
  from  tab
  where spalte3 = -3 and spalte8 is null !
select  substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4,substr(spalte5,1,18),
        substr(spalte1,1,8),spalte2,spalte3,spalte4
  into  :a1,:a2,:a3,:a4,:a5,:a6,:a7,:a8,:a9,:a10,
        :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19,:a20,
        :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29,:a30,
        :a31,:a32,:a33,:a34,:a35,:a36,:a37,:a38,:a39,:a40,
        :a41,:a42,:a43,:a44,:a45,:a46,:a47,:a48,:a49,:a50,
        :a51,:a52,:a53,:a54,:a55,:a56,:a57,:a58,:a59,:a60,
        :a61,:a62,:a63,:a64,:a65,:a66,:a67,:a68,:a69,:a70,
        :a71,:a72,:a73,:a74,:a75,:a76,:a77,:a78,:a79,:a80,
        :a81,:a82,:a83,:a84,:a85,:a86,:a87,:a88,:a89,:a90,
        :a91,:a92,:a93,:a94,:a95,:a96,:a97,:a98,:a99,:a100,
        :b1,:b2,:b3,:b4,:b5,
        :b6,:b7,:b8,:b9,:b10,
        :b11,:b12,:b13,:b14,:b15,
        :b16,:b17,:b18,:b19,:b20,
        :b21,:b22,:b23,:b24,:b25,
        :b26,:b27,:b28,:b29,:b30,
        :b31,:b32,:b33,:b34,:b35,
        :b36,:b37,:b38,:b39,:b40,
        :b41,:b42,:b43,:b44,:b45,
        :b46,:b47,:b48,:b49,:b50,
        :b51,:b52,:b53,:b54,:b55,
        :b56,:b57,:b58,:b59,:b60,
        :b61,:b62,:b63,:b64,:b65,
        :b66,:b67,:b68,:b69,:b70,
        :b71,:b72,:b73,:b74,:b75,
        :b76,:b77,:b78,:b79,:b80,
        :b81,:b82,:b83,:b84,:b85,
        :b86,:b87,:b88,:b89,:b90,
        :b91,:b92,:b93,:b94,:b95,
        :b96,:b97,:b98,:b99,:b100,
        :c1,:c2,:c3,:c4,:c5,
        :c6,:c7,:c8,:c9,:c10,
        :c11,:c12,:c13,:c14,:c15,
        :c16,:c17,:c18,:c19,:c20,
        :c21,:c22,:c23,:c24,:c25,
        :c26,:c27,:c28,:c29,:c30,
        :c31,:c32,:c33,:c34,:c35,
        :c36,:c37,:c38,:c39,:c40,
        :c41,:c42,:c43,:c44,:c45,
        :c46,:c47,:c48,:c49,:c50,
        :c51,:c52,:c53,:c54
  from  tab
  where spalte1 = 'HEINRICH' !
 *****************************************************!
select  tab.spalte5, spalte2, spalte4
  into  :a1, :a2, :a3
  from  tab
  where spalte2 = - 33 !
select  spalte5 , tab.spalte2, spalte4
  into  :a1, :a2, :a3
  from  tab
  where spalte2 = - 33 !
select  tab.spalte5, tab.spalte2, tab.spalte4
  into  :a1, :a2, :a3
  from  tab
  where spalte2 = - 33 !
 ****************************************************!
select  spalte3 neu3
  into  :a1
  from  tab
  where spalte1 = 'ILSE' !
select  tab.spalte3 neu3
  into  :a1
  from  tab
  where spalte1 = 'ILSE' !
select  spalte2 neu2, tab.spalte4 neu4,
        tab.spalte3 neu, spalte8 neu8,
        tab.spalte7 neu
  into  :a1 , :a2  ,  :a3,:a4 ,:a5
  from  tab
  where spalte1 =  'ILSE' !
select spalte4,  spalte8 neu8, tab.spalte5 ,
       tab.spalte6 neu6
  into  :a1, :a2,  :a3  ,:a4
  from  tab
  where spalte4  between 100 and 10000 !
select  spalte4 neu, spalte8 , tab.spalte5 neu5,
        tab.spalte6
  into  :a1, :a2, :a3,  :a4
  from  tab
  where spalte4 between 100 and 10000 !
 ****************************************************!
select distinct tab.spalte8 neu8, spalte5 , spalte7 neu7,
                spalte1, spalte1, tab.spalte2 neu2,
                tab.spalte2 neu2
  into :a1,:a2,:a3,:a4,:a5,:a6,:a7
  from tab
  where spalte4 = 0 !
select   distinct   spalte1
  into  :a1
  from tab
  where spalte1  like  'G_S_L_'
    and spalte7  in  ( 'HAMBURG', 'MUENCHEN' ) !
select   distinct   tab.spalte1
  into  :a1
  from tab
  where spalte1  like  'G_S_L_'
    and spalte7  in  ( 'HAMBURG', 'MUENCHEN' ) !
 *****************************************************!
select all tab.spalte3 neu3, spalte1
  into :a1,:a2
  from tab
  where (spalte3, spalte1) in ((spalte3, 'HANS')) !
select   all  tab.spalte3, spalte1 neu1
  into :a1,:a2
  from tab
  where  not (  spalte3 is null )
    and spalte1 = 'HANS' !
select all  spalte3, spalte1 neu1
  into :a1,:a2
  from tab
  where not (spalte3 is null)
    and spalte1 = 'HANS' !
 ***********************************************************!
select tab.spalte3
  into :a1
  from tab
  where spalte1 = 'GERHARD' !
select tab.spalte8
  into :a1
  from tab
  where spalte7 like 'HAM%'
    and spalte4 between 3 and 1E10 !
select tab.spalte5, tab.spalte4,
       tab.spalte1 neu1
  into :a1, :a2,:a3
  from tab
  where spalte10 is not null !
select  distinct  tab.spalte1, tab.spalte2 neu2
  into :a1, :a2
  from tab
  where spalte1 = user
    and spalte5 is null !
select all tab.spalte5, spalte1
  into :a1,:a2
  from tab
  where spalte1 = 'ELISABETH' !
commit work release !
