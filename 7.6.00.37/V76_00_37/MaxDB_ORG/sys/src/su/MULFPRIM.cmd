*   *ID* MULFPRIM DBCMD    changed on 1992-09-28-13.24.39 by ADMIN     *
drop table t1 !
create table t1 (s1 char(05),
                 s2 char(10),
                 sl char(10)) !
insert t1 values ('aaaaa',  '          ', '          ')!
commit !
create table ta (a0 char(05),
a101dummy char(1),a102dummy char(1),a103dummy char(1),a104dummy char(1),
a105dummy char(1),a106dummy char(1),a107dummy char(1),a108dummy char(1),
a109dummy char(1),a110dummy char(1),a111dummy char(1),a112dummy char(1),
a113dummy char(1),a114dummy char(1),a115dummy char(1),a116dummy char(1),
a117dummy char(1),a118dummy char(1),a119dummy char(1),a120dummy char(1),
a121dummy char(1),a122dummy char(1),a123dummy char(1),a124dummy char(1),
a125dummy char(1),a126dummy char(1),a127dummy char(1),a128dummy char(1),
a129dummy char(1),a130dummy char(1),a131dummy char(1),a132dummy char(1),
a133dummy char(1),a134dummy char(1),a135dummy char(1),a136dummy char(1),
a137dummy char(1),a138dummy char(1),a139dummy char(1),a140dummy char(1),
a141dummy char(1),a142dummy char(1),a143dummy char(1),a144dummy char(1),
a145dummy char(1),a146dummy char(1),a147dummy char(1),a148dummy char(1),
a149dummy char(1),a150dummy char(1),a151dummy char(1),a152dummy char(1),
a153dummy char(1),a154dummy char(1),a155dummy char(1),a156dummy char(1),
a157dummy char(1),a158dummy char(1),a159dummy char(1),a160dummy char(1),
a161dummy char(1),a162dummy char(1),a163dummy char(1),a164dummy char(1),
a165dummy char(1),a166dummy char(1),a167dummy char(1),a168dummy char(1),
a169dummy char(1),a170dummy char(1),a171dummy char(1),a172dummy char(1),
a173dummy char(1),a174dummy char(1),a175dummy char(1),a176dummy char(1),
a177dummy char(1),a178dummy char(1),a179dummy char(1),a180dummy char(1),
a181dummy char(1),a182dummy char(1),a183dummy char(1),a184dummy char(1),
a185dummy char(1),a186dummy char(1),a187dummy char(1),a188dummy char(1),
a189dummy char(1),a190dummy char(1),a191dummy char(1),a192dummy char(1),
a193dummy char(1),a194dummy char(1),a195dummy char(1),a196dummy char(1),
a197dummy char(1),a198dummy char(1),a199dummy char(1),
                 a3 char(10),
a401dummy char(1),a402dummy char(1),a403dummy char(1),a404dummy char(1),
a405dummy char(1),a406dummy char(1),a407dummy char(1),a408dummy char(1),
a409dummy char(1),a410dummy char(1),a411dummy char(1),a412dummy char(1),
a413dummy char(1),a414dummy char(1),a415dummy char(1),a416dummy char(1),
a417dummy char(1),a418dummy char(1),a419dummy char(1),a420dummy char(1),
a421dummy char(1),a422dummy char(1),a423dummy char(1),a424dummy char(1),
a425dummy char(1),a426dummy char(1),a427dummy char(1),a428dummy char(1),
a429dummy char(1),a430dummy char(1),a431dummy char(1),a432dummy char(1),
a433dummy char(1),a434dummy char(1),a435dummy char(1),a436dummy char(1),
a437dummy char(1),a438dummy char(1),a439dummy char(1),a440dummy char(1),
a441dummy char(1),a442dummy char(1),a443dummy char(1),a444dummy char(1),
a445dummy char(1),a446dummy char(1),a447dummy char(1),a448dummy char(1),
a449dummy char(1),a450dummy char(1),a451dummy char(1),a452dummy char(1),
a453dummy char(1),a454dummy char(1),a455dummy char(1),a456dummy char(1),
a457dummy char(1),a458dummy char(1),a459dummy char(1),a460dummy char(1),
a461dummy char(1),a462dummy char(1),a463dummy char(1),a464dummy char(1),
a465dummy char(1),a466dummy char(1),a467dummy char(1),a468dummy char(1),
a469dummy char(1),a470dummy char(1),a471dummy char(1),a472dummy char(1),
a473dummy char(1),a474dummy char(1),a475dummy char(1),a476dummy char(1),
a477dummy char(1),a478dummy char(1),a479dummy char(1),a480dummy char(1),
a481dummy char(1),a482dummy char(1),a483dummy char(1),a484dummy char(1),
a485dummy char(1),a486dummy char(1),a487dummy char(1),a488dummy char(1),
a489dummy char(1),a490dummy char(1),a491dummy char(1),a492dummy char(1),
a493dummy char(1),a494dummy char(1),a495dummy char(1),a496dummy char(1),
a497dummy char(1),a498dummy char(1),a499dummy char(1),
                 a9 char(10),
                 PRIMARY KEY (a0, a3, a9)) !
