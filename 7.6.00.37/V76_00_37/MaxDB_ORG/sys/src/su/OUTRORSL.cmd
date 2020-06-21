*   *ID* OUTRORSL DBCMD    changed on 1992-09-28-13.27.38 by ADMIN     *
select ta.*, tb.* from <ta, tb> where a1    =    b1  or
                                      a1 = 100 * b1 !
FETCH !
select ta.*, tb.* from <tb, ta> where a1    =    b1  or
                                      a1 = 100 * b1 !
FETCH !
select ta.*, tb.* from ta, tb where a1    =    b1  or
                                    a1 = 100 * b1 !
FETCH !
explain select ta.*, tb.* from <ta, tb> where a1 (+)=b1(+) or
                                              a1 = 100 * b1 !
select ta.*, tb.* from <ta, tb> where a1 (+)=b1(+)  or
                                      a1 = 100 * b1 !
FETCH !
explain select ta.*, tb.* from <tb, ta> where a1 (+)=b1(+)  or
                                              a1 = 100 * b1 !
select ta.*, tb.* from <tb, ta> where a1 (+)=b1(+)  or
                                      a1 = 100 * b1 !
FETCH !
explain select ta.*, tb.* from <ta, tb> where a1 (+)=    b1  or
                                              a1 = 100 * b1 !
select ta.*, tb.* from <ta, tb> where a1 (+)=    b1  or
                                      a1 = 100 * b1 !
FETCH !
explain select ta.*, tb.* from <tb, ta> where a1 (+)=    b1  or
a1 = 100 * b1 !
select ta.*, tb.* from <tb, ta> where a1 (+)=    b1  or
                                      a1 = 100 * b1 !
FETCH !
explain select ta.*, tb.* from <ta, tb> where a1    =b1(+)  or
                                              a1 = 100 * b1 !
select ta.*, tb.* from <ta, tb> where a1    =b1(+)  or
                                      a1 = 100 * b1 !
FETCH !
explain select ta.*, tb.* from <tb, ta> where a1    =b1(+)  or
                                              a1 = 100 * b1 !
select ta.*, tb.* from <tb, ta> where a1    =b1(+)  or
                                      a1 = 100 * b1 !
FETCH !
