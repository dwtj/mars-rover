/*
 * comm.h
 *
 * Created: 2/20/2014 10:16:43 AM
 *  Author: Flavia and David
 */ 


#ifndef COMM_H
#define COMM_H

#include <avr/io.h>


#define NUM_QUERY_CODES 2

/* The following enumerated types are classes of header codes that head
   packets being sent between the the control program and the rover. */

/* The `query` codes are used by the control program to initiate a query
   sequence with the rover. The rover's responses are headed by the same
   code. */

typedef enum {
	query_null = 0,
	query_ping = 1,
	num_query_codes
} query;

/* Declares an array of functions, each of which will handle a query request. */
extern uint8_t (*query_handlers[num_query_codes])(char *);


bool is_valid_query_code(query type);



#endif /* COMM_H */