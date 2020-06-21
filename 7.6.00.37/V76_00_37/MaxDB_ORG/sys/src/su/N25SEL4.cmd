*   *ID* N25SEL4  DBCMD    changed on 1992-09-28-13.25.47 by ADMIN     *
 *** <single select statement> wird getestet.
 *** viele Tabellen und viele JOINS. !
file connect ( kern test !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref22.spalte21 = ref32.spalte31
       and ref21.spalte21 = ref32.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref23.spalte21 = ref24.spalte21 !
select * into :a1,:a2,:a3,:a4,:a5,:a6
   from tab1, tab3
   where tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31 !
select distinct * into :a1,:a2,:a3,:a4,:a5,:a6
   from tab1, tab3
   where tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31
     and tab1.spalte13 = tab3.spalte31 !
select all  * into :a1,:a2,:a3,:a4,:a5,:a6
   from tab1, tab2
   where tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21
     and tab1.spalte13 = tab2.spalte21 !
select distinct * into :a1, :a2, :a3, :a4, :a5, :a6
   from tab1, tab3
   where tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33
     and tab1.spalte13 = tab3.spalte33 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref22.spalte21 = ref32.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref23.spalte21 = ref24.spalte21 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref12.spalte13 < ref11.spalte13
       and ref22.spalte21 = ref32.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref13.spalte13 < ref33.spalte31
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref14.spalte13 < ref33.spalte31
       and ref23.spalte21 = ref24.spalte21 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
                                   tab3 ref33
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref22.spalte21 = ref32.spalte31
       and ref31.spalte31 < ref33.spalte31
       and ref32.spalte31 = ref33.spalte31 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref22.spalte21 = ref32.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref14.spalte13 < ref32.spalte31
       and ref23.spalte21 = ref24.spalte21 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref12.spalte13 < ref31.spalte31
       and ref22.spalte21 = ref32.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref14.spalte13 < ref32.spalte31
       and ref23.spalte21 = ref24.spalte21 !
select * into :a1 ,:a2 ,:a3 ,:a4 ,:a5 ,:a6 ,:a7 ,:a8 ,:a9 , :a10,
              :a11,:a12,:a13,:a14,:a15,:a16,:a17,:a18,:a19, :a20,
              :a21,:a22,:a23,:a24,:a25,:a26,:a27,:a28,:a29, :a30,
              :a31,:a32,:a33
    from tab1 ref11 , tab2 ref21 , tab3 ref31 ,
         tab1 ref12 , tab2 ref22 , tab3 ref32 ,
         tab1 ref13 , tab2 ref23 , tab3 ref33 ,
         tab1 ref14 , tab2 ref24
     where ref11.spalte13 < ref21.spalte21
       and ref11.spalte11 = ref13.spalte11
       and ref21.spalte21 = ref31.spalte31
       and ref12.spalte13 < ref22.spalte21
       and ref22.spalte21 = ref32.spalte31
       and ref22.spalte21 = ref31.spalte31
       and ref13.spalte13 < ref23.spalte21
       and ref23.spalte21 = ref33.spalte31
       and ref22.spalte21 = ref33.spalte31
       and ref14.spalte13 < ref24.spalte21
       and ref14.spalte13 < ref32.spalte31
       and ref23.spalte21 = ref24.spalte21 !
commit work release !
