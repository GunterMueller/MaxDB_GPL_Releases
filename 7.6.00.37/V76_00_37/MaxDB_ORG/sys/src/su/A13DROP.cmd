*   *ID* A13DROP  DBCMD    changed on 1992-09-28-13.22.30 by ADMIN     *
file connect ( kern test !
create table t1 (s1 integer, s2 integer, s3 integer, s4 integer,
s5   integer, s6   integer, s7   integer, s8   integer, s9   integer,
s10  integer, s11  integer, s12  integer, s13  integer, s14  integer,
s15  integer, s16  integer, s17  integer, s18  integer, s19  integer,
s20  integer, s21  integer, s22  integer, s23  integer, s24  integer,
s25  integer, s26  integer, s27  integer, s28  integer, s29  integer,
s30  integer, s31  integer, s32  integer, s33  integer, s34  integer,
s35  integer, s36  integer, s37  integer, s38  integer, s39  integer,
s40  integer, s41  integer, s42  integer, s43  integer, s44  integer,
s45  integer, s46  integer, s47  integer, s48  integer, s49  integer,
s50  integer, s51  integer, s52  integer, s53  integer, s54  integer,
s55  integer, s56  integer, s57  integer, s58  integer, s59  integer,
s60  integer, s61  integer, s62  integer, s63  integer, s64  integer,
s65  integer, s66  integer, s67  integer, s68  integer, s69  integer,
s70  integer, s71  integer, s72  integer, s73  integer, s74  integer,
s75  integer, s76  integer, s77  integer, s78  integer, s79  integer,
s80  integer, s81  integer, s82  integer, s83  integer, s84  integer,
s85  integer, s86  integer, s87  integer, s88  integer, s89  integer,
s90  integer, s91  integer, s92  integer, s93  integer, s94  integer,
s95  integer, s96  integer, s97  integer, s98  integer, s99  integer,
s100 integer, s101 integer, s102 integer, s103 integer, s104 integer,
s105 integer, s106 integer, s107 integer, s108 integer, s109 integer,
s110 integer, s111 integer, s112 integer, s113 integer, s114 integer,
s115 integer, s116 integer, s117 integer, s118 integer, s119 integer,
s120 integer, s121 integer, s122 integer, s123 integer, s124 integer,
s125 integer, s126 integer, s127 integer, s128 integer, s129 integer,
s130 integer, s131 integer, s132 integer, s133 integer, s134 integer,
s135 integer, s136 integer, s137 integer, s138 integer, s139 integer,
s140 integer, s141 integer, s142 integer, s143 integer, s144 integer,
s145 integer, s146 integer, s147 integer, s148 integer, s149 integer,
s150 integer, s151 integer, s152 integer, s153 integer, s154 integer,
s155 integer, s156 integer, s157 integer, s158 integer, s159 integer,
s160 integer, s161 integer, s162 integer, s163 integer, s164 integer,
s165 integer, s166 integer, s167 integer, s168 integer, s169 integer,
s170 integer, s171 integer, s172 integer, s173 integer, s174 integer,
s175 integer, s176 integer, s177 integer, s178 integer, s179 integer,
s180 integer, s181 integer, s182 integer, s183 integer, s184 integer,
s185 integer, s186 integer, s187 integer, s188 integer, s189 integer,
s190 integer, s191 integer, s192 integer, s193 integer, s194 integer,
s195 integer, s196 integer, s197 integer, s198 integer, s199 integer,
s200 integer, s201 integer, s202 integer, s203 integer, s204 integer,
s205 integer, s206 integer, s207 integer, s208 integer, s209 integer,
s210 integer, s211 integer, s212 integer, s213 integer, s214 integer,
s215 integer, s216 integer, s217 integer, s218 integer, s219 integer,
s220 integer, s221 integer, s222 integer, s223 integer, s224 integer,
s225 integer, s226 integer, s227 integer, s228 integer, s229 integer,
s230 integer, s231 integer, s232 integer, s233 integer, s234 integer,
s235 integer, s236 integer, s237 integer, s238 integer, s239 integer,
s240 integer, s241 integer, s242 integer, s243 integer, s244 integer,
s245 integer)!
create table t2 (t1 integer, t2 integer, t3 integer)!
create view v1 (
a1  ,a2  ,a3  ,a4  ,a5  ,a6  ,a7  ,a8  ,a9  ,a10,
a11 ,a12  ,a13  ,a14  ,a15  ,a16  ,a17  ,a18  ,a19  ,a20,
a21 ,a22  ,a23  ,a24  ,a25  ,a26  ,a27  ,a28  ,a29  ,a30,
a31 ,a32  ,a33  ,a34  ,a35  ,a36  ,a37  ,a38  ,a39  ,a40,
a41 ,a42  ,a43  ,a44  ,a45  ,a46  ,a47  ,a48  ,a49  ,a50,
a51 ,a52  ,a53  ,a54  ,a55  ,a56  ,a57  ,a58  ,a59  ,a60,
a61 ,a62  ,a63  ,a64  ,a65  ,a66  ,a67  ,a68  ,a69  ,a70,
a71 ,a72  ,a73  ,a74  ,a75  ,a76  ,a77  ,a78  ,a79  ,a80,
a81 ,a82  ,a83  ,a84  ,a85  ,a86  ,a87  ,a88  ,a89  ,a90,
a91 ,a92  ,a93  ,a94  ,a95  ,a96  ,a97  ,a98  ,a99  ,a100,
a101,a102,a103,a104,a105,a106,a107,a108,a109,a110,
a111,a112,a113,a114,a115,a116,a117,a118,a119,a120,
a121,a122,a123,a124,a125,a126,a127,a128,a129,a130,
a131,a132,a133,a134,a135,a136,a137,a138,a139,a140,
a141,a142,a143,a144,a145,a146,a147,a148,a149,a150,
a151,a152,a153,a154,a155,a156,a157,a158,a159,a160,
a161,a162,a163,a164,a165,a166,a167,a168,a169,a170,
a171,a172,a173,a174,a175,a176,a177,a178,a179,a180,
a181,a182,a183,a184,a185,a186,a187,a188,a189,a190,
a191,a192,a193,a194,a195,a196,a197,a198,a199,a200,
a201,a202,a203,a204,a205,a206,a207,a208,a209,a210,
a211,a212,a213,a214,a215,a216,a217,a218,a219,a220,
a221,a222,a223,a224,a225,a226,a227,a228,a229,a230,
a231,a232,a233,a234,a235,a236,a237,a238,a239,a240,
a241,a242,a243,a244,a245 )
as select * from t1 where s244 = 100!
file sel_cols_tab ( V1 !
commit work !
create view v2 (a,b,c) as select a1, a2, a3 from v1 !
create view v3 (a,b,c) as select s1, s2, s3 from t1 !
create view v4 as select s1 x1, s2 x2 ,  s3 x3 from t1 !
commit work !
create view v5 (x,y,z) as select * from t2 !
create view v6 (v1,v2,v3,v4,v5,v6) as select * from v2, v5 !
create view v7 (v1,v2,v3,v4,v5,v6) as select s3,s2,s1,b,c,a from
t1, v2 !
create view v8 as select t2.*, s1 from t2, t1!
commit work !
alter table t1 drop (s1, s3) !
file sel_view ( V1 !
file sel_view ( V2 !
file sel_view ( V3 !
file sel_view ( V4 !
file sel_view ( V5 !
file sel_view ( V6 !
file sel_view ( V7 !
file sel_view ( V8 !

* Ueberprufung, ob beim Droppen einer View der Katalog-Eintrag der
* Basistabelle (auch auf den Folgeseiten) korrekt gepflegt wird!
* (Fehlermeldung von R3 am 14.8.95)!
create index "t1.s6"
	on t1( s6 ) !
create view vx as select * from t1!
file sel_index_1tab ( KERN T1 !
drop view vx!
file sel_index_1tab ( KERN T1 !
drop index "t1.s6" !
drop table t1 !
drop table t2 !
commit work release !
