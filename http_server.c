#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include <stdio.h>
#include <string.h>
#include "http_server.h"

#define LEN 1024
char data[LEN];

void http_server_serve(struct netconn *conn) 
{
    struct netbuf *inbuf;
    err_t res;
    char* buf;
    u16_t buflen;
    size_t len;
    static u16_t call_times = 0;
        
    res = netconn_recv(conn, &inbuf);
    
    if (res == ERR_OK)
    {
        netbuf_data(inbuf, (void**)&buf, &buflen);
        if ((buflen >=5) && (strncmp(buf, "GET /", 5) == 0))
        {
            sprintf(data, "Hello %d times!", call_times++);
            len = strlen(data);
            netconn_write(conn, (const unsigned char*)(data), (size_t)len, NETCONN_NOCOPY);
        }
    }
    
    /* Close the connection (server closes in HTTP) */
    netconn_close(conn);

    /* Delete the buffer (netconn_recv gives us ownership,
    so we have to make sure to deallocate the buffer) */
    netbuf_delete(inbuf);
}

void http_server_netconn_thread()
{ 
  struct netconn *conn, *newconn;
  err_t err;
  
  /* Create a new TCP connection handle */
  conn = netconn_new(NETCONN_TCP);
  
  if (conn!= NULL)
  {
    /* Bind to port 80 (HTTP) with default IP address */
    err = netconn_bind(conn, NULL, 80);
    
    if (err == ERR_OK)
    {
      /* Put the connection into LISTEN state */
      netconn_listen(conn);
  
      while(1) 
      {
        /* accept any icoming connection */
        err = netconn_accept(conn, &newconn);
      
        /* serve connection */
        http_server_serve(newconn);
      
        /* delete connection */
        netconn_delete(newconn);
      }
    }
    else
    {
      printf("can not bind netconn");
    }
  }
  else
  {
    printf("can not create netconn");
  }
}