*   *ID* AK02T2   DBCMD    changed on 1992-09-28-13.22.08 by ADMIN     *
file connect ( sut sut !
create user freier password w dba !
commit work release !
file connect ( freier w !
create table fr1 (f1 char (12)) !
create table fr11 (f1 char (12)) !
insert fr1 values ('freierfr11') !
insert fr1 values ('freierfr12') !
insert fr1 values ('freierfr13') !
insert fr11 values ('public11') !
insert fr11 values ('public12') !
insert fr11 values ('public13') !
create user frank password w !
create synonym  syn1 for fr11  !
create synonym  syn2 for freier.fr1  !

file sel_user !
file sel_table !
file sel_synonym !
select * from  fr1 !
fetch !
select * from  fr11 !
fetch !
commit work release !
file connect ( sut sut !

file sel_user !
file sel_table !
file sel_synonym !
alter user freier resource !

file sel_user !
file sel_table !
file sel_synonym !
alter user freier !

file sel_user !
file sel_table !
file sel_synonym !
drop  USER FREIER !

file sel_user !
file sel_table !
file sel_synonym !
coMMIT WORK RELEASE !
file connect ( sut sut !
drop user frank !
COMMIT WORK RELEASE !
