line 1 appears
&define	A
line 3 appears
&undef	A
&define	A	7
&define	B	T/35.7-8
&define	C	pc_sco	# comment
&define	D	4
&define	E	$LOGNAME
line 10 appears
&ifdef	A
	line 12 appears
&endif	
line 14 appears
&ifndef	A
	line 16 suppressed
&endif	
line 18 appears
&if	A == 7	#NOTE: not $A (no substitution, string 'A' is meant)
	line 20 suppressed
&elif	(A != abc#def) && ($D == 4)	#again string 'A' is meant
	line 22 appears
&elif	!(A==7)				#again string 'A' is meant
	line 24 suppressed
&else
	line 26 suppressed
&endif	#comment
line 28 appears
&if	$C in [ T/35 , T/31 , $B ]
	line 30 suppressed
&elif	$B in [ T/31 , T/35.7-8 , pc_sco ]
	line 32 appears
&elif	$C !in [ T/35 , T/31 , $B ]
	line 34 suppressed
&elif	1
	line 36 suppressed
&elif	1
	line 38 suppressed
&endif
line 40 appears
&if	1
&	if	0
		line 43 suppressed
&		if	1
			line 45 suppressed
&		else
			line 47 suppressed
&		endif
		line 49 suppressed
&	else
&		if	1
			line 52 appears
&			if	0
				line 54 suppressed
&			elif	0
				line 56 suppressed
&			elif	1
				line 58 appears
&				if	0
					line 60 suppressed
&				else
					line 62 appears
&				endif
				line 64 appears
&				if	1
					line 66 appears
&				else
					line 68 suppressed
&				endif
				line 70 appears
&			elif	0
				line 72 suppressed
&			else
				line 74 suppressed
&			endif
			line 76 appears
&		else
			line 78 suppressed
&		endif
&	endif
&else
	line 82 suppressed
&	if	0
		line 84 suppressed
&	else
		line 86 suppressed
&		if	1
			line 88 suppressed
&		else
&		endif
&	endif
&endif
line 93 appears
&if	$C in [ T/35 , T/31 , $B , pc_sco ]
	line 95 appears
&elif	$B in [ T/31 , T/35.7-8 , pc_sco ]
	line 97 suppressed
&elif	$C !in [ T/35 , T/31 , $B ]
	line 99 suppressed
&elif	1
	line 101 suppressed
&else	#comment
	line 103 suppressed
&endif
line 105 appears
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Warnings:
&define	A	8
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Errors:
&define	x	pc_sco	pc_att
&endif
&if
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! New Features:
line 113 appears
&define x	"pc_sco pc_att"
&if	$x = "pc_sco pc_att"
line 116 appears
&endif
!!! -DIGNORECASE ???
&UNDEF	X
!!! if without endif
&ifdef	a
