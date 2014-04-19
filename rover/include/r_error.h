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
	error_txq = 0,
	error_bad_request = 1,
	error_bad_data_length =2,
} error;


// Custom error handling function.
void r_error(error err_num, char* mesg);


// A boolean flag indicating whether an error has occurred.
// Main execution should halt if this is set.
extern bool has_errored;


#endif /* R_ERROR_H_ */