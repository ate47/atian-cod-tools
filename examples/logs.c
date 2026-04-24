#include <stdio.h>
#include <acts_api/version.h>
#include <acts_api/logs.h>


void example_logs() {
	// see api.c
	if (ACTS_NOT_OK(ActsAPIVersion_ValidateVersion())) {
		fprintf(stderr, "ActsAPIVersion_ValidateVersion error : %s\n", ActsGetAPILastMessage());
		return; // we should not continue after failing a version
	}

	// can be used to log messages with different log levels
	ACTSLOG_INFO("This is an info log");
	ACTSLOG_WARNING("This is a warning log");
	ACTSLOG_ERROR("This is an error log");

	// can also be used with formatting
	ACTSLOG_INFOF("This is a log with a number : %d", 42);

	// by default the logger is set to log info and above, so debug and 
	// trace logs are ignored, but we can change the log level to log them.
	ActsAPILog_SetLevel(LVL_TRACE);

	// now debug and trace logs will be printed
	ACTSLOG_DEBUG("This is a debug log");
	ACTSLOG_TRACE("This is a trace log");

	// the logger can print the source file and line of the log, but only 
	// if the log level is set to LVL_TRACE_PATH, otherwise it will ignore
	// it to avoid the performance cost of getting the source file and line.
	ActsAPILog_SetLevel(LVL_TRACE_PATH);

	ACTSLOG_INFO("This log will contain the file name");
}