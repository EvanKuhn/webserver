#include "logging.h"
#include "utils.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>

//==============================================================================
// Data and constants
//==============================================================================
// Write log messages to console?
static bool ECHO_TO_CONSOLE = true;

// File objects for log files
static FILE* STD_LOG_FILE = NULL;
static FILE* ERR_LOG_FILE = NULL;

// Day of last open. Used to rotate files.
static int LOG_FILE_DAY = 0;

// Default log dir
static const char* DEFAULT_LOG_DIR = "/etc/webserver/logs";

// Log types
enum ELogType { LOG_TYPE_STD, LOG_TYPE_ERR };

//==============================================================================
// Utility functions
//==============================================================================
// Make sure the log files are open and have the correct filename (by date)
Status rotate_log_files() {
  // Get the current day
  time_t rawtime;
  time(&rawtime);
  struct tm* timeinfo = gmtime(&rawtime);

  // If the current day is different than the log file day, rotate
  if(timeinfo->tm_mday != LOG_FILE_DAY) {
    // Close existing files
    close_log_files();

    // Get the log file names
    char date[10];
    strftime(date, 10, "%Y%m%d", timeinfo);
    char std_log_file[1024];
    char err_log_file[1024];
    sprintf(std_log_file, "%s/webserver-std-%s.log", DEFAULT_LOG_DIR, date);
    sprintf(err_log_file, "%s/webserver-err-%s.log", DEFAULT_LOG_DIR, date);

    // Open the log files
    STD_LOG_FILE = fopen(std_log_file, "a");
    if(!STD_LOG_FILE) {
      fprintf(stderr, "Unable to open log file %s (errno: %i)\n", std_log_file, errno);
      return get_status(false);
    }
    ERR_LOG_FILE = fopen(err_log_file, "a");
    if(!ERR_LOG_FILE) {
      fprintf(stderr, "Unable to open log file %s (errno: %i)\n", err_log_file, errno);
      return get_status(false);
    }

    // Update the log file date
    LOG_FILE_DAY = timeinfo->tm_mday;
  }

  return get_status(true);
}

// Helper function used by logging functions
// - Takes a log type, rather than a FILE*, because the log's FILE* value may
//   change after calling rotate_log_files().
void log_helper(enum ELogType type, const char* format, va_list args) {
  // Get a timestamp for the current time
  timebuf_t tb;
  timestamp(tb);

  // Copy the args in case we need them later
  va_list args2;
  va_copy(args2, args);

  // Log to files
  if(rotate_log_files().ok) {
    FILE* file = (type == LOG_TYPE_ERR ? ERR_LOG_FILE : STD_LOG_FILE);
    fprintf(file, "%s | ", tb);    // Timestamp
    vfprintf(file, format, args);  // Formatted message
    fprintf(file, "\n");           // Newline
    fflush(file);
  }

  // Repeat the logging to stdout/stderr
  if(ECHO_TO_CONSOLE) {
    FILE* file = (type == LOG_TYPE_ERR ? stderr : stdout);
    fprintf(file, "%s | ", tb);     // Timestamp
    vfprintf(file, format, args2);  // Formatted message
    fprintf(file, "\n");            // Newline
    fflush(file);
  }
}

//==============================================================================
// Logging functions
//==============================================================================
void echo_log_to_console(bool echo) {
  if(echo != ECHO_TO_CONSOLE) {
    ECHO_TO_CONSOLE = echo;
    close_log_files();
  }
}

Status open_log_files() {
  return rotate_log_files();
}

void close_log_files() {
  if(STD_LOG_FILE) { fclose(STD_LOG_FILE); STD_LOG_FILE = NULL; }
  if(ERR_LOG_FILE) { fclose(ERR_LOG_FILE); ERR_LOG_FILE = NULL; }
}

void stdlog(const char* format, ...) {
  va_list args;
  va_start(args, format);
  log_helper(LOG_TYPE_STD, format, args);
  va_end(args);
}

void errlog(const char* format, ...) {
  va_list args;
  va_start(args, format);
  log_helper(LOG_TYPE_ERR, format, args);
  va_end(args);
}
