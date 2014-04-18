/*
 * r_error.h
 *
 * Created: 4/17/2014 9:19:51 PM
 *  Author: dwtj
 */ 


#ifndef R_ERROR_H_
#define R_ERROR_H_

// Custom error codes.
typedef enum {
	error_txq = 0,
	error_bad_request = 1,
} error;


// Custom error handling function.
void r_error(error err_num, char* msg);


// A boolean flag indicating whether an error has occurred.
// Main execution should halt if this is set.
extern bool has_errored;


#endif /* R_ERROR_H_ */