/*      vmakmisc.c       

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
#ifndef         _VMAKMISC_H
#define         _VMAKMISC_H

#include "vmake.h"


global	int     get_file        (char*,char*,char*,char*,int,valid_bit_arr, char**,time_t*,int*,int);
global	int     get_directory   (char*,int,valid_bit_arr, char**);
global	int		get_dates_file  (struct trg_info *,int,time_t,valid_bit_arr);
global	int		new_file        (char*,char*,char*,char*,char**);
global	int		vcopy_file      (char*,char*,time_t,int);
global	int		copy_file       (char*,char*,struct stat*);
global	int		get_date_and_size (char*,time_t*,long*, int);
global	int		touch_file      (struct trg_info*,char*,time_t,char*,int);
global	int		make_dates_file (struct trg_info*, time_t);
global	int		check_buffer    (char**,unsigned*,unsigned);
global	int		allocate_string (char**,char*);
global	char	* palloc       (unsigned);
global	char	* repalloc     (char*,unsigned);
global	char	* ascdate       (time_t);
global	char	* versname      (char);
global	char	getdefvers      (char);
global	int		move_file       (char*,char*);
global	int		link_file       (char*,char*);
global	int		unlink_file     (char*);
global	int		cmdpgm          (char*);
global	int		cmdscript       (char*);
global	int		cmdarg          (int, char*, ...);

global  int     cmdarg_obj      (int, char, char *); /* CTS 1105495 */

global	void	outstamp        (char*, ...);
global	void	voutwrite       (char*, ...);
global	void	outstamp_add    (char*, ...);
global	void	mprotwrite      (char*, ...);
global	int		check_dir       (char*);
global	int		cmdexe          (void);
global	void	set_protocol    (char*,char);
global	void	delete_protocol (char*,char);
global	int		set_work_dir    (char);
global	int		set_work_dir_in_objdir    (char, char*);

global	void	dowrite         (char*,int);
global	void	dowrite_to_protocols (char*,int);
global	void	dowrite_to_console (char*,int);
global	char	* serrno        (void);
global	void	exit_vmake      (int, int);
global	void	vmake_term      (void);
global	void	whatmod         (char*,time_t);
global  int		is_global_include (char*);
global  void    get_layer       (char*, char**);
global  int     cmp_layer       (char*, char*);
global  int     is_include      (char*);
global  char    * get_extension (char*);
global  char    * skip_extension(char*); /*  &gar CTS 1000237 */
global  char    * skip_index    (char*);
global  int     is_oldstyled_source (char*,char**,char[NAMESIZE]);
global  int     is_qualified_source(char*,char**,char[NAMESIZE]);
global  int     is_mapped_source(char*,char**,char[NAMESIZE]); /* PTS 1105037 */
global  int     is_valid_sourcename (char*,char**,char[NAMESIZE]);
global  int     is_qualified_desc(char*,char**,char[NAMESIZE]);
global  int     is_valid_descname(char*,char**,char[ NAMESIZE ]);
global  char    * get_basename(char*,char*);

global	int		get_intern_variables(char*,char**,struct trg_info*);
global  int     get_variants(char*,char	**,struct variant_info**);
global  int     insert_variant(struct variant_info**,char*, int);
global  int     insert_variants(struct variant_info**,struct variant_info*);
global  int     append_global_variants(struct variant_info**,struct variant_info*);
global  int		check_variants (char *,struct variant_info *);
global  int		build_full_variant_name(char **,char *,struct variant_info *);
global  int     create_globalvariant_filename(char*,char**,struct variant_info*);
global  int		cut_global_variants(char*);
global  int		get_global_variant_dir(struct variant_info*,char*);
global  int		substitute_internal_variables(char **,struct trg_info *);
global	int		substitute_all  (char*,char**, char);

global int recursive_dircopy(char*,char*,int, int);
global int allcopy(struct dir_entry*,char*,char*,int, int);


global  int     test_for_variants(char*);
global  void    remove_ext(char*,char*);
global int      propagate_value (struct env_info*);
global int      propagate_env (struct env_info*);

global int      is_absolute_path (char*);
global int     set_finished (struct trg_info *);

/* PTS 1102705 */
/*                                     directory, source, target */
global int rename_file_case_sensitive ( char *, char* , char* );

/* PTS 1104605 */
char * extension_exists ( char * );

global int init_wrk (void); /* CTS 1105673/2 */
global void set_DIR_WRK();
global int init_outdir (void);

global int is_mapped_source_with_full_path(char *, char**, char[NAMESIZE ]);

global int     createPIDFile (pid_t);
global int     unlinkPIDFile (int);
global int     writePIDtoFile(pid_t);
global int     checkforPIDFile ();

/* CTS 1112006 */
global int clear_outputlist ();
global int add_to_outputlist (char *, int);
global int flush_outputlist (struct trg_info*, int, int);
global int ewrite_targetheader (struct trg_info*, int);
global void eprotwrite (char*, ...);
global	int ewrite_all (struct trg_info*, int, int, char* ,...);
global int wwrite_targetheader (struct trg_info*, int);
global void wprotwrite (char*, ...);


global	int		read_dir_info    (char*,struct dir_info**);

global int unlock (int);
global int setLock (int);
global int testLock (int);
global FILE* get_tmp_file();

#endif

