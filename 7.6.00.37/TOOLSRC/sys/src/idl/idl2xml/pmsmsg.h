/*


    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end


*/
typedef enum {s_success, s_info, s_warning, s_error, s_fatal} severity_code;

void PmsMsg
(
  unsigned short        msg_id,
  char                  modname[5],
  int                   line_no,
  severity_code         s_code,
  unsigned short        arg_ct,
  char                 *msg_line,
  ...
);

#define _TSTMSG(p,p0,p1,p2,p3) PmsMsg(0,M__NAME,__LINE__,p0,3,p,p1,p2,p3)
#define _MXNNOD(p,p0,p1) PmsMsg(1,M__NAME,__LINE__,p0,1,p,p1)
#define _MXNPAT(p,p0,p1) PmsMsg(2,M__NAME,__LINE__,p0,1,p,p1)
#define _MXNRPL(p,p0,p1) PmsMsg(3,M__NAME,__LINE__,p0,1,p,p1)
#define _INVROP(p,p0,p1) PmsMsg(4,M__NAME,__LINE__,p0,1,p,p1)
#define _MXNARG(p,p0,p1) PmsMsg(5,M__NAME,__LINE__,p0,1,p,p1)
#define _INVSYN(p,p0,p1) PmsMsg(6,M__NAME,__LINE__,p0,1,p,p1)
#define _PATNFD(p,p0,p1) PmsMsg(7,M__NAME,__LINE__,p0,1,p,p1)
#define _RPNNEX(p,p0,p1) PmsMsg(8,M__NAME,__LINE__,p0,1,p,p1)
#define _MXNVAR(p,p0,p1) PmsMsg(9,M__NAME,__LINE__,p0,1,p,p1)
#define _MXNMLG(p,p0,p1) PmsMsg(10,M__NAME,__LINE__,p0,1,p,p1)
#define _MXVLLG(p,p0,p1) PmsMsg(11,M__NAME,__LINE__,p0,1,p,p1)
#define _MXVAEX(p,p0,p1) PmsMsg(12,M__NAME,__LINE__,p0,1,p,p1)
#define _FCNNFD(p,p0,p1) PmsMsg(13,M__NAME,__LINE__,p0,1,p,p1)
#define _ARGERR(p,p0,p1) PmsMsg(14,M__NAME,__LINE__,p0,1,p,p1)
#define _PMINIT(p,p0,p1,p2,p3) PmsMsg(15,M__NAME,__LINE__,p0,3,p,p1,p2,p3)
#define _INTERR(p,p0,p1) PmsMsg(16,M__NAME,__LINE__,p0,1,p,p1)
#define _INVNOD(p,p0,p1) PmsMsg(17,M__NAME,__LINE__,p0,1,p,p1)
#define _INVSUB(p,p0) PmsMsg(18,M__NAME,__LINE__,p0,0,p)
#define _INVPAT(p,p0,p1) PmsMsg(19,M__NAME,__LINE__,p0,1,p,p1)
#define _MXNALT(p,p0,p1) PmsMsg(20,M__NAME,__LINE__,p0,1,p,p1)
#define _NOMRAL(p,p0) PmsMsg(21,M__NAME,__LINE__,p0,0,p)
#define _MXNINC(p,p0,p1) PmsMsg(22,M__NAME,__LINE__,p0,1,p,p1)
#define _NOMRIN(p,p0,p1) PmsMsg(23,M__NAME,__LINE__,p0,1,p,p1)
#define _MXNASS(p,p0,p1) PmsMsg(24,M__NAME,__LINE__,p0,1,p,p1)
#define _NOMRAS(p,p0) PmsMsg(25,M__NAME,__LINE__,p0,0,p)
#define _MXNPCP(p,p0,p1) PmsMsg(26,M__NAME,__LINE__,p0,1,p,p1)
#define _NOMRPC(p,p0) PmsMsg(27,M__NAME,__LINE__,p0,0,p)
#define _NOTIMP(p,p0,p1) PmsMsg(28,M__NAME,__LINE__,p0,1,p,p1)
#define _VARNST(p,p0,p1) PmsMsg(29,M__NAME,__LINE__,p0,1,p,p1)
#define _STRNST(p,p0,p1) PmsMsg(30,M__NAME,__LINE__,p0,1,p,p1)
#define _STRERR(p,p0,p1) PmsMsg(31,M__NAME,__LINE__,p0,1,p,p1)
#define _PRNBLD(p,p0,p1) PmsMsg(32,M__NAME,__LINE__,p0,1,p,p1)
#define _STXERR(p,p0,p1) PmsMsg(33,M__NAME,__LINE__,p0,1,p,p1)
#define _PMTERM(p,p0,p1,p2,p3) PmsMsg(34,M__NAME,__LINE__,p0,3,p,p1,p2,p3)
#define _PMFAIL(p,p0,p1,p2) PmsMsg(35,M__NAME,__LINE__,p0,2,p,p1,p2)
#define _PMSUCC(p,p0,p1,p2,p3) PmsMsg(36,M__NAME,__LINE__,p0,3,p,p1,p2,p3)
#define _ASGNER(p,p0,p1) PmsMsg(37,M__NAME,__LINE__,p0,1,p,p1)
#define _DUPNAM(p,p0,p1) PmsMsg(38,M__NAME,__LINE__,p0,1,p,p1)
#define _LNGNAM(p,p0,p1,p2) PmsMsg(39,M__NAME,__LINE__,p0,2,p,p1,p2)
#define _MXNSUB(p,p0) PmsMsg(40,M__NAME,__LINE__,p0,0,p)
#define _CALSLF(p,p0) PmsMsg(41,M__NAME,__LINE__,p0,0,p)
#define _MXLBUF(p,p0) PmsMsg(42,M__NAME,__LINE__,p0,0,p)
#define _ALTIGN(p,p0) PmsMsg(43,M__NAME,__LINE__,p0,0,p)
#define _MXNARB(p,p0) PmsMsg(44,M__NAME,__LINE__,p0,0,p)
#define _MXNSTP(p,p0,p1) PmsMsg(45,M__NAME,__LINE__,p0,1,p,p1)
#define _INVQOT(p,p0,p1,p2) PmsMsg(46,M__NAME,__LINE__,p0,2,p,p1,p2)
#define _NOMRAR(p,p0) PmsMsg(47,M__NAME,__LINE__,p0,0,p)
#define _MXNFOR(p,p0,p1) PmsMsg(48,M__NAME,__LINE__,p0,1,p,p1)
#define _CCTNAL(p,p0,p1) PmsMsg(49,M__NAME,__LINE__,p0,1,p,p1)
#define _VARNFD(p,p0,p1) PmsMsg(50,M__NAME,__LINE__,p0,1,p,p1)
#define _RESWRD(p,p0,p1) PmsMsg(51,M__NAME,__LINE__,p0,1,p,p1)
#define _PATNAM(p,p0,p1) PmsMsg(52,M__NAME,__LINE__,p0,1,p,p1)
