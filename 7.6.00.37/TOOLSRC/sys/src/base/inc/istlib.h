/*	istlib.h	

    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

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

#ifndef	_INGOS_LIBRARY
#define	_INGOS_LIBRARY

/*	abs_ph.c	*/
extern	int			abs_ph ();

/*	allocate.c	*/
#if defined(lint) && ! defined(__STDC__)
  /* avoid old lint's complaints about "possible pointer alignment problem" */
  extern	char			**p_calloc ();
  extern	char			**p_malloc ();
  extern	char			**p_realloc ();
#else
# define	p_calloc(_n_,_s_)	calloc(_n_,_s_)
# define	p_malloc(_s_)		malloc(_s_)
# define	p_realloc(_p_,_s_)	realloc(_p_,_s_)
#endif

/*	ascdate.c	*/
extern	char			*ascdate ();

/*	backslas.c	*/
#if MSDOS || OS2 || WIN32
extern	void			back2slash ();
#endif

/*	backup.c	*/
extern	int			backup_file ();
extern	char			*backup_append ;

/*	beep.c		*/
extern	void			putbeep ();
extern	int			beep_disable ;

/*	cat_file.c	*/
extern	int			cat_file ();

/*	checkbuf.c	*/
extern	int			check_buffer ();

/*	checkdir.c	*/
extern	int			check_file_dir ();
extern	int			test_file_dir ();
extern	int			check_dir ();
extern	int			test_dir ();

/*	cipher.c	*/
extern	int			cipher ();

/*	copyfile.c	*/
extern	int			copy_file ();

/*	dbg.c		*/
extern	void			dbgdbg ();
extern	int			dbgdbglvl ;
extern	int			dbgdbgfd ;

/*	dosfname.c	*/
#if MSDOS
extern	int			dos_filename_ok ();
#endif

/*	err.c		*/
extern	void			err ();
extern	int			errfd ;

/*	filename.c		*/
extern	int			filename_gen ();

/*	get_key.c	*/
extern	int			get_key ();
extern	int			init_key ();

/*	get_line.c	*/
extern	int			get_line ();

/*	get_host.c	*/
extern	int			get_host_name ();

/*	get_seq.c	*/
extern	int			get_sequence ();

/*	get_tty.c	*/
extern	int			get_tty_name ();
extern	char			*tty_name ;

/*	i_stat.c	*/
extern	int			i_stat ();

/*	interrpt.c	*/
extern	int			was_interrupted ();
extern	void			not_interrupted ();

/*	movefile.c	*/
extern	int			move_file ();

/*	out.c		*/
extern	void			out ();
extern	int			outfd ;

/*	outwrite.c		*/
extern	void			outwrite ();
extern	int			outwrite_timestamp ;

/*	pmt_bool.c	*/
extern	int			prompt_bool ();

/*	pmt_char.c	*/
extern	int			prompt_char ();

/*	pmt_long.c	*/
extern	long			prompt_long ();

/*	pmt_str.c	*/
extern	char			*prompt_string ();

/*	put_line.c	*/
extern	int			put_line ();

/*	put_seq.c	*/
extern	int			put_sequence ();

/*	regexp.c	*/
extern	int			compile_fileexp ();
extern	int			compile_regexp ();
extern	int			match_regexp ();
extern	void			clear_regexp ();
extern	int			no_fileexp_supp_dot ;
extern	char			*loc1 ;
extern	char			*loc2 ;
extern	char			*locs ;
extern	int			nbra ;
extern	int			sed ;
extern	char			*braslist [];
extern	char			*braelist [];

/*	rel_ph.c	*/
extern	int			rel_ph ();

/*	restore.c	*/
extern	int			restore_file ();

/*	security.c	*/
#if WIN32
extern	int			get_file_sec ();
extern	int			get_owner_name ();
extern	int			get_group_name ();
extern	int			set_file_sec ();
#endif

/*	serrno.c	*/
extern	char			*serrno ();

/*	signame.c	*/
extern	char			*signame ();

/*	skt_serr.c	*/
#if WIN32
extern	char			*skt_serrno ( void );
#endif

/*	spawn.c		*/
#if unix
# define	_P_WAIT		0
# define	_P_NOWAIT	1
# define	_P_OVERLAY	2
# define	_P_NOWAITO	3
# define	_P_DETACH	4
extern	int			_spawnv ();
extern	int			_spawnvp ();
#endif /*unix*/

/*	str_app.c	*/
extern	int			str_append ();

/*	str_cat.c	*/
extern	int			str_cat ();

/*	str_cpy.c	*/
extern	int			str_cpy ();

/*	str_crea.c	*/
extern	int			str_create ();

/*	str_time.c	*/
extern	int			str_time_load ();
extern	void			str_time_free ();
extern	int			str_time ();

/*	strcase.c	*/
#ifdef	unix
extern	int			strcasecmp ();
#else
# define	strcasecmp(_s1_,_s2_,_l_)	stricmp((_s1_),(_s2_),(_l_))
#endif	/*unix*/

/*	strncase.c	*/
#ifdef	unix
extern	int			strncasecmp ();
#else
# define	strncasecmp(_s1_,_s2_,_l_)	strnicmp((_s1_),(_s2_),(_l_))
#endif	/*unix*/

/*	strnsub.c	*/
extern  char    		*strnsub ();

/*	strsub.c	*/
extern  char    		*strsub ();

/*	termio.c	*/
extern	int			ini_term ();
extern	int			fin_term ();

/*	winsiz.c	*/
extern	int			get_window_size ();

/*	workdir.c	*/
extern	int			get_work_dir ();
extern	int			gwd_no_warn ;
extern	char			*work_dir ;

#endif /*_INGOS_LIBRARY*/

