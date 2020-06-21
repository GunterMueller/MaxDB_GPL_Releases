*   *ID* AK07ADRO DBCMD    changed on 1992-09-28-13.22.10 by ADMIN     *
 ***************************
 ***   A K 0 7 A D R O   ***
 ***************************!
file connect ( kern test !
create table kern.t1 (f1 char (6) range in ('aaa', 'ccc' ) ,
    f2 char (9) default  'wwww' ,
    f3 fixed (4,1),
    f4 float (12) ,
    zz float (12) ,
    f5 date ) !
create table kern.t2 (asdfgww1 char (12),
                 asdfgww2 char (12),
                 asdfgww3 char (12),
                 asdfgww4 char (12),
                 asdfgww5 char (12)) !
create index "kern.t1.f2"
	on kern.t1( f2 ) !
create index "kern.t1.f3"
	on kern.t1( f3 ) !
create index f2f4  on kern.t1 (f2, f4) !
create user freier password w !
grant select (f2, f5, f1 ) on kern.t1 to freier !
create user rudi password w !
grant select (f2, f5 ) on kern.t1 to rudi   !
create view kern.v1 as select * from kern.t1 where f2 = 'aaa' !
create view kern.v2 as select * from kern.t1 where f1 = 'aaa' !
file sel_table !
file sel_view1 ( KERN V1 !
file sel_view1 ( KERN V2 !
file sel_index_1tab ( KERN T1 !
file sel_priv_granted ( FREIER !
file sel_priv_granted ( RUDI !
commit !
alter table kern.t1 drop (f2, f5 ) !
alter table kern.t1 drop (f1 ) !
alter table kern.t1 drop (zz ) !
alter table kern.t1 drop (f3, f4 ) !
alter table kern.t2 drop (asdfgww2, asdfgww5 ) !
alter table kern.t2 drop (asdfgww1 ) !
alter table kern.t2 drop (asdfgww4 ) !
alter table kern.t2 drop (asdfgww3 ) !
alter table kern.t1 add (f2 char (12) ) !
alter table kern.t2 add (asdfgww2 char (12) ) !
create table kern.lt1 like kern.t1 !
create table kern.lt2 like kern.t2 !
file sel_table !
file sel_cols_1tab ( KERN T1 !
file sel_view1 ( KERN V1 !
file sel_view1 ( KERN V2 !
file sel_index_1tab ( KERN T1 !
file sel_priv_granted ( FREIER !
file sel_priv_granted ( RUDI !
commit release !
file connect ( kern test !
create table kern.maxtab (
asdfg001 char(12) ASCII key ,
asdfg002 char(12) ASCII ,
asdfg003 char(12) ASCII , asdfg004 char(12) ASCII ,
asdfg005 char(12) ASCII , asdfg006 char(12) ASCII ,
asdfg007 char(12) ASCII , asdfg008 char(12) ASCII ,
asdfg009 char(12) ASCII , asdfg010 char(12) ASCII ,
asdfg011 char(12) ASCII , asdfg012 char(12) ASCII ,
asdfg013 char(12) ASCII , asdfg014 char(12) ASCII ,
asdfg015 char(12) ASCII , asdfg016 char(12) ASCII ,
asdfg017 char(12) ASCII , asdfg018 char(12) ASCII ,
asdfg019 char(12) ASCII , asdfg020 char(12) ASCII ,
asdfg021 char(12) ASCII , asdfg022 char(12) ASCII ,
asdfg023 char(12) ASCII , asdfg024 char(12) ASCII ,
asdfg025 char(12) ASCII , asdfg026 char(12) ASCII ,
asdfg027 char(12) ASCII , asdfg028 char(12) ASCII ,
asdfg029 char(12) ASCII , asdfg030 char(12) ASCII ,
asdfg031 char(12) ASCII , asdfg032 char(12) ASCII ,
asdfg033 char(12) ASCII , asdfg034 char(12) ASCII ,
asdfg035 char(12) ASCII , asdfg036 char(12) ASCII ,
asdfg037 char(12) ASCII , asdfg038 char(12) ASCII ,
asdfg039 char(12) ASCII , asdfg040 char(12) ASCII ,
asdfg041 char(12) ASCII , asdfg042 char(12) ASCII ,
asdfg043 char(12) ASCII , asdfg044 char(12) ASCII ,
asdfg045 char(12) ASCII , asdfg046 char(12) ASCII ,
asdfg047 char(12) ASCII , asdfg048 char(12) ASCII ,
asdfg049 char(12) ASCII , asdfg050 char(12) ASCII ,
asdfg051 char(12) ASCII , asdfg052 char(12) ASCII ,
asdfg053 char(12) ASCII , asdfg054 char(12) ASCII ,
asdfg055 char(12) ASCII , asdfg056 char(12) ASCII ,
asdfg057 char(12) ASCII , asdfg058 char(12) ASCII ,
asdfg059 char(12) ASCII , asdfg060 char(12) ASCII ,
asdfg061 char(12) ASCII , asdfg062 char(12) ASCII ,
asdfg063 char(12) ASCII , asdfg064 char(12) ASCII ,
asdfg065 char(12) ASCII , asdfg066 char(12) ASCII ,
asdfg067 char(12) ASCII , asdfg068 char(12) ASCII ,
asdfg069 char(12) ASCII , asdfg070 char(12) ASCII ,
asdfg071 char(12) ASCII , asdfg072 char(12) ASCII ,
asdfg073 char(12) ASCII , asdfg074 char(12) ASCII ,
asdfg075 char(12) ASCII , asdfg076 char(12) ASCII ,
asdfg077 char(12) ASCII , asdfg078 char(12) ASCII ,
asdfg079 char(12) ASCII , asdfg080 char(12) ASCII ,
asdfg081 char(12) ASCII , asdfg082 char(12) ASCII ,
asdfg083 char(12) ASCII , asdfg084 char(12) ASCII ,
asdfg085 char(12) ASCII , asdfg086 char(12) ASCII ,
asdfg087 char(12) ASCII , asdfg088 char(12) ASCII ,
asdfg089 char(12) ASCII , asdfg090 char(12) ASCII ,
asdfg091 char(12) ASCII , asdfg092 char(12) ASCII ,
asdfg093 char(12) ASCII , asdfg094 char(12) ASCII ,
asdfg095 char(12) ASCII , asdfg096 char(12) ASCII ,
asdfg097 char(12) ASCII , asdfg098 char(12) ASCII ,
asdfg099 char(12) ASCII , asdfg100 char(12) ASCII ,
asdfg101 char(12) ASCII , asdfg102 char(12) ASCII ,
asdfg103 char(12) ASCII , asdfg104 char(12) ASCII ,
asdfg105 char(12) ASCII , asdfg106 char(12) ASCII ,
asdfg107 char(12) ASCII , asdfg108 char(12) ASCII ,
asdfg109 char(12) ASCII , asdfg110 char(12) ASCII ,
asdfg111 char(12) ASCII , asdfg112 char(12) ASCII ,
asdfg113 char(12) ASCII , asdfg114 char(12) ASCII ,
asdfg115 char(12) ASCII , asdfg116 char(12) ASCII ,
asdfg117 char(12) ASCII , asdfg118 char(12) ASCII ,
asdfg119 char(12) ASCII , asdfg120 char(12) ASCII ,
asdfg121 char(12) ASCII , asdfg122 char(12) ASCII ,
asdfg123 char(12) ASCII , asdfg124 char(12) ASCII ,
asdfg125 char(12) ASCII , asdfg126 char(12) ASCII ) !
alter table kern.maxtab add (
asdfg127 char(12) ascii, asdfg128 char(12) ascii,
asdfg129 char(12) ascii, asdfg130 char(12) ascii,
asdfg131 char(12) ascii, asdfg132 char(12) ascii,
asdfg133 char(12) ascii, asdfg134 char(12) ascii,
asdfg135 char(12) ascii, asdfg136 char(12) ascii,
asdfg137 char(12) ascii, asdfg138 char(12) ascii,
asdfg139 char(12) ascii, asdfg140 char(12) ascii,
asdfg141 char(12) ascii, asdfg142 char(12) ascii,
asdfg143 char(12) ascii, asdfg144 char(12) ascii,
asdfg145 char(12) ascii, asdfg146 char(12) ascii,
asdfg147 char(12) ascii, asdfg148 char(12) ascii,
asdfg149 char(12) ascii, asdfg150 char(12) ascii,
asdfg151 char(12) ascii, asdfg152 char(12) ascii,
asdfg153 char(12) ascii, asdfg154 char(12) ascii,
asdfg155 char(12) ascii, asdfg156 char(12) ascii,
asdfg157 char(12) ascii, asdfg158 char(12) ascii,
asdfg159 char(12) ascii, asdfg160 char(12) ascii,
asdfg161 char(12) ascii, asdfg162 char(12) ascii,
asdfg163 char(12) ascii, asdfg164 char(12) ascii,
asdfg165 char(12) ascii, asdfg166 char(12) ascii,
asdfg167 char(12) ascii, asdfg168 char(12) ascii,
asdfg169 char(12) ascii, asdfg170 char(12) ascii,
asdfg171 char(12) ascii, asdfg172 char(12) ascii,
asdfg173 char(12) ascii, asdfg174 char(12) ascii,
asdfg175 char(12) ascii, asdfg176 char(12) ascii,
asdfg177 char(12) ascii, asdfg178 char(12) ascii,
asdfg179 char(12) ascii, asdfg180 char(12) ascii,
asdfg181 char(12) ascii, asdfg182 char(12) ascii,
asdfg183 char(12) ascii, asdfg184 char(12) ascii,
asdfg185 char(12) ascii, asdfg186 char(12) ascii,
asdfg187 char(12) ascii, asdfg188 char(12) ascii,
asdfg189 char(12) ascii, asdfg190 char(12) ascii,
asdfg191 char(12) ascii, asdfg192 char(12) ascii,
asdfg193 char(12) ascii, asdfg194 char(12) ascii,
asdfg195 char(12) ascii, asdfg196 char(12) ascii,
asdfg197 char(12) ascii, asdfg198 char(12) ascii) !
commit release !
file connect ( kern test !
alter table kern.maxtab drop (asdfg064, asdfg065) !
commit release !
file connect ( kern test !
alter table kern.maxtab drop (asdfg131, asdfg132,
 asdfg133)  !
commit release !
