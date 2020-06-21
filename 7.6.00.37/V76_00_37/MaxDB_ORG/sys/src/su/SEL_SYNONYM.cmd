* show synonym !
declare sel_synonym cursor for 
 select substr(synonymname,1,18) synonymname,
        substr(tableowner,1,18)  owner,
        substr(tablename,1,18)   tablename
   from domain.synonyms 
 where tableowner <> 'SUT' !
fetch sel_synonym into :a,:b,:c !
close sel_synonym !
