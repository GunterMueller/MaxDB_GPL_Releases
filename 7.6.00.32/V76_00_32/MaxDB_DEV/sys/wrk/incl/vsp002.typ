
      tsp2_devname      = tsp00_VFilename;
      (**)
      tsp2_dev_type     = (
            sp2dt_sys, (* for migration *)
            sp2dt_log,
            sp2dt_mirr_log,
            sp2dt_data,
            sp2dt_trace);
      (**)
      tsp2_io_op_code   = (
            sp2ioc_read,
            sp2ioc_write);
      (* THE FOLLOWING MUST BE REMOVED BY RTE BECAUSE IT IS OBSOLETE *)
      (* LOOK INTO RTETask_ITask::TaskType - UH 2004-11-12           *)
      tsp2_process_type    = (
            sp2pt_rte,
            sp2pt_utility,
            sp2pt_timeout,
            sp2pt_user,
            sp2pt_filler1,
            sp2pt_log_writer,
            sp2pt_trace_writer,
            sp2pt_filler2,
            sp2pt_filler3,
            sp2pt_server,
            sp2pt_pager,
            sp2pt_filler4,
            sp2pt_event,
            sp2pt_backup,
            sp2pt_garbage_collector,
            sp2pt_filler5);
 