insert ta set a0 = 'aaaaa',
               a3 = '     ',
               a9 = '     ' !
explain select * from <t1, ta> where s1 = a0 and s2 = a3 and sl = a9 !
select a0, a3, a9 from <t1, ta>
where s1 = a0 and s2 = a3 and sl = a9 !
FETCH !
rollback !
create table ta (a0 char(05),
a101dummy char(1),a102dummy char(1),a103dummy char(1),a104dummy char(1),
a105dummy char(1),a106dummy char(1),a107dummy char(1),a108dummy char(1),
a109dummy char(1),a110dummy char(1),a111dummy char(1),a112dummy char(1),
a113dummy char(1),a114dummy char(1),a115dummy char(1),a116dummy char(1),
a117dummy char(1),a118dummy char(1),a119dummy char(1),a120dummy char(1),
a121dummy char(1),a122dummy char(1),a123dummy char(1),a124dummy char(1),
a125dummy char(1),a126dummy char(1),a127dummy char(1),a128dummy char(1),
a129dummy char(1),a130dummy char(1),a131dummy char(1),a132dummy char(1),
a133dummy char(1),a134dummy char(1),a135dummy char(1),a136dummy char(1),
a137dummy char(1),a138dummy char(1),a139dummy char(1),a140dummy char(1),
a141dummy char(1),a142dummy char(1),a143dummy char(1),a144dummy char(1),
a145dummy char(1),a146dummy char(1),a147dummy char(1),a148dummy char(1),
a149dummy char(1),a150dummy char(1),a151dummy char(1),a152dummy char(1),
a153dummy char(1),a154dummy char(1),a155dummy char(1),a156dummy char(1),
a157dummy char(1),a158dummy char(1),a159dummy char(1),a160dummy char(1),
a161dummy char(1),a162dummy char(1),a163dummy char(1),a164dummy char(1),
a165dummy char(1),a166dummy char(1),a167dummy char(1),a168dummy char(1),
a169dummy char(1),a170dummy char(1),a171dummy char(1),a172dummy char(1),
a173dummy char(1),a174dummy char(1),a175dummy char(1),a176dummy char(1),
a177dummy char(1),a178dummy char(1),a179dummy char(1),a180dummy char(1),
a181dummy char(1),a182dummy char(1),a183dummy char(1),a184dummy char(1),
a185dummy char(1),a186dummy char(1),a187dummy char(1),a188dummy char(1),
a189dummy char(1),a190dummy char(1),a191dummy char(1),a192dummy char(1),
a193dummy char(1),a194dummy char(1),a195dummy char(1),a196dummy char(1),
a197dummy char(1),a198dummy char(1),a199dummy char(1),
                 a3 char(10),
a401dummy char(1),a402dummy char(1),a403dummy char(1),a404dummy char(1),
a405dummy char(1),a406dummy char(1),a407dummy char(1),a408dummy char(1),
a409dummy char(1),a410dummy char(1),a411dummy char(1),a412dummy char(1),
a413dummy char(1),a414dummy char(1),a415dummy char(1),a416dummy char(1),
a417dummy char(1),a418dummy char(1),a419dummy char(1),a420dummy char(1),
a421dummy char(1),a422dummy char(1),a423dummy char(1),a424dummy char(1),
a425dummy char(1),a426dummy char(1),a427dummy char(1),a428dummy char(1),
a429dummy char(1),a430dummy char(1),a431dummy char(1),a432dummy char(1),
a433dummy char(1),a434dummy char(1),a435dummy char(1),a436dummy char(1),
a437dummy char(1),a438dummy char(1),a439dummy char(1),a440dummy char(1),
a441dummy char(1),a442dummy char(1),a443dummy char(1),a444dummy char(1),
a445dummy char(1),a446dummy char(1),a447dummy char(1),a448dummy char(1),
a449dummy char(1),a450dummy char(1),a451dummy char(1),a452dummy char(1),
a453dummy char(1),a454dummy char(1),a455dummy char(1),a456dummy char(1),
a457dummy char(1),a458dummy char(1),a459dummy char(1),a460dummy char(1),
a461dummy char(1),a462dummy char(1),a463dummy char(1),a464dummy char(1),
a465dummy char(1),a466dummy char(1),a467dummy char(1),a468dummy char(1),
a469dummy char(1),a470dummy char(1),a471dummy char(1),a472dummy char(1),
a473dummy char(1),a474dummy char(1),a475dummy char(1),a476dummy char(1),
a477dummy char(1),a478dummy char(1),a479dummy char(1),a480dummy char(1),
a481dummy char(1),a482dummy char(1),a483dummy char(1),a484dummy char(1),
a485dummy char(1),a486dummy char(1),a487dummy char(1),a488dummy char(1),
a489dummy char(1),a490dummy char(1),a491dummy char(1),a492dummy char(1),
a493dummy char(1),a494dummy char(1),a495dummy char(1),a496dummy char(1),
a497dummy char(1),a498dummy char(1),a499dummy char(1),
                 a9 char(10),
                 PRIMARY KEY (a0, a9, a3)) !
