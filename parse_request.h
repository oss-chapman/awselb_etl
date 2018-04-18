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
#ifndef _PARSE_REQUEST_H_
#define _PARSE_REQUEST_H_

struct parsed_request {
  // "GET https://jirasw.prd.nibr.novartis.net:8444/s/870534f3dea88c2ac9316156a3533164-CDN/-e4gdkl/73018/b6b48b2829824b869586ac216d119363/bca19ec1db8e505bd4bfde9425964399/_/download/contextbatch/css/jira.global.look-and-feel,-_super/batch.css?locale=en-US HTTP/1.1"
  char *method;
  char *url_protocol;
  char *hostname;
  int  port;
  char *path;
  char *args;
  char *protocol;
};


extern int parse_request(const char* request, struct parsed_request** req);
extern int free_request(struct parsed_request* req);

#endif //_PARSE_REQUEST_H_
