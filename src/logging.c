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
static bool echo_to_console = true;

// File objects for log files
static FILE* std_log_file = NULL;
static FILE* err_log_file = NULL;

// Day of last open. Used to rotate files.
static int log_file_day = 0;

// Default log dir
static const char* DEFAULT_LOG_DIR = "/etc/webserver/logs";

// Logging targets. Can be binary-OR'd together.
enum ELogTarget {
  LOG_TARGET_STD = 0x1 << 0,  // Standard log file
  LOG_TARGET_ERR = 0x1 << 1  // Error log file
};

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
  if(timeinfo->tm_mday != log_file_day) {
    // Close existing files
    close_log_files();

    // Get the log file names
    char date[10];
    strftime(date, 10, "%Y%m%d", timeinfo);
    char std_log_file_path[1024];
    char err_log_file_path[1024];
    sprintf(std_log_file_path, "%s/webserver-std-%s.log", DEFAULT_LOG_DIR, date);
    sprintf(err_log_file_path, "%s/webserver-err-%s.log", DEFAULT_LOG_DIR, date);

    // Open the log files
    std_log_file = fopen(std_log_file_path, "a");
    if(!std_log_file) {
      fprintf(stderr, "Unable to open log file %s (errno: %i)\n", std_log_file_path, errno);
      return get_status(false);
    }
    err_log_file = fopen(err_log_file_path, "a");
    if(!err_log_file) {
      fprintf(stderr, "Unable to open log file %s (errno: %i)\n", err_log_file_path, errno);
      return get_status(false);
    }

    // Update the log file date
    log_file_day = timeinfo->tm_mday;
  }

  return get_status(true);
}

void write_log_line(FILE* file, timebuf_t tb, const char* format, va_list args) {
  fprintf(file, "%s | ", tb);    // Print timestamp
  vfprintf(file, format, args);  // Print formatted message
  fprintf(file, "\n");           // Print newline
  fflush(file);                  // Flush stream
}

// Helper function used by logging functions
// - Takes a log type, rather than a FILE*, because the log's FILE* value may
//   change after calling rotate_log_files().
void log_helper(enum ELogTarget target, FILE* ostream, const char* format, va_list orig_args)
{
  // Get a timestamp for the current time
  timebuf_t tb;
  timestamp(tb);

  // Figure out where we're logging
  const bool log_std = (target & LOG_TARGET_STD);
  const bool log_err = (target & LOG_TARGET_ERR);

  // Rotate log files if logging to disk
  if(log_std || log_err) {
    if(rotate_log_files().ok) {
      if(log_std) {
        va_list args;
        va_copy(args, orig_args);
        write_log_line(std_log_file, tb, format, args);
      }
      if(log_err) {
        va_list args;
        va_copy(args, orig_args);
        write_log_line(err_log_file, tb, format, args);
      }
    }
  }

  // Log to console
  if(ostream) {
    va_list args;
    va_copy(args, orig_args);
    write_log_line(ostream, tb, format, args);
  }
}

//==============================================================================
// Logging functions
//==============================================================================
void echo_log_to_console(bool echo) {
  if(echo != echo_to_console) {
    echo_to_console = echo;
    close_log_files();
  }
}

Status open_log_files() {
  return rotate_log_files();
}

void close_log_files() {
  if(std_log_file) { fclose(std_log_file); std_log_file = NULL; }
  if(err_log_file) { fclose(err_log_file); err_log_file = NULL; }
}

void log_std(const char* format, ...) {
  FILE* stream = (echo_to_console ? stdout : NULL);
  va_list args;
  va_start(args, format);
  log_helper(LOG_TARGET_STD, stream, format, args);
  va_end(args);
}

void log_err(const char* format, ...) {
  FILE* stream = (echo_to_console ? stderr : NULL);
  va_list args;
  va_start(args, format);
  log_helper(LOG_TARGET_ERR, stream, format, args);
  va_end(args);
}

void log_all(const char* format, ...) {
  va_list args;
  va_start(args, format);
  log_helper(LOG_TARGET_STD | LOG_TARGET_ERR, stdout, format, args);
  va_end(args);
}