insert ta set a0 = 'aaaaa',
              a3 = '     ',
              a9 = '     ' !
explain select * from <t1, ta> where s1 = a0 and s2 = a3 and sl = a9 !
select a0, a3, a9 from <t1, ta>
where s1 = a0 and s2 = a3 and sl = a9 !
FETCH !
rollback !
create table ta (a0 char(05),
a101dummy char(1),a102dummy char(1),a103dummy char(1),a104dummy char(1),
a105dummy char(1),a106dummy char(1),a107dummy char(1),a108dummy char(1),
a109dummy char(1),a110dummy char(1),a111dummy char(1),a112dummy char(1),
a113dummy char(1),a114dummy char(1),a115dummy char(1),a116dummy char(1),
a117dummy char(1),a118dummy char(1),a119dummy char(1),a120dummy char(1),
a121dummy char(1),a122dummy char(1),a123dummy char(1),a124dummy char(1),
a125dummy char(1),a126dummy char(1),a127dummy char(1),a128dummy char(1),
a129dummy char(1),a130dummy char(1),a131dummy char(1),a132dummy char(1),
a133dummy char(1),a134dummy char(1),a135dummy char(1),a136dummy char(1),
a137dummy char(1),a138dummy char(1),a139dummy char(1),a140dummy char(1),
a141dummy char(1),a142dummy char(1),a143dummy char(1),a144dummy char(1),
a145dummy char(1),a146dummy char(1),a147dummy char(1),a148dummy char(1),
a149dummy char(1),a150dummy char(1),a151dummy char(1),a152dummy char(1),
a153dummy char(1),a154dummy char(1),a155dummy char(1),a156dummy char(1),
a157dummy char(1),a158dummy char(1),a159dummy char(1),a160dummy char(1),
a161dummy char(1),a162dummy char(1),a163dummy char(1),a164dummy char(1),
a165dummy char(1),a166dummy char(1),a167dummy char(1),a168dummy char(1),
a169dummy char(1),a170dummy char(1),a171dummy char(1),a172dummy char(1),
a173dummy char(1),a174dummy char(1),a175dummy char(1),a176dummy char(1),
a177dummy char(1),a178dummy char(1),a179dummy char(1),a180dummy char(1),
a181dummy char(1),a182dummy char(1),a183dummy char(1),a184dummy char(1),
a185dummy char(1),a186dummy char(1),a187dummy char(1),a188dummy char(1),
a189dummy char(1),a190dummy char(1),a191dummy char(1),a192dummy char(1),
a193dummy char(1),a194dummy char(1),a195dummy char(1),a196dummy char(1),
a197dummy char(1),a198dummy char(1),a199dummy char(1),
                 a3 char(10),
a401dummy char(1),a402dummy char(1),a403dummy char(1),a404dummy char(1),
a405dummy char(1),a406dummy char(1),a407dummy char(1),a408dummy char(1),
a409dummy char(1),a410dummy char(1),a411dummy char(1),a412dummy char(1),
a413dummy char(1),a414dummy char(1),a415dummy char(1),a416dummy char(1),
a417dummy char(1),a418dummy char(1),a419dummy char(1),a420dummy char(1),
a421dummy char(1),a422dummy char(1),a423dummy char(1),a424dummy char(1),
a425dummy char(1),a426dummy char(1),a427dummy char(1),a428dummy char(1),
a429dummy char(1),a430dummy char(1),a431dummy char(1),a432dummy char(1),
a433dummy char(1),a434dummy char(1),a435dummy char(1),a436dummy char(1),
a437dummy char(1),a438dummy char(1),a439dummy char(1),a440dummy char(1),
a441dummy char(1),a442dummy char(1),a443dummy char(1),a444dummy char(1),
a445dummy char(1),a446dummy char(1),a447dummy char(1),a448dummy char(1),
a449dummy char(1),a450dummy char(1),a451dummy char(1),a452dummy char(1),
a453dummy char(1),a454dummy char(1),a455dummy char(1),a456dummy char(1),
a457dummy char(1),a458dummy char(1),a459dummy char(1),a460dummy char(1),
a461dummy char(1),a462dummy char(1),a463dummy char(1),a464dummy char(1),
a465dummy char(1),a466dummy char(1),a467dummy char(1),a468dummy char(1),
a469dummy char(1),a470dummy char(1),a471dummy char(1),a472dummy char(1),
a473dummy char(1),a474dummy char(1),a475dummy char(1),a476dummy char(1),
a477dummy char(1),a478dummy char(1),a479dummy char(1),a480dummy char(1),
a481dummy char(1),a482dummy char(1),a483dummy char(1),a484dummy char(1),
a485dummy char(1),a486dummy char(1),a487dummy char(1),a488dummy char(1),
a489dummy char(1),a490dummy char(1),a491dummy char(1),a492dummy char(1),
a493dummy char(1),a494dummy char(1),a495dummy char(1),a496dummy char(1),
a497dummy char(1),a498dummy char(1),a499dummy char(1),
                 a9 char(10),
                 PRIMARY KEY (a3, a0, a9)) !
