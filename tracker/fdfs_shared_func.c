/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

#include <stdlib.h>
#include <string.h>
#include <limits.h>
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

		pStorageReservedSpace->rs.ratio /= 100.00;
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
		sprintf(buff, "%d MB", pStorageReservedSpace->rs.mb);
	}
	else
	{
		sprintf(buff, "%.2f%%", 100.00 * \
			pStorageReservedSpace->rs.ratio);
	}

	return buff;
}

const char *fdfs_storage_reserved_space_to_string_ex(const bool flag, \
	const int space_mb, const int total_mb, const double space_ratio, \
	char *buff)
{
	if (flag == TRACKER_STORAGE_RESERVED_SPACE_FLAG_MB)
	{
		sprintf(buff, "%d MB", space_mb);
	}
	else
	{
		sprintf(buff, "%d MB(%.2f%%)", (int)(total_mb * space_ratio), \
			 100.00 * space_ratio);
	}

	return buff;
}

int fdfs_get_storage_reserved_space_mb(const int total_mb, \
		FDFSStorageReservedSpace *pStorageReservedSpace)
{
	if (pStorageReservedSpace->flag == \
			TRACKER_STORAGE_RESERVED_SPACE_FLAG_MB)
	{
		return pStorageReservedSpace->rs.mb;
	}
	else
	{
		return (int)(total_mb * pStorageReservedSpace->rs.ratio);
	}
}

bool fdfs_check_reserved_space(FDFSGroupInfo *pGroup, \
	FDFSStorageReservedSpace *pStorageReservedSpace)
{
	if (pStorageReservedSpace->flag == \
			TRACKER_STORAGE_RESERVED_SPACE_FLAG_MB)
	{
		return pGroup->free_mb > pStorageReservedSpace->rs.mb;
	}
	else
	{
		if (pGroup->total_mb == 0)
		{
			return false;
		}

		/*
		logInfo("storage=%.4f, rs.ratio=%.4f", 
			((double)pGroup->free_mb / (double)pGroup->total_mb),
			pStorageReservedSpace->rs.ratio);
		*/

		return ((double)pGroup->free_mb / (double)pGroup->total_mb) > \
			pStorageReservedSpace->rs.ratio;
	}
}

bool fdfs_check_reserved_space_trunk(FDFSGroupInfo *pGroup, \
	FDFSStorageReservedSpace *pStorageReservedSpace)
{
	if (pStorageReservedSpace->flag == \
			TRACKER_STORAGE_RESERVED_SPACE_FLAG_MB)
	{
		return (pGroup->free_mb + pGroup->trunk_free_mb > 
			pStorageReservedSpace->rs.mb);
	}
	else
	{
		if (pGroup->total_mb == 0)
		{
			return false;
		}

		/*
		logInfo("storage trunk=%.4f, rs.ratio=%.4f", 
		((double)(pGroup->free_mb + pGroup->trunk_free_mb) / \
		(double)pGroup->total_mb), pStorageReservedSpace->rs.ratio);
		*/

		return ((double)(pGroup->free_mb + pGroup->trunk_free_mb) / \
		(double)pGroup->total_mb) > pStorageReservedSpace->rs.ratio;
	}
}

bool fdfs_check_reserved_space_path(const int64_t total_mb, \
	const int64_t free_mb, const int avg_mb, \
	FDFSStorageReservedSpace *pStorageReservedSpace)
{
	if (pStorageReservedSpace->flag == \
			TRACKER_STORAGE_RESERVED_SPACE_FLAG_MB)
	{
		return free_mb > avg_mb;
	}
	else
	{
		if (total_mb == 0)
		{
			return false;
		}

		/*
		logInfo("storage path, free_mb="INT64_PRINTF_FORMAT \
			", total_mb="INT64_PRINTF_FORMAT", " \
			"real ratio=%.4f, rs.ratio=%.4f", \
			free_mb, total_mb, ((double)free_mb / total_mb), \
			pStorageReservedSpace->rs.ratio);
		*/

		return ((double)free_mb / (double)total_mb) > \
			pStorageReservedSpace->rs.ratio;
	}
}

bool fdfs_is_server_id_valid(const char *id)
{
	long n;
	char *endptr;
	char buff[FDFS_STORAGE_ID_MAX_SIZE];

	if (*id == '\0')
	{
		return false;
	}

	endptr = NULL;
	n = strtol(id, &endptr, 10);
	if (endptr != NULL && *endptr != '\0')
	{
		return false;
	}

	if (n <= 0 || n > FDFS_MAX_SERVER_ID)
	{
		return false;
	}

	snprintf(buff, sizeof(buff), "%ld", n);
	return strcmp(buff, id) == 0;
}

int  fdfs_get_server_id_type(const int id)
{
  if (id > 0 && id <= FDFS_MAX_SERVER_ID)
  {
    return FDFS_ID_TYPE_SERVER_ID;
  }
  else
  {
    return FDFS_ID_TYPE_IP_ADDRESS;
  }
}

