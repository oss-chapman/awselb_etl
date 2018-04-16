

#include <stdlib.h>
#include <string.h>
#include "parse_request.h"


int parse_request(const char* request, struct parsed_request** req)
{
  char* in;
  char* end;
  char* div;

  *req = calloc(1, sizeof(struct parsed_request));
  in = (*req)->method = strdup(request);
  end =  in + strlen(in);

  // reset everything to empty string:
  (*req)->url_protocol = end;
  (*req)->hostname = end;
  (*req)->port = -1;
  (*req)->path = end;
  (*req)->args = end;
  (*req)->protocol = end;

  // this slices off method from the front.
  div = strchr(in, ' ');
  if (div == NULL) return 0;
  div[0] = 0;
  in=div+1;
  if (in >= end) return 0;
    
  // pop the end off, (HTTP/1.1)
  div = strrchr(in, ' ');
  if (div == NULL) return 0;
  div[0] = 0;
  (*req)->protocol = div+1;
  end = div;
  if (in >= end) return 0;
  
  // now we have the middle [in, end]. proto://hostname:port/path/stuff?args=values
  div = strchr(in, ':');
  if (div == NULL) return 0;
  div[0] = 0;
  (*req)->url_protocol = in;
  in = div + 3 ; // skip '://'
  if (in >= end) return 0;
  
  // hostname(:port)?
  (*req)->hostname = in;
  div = strchr(in, '/');
  if (div != NULL) {
    div[0] = 0;
    in = div+1;
  } else {
    in = end;
  }
  div = strrchr((*req)->hostname, ':');
  if (div == NULL) {
    (*req)->port = -1;
  } else {
    div[0] = 0;
    (*req)->port = atoi(div+1);
  }
  if (in >= end) return 0;
  
  // get the path, (everything between host and ?)
  div = strchr(in, '?');
  if (div != NULL) {
    div[0] = 0;
  }
  (*req)->path = in;
  if (div != NULL) {
    // whatever is left is the args
    (*req)->args = div+1;
  }
  return 0;  
}

int free_request(struct parsed_request* req)
{
  free(req->method);
  free(req);
  return 0;
}
  