insert ta set a0 = 'aaaaa',
              a3 = '     ',
              a9 = '     ' !
explain select * from <t1, ta> where s1 = a0 and s2 = a3 and sl = a9 !
select a0, a3, a9 from <t1, ta>
where s1 = a0 and s2 = a3 and sl = a9 !
FETCH !
rollback !
create table ta (a0 char(05),
a101dummy char(1),a102dummy char(1),a103dummy char(1),a104dummy char(1),
a105dummy char(1),a106dummy char(1),a107dummy char(1),a108dummy char(1),
a109dummy char(1),a110dummy char(1),a111dummy char(1),a112dummy char(1),
a113dummy char(1),a114dummy char(1),a115dummy char(1),a116dummy char(1),
a117dummy char(1),a118dummy char(1),a119dummy char(1),a120dummy char(1),
a121dummy char(1),a122dummy char(1),a123dummy char(1),a124dummy char(1),
a125dummy char(1),a126dummy char(1),a127dummy char(1),a128dummy char(1),
a129dummy char(1),a130dummy char(1),a131dummy char(1),a132dummy char(1),
a133dummy char(1),a134dummy char(1),a135dummy char(1),a136dummy char(1),
a137dummy char(1),a138dummy char(1),a139dummy char(1),a140dummy char(1),
a141dummy char(1),a142dummy char(1),a143dummy char(1),a144dummy char(1),
a145dummy char(1),a146dummy char(1),a147dummy char(1),a148dummy char(1),
a149dummy char(1),a150dummy char(1),a151dummy char(1),a152dummy char(1),
a153dummy char(1),a154dummy char(1),a155dummy char(1),a156dummy char(1),
a157dummy char(1),a158dummy char(1),a159dummy char(1),a160dummy char(1),
a161dummy char(1),a162dummy char(1),a163dummy char(1),a164dummy char(1),
a165dummy char(1),a166dummy char(1),a167dummy char(1),a168dummy char(1),
a169dummy char(1),a170dummy char(1),a171dummy char(1),a172dummy char(1),
a173dummy char(1),a174dummy char(1),a175dummy char(1),a176dummy char(1),
a177dummy char(1),a178dummy char(1),a179dummy char(1),a180dummy char(1),
a181dummy char(1),a182dummy char(1),a183dummy char(1),a184dummy char(1),
a185dummy char(1),a186dummy char(1),a187dummy char(1),a188dummy char(1),
a189dummy char(1),a190dummy char(1),a191dummy char(1),a192dummy char(1),
a193dummy char(1),a194dummy char(1),a195dummy char(1),a196dummy char(1),
a197dummy char(1),a198dummy char(1),a199dummy char(1),
                 a3 char(10),
a401dummy char(1),a402dummy char(1),a403dummy char(1),a404dummy char(1),
a405dummy char(1),a406dummy char(1),a407dummy char(1),a408dummy char(1),
a409dummy char(1),a410dummy char(1),a411dummy char(1),a412dummy char(1),
a413dummy char(1),a414dummy char(1),a415dummy char(1),a416dummy char(1),
a417dummy char(1),a418dummy char(1),a419dummy char(1),a420dummy char(1),
a421dummy char(1),a422dummy char(1),a423dummy char(1),a424dummy char(1),
a425dummy char(1),a426dummy char(1),a427dummy char(1),a428dummy char(1),
a429dummy char(1),a430dummy char(1),a431dummy char(1),a432dummy char(1),
a433dummy char(1),a434dummy char(1),a435dummy char(1),a436dummy char(1),
a437dummy char(1),a438dummy char(1),a439dummy char(1),a440dummy char(1),
a441dummy char(1),a442dummy char(1),a443dummy char(1),a444dummy char(1),
a445dummy char(1),a446dummy char(1),a447dummy char(1),a448dummy char(1),
a449dummy char(1),a450dummy char(1),a451dummy char(1),a452dummy char(1),
a453dummy char(1),a454dummy char(1),a455dummy char(1),a456dummy char(1),
a457dummy char(1),a458dummy char(1),a459dummy char(1),a460dummy char(1),
a461dummy char(1),a462dummy char(1),a463dummy char(1),a464dummy char(1),
a465dummy char(1),a466dummy char(1),a467dummy char(1),a468dummy char(1),
a469dummy char(1),a470dummy char(1),a471dummy char(1),a472dummy char(1),
a473dummy char(1),a474dummy char(1),a475dummy char(1),a476dummy char(1),
a477dummy char(1),a478dummy char(1),a479dummy char(1),a480dummy char(1),
a481dummy char(1),a482dummy char(1),a483dummy char(1),a484dummy char(1),
a485dummy char(1),a486dummy char(1),a487dummy char(1),a488dummy char(1),
a489dummy char(1),a490dummy char(1),a491dummy char(1),a492dummy char(1),
a493dummy char(1),a494dummy char(1),a495dummy char(1),a496dummy char(1),
a497dummy char(1),a498dummy char(1),a499dummy char(1),
                 a9 char(10),
                 PRIMARY KEY (a3, a9, a0)) !
