/*---------------------------------------------------*/

/* Public Domain version of printf                   */

/*                                                   */

/* Rud Merriam, Compsult, Inc. Houston, Tx.          */

/*                                                   */

/* For Embedded Systems Programming, 1991            */

/*                                                   */

/*---------------------------------------------------*/



#include "rprintf.h"

/*---------------------------------------------------*/

/* The purpose of this routine is to output data the */

/* same as the standard printf function without the  */

/* overhead most run-time libraries involve. Usually */

/* the printf brings in many kiolbytes of code and   */

/* that is unacceptable in most embedded systems.    */

/*---------------------------------------------------*/



static func_ptr out_char;

static int do_padding;

static int left_flag;

static int len;

static int num1;

static int num2;

static char pad_
