*   *ID* N44INDX3 DBCMD    changed on 1992-09-28-13.26.34 by ADMIN     *
file connect ( kern test !
create table aa (
aadstnr    char (5 ) key,
aadat      char (6 ) key,
aalfdnr    char (5 ) key,
aaswa      char (14) ,
aaadat     char (6 ) ,
aabkza     char (7 ) ,
aaregdst   char (2 ) ,
aawvdat    char (6 ) ,
aaabeg     char (6 ) ,
aabewart   char (1 ) ,
aabewul1   char (1 ) ,
aavab      char (6 ) ,
aavbis     char (6 ) ,
aaschart   char (4 ) ,
aabeh      char (1 ) ,
aanat      char (3 ) ,
aadauf     char (3 ) ,
aasopro    char (1 ) ,
aastatkn   char (1 ) ,
aastatdt   char (6 ) ,
aavstat    char (1 ) ,
aabesr1    char (1 ) ,
aabesr2    char (3 ) ,
aalevv     char (6 ) ,
aamitf1    char (5 ) ,
aamitf2    char (5 ) ,
aamitf3    char (5 ) ,
aamitf4    char (5 ) ,
aaevor1    char (24) ,
aaevor2    char (30) ,
aaevor3    char (30) ,
aaevor4    char (30) ,
aaevor5    char (16) ,
aabewul2   char (1 ) ,
aabewul3   char (1 ) ,
aaanzvv    char (3 ) ,
aagst      char (3 ) ,
aaost      char (3 ) ) !
************************
create table ab (
abswb      char (14) key,
abbkzb     char (7 ) ,
abwzw      char (3 ) ,
abstrb     char (32) ,
abplzb     char (4 ) ,
abortb     char (32) ,
abaparb    char (32) ,
abzust     char (3 ) ,
ab18aab    char (6 ) ,
ab18aan    char (6 ) ,
ab18ale    char (1 ) ,
abtrart    char (32) ,
abegro     char (3 ) ,
abangam    char (6 ) ,
absvkn1    char (1 ) ,
absvdt1    char (6 ) ,
absvkn2    char (1 ) ,
absvdt2    char (6 ) ,
absvkn3    char (1 ) ,
absvdt3    char (6 ) ,
absvkn4    char (1 ) ,
absvdt4    char (6 ) ,
absvkn5    char (1 ) ,
absvdt5    char (6 ) ,
absvkn6    char (1 ) ,
absvdt6    char (6 ) ,
abbemk1    char (18) ,
abbemk2    char (31) ,
abbemk3    char (31) ,
abbemk4    char (31) ,
abbemk5    char (31) ,
abbemk6    char (31) ,
abbemk7    char (31) ,
abbemk8    char (31) ,
abbemk9    char (31) ,
ablekon    char (1 ) ,
ablekdt    char (6 ) ,
ablebar    char (6 ) ) !
************************
create index "aa.aabkza"
	on aa( aabkza ) !
create index "aa.aaswa"
	on aa( aaswa ) !
************************
insert aa set aadstnr = '12345', aadat = '050687', aalfdnr = '1',
aaswa = 'abcdef', aabkza = '52301', aastatkn = 'O' !
insert ab set abswb = 'abcdef', abbkzb = '52301',
abortb = 'A13' !
************************
select ab.abortb, aa.aabkza, ab.abswb, aa.aastatkn, aa.aaost, aa.aaadat
from aa, ab
where
aa.aabkza = ab.abbkzb and
ab.abswb = aa.aaswa and
aa.aabkza between '0110' and '53120' and
ab.abortb like 'A13'
order by ab.abortb, aa.aabkza, ab.abswb !
fetch !
select ab.abortb, aa.aabkza, ab.abswb, aa.aastatkn, aa.aaost, aa.aaadat
from aa, ab
where
aa.aastatkn = 'O' and
aa.aabkza = ab.abbkzb and
ab.abswb = aa.aaswa and
aa.aabkza between '0110' and '53120' and
ab.abortb like 'A13'
order by ab.abortb, aa.aabkza, ab.abswb !
fetch !
commit work release !
