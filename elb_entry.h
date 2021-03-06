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

#ifndef _ELB_ENTRY_H_
#define _ELB_ENTRY_H_

extern gzFile gzyyin;

typedef char * t_sptr;
struct elb_entry {
  t_sptr protocol;
  t_sptr timestamp;
  t_sptr elb;
  t_sptr client_ipaddr;
  int    client_port;
  t_sptr target_ipaddr;
  int    target_port;
  float  request_processing_time;
  float  target_processing_time;
  float  response_processing_time;
  int    elb_status_code;
  int    target_status_code;
  int    received_bytes;
  int    sent_bytes;
  t_sptr request;
  t_sptr user_agent;
  t_sptr ssl_cipher;
  t_sptr ssl_protocol;
  t_sptr target_group_arn;
  t_sptr trace_id;
  t_sptr domain_name;
  t_sptr chosen_cert_arn;
  int    matched_rule_priority;
};

enum elb_entry_field {
  PROTOCOL = 0,
  TIMESTAMP = 1,
  ELB =2,
  CLIENT_PORT=3,
  TARGET_PORT=4,
  REQUEST_PROCESSING_TIME=5,
  TARGET_PROCESSING_TIME=6,
  RESPONSE_PROCESSING_TIME=7,
  ELB_STATUS_CODE=8,
  TARGET_STATUS_CODE=9,
  RECEIVED_BYTES=10,
  SENT_BYTES=11,
  REQUEST=12,
  USER_AGENT=13,
  SSL_CIPHER=14,
  SSL_PROTOCOL=15,
  TARGET_GROUP_ARN=16,
  TRACE_ID=17,
  DOMAIN_NAME=18,
  CHOSEN_CERT_ARN=19,
  MATCHED_RULE_PRIORITY=20,
  EOL_FULL_PARSE=21,
};

int found_aws_elb_entry(const char* filename, int lineno, const struct elb_entry* entry, enum elb_entry_field state);


#endif /*_ELB_ENTRY_H_*/
