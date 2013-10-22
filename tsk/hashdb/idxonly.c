/*
 * The Sleuth Kit
 *
 * Brian Carrier [carrier <at> sleuthkit [dot] org]
 * Copyright (c) 2007-2011 Brian Carrier.  All rights reserved
 *
 * This software is distributed under the Common Public License 1.0
 */

/**
 * \file idxonly_index.c
 * Contains the dummy functions that are used when only an index is used for lookups and the 
 * original database is gone. 
 */

#include "tsk_hashdb_i.h"


/**
 * Set db_name using information from this database type
 *
 * @param hdb_info the hash database object
 */
void
idxonly_name(TSK_HDB_INFO * hdb_info)
{
    FILE * hFile;
    char buf[TSK_HDB_NAME_MAXLEN];
    char *bufptr = buf;
    size_t i = 0;
    memset(hdb_info->db_name, '\0', TSK_HDB_NAME_MAXLEN);

    //Calling tsk_hdb_hasindex() has the side effect to set up the index structure (if the index exist)
    if(tsk_hdb_hasindex(hdb_info, TSK_HDB_HTYPE_MD5_ID) == 0) {
        if (tsk_verbose)
            fprintf(stderr,
                "Failed to get name from index (index does not exist); using file name instead");
        tsk_hdb_name_from_path(hdb_info);
        return;
    }

    // Get the DB name. Alternatively, we could query the index for Hashset Name.
    if (hdb_info->idx_info->index_type == TSK_HDB_ITYPE_SQLITE_V1) {
        tsk_hdb_name_from_path(hdb_info);
        return;
    } else {
        // Get name from legacy (bin search text) index
        hFile = hdb_info->idx_info->idx_struct.idx_binsrch->hIdx;
        fseeko(hFile, 0, 0);
        if(NULL == fgets(buf, TSK_HDB_NAME_MAXLEN, hFile) ||
            NULL == fgets(buf, TSK_HDB_NAME_MAXLEN, hFile) ||
            strncmp(buf,
                    TSK_HDB_IDX_HEAD_NAME_STR,
                    strlen(TSK_HDB_IDX_HEAD_NAME_STR)) != 0) {
            if (tsk_verbose)
                fprintf(stderr,
                    "Failed to read name from index; using file name instead");
            tsk_hdb_name_from_path(hdb_info);
            return;
        }
        bufptr = strchr(buf, '|');
        bufptr++;
        while(bufptr[i] != '\r' && bufptr[i] != '\n' && i < strlen(bufptr))
        {
            hdb_info->db_name[i] = bufptr[i];
            i++;
        }
    }
}


/**
 * This function creates an empty
 *
 * @param hdb_info Hash database to make index of.
 * @param dbtype Type of hash database. Ignored for IDX only.
 *
 * @return 1 on error and 0 on success.
 */
uint8_t
idxonly_makeindex(TSK_HDB_INFO * hdb_info, TSK_TCHAR * dbtype)
{
    //tsk_error_reset();
    //tsk_error_set_errno(TSK_ERR_HDB_ARG);
    //tsk_error_set_errstr(
    //         "idxonly_makeindex: Make index not supported when INDEX ONLY option is used");

    ///@temporary until we exorcise all the htype conditionals out
    TSK_TCHAR dbType[1024];
    TSNPRINTF(dbType, 1024, _TSK_T("%") PRIcTSK, TSK_HDB_DBTYPE_MD5SUM_STR);

    /* Initialize the TSK index file */
    if (tsk_hdb_idxinitialize(hdb_info, dbtype)) {
        tsk_error_set_errstr2( "idxonly_makeindex");
        return 1;
    }

    return 0;
}


/**
 * This function should find the corresponding name at a
 * given offset.  In this case though, we do not have the original database,
 * so just make an error...
 *
 * @param hdb_info Hash database to get data from
 * @param hash MD5 hash value that was searched for
 * @param offset Byte offset where hash value should be located in db_file
 * @param flags (not used)
 * @param action Callback used for each entry found in lookup
 * @param cb_ptr Pointer to data passed to callback
 *
 * @return 1 on error and 0 on succuss
 */
uint8_t
idxonly_getentry(TSK_HDB_INFO * hdb_info, const char *hash,
                 TSK_OFF_T offset, TSK_HDB_FLAG_ENUM flags,
                 TSK_HDB_LOOKUP_FN action, void *cb_ptr)
{
    tsk_error_reset();
    tsk_error_set_errno(TSK_ERR_HDB_ARG);
    tsk_error_set_errstr(
             "idxonly_getentry: Not supported when INDEX ONLY option is used");
    return 1;
}
