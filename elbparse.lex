%{
/*
 * Author:: Matt Chapman (MatthewaChap@gmail.com)
 * 
 * Copyright (C) 2018  Matt Chapman
 *
 *             This program is free software: you can redistribute it and/or modify
 *             it under the terms of the GNU General Public License as published by
 *             the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 *             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *             GNU General Public License for more details.
 *
 *             You should have received a copy of the GNU General Public License
 *             along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>  
#include <error.h>
#include <malloc.h>
#include <zlib.h>
#include "elb_entry.h"

  gzFile gzyyin;

  char * current_filename;
  int lineno=1;
  
  static struct elb_entry e;
  enum elb_entry_field state=PROTOCOL;

  static void reset_e(void) {
    if (e.protocol) free(e.protocol);
    if (e.timestamp) free(e.timestamp);
    if (e.elb) free(e.elb);
    if (e.client_ipaddr) free(e.client_ipaddr);
    if (e.target_ipaddr) free(e.target_ipaddr);
    if (e.request) free(e.request);
    if (e.user_agent) free(e.user_agent);
    if (e.ssl_cipher) free(e.ssl_cipher);
    if (e.ssl_protocol) free(e.ssl_protocol);
    if (e.target_group_arn) free(e.target_group_arn);
    if (e.trace_id) free(e.trace_id);
    if (e.domain_name) free (e.domain_name);
    if (e.chosen_cert_arn) free (e.chosen_cert_arn);
    memset(&e, 0, sizeof(struct elb_entry) );
    state = PROTOCOL;
  }

#define YY_INPUT(buf, result, max_size) \
  { \
    int readret;                                \
    readret = gzread(gzyyin, buf, max_size);    \
    result = ( readret == -1 ) ? YY_NULL : readret; \
  }
  
  int yywrap(void)
  {
    lineno = 1;
    return 1;
  }
  
%}

IPADDR [0-9]+\.[0-9]+\.[0-9]+\.[0-9]+





%%
[ \t]            {
}

[\n]+ {
  found_aws_elb_entry(current_filename, lineno, &e, state);  
  if (state != PROTOCOL) {
    reset_e();
  }
  lineno += strlen(yytext);
}

"-"?[0-9]+\.[0-9]+ {

  switch (state) {
    
  case REQUEST_PROCESSING_TIME:
    e.request_processing_time = strtof(yytext, NULL);
    state++;
    break;
  case TARGET_PROCESSING_TIME:
    e.target_processing_time = strtof(yytext, NULL);
    state++;
    break;
  case RESPONSE_PROCESSING_TIME:
    e.response_processing_time = strtof(yytext, NULL);
    state ++;
    break;
  default:
    error_at_line(6,0,current_filename, lineno,
                  "float number at field %d, value is '%s'",
                  state, yytext);
    
  }
}


"-"|[0-9]+|-1      {
  int value;
  if (strcmp("-", yytext) == 0) {
    value = -999;
  } else {
    value = atoi(yytext);
  }
  
  
  switch (state) {
    
  case RECEIVED_BYTES:
    e.received_bytes = value;
    state++;
    break;
    
  case SENT_BYTES:
    e.sent_bytes = value;
    state++;
    break;
    
  case ELB_STATUS_CODE:
    e.elb_status_code = value;
    state++;
    break;
    
  case TARGET_STATUS_CODE:
    e.target_status_code = value;
    state++;
    break;

  case MATCHED_RULE_PRIORITY:
    e.matched_rule_priority = value;
    state++;
    break;

  case TARGET_PROCESSING_TIME:
    e.target_processing_time = strtof(yytext, NULL);
    state++;
    break;

  case RESPONSE_PROCESSING_TIME:
    e.response_processing_time = strtof(yytext, NULL);
    state ++;
    break;

    
  default:   
    error_at_line(6,0,current_filename, lineno,
                  "Unexpected integer at field %d, value is '%s'",
                  state, yytext);
    break;
  }
    
}

[\"][^\"]+[\"]          {
  char * text = strdup(yytext+1);
  text[strlen(text)-1] = 0;
  
  switch (state) {
    
  case REQUEST:
    e.request = text;
    state++;
    break;    
    
  case USER_AGENT:
    e.user_agent = text;
    state++;
    break;
  
  case TRACE_ID:
    e.trace_id = text;
    state++;
    break;
  
  case DOMAIN_NAME: 
    e.domain_name = text;
    state++;
    break;
  
  case CHOSEN_CERT_ARN:
    e.chosen_cert_arn = text;
    state++;
    break;
    
  default: 
    error_at_line(5,0,current_filename, lineno,
                  "Unexpected quoted string at field %d, value is '%s'",
                  state, yytext);
    break;

  }
 
}

{IPADDR}:[0-9]+ {
  char* text = strdup(yytext);

  char* colon_ptr= strrchr(text, ':');

  *colon_ptr = 0; // null terminate the ipaddr.
  colon_ptr ++; // advance to the port no.
  
  int port = atoi(colon_ptr);

  
  switch (state) {
    
  case CLIENT_PORT:
    e.client_ipaddr = text;
    e.client_port = port;
    state++;
    break;
    
  case TARGET_PORT:
    e.target_ipaddr = text;
    e.target_port = port;
    state++;
    break;
    
  default:
    error_at_line(5,0,current_filename, lineno,
                  "Unexpected ipaddr:port string at field %d, value is '%s'",
                  state, yytext);
    break;
  }
  
}
  

[^ \t\n\"]+ {
  char * text = strdup(yytext);
  switch(state) {
  case PROTOCOL:
    e.protocol = text;
    state++;
    break;
  case TIMESTAMP:
    e.timestamp=text;
    state++;
    break;
  case ELB:
    e.elb = text;
    state++;
    break;
  case REQUEST:
    e.request = text;
    state++;
    break;
  case USER_AGENT:
    e.user_agent = text;
    state++;
    break;
  case SSL_CIPHER:
    e.ssl_cipher = text;
    state++;
    break;
  case SSL_PROTOCOL:
    e.ssl_protocol = text;
    state++;
    break;
  case TARGET_GROUP_ARN:
    e.target_group_arn = text;
    state++;
    break;
  case TRACE_ID:
    e.trace_id = text;
    state++;
    break;
  case DOMAIN_NAME:
    e.domain_name = text;
    state++;
    break;
  case CHOSEN_CERT_ARN:
    e.chosen_cert_arn = text;
    state++;
    break;
  default:
    error_at_line(5,0,current_filename, lineno,
                  "Unexpected unquoted string at field %d, value is '%s'",
                  state, yytext);
    break;
    
  }
  
}

%%
