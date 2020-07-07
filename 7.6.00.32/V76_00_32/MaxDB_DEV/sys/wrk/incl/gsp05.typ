
      tsp05_RteFileWarning = (
            sp5vfw_no_warning,
            sp5vfw_no_eol_found,
            sp5vfw_no_next_tape,
            sp5vfw_already_locked);
 
      tsp05_RteFileError = RECORD
            sp5fe_result       : tsp00_VfReturn;
            sp5fe_warning      : tsp05_RteFileWarning;
            filler             : tsp00_C2;
            sp5fe_text         : tsp00_C40;
      END;
 
      tsp05_RteDataKind = (
            sp5vf_text,
            sp5vf_binary,
            sp5vf_encoded);
            
      tsp05_RteFileMode = (
            sp5vf_read,
            sp5vf_write,
            sp5vf_readwrite,
            sp5vf_append);
            
      tsp05_RteBufferingKind = (
            sp5bk_buffered,
            sp5bk_unbuffered,
            sp5bk_disksynced);
            
      tsp05_RteCloseOption = (
            sp5vf_close_normal,
            sp5vf_close_rewind,
            sp5vf_close_next_tape);
      tsp05_RteSeekKind = (
            sp5vf_seek_begin,
            sp5vf_seek_cur,
            sp5vf_seek_end);
 
      tsp05_RteSize64 = ARRAY [1..2] of tsp00_Int4 (*ptoc00Ignore*);
      (*inlineC typedef teo00_Uint8 tsp05_RteSize64;*)

      tsp05_RteFileInfo = RECORD
            sp5fi_exists        : boolean;
            sp5fi_readable      : boolean;
            sp5fi_writeable     : boolean;
            sp5fi_is_link       : boolean;
            sp5fi_size          : tsp00_Longint;
            sp5fi_size_x        : tsp00_Longint;
            sp5fi_size64        : tsp05_RteSize64; (* PTS 1105446 *)
            sp5fi_date_modified : tsp00_Date;
            sp5fi_time_modified : tsp00_Time;
            sp5fi_media_kind    : tsp00_VfType;
      END;
      
      (*inlineC typedef struct tsp05_SpecialEnumeration tsp05_SpecialEnumeration; *)
      
      tsp05_SpecialdevKind = (
            sdk_none_sp05,
            sdk_tapes_sp05,
            sdk_drives_sp05);

 
