*   *ID* c00e273    cmd *
file connect ( kern test !
create table t1 ( 
 s1 char(40)ascii, s2 char(40)ascii,  s3 char(40)ascii,  s4 char(40)ascii,  
 s5 char(40)ascii, s6 char(40)ascii,  s7 char(40)ascii,  s8 char(40)ascii,  
 s9 char(40)ascii, s10 char(40)ascii,
s11 char(40)ascii, s12 char(40)ascii, s13 char(40)ascii, s14 char(40)ascii, 
s15 char(40)ascii, s16 char(40)ascii, s17 char(40)ascii, s18 char(40)ascii, 
s19 char(40)ascii, s20 char(40)ascii, 
s21 char(40)ascii, s22 char(40)ascii, s23 char(40)ascii, s24 char(40)ascii, 
s25 char(40)ascii, s26 char(40)ascii, s27 char(40)ascii, s28 char(40)ascii, 
s29 char(40)ascii, s30 char(40)ascii,
s31 char(40)ascii, s32 char(40)ascii, s33 char(40)ascii, s34 char(40)ascii, 
s35 char(40)ascii, s36 char(40)ascii, s37 char(40)ascii, s38 char(40)ascii, 
s39 char(40)ascii, s40 char(40)ascii,
s41 char(40)ascii, s42 char(40)ascii, s43 char(40)ascii, s44 char(40)ascii, 
s45 char(40)ascii, s46 char(40)ascii, s47 char(40)ascii, s48 char(40)ascii, 
s49 char(40)ascii, s50 char(40)ascii,
s51 char(40)ascii, s52 char(40)ascii, s53 char(40)ascii, s54 char(40)ascii, 
s55 char(40)ascii, s56 char(40)ascii, s57 char(40)ascii, s58 char(40)ascii, 
s59 char(40)ascii, s60 char(40)ascii,
s61 char(40)ascii, s62 char(40)ascii, s63 char(40)ascii, s64 char(40)ascii, 
s65 char(40)ascii, s66 char(40)ascii, s67 char(40)ascii, s68 char(40)ascii, 
s69 char(40)ascii, s70 char(40)ascii,
s71 char(40)ascii, s72 char(40)ascii, s73 char(40)ascii, s74 char(40)ascii, 
s75 char(40)ascii, s76 char(40)ascii, s77 char(40)ascii, s78 char(40)ascii, 
s79 char(40)ascii, s80 char(40)ascii,
s81 char(40)ascii, s82 char(40)ascii, s83 char(40)ascii, s84 char(40)ascii, 
s85 char(40)ascii, s86 char(40)ascii, s87 char(40)ascii, s88 char(40)ascii, 
s89 char(40)ascii, s90 char(40)ascii,
s91 char(40)ascii, s92 char(40)ascii, s93 char(40)ascii, s94 char(40)ascii, 
s95 char(40)ascii, s96 char(40)ascii, s97 char(40)ascii, s98 char(40)ascii, 
s99 char(40)ascii, s100 char(40)ascii)!
insert t1 set s1 = 'abcdef' !
select 
 s1,  s2,  s3,  s4,  s5,
 s6,  s7,  s8,  s9, s10,
s11, s12, s13, s14, s15,
s16, s17, s18, s19, s20,
s21, s22, s23, s24, s25,
s26, s27, s28, s29, s30,
s31, s32, s33, s34, s35,
s36, s37, s38, s39, s40,
s41, s42, s43, s44, s45,
s46, s47, s48, s49, s50,
s51, s52, s53, s54, s55,
s56, s57, s58, s59, s60,
s61, s62, s63, s64, s65,
s66, s67, s68, s69, s70,
s71, s72, s73, s74, s75,
s76, s77, s78, s79, s80,
s81, s82, s83, s84, s85,
s86, s87, s88, s89, s90,
s91, s92, s93, s94, s95,
s96, s97, s98, s99, s100,
 s1,  s2,  s3,  s4,  s5,
 s6,  s7,  s8,  s9, s10,
s11, s12, s13, s14, s15,
s16, s17, s18, s19, s20,
s21, s22, s23, s24, s25,
s26, s27, s28, s29, s30,
s31, s32, s33, s34, s35,
s36, s37, s38, s39, s40,
s41, s42, s43, s44, s45,
s46, s47, s48, s49, s50,
s51, s52, s53, s54, s55,
s56, s57, s58, s59, s60,
s61, s62, s63, s64, s65,
s66, s67, s68, s69, s70,
s71, s72, s73, s74, s75,
s76, s77, s78, s79, s80,
s81, s82, s83, s84, s85,
s86, s87, s88, s89, s90,
s91, s92, s93, s94, s95,
s96, s97, s98, s99, s100
from t1 !
fetch !
select t1.*, t1.* from t1 !
fetch ! 
select t1.*, r.* from t1, t1 r !
fetch !
rollback work release !
