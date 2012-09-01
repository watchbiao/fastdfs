/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

#include <stdlib.h>
#include <string.h>
#include "ini_file_reader.h"
#include "logger.h"
#include "shared_func.h"
#include "fdfs_shared_func.h"

int fdfs_get_tracker_leader_index_ex(TrackerServerGroup *pServerGroup, \
		const char *leaderIp, const int leaderPort)
{
	TrackerServerInfo *pServer;
	TrackerServerInfo *pEnd;

	if (pServerGroup->server_count == 0)
	{
		return -1;
	}

	pEnd = pServerGroup->servers + pServerGroup->server_count;
	for (pServer=pServerGroup->servers; pServer<pEnd; pServer++)
	{
		if (strcmp(pServer->ip_addr, leaderIp) == 0 && \
			pServer->port == leaderPort)
		{
			return pServer - pServerGroup->servers;
		}
	}

	return -1;
}

int fdfs_parse_storage_reserved_space(IniContext *pIniContext, \
		FDFSStorageReservedSpace *pStorageReservedSpace)
{
	int result;
	int len;
	char *pReservedSpaceStr;
	int64_t storage_reserved;

	pReservedSpaceStr = iniGetStrValue(NULL, \
			"reserved_storage_space", pIniContext);
	if (pReservedSpaceStr == NULL)
	{
		pStorageReservedSpace->flag = \
				TRACKER_STORAGE_RESERVED_SPACE_FLAG_MB;
		pStorageReservedSpace->rs.mb = FDFS_DEF_STORAGE_RESERVED_MB;
		return 0;
	}
	if (*pReservedSpaceStr == '\0')
	{
		logError("file: "__FILE__", line: %d, " \
			"item \"reserved_storage_space\" is empty!", \
			__LINE__);
		return EINVAL;
	}

	len = strlen(pReservedSpaceStr);
	if (*(pReservedSpaceStr + len - 1) == '%')
	{
		char *endptr;
		pStorageReservedSpace->flag = \
				TRACKER_STORAGE_RESERVED_SPACE_FLAG_RATIO;
		endptr = NULL;
		*(pReservedSpaceStr + len - 1) = '\0';
		pStorageReservedSpace->rs.ratio = \
					strtod(pReservedSpaceStr, &endptr);
		if (endptr != NULL && *endptr != '\0')
		{
			logError("file: "__FILE__", line: %d, " \
				"item \"reserved_storage_space\": %s%%"\
				" is invalid!", __LINE__, \
				pReservedSpaceStr);
			return EINVAL;
		}

		if (pStorageReservedSpace->rs.ratio <= 0.00 || \
			pStorageReservedSpace->rs.ratio >= 100.00)
		{
			logError("file: "__FILE__", line: %d, " \
				"item \"reserved_storage_space\": %s%%"\
				" is invalid!", __LINE__, \
				pReservedSpaceStr);
			return EINVAL;
		}

		return 0;
	}

	if ((result=parse_bytes(pReservedSpaceStr, 1, &storage_reserved)) != 0)
	{
		return result;
	}

	pStorageReservedSpace->flag = TRACKER_STORAGE_RESERVED_SPACE_FLAG_MB;
	pStorageReservedSpace->rs.mb = storage_reserved / FDFS_ONE_MB;
	return 0;
}

const char *fdfs_storage_reserved_space_to_string(FDFSStorageReservedSpace \
			*pStorageReservedSpace, char *buff)
{
	if (pStorageReservedSpace->flag == \
			TRACKER_STORAGE_RESERVED_SPACE_FLAG_MB)
	{
		sprintf(buff, "%dMB", pStorageReservedSpace->rs.mb);
	}
	else
	{
		sprintf(buff, "%.2f%%", pStorageReservedSpace->rs.ratio);
	}

	return buff;
}

