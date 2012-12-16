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

int conn_pool_init(ConnectionPool *cp, const int max_count_per_entry)
{
	cp->max_count_per_entry = max_count_per_entry;

	return hash_init(&(cp->hash_array), simple_hash, 10, 0.75);
}

void conn_pool_destroy(ConnectionPool *cp)
{
}

void conn_pool_disconnect_server(ConnectionInfo *pConnection)
{
	if (pConnection->sock >= 0)
	{
		close(pConnection->sock);
		pConnection->sock = -1;
	}
}

int conn_pool_connect_server(ConnectionInfo *pConnection, \
		const int connect_timeout)
{
	int result;

	if (pConnection->sock >= 0)
	{
		close(pConnection->sock);
	}

	pConnection->sock = socket(AF_INET, SOCK_STREAM, 0);
	if(pConnection->sock < 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"socket create failed, errno: %d, " \
			"error info: %s", __LINE__, errno, STRERROR(errno));
		return errno != 0 ? errno : EPERM;
	}

	if ((result=tcpsetnonblockopt(pConnection->sock)) != 0)
	{
		close(pConnection->sock);
		pConnection->sock = -1;
		return result;
	}

	if ((result=connectserverbyip_nb(pConnection->sock, \
		pConnection->ip_addr, pConnection->port, \
		connect_timeout)) != 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"connect to %s:%d fail, errno: %d, " \
			"error info: %s", __LINE__, pConnection->ip_addr, \
			pConnection->port, result, STRERROR(result));

		close(pConnection->sock);
		pConnection->sock = -1;
		return result;
	}

	return 0;
}

