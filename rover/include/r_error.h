/*
 * r_error.h
 *
 * Created: 4/17/2014 9:19:51 PM
 *  Author: dwtj
 */ 


#ifndef R_ERROR_H_
#define R_ERROR_H_

#include <stdbool.h>

// Custom error codes.
typedef enum {
    error_unknown = 0,      // An unknown kind of error occurred.
	error_txq = 1,          // Something went wrong when using `txq`.
	error_bad_message = 2,  // A malformed message was received from `control`.
    error_from_control = 3, // An error message was received from `control`.
    error_data_overrun = 4, // Data from `control` was lost by data overrun.
    error_parity = 5,
    error_frame = 6,
    error_unimplemented = 7,
} error;


//Error handler moved during refactoring.
void error_handler();


// Custom error handling function.
void r_error(error err_num, char* mesg);


// A boolean flag indicating whether an error has occurred.
// Main execution should halt if this is set.
extern bool has_errored;


// Temporary buffer for generating error messages. (Should be big enough.)
#define R_ERROR_BUF_SIZE 60
extern char r_error_buf[R_ERROR_BUF_SIZE];


#endif /* R_ERROR_H_ */
