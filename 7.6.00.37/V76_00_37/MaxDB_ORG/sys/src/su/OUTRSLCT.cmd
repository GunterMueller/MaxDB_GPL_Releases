*   *ID* OUTRSLCT DBCMD    changed on 1992-09-28-13.27.39 by ADMIN     *
select ta.*, tb.* from <ta, tb> where a1    =    b1 !
FETCH !
select ta.*, tb.* from <tb, ta> where a1    =    b1 !
FETCH !
select ta.*, tb.* from ta, tb !
FETCH !
explain select ta.*, tb.* from <ta, tb> where a1 (+)=b1 (+)!
select ta.*, tb.* from <ta, tb> where a1 (+)=b1 (+)!
FETCH !
explain select ta.*, tb.* from <tb, ta> where a1 (+)=b1 (+)!
select ta.*, tb.* from <tb, ta> where a1 (+)=b1 (+)!
FETCH !
explain select ta.*, tb.* from <ta, tb> where a1 (+)=    b1 !
select ta.*, tb.* from <ta, tb> where a1 (+)=    b1 !
FETCH !
explain select ta.*, tb.* from <tb, ta> where a1 (+)=    b1 !
select ta.*, tb.* from <tb, ta> where a1 (+)=    b1 !
FETCH !
explain select ta.*, tb.* from <ta, tb> where a1    =b1 (+)!
select ta.*, tb.* from <ta, tb> where a1    =b1 (+)!
FETCH !
explain select ta.*, tb.* from <tb, ta> where a1    =b1 (+)!
select ta.*, tb.* from <tb, ta> where a1    =b1 (+)!
FETCH !