insert ta set a0 = 'aaaaa',
               a3 = '     ',
               a9 = '     ' !
explain select * from <t1, ta> where s1 = a0 and s2 = a3 and sl = a9 !
select a0, a3, a9 from <t1, ta>
where s1 = a0 and s2 = a3 and sl = a9 !
FETCH !
rollback !
create table ta (a0 char(05),
a101dummy char(1),a102dummy char(1),a103dummy char(1),a104dummy char(1),
a105dummy char(1),a106dummy char(1),a107dummy char(1),a108dummy char(1),
a109dummy char(1),a110dummy char(1),a111dummy char(1),a112dummy char(1),
a113dummy char(1),a114dummy char(1),a115dummy char(1),a116dummy char(1),
a117dummy char(1),a118dummy char(1),a119dummy char(1),a120dummy char(1),
a121dummy char(1),a122dummy char(1),a123dummy char(1),a124dummy char(1),
a125dummy char(1),a126dummy char(1),a127dummy char(1),a128dummy char(1),
a129dummy char(1),a130dummy char(1),a131dummy char(1),a132dummy char(1),
a133dummy char(1),a134dummy char(1),a135dummy char(1),a136dummy char(1),
a137dummy char(1),a138dummy char(1),a139dummy char(1),a140dummy char(1),
a141dummy char(1),a142dummy char(1),a143dummy char(1),a144dummy char(1),
a145dummy char(1),a146dummy char(1),a147dummy char(1),a148dummy char(1),
a149dummy char(1),a150dummy char(1),a151dummy char(1),a152dummy char(1),
a153dummy char(1),a154dummy char(1),a155dummy char(1),a156dummy char(1),
a157dummy char(1),a158dummy char(1),a159dummy char(1),a160dummy char(1),
a161dummy char(1),a162dummy char(1),a163dummy char(1),a164dummy char(1),
a165dummy char(1),a166dummy char(1),a167dummy char(1),a168dummy char(1),
a169dummy char(1),a170dummy char(1),a171dummy char(1),a172dummy char(1),
a173dummy char(1),a174dummy char(1),a175dummy char(1),a176dummy char(1),
a177dummy char(1),a178dummy char(1),a179dummy char(1),a180dummy char(1),
a181dummy char(1),a182dummy char(1),a183dummy char(1),a184dummy char(1),
a185dummy char(1),a186dummy char(1),a187dummy char(1),a188dummy char(1),
a189dummy char(1),a190dummy char(1),a191dummy char(1),a192dummy char(1),
a193dummy char(1),a194dummy char(1),a195dummy char(1),a196dummy char(1),
a197dummy char(1),a198dummy char(1),a199dummy char(1),
                 a3 char(10),
a401dummy char(1),a402dummy char(1),a403dummy char(1),a404dummy char(1),
a405dummy char(1),a406dummy char(1),a407dummy char(1),a408dummy char(1),
a409dummy char(1),a410dummy char(1),a411dummy char(1),a412dummy char(1),
a413dummy char(1),a414dummy char(1),a415dummy char(1),a416dummy char(1),
a417dummy char(1),a418dummy char(1),a419dummy char(1),a420dummy char(1),
a421dummy char(1),a422dummy char(1),a423dummy char(1),a424dummy char(1),
a425dummy char(1),a426dummy char(1),a427dummy char(1),a428dummy char(1),
a429dummy char(1),a430dummy char(1),a431dummy char(1),a432dummy char(1),
a433dummy char(1),a434dummy char(1),a435dummy char(1),a436dummy char(1),
a437dummy char(1),a438dummy char(1),a439dummy char(1),a440dummy char(1),
a441dummy char(1),a442dummy char(1),a443dummy char(1),a444dummy char(1),
a445dummy char(1),a446dummy char(1),a447dummy char(1),a448dummy char(1),
a449dummy char(1),a450dummy char(1),a451dummy char(1),a452dummy char(1),
a453dummy char(1),a454dummy char(1),a455dummy char(1),a456dummy char(1),
a457dummy char(1),a458dummy char(1),a459dummy char(1),a460dummy char(1),
a461dummy char(1),a462dummy char(1),a463dummy char(1),a464dummy char(1),
a465dummy char(1),a466dummy char(1),a467dummy char(1),a468dummy char(1),
a469dummy char(1),a470dummy char(1),a471dummy char(1),a472dummy char(1),
a473dummy char(1),a474dummy char(1),a475dummy char(1),a476dummy char(1),
a477dummy char(1),a478dummy char(1),a479dummy char(1),a480dummy char(1),
a481dummy char(1),a482dummy char(1),a483dummy char(1),a484dummy char(1),
a485dummy char(1),a486dummy char(1),a487dummy char(1),a488dummy char(1),
a489dummy char(1),a490dummy char(1),a491dummy char(1),a492dummy char(1),
a493dummy char(1),a494dummy char(1),a495dummy char(1),a496dummy char(1),
a497dummy char(1),a498dummy char(1),a499dummy char(1),
                 a9 char(10),
                 PRIMARY KEY (a9, a3, a0)) !
