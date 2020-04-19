#include "sd_log.h"

/// @file

/// Appender for USB Serial output
static plog::SerialAppender<plog::TxtFormatter> serialAppender(Serial);
/// Appender to save system log to SD card
static plog::SimpleFileAppender<plog::TxtFormatter> sdAppender("syslog.txt");

void syslog_init(void){
  // Initialize plog with Serial outout and SD output
  plog::init(plog::verbose, &serialAppender).addAppender(&sdAppender);
  // If watchdog caused reset, record the issue to file
  if( FeatherFault::DidFault() ){
    FeatherFault::FaultData fd = FeatherFault::GetFault();
    char * cause_msg;
    switch (fd.cause) {
      case FeatherFault::FAULT_HUNG: cause_msg="watchdog timeout"; break;
      case FeatherFault::FAULT_HARDFAULT: cause_msg="hard fault"; break;
      case FeatherFault::FAULT_OUTOFMEMORY: cause_msg="stack/heap overlap"; break;
      default: cause_msg="watchdog handler";
    }
    PLOG_FATAL << "Watchdog reset #"<< fd.failnum 
    << " caused by " << cause_msg
    << " after line " << fd.line << " in " << fd.file;
  }
}

bool inrange(int treeID, char * name, double val, double min, double max){
  if( val > max ){
    PLOG_WARNING << "tree" << treeID << " "<< name<< " (" << val <<
    ") is above max value(" << max << ")";
    return false;
  } else if (val < min ){
    PLOG_WARNING << "tree" << treeID << " "<< name<< " (" << val <<
    ") is below min value(" << min << ")";
    return false;
  }
  return true;
}
