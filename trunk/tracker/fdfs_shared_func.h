/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

//fdfs_shared_func.h

#ifndef _FDFS_SHARED_FUNC_H
#define _FDFS_SHARED_FUNC_H

#include "common_define.h"
#include "tracker_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int fdfs_get_tracker_leader_index_ex(TrackerServerGroup *pServerGroup, \
		const char *leaderIp, const int leaderPort);

int fdfs_parse_storage_reserved_space(IniContext *pIniContext, \
		FDFSStorageReservedSpace *pStorageReservedSpace);

const char *fdfs_storage_reserved_space_to_string(FDFSStorageReservedSpace \
			*pStorageReservedSpace, char *buff);

#ifdef __cplusplus
}
#endif

#endif