insert ta set a0 = 'aaaaa',
              a3 = '     ',
              a9 = '     ' !
explain select * from <t1, ta> where s1 = a0 and s2 = a3 and sl = a9 !
select a0, a3, a9 from <t1, ta>
where s1 = a0 and s2 = a3 and sl = a9 !
FETCH !
rollback !
create table ta (a0 char(05),
a101dummy char(1),a102dummy char(1),a103dummy char(1),a104dummy char(1),
a105dummy char(1),a106dummy char(1),a107dummy char(1),a108dummy char(1),
a109dummy char(1),a110dummy char(1),a111dummy char(1),a112dummy char(1),
a113dummy char(1),a114dummy char(1),a115dummy char(1),a116dummy char(1),
a117dummy char(1),a118dummy char(1),a119dummy char(1),a120dummy char(1),
a121dummy char(1),a122dummy char(1),a123dummy char(1),a124dummy char(1),
a125dummy char(1),a126dummy char(1),a127dummy char(1),a128dummy char(1),
a129dummy char(1),a130dummy char(1),a131dummy char(1),a132dummy char(1),
a133dummy char(1),a134dummy char(1),a135dummy char(1),a136dummy char(1),
a137dummy char(1),a138dummy char(1),a139dummy char(1),a140dummy char(1),
a141dummy char(1),a142dummy char(1),a143dummy char(1),a144dummy char(1),
a145dummy char(1),a146dummy char(1),a147dummy char(1),a148dummy char(1),
a149dummy char(1),a150dummy char(1),a151dummy char(1),a152dummy char(1),
a153dummy char(1),a154dummy char(1),a155dummy char(1),a156dummy char(1),
a157dummy char(1),a158dummy char(1),a159dummy char(1),a160dummy char(1),
a161dummy char(1),a162dummy char(1),a163dummy char(1),a164dummy char(1),
a165dummy char(1),a166dummy char(1),a167dummy char(1),a168dummy char(1),
a169dummy char(1),a170dummy char(1),a171dummy char(1),a172dummy char(1),
a173dummy char(1),a174dummy char(1),a175dummy char(1),a176dummy char(1),
a177dummy char(1),a178dummy char(1),a179dummy char(1),a180dummy char(1),
a181dummy char(1),a182dummy char(1),a183dummy char(1),a184dummy char(1),
a185dummy char(1),a186dummy char(1),a187dummy char(1),a188dummy char(1),
a189dummy char(1),a190dummy char(1),a191dummy char(1),a192dummy char(1),
a193dummy char(1),a194dummy char(1),a195dummy char(1),a196dummy char(1),
a197dummy char(1),a198dummy char(1),a199dummy char(1),
                 a3 char(10),
a401dummy char(1),a402dummy char(1),a403dummy char(1),a404dummy char(1),
a405dummy char(1),a406dummy char(1),a407dummy char(1),a408dummy char(1),
a409dummy char(1),a410dummy char(1),a411dummy char(1),a412dummy char(1),
a413dummy char(1),a414dummy char(1),a415dummy char(1),a416dummy char(1),
a417dummy char(1),a418dummy char(1),a419dummy char(1),a420dummy char(1),
a421dummy char(1),a422dummy char(1),a423dummy char(1),a424dummy char(1),
a425dummy char(1),a426dummy char(1),a427dummy char(1),a428dummy char(1),
a429dummy char(1),a430dummy char(1),a431dummy char(1),a432dummy char(1),
a433dummy char(1),a434dummy char(1),a435dummy char(1),a436dummy char(1),
a437dummy char(1),a438dummy char(1),a439dummy char(1),a440dummy char(1),
a441dummy char(1),a442dummy char(1),a443dummy char(1),a444dummy char(1),
a445dummy char(1),a446dummy char(1),a447dummy char(1),a448dummy char(1),
a449dummy char(1),a450dummy char(1),a451dummy char(1),a452dummy char(1),
a453dummy char(1),a454dummy char(1),a455dummy char(1),a456dummy char(1),
a457dummy char(1),a458dummy char(1),a459dummy char(1),a460dummy char(1),
a461dummy char(1),a462dummy char(1),a463dummy char(1),a464dummy char(1),
a465dummy char(1),a466dummy char(1),a467dummy char(1),a468dummy char(1),
a469dummy char(1),a470dummy char(1),a471dummy char(1),a472dummy char(1),
a473dummy char(1),a474dummy char(1),a475dummy char(1),a476dummy char(1),
a477dummy char(1),a478dummy char(1),a479dummy char(1),a480dummy char(1),
a481dummy char(1),a482dummy char(1),a483dummy char(1),a484dummy char(1),
a485dummy char(1),a486dummy char(1),a487dummy char(1),a488dummy char(1),
a489dummy char(1),a490dummy char(1),a491dummy char(1),a492dummy char(1),
a493dummy char(1),a494dummy char(1),a495dummy char(1),a496dummy char(1),
a497dummy char(1),a498dummy char(1),a499dummy char(1),
                 a9 char(10),
                 PRIMARY KEY (a9, a0, a3)) !
insert ta set a0 = 'aaaaa',
              a3 = '     ',
              a9 = '     ' !
explain select * from <t1, ta> where s1 = a0 and s2 = a3 and sl = a9 !
select a0, a3, a9 from <t1, ta>
where s1 = a0 and s2 = a3 and sl = a9 !
FETCH !
rollback !
drop table t1!
commit release!
