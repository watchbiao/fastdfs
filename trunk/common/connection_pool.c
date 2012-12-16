/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "logger.h"
#include "sockopt.h"
#include "shared_func.h"
#include "connection_pool.h"

int conn_pool_init(ConnectionPool *cp, conn_pool_connect_func connect_func, 
	conn_pool_disconnect_func disconnect_func, const int conn_size)
{
	cp->connect_func = connect_func;
	cp->disconnect_func = diconnect_func;
	cp->conn_size = conn_size;

	return hash_init(&(cp->hash_array), simple_hash, 10, 0.75);
}

void conn_pool_destroy(ConnectionPool *cp);
{
}

