file connect ( sut sut sqlmode oracle !
create table seq1 (s1 integer) !
create table seq2 (s2 integer) !
create sequence seq2 !
create sequence seq3 !
grant select on seq1 to kern !
grant select on seq2 to kern !
grant select on seq3 to kern !
grant select on seq4 to kern !
revoke select on seq1 from kern !
revoke select on seq2 from kern !
revoke select on seq3 from kern !
revoke select on seq4 from kern !
drop sequence seq2 !
drop sequence seq3 !
drop table seq1 !
drop table seq2 !
rollback release !
