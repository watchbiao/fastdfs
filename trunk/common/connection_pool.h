/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//connection_pool.h

#ifndef _CONNECTION_POOL_H
#define _CONNECTION_POOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "common_define.h"
#include "hash.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	int sock;
	int port;
	char ip_addr[IP_ADDRESS_SIZE];
} ConnectionInfo;

typedef void (*conn_pool_init_func)(void *conn);
typedef int (*conn_pool_connect_func)(void *conn, const int connect_timeout);
typedef void (*conn_pool_disconnect_func)(void *conn);

typedef struct tagConnectionPool {
	conn_pool_connect_func connect_func;
	conn_pool_disconnect_func disconnect_func;
	int conn_size;
	HashArray hash_array;  //key is ip:port, value is ConnectionNode
} ConnectionPool;

typedef struct tagConnectionNode {
	void *conn;
	struct tagConnectionNode *next;
} ConnectionNode;

int conn_pool_init(ConnectionPool *cp, conn_pool_connect_func connect_func, 
	conn_pool_disconnect_func disconnect_func, const int conn_size);

void conn_pool_destroy(ConnectionPool *cp);

ConnectionNode *conn_pool_get_connection(ConnectionPool *cp, const void *conn);
void conn_pool_close_connection(ConnectionPool *cp, ConnectionNode *conn_node);

#ifdef __cplusplus
}
#endif

#endif

