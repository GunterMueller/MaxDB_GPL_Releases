* show user &1 !
declare sel_user cursor for 
 select username,groupname,owner,usermode,
        connectmode as "CONNECT",
        maxtimeout,costwarning,costlimit,
        pwcreadate as "PWCREATEDATE",
        pwcreatime as "PWCREATETIME" 
   from domain.users
  where username = '&1' !
fetch sel_user into :a, :b, :c, :d, :e, :f, :g, :h, :i, :j !
close sel_user !
