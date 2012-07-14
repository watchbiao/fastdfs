/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//trunk_free_block_checker.c

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "fdfs_define.h"
#include "logger.h"
#include "shared_func.h"
#include "avl_tree.h"
#include "tracker_types.h"
#include "storage_global.h"
#include "trunk_free_block_checker.h"

static AVLTreeInfo tree_info_by_id = {NULL, NULL, NULL}; //for unique block nodes

static int storage_trunk_node_compare_entry(void *p1, void *p2)
{
	return memcmp(&(((FDFSTrunksById *)p1)->trunk_file_id), \
			&(((FDFSTrunksById *)p2)->trunk_file_id), \
			sizeof(FDFSTrunkFileIdentifier));
}

int trunk_free_block_checker_init()
{
	int result;
	if ((result=avl_tree_init(&tree_info_by_id, free, \
			storage_trunk_node_compare_entry)) != 0)
	{
		logError("file: "__FILE__", line: %d, " \
			"avl_tree_init fail, " \
			"errno: %d, error info: %s", \
			__LINE__, result, STRERROR(result));
		return result;
	}

	return 0;
}

void trunk_free_block_checker_destroy()
{
	avl_tree_destroy(&tree_info_by_id);
}

int trunk_free_block_tree_node_count()
{
	return avl_tree_count(&tree_info_by_id);
}

static int block_tree_walk_callback(void *data, void *args)
{
	int *pcount;
	pcount = (int *)args;

	*pcount += ((FDFSTrunksById *)data)->block_array.count;
	return 0;
}

int trunk_free_block_total_count()
{
	int count;
	count = 0;
	avl_tree_walk(&tree_info_by_id, block_tree_walk_callback, &count);
	return count;
}

int trunk_free_block_check_duplicate(FDFSTrunkFullInfo *pTrunkInfo)
{
	if (avl_tree_find(&tree_info_by_id, pTrunkInfo) != NULL)
	{
		char buff[256];
		logWarning("file: "__FILE__", line: %d, " \
			"node already exist, trunk entry: %s", __LINE__, \
			trunk_info_dump(pTrunkInfo, buff, sizeof(buff)));
		return EEXIST;
	}

	return 0;
}

int trunk_free_block_insert(FDFSTrunkFullInfo *pTrunkInfo)
{
	int result;
	if (avl_tree_insert(&tree_info_by_id, pTrunkInfo) != 1)
	{
		result = errno != 0 ? errno : ENOMEM;
		logError("file: "__FILE__", line: %d, " \
			"avl_tree_insert fail, " \
			"errno: %d, error info: %s", \
			__LINE__, result, STRERROR(result));
		return result;
	}
	else
	{
		return 0;
	}
}

int trunk_free_block_delete(FDFSTrunkFullInfo *pTrunkInfo)
{
	if (avl_tree_delete(&tree_info_by_id, pTrunkInfo) != 1)
	{
		char buff[256];
		logWarning("file: "__FILE__", line: %d, " \
			"can't delete block entry, trunk info: %s", \
			__LINE__, trunk_info_dump(pTrunkInfo, buff, \
			sizeof(buff)));
		return ENOENT;
	}

	return 0;
}

