/*
 * File:   main.c
 * Author: dyn
 *
 * Created on November 13, 2013, 5:55 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include "httpd_svc_api.h"
#include "dgt_ntp.h"
#include "dgt_http.h"
/*
 *
 */
int main(int argc, char** argv) {

	// start HTTP protocol handler
	dgt_ntp_start();

	httpd_start();

	// start HTTP request handler
	dgt_http_req_start();

	return (EXIT_SUCCESS);
}

