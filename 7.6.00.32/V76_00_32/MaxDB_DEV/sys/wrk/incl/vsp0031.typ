
      tsp31_event_ident = (
            sp31ei_nil,                (* not valid                        *)
            sp31ei_filler1,                                               (* was alive *)
            sp31ei_autosave,
            sp31ei_filler3,                                               (* was backup_pages *)
            sp31ei_filler4,                                               (* was datacache_hitrate *)
            sp31ei_db_filling_above_limit,
            sp31ei_db_filling_below_limit,
            sp31ei_event,              (* if an event was set/deleted *)
            sp31ei_log_above_limit,
            sp31ei_filler9,                                               (* was restart_shutdown *)
            sp31ei_filler10,                                              (* was sessions *)
            sp31ei_upd_stat_wanted,
            sp31ei_filler12, 
            sp31ei_error,                                             (* was warning *)
            sp31ei_admin,
            sp31ei_backupresult,
            sp31ei_checkdata,
            sp31ei_databasefull,
            sp31ei_logfull,
            sp31ei_online,
            sp31ei_outofsessions,
            sp31ei_standby,
            sp31ei_systemerror,
            sp31ei_logsegmentfull,
            sp31ei_last_but_not_used)  (* !!! has to be the last in enum *);
      (* *)
      tsp31_event_prio = (
            sp31ep_nil,
            sp31ep_low,
            sp31ep_medium,
            sp31ep_high);
      (* *)
      tsp31_event_text = PACKED ARRAY
            [csp31_min_text_field..csp31_max_text_field] OF char;
      (* *)
      tsp31_event_description = RECORD
            sp31ed_date         : tsp00_Date;             (* only set by rte *)
            sp31ed_time         : tsp00_Time;             (* only set by rte *)
            (**)
            sp31ed_value_1      : tsp00_Int4;
            sp31ed_value_2      : tsp00_Int4;
            (**)
            sp31ed_eventcnt     : tsp00_Int4;
            sp31ed_ident        : tsp31_event_ident;
            sp31ed_priority     : tsp31_event_prio;
            sp31ed_text_len     : tsp00_Int2;
            (**)
            sp31ed_text_value   : tsp31_event_text
      END;
 
