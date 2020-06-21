
;
;    ========== licence begin LGPL
;    Copyright (C) 2002 SAP AG
;
;    This library is free software; you can redistribute it and/or
;    modify it under the terms of the GNU Lesser General Public
;    License as published by the Free Software Foundation; either
;    version 2.1 of the License, or (at your option) any later version.
;
;    This library is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;    Lesser General Public License for more details.
;
;    You should have received a copy of the GNU Lesser General Public
;    License along with this library; if not, write to the Free Software
;    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;    ========== licence end
;
	.file   "stkset_dummy.s"

/	'stket_run' is called immediately after build up of the stack frame:
/		- saved %eip
/		- saved %ebp
/		- optionally allocated stack space ( subl $x,%esp )
/		- optionally saved nonvolatile registers (%ebp,%esi,%edi)
/	Thus, the stack contains additionally:
/		- saved %eip (the instruction to use after stkset_run)
/		- saved %edi (explicitly saved by stkset_run, see below)
/
/	The stack space to set up with the fill value is the
/	'optionally allocated stack space'. This is the stack location
/	from the %eip saved by stkset_run, optionally incremented by the
/	space used for the saved novolatile registers, up to the location
/	just below the saved %ebp.
/
/       stkset_run ( register_saved_mask )
/
/               1       %ebx
/               2       %esi
/               4       %edi

	.globl  stkset_run
	.text
stkset_run :
	ret

