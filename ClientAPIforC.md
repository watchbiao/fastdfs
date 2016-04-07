### Tracker functions ###
```
/**
* get a connection to tracker server
* params:
* return: != NULL for success, NULL for fail
**/
TrackerServerInfo *tracker_get_connection();

/**
* connect to the tracker server
* params:
*       pTrackerServer: tracker server
* return: 0 success, !=0 fail, return the error code
**/
int tracker_connect_server(TrackerServerInfo *pTrackerServer);

/**
* close all connections to tracker servers
* params:
*       pTrackerServer: tracker server
* return:
**/
void tracker_disconnect_server(TrackerServerInfo *pTrackerServer);

/**
* close all connections to tracker servers
* params:
* return:
**/
void tracker_close_all_connections();

/**
* list all groups
* params:
*       pTrackerServer: tracker server
*       group_stats: return group info array
*       max_groups: max group count(group array capacity)
*       group_count: return group count
* return: 0 success, !=0 fail, return the error code
**/
int tracker_list_groups(TrackerServerInfo *pTrackerServer, \
                FDFSGroupStat *group_stats, const int max_groups, \
                int *group_count);

/**
* list all servers of the specified group
* params:
*       pTrackerServer: tracker server
*       szGroupName: group name to query
*       storage_infos: return storage info array
*       max_storages: max storage count(storage array capacity)
*       storage_count: return storage count
* return: 0 success, !=0 fail, return the error code
**/
int tracker_list_servers(TrackerServerInfo *pTrackerServer, \
                const char *szGroupName, \
                FDFSStorageInfo *storage_infos, const int max_storages, \
                int *storage_count);

/**
* query storage server to upload file
* params:
*       pTrackerServer: tracker server
*       pStorageServer: return storage server
* return: 0 success, !=0 fail, return the error code
**/
int tracker_query_storage_store(TrackerServerInfo *pTrackerServer, \
                TrackerServerInfo *pStorageServer);


/**
* query storage server to download file
* params:
*       pTrackerServer: tracker server
*       pStorageServer: return storage server
*       group_name: the group name of storage server
*       filename: filename on storage server
* return: 0 success, !=0 fail, return the error code
**/
int tracker_query_storage_fetch(TrackerServerInfo *pTrackerServer, \
                TrackerServerInfo *pStorageServer, \
                const char *group_name, const char *filename);
```

### Storage functions ###
```
/**
* upload file to storage server (by file name)
* params:
*       pTrackerServer: tracker server
*       pStorageServer: storage server
*       local_filename: local filename to upload
*       meta_list: meta info array
*       meta_count: meta item count
*       group_name: return the group name to store the file
*       remote_filename: return the new created filename
* return: 0 success, !=0 fail, return the error code
**/
int storage_upload_by_filename(TrackerServerInfo *pTrackerServer, \
                        TrackerServerInfo *pStorageServer, \
                        const char *local_filename, \
                        const FDFSMetaData *meta_list, \
                        const int meta_count, \
                        char *group_name, \
                        char *remote_filename);

/**
* upload file to storage server (by file buff)
* params:
*       pTrackerServer: tracker server
*       pStorageServer: storage server
*       file_buff: file content/buff
*       file_size: file size (bytes)
*       meta_list: meta info array
*       meta_count: meta item count
*       group_name: return the group name to store the file
*       remote_filename: return the new created filename
* return: 0 success, !=0 fail, return the error code
**/
int storage_upload_by_filebuff(TrackerServerInfo *pTrackerServer, \
                        TrackerServerInfo *pStorageServer, \
                        const char *file_buff, const int file_size, \
                        const FDFSMetaData *meta_list, \
                        const int meta_count, \
                        char *group_name, \
                        char *remote_filename);

/**
* delete file from storage server
* params:
*       pTrackerServer: tracker server
*       pStorageServer: storage server
*       group_name: the group name of storage server
*       filename: filename on storage server
* return: 0 success, !=0 fail, return the error code
**/
int storage_delete_file(TrackerServerInfo *pTrackerServer, \
                        TrackerServerInfo *pStorageServer, \
                        const char *group_name, const char *filename);

/**
* set metadata items to storage server
* params:
*       pTrackerServer: tracker server
*       pStorageServer: storage server
*       group_name: the group name of storage server
*       filename: filename on storage server
*       meta_list: meta item array
*       meta_count: meta item count
*       op_flag:
*            # STORAGE_SET_METADATA_FLAG_OVERWRITE('O'): overwrite all old
*                               metadata items
*            # STORAGE_SET_METADATA_FLAG_MERGE ('M'): merge, insert when
*                               the metadata item not exist, otherwise update it
* return: 0 success, !=0 fail, return the error code
**/
int storage_set_metadata(TrackerServerInfo *pTrackerServer, \
                        TrackerServerInfo *pStorageServer, \
                        const char *group_name, const char *filename, \
                        FDFSMetaData *meta_list, const int meta_count, \
                        const char op_flag);

/**
* download file from storage server
* params:
*       pTrackerServer: tracker server
*       pStorageServer: storage server
*       group_name: the group name of storage server
*       filename: filename on storage server
*       file_buff: return file content/buff, must be freed
*       file_size: return file size (bytes)
* return: 0 success, !=0 fail, return the error code
**/
int storage_download_file(TrackerServerInfo *pTrackerServer, \
                        TrackerServerInfo *pStorageServer, \
                        const char *group_name, const char *filename, \
                        char **file_buff, int *file_size);

/**
* get all metadata items from storage server
* params:
*       pTrackerServer: tracker server
*       pStorageServer: storage server
*       group_name: the group name of storage server
*       filename: filename on storage server
*       meta_list: return meta info array, must be freed
*       meta_count: return meta item count
* return: 0 success, !=0 fail, return the error code
**/
int storage_get_metadata(TrackerServerInfo *pTrackerServer, \
                        TrackerServerInfo *pStorageServer,  \
                        const char *group_name, const char *filename, \
                        FDFSMetaData **meta_list, \
                        int *meta_count);
```