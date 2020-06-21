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

#define MSG_NO    53
#define MSG_LEN   83

static char * MsgLst[MSG_NO] =
{
"TSTMSG short %hd long %ld string %s",
"MXNNOD Error %hd: pattern has too many nodes",
"MXNPAT Error %hd: too many patterns for table ",
"MXNRPL Error %hd: too many replacement definitions",
"INVROP Error %hd: invalid RPN-Operation",
"MXNARG Error %hd: too many RPN arguments on stack",
"INVSYN Error %hd: invalid pattern syntax ",
"PATNFD Pattern '%s' not found",
"RPNNEX Error %hd: RPN stack not exhausted",
"MXNVAR maximum number of variables is %hd for table",
"MXNMLG Error %hd: variable name too long ",
"MXVLLG Error %hd: value too long for entry",
"MXVAEX Error %hd:  variable exists already",
"FCNNFD function name '%s' not found",
"ARGERR error in argument '%s' ",
"PMINIT rc %d: %s for '%s' failed ",
"INTERR internal error in '%s' ",
"INVNOD Error %hd: invalid node",
"INVSUB invalid sub???",
"INVPAT invalid pattern '%s'",
"MXNALT maximum number (%HD) of alternations exceeded",
"NOMRAL no more alternations",
"MXNINC maximum number (%HD) of incarnations exceeded",
"NOMRIN no more incarnations",
"MXNASS maximum number (%HD) of assignments exceeded",
"NOMRAS no more assignments in stack",
"MXNPCP too many precursor pushes (%hd) for assignment  ",
"NOMRPC no more precursors in stack",
"NOTIMP '%s' not implemented",
"VARNST variable '%s' not stored",
"STRNST string '%s' not stored",
"STRERR string error in '%s'",
"PRNBLD primitive '%s' could not be built",
"STXERR syntax error in '%s'",
"PMTERM rc %d: %s for '%s' failed ",
"PMFAIL pattern %s > %s",
"PMSUCC c: %d %s > %s ",
"ASGNER assignment requires a variable, not a constant '%s'",
"DUPNAM duplicate name '%s'",
"LNGNAM name '%s' has more than %hd characters",
"MXNSUB too many sub calls",
"CALSLF pattern calls itself",
"MXLBUF maximum length of buffer exceeded, replacement incomplete ",
"ALTIGN alternation following ARBNO ignored",
"MXNARB too many nested ARBNOs  ",
"MXNSTP maximum number (%hd * 1000000) steps executed  ",
"INVQOT invalid quote usage in pattern '%s' body '%s'  ",
"NOMRAR no more iterations in stack",
"MXNFOR maximum number %s iterations exceeded, replaced by zero ",
"CCTNAL String concatenation is not allowed with string set '%s' ",
"VARNFD Variable '%s' not found",
"RESWRD '%s' is a reserved word, cannot be used as pattern or variable name",
"PATNAM '%s' is a pattern name"
};
