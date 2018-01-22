/* WARNING: THIS FILE IS AUTOMATICALLY GENERATED FROM A .SM FILE.
 * Changes made here will certainly be overwritten.
 */

/* 
 * (C) 2003 Clemson University and The University of Chicago 
 *
 * See COPYING in top-level directory.
 */

/** \file PVFS system calls for reading extended attributes
 *  \ingroup sysint
 */

#include <string.h>
#include <assert.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "client-state-machine.h"
#include "pvfs2-debug.h"
#include "pvfs2-util.h"
#include "job.h"
#include "gossip.h"
#include "str-utils.h"
#include "pint-cached-config.h"
#include "PINT-reqproto-encode.h"
#include "pint-eattr.h"
#include "security-util.h"

extern job_context_id pint_client_sm_context;

static int get_eattr_comp_fn(
    void *v_p,
    struct PVFS_server_resp *resp_p,
    int i);


static PINT_sm_action get_eattr_setup_msgpair(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_setup_msgpair;
static struct PINT_pjmp_tbl_s ST_setup_msgpair_pjtbl[];
static struct PINT_tran_tbl_s ST_setup_msgpair_trtbl[];
static struct PINT_state_s ST_xfer_msgpair;
static struct PINT_pjmp_tbl_s ST_xfer_msgpair_pjtbl[];
static struct PINT_tran_tbl_s ST_xfer_msgpair_trtbl[];

static PINT_sm_action get_eattr_cleanup(
	struct PINT_smcb *smcb, job_status_s *js_p);

static struct PINT_state_s ST_cleanup;
static struct PINT_pjmp_tbl_s ST_cleanup_pjtbl[];
static struct PINT_tran_tbl_s ST_cleanup_trtbl[];

struct PINT_state_machine_s pvfs2_client_get_eattr_sm = {
	.name = "pvfs2_client_get_eattr_sm",
	.first_state = &ST_setup_msgpair
};

static struct PINT_state_s ST_setup_msgpair = {
	 .state_name = "setup_msgpair" ,
	 .parent_machine = &pvfs2_client_get_eattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = get_eattr_setup_msgpair ,
	 .pjtbl = NULL ,
	 .trtbl = ST_setup_msgpair_trtbl 
};

static struct PINT_tran_tbl_s ST_setup_msgpair_trtbl[] = {
	{ .return_value = 0 ,
	 .next_state = &ST_xfer_msgpair },
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_xfer_msgpair = {
	 .state_name = "xfer_msgpair" ,
	 .parent_machine = &pvfs2_client_get_eattr_sm ,
	 .flag = SM_JUMP ,
	 .action.nested = &pvfs2_msgpairarray_sm ,
	 .pjtbl = NULL ,
	 .trtbl = ST_xfer_msgpair_trtbl 
};

static struct PINT_tran_tbl_s ST_xfer_msgpair_trtbl[] = {
	{ .return_value = -1 ,
	 .next_state = &ST_cleanup }
};

static struct PINT_state_s ST_cleanup = {
	 .state_name = "cleanup" ,
	 .parent_machine = &pvfs2_client_get_eattr_sm ,
	 .flag = SM_RUN ,
	 .action.func = get_eattr_cleanup ,
	 .pjtbl = NULL ,
	 .trtbl = ST_cleanup_trtbl 
};

static struct PINT_tran_tbl_s ST_cleanup_trtbl[] = {
	{ .return_value = -1 ,

	 .flag = SM_TERM }
};

#ifndef WIN32
# 58 "src/client/sysint/sys-get-eattr.sm"
#endif


PVFS_error PVFS_isys_geteattr_list(
        PVFS_object_ref ref,
        const PVFS_credential *credential,
        int32_t nkey,
        PVFS_ds_keyval *key_array,
        PVFS_sysresp_geteattr *resp_p,
        PVFS_sys_op_id *op_id,
        PVFS_hint hints,
        void *user_ptr)
{
    int ret = -PVFS_EINVAL;
    PINT_smcb *smcb = NULL;
    PINT_client_sm *sm_p = NULL;

    gossip_debug(GOSSIP_CLIENT_DEBUG,
                 "PINT_isys_geteattr entered\n");

    if ((ref.handle == PVFS_HANDLE_NULL) ||
       (ref.fs_id == PVFS_FS_ID_NULL) || (resp_p == NULL))
    {
        gossip_err("invalid (NULL) required argument\n");
	return ret;
    }

    PINT_smcb_alloc(&smcb, PVFS_SYS_GETEATTR,
             sizeof(struct PINT_client_sm),
             client_op_state_get_machine,
             client_state_machine_terminate,
             pint_client_sm_context);
    if (smcb == NULL)
    {
        return -PVFS_ENOMEM;
    }
    sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);

    PINT_init_msgarray_params(sm_p, ref.fs_id);
    PINT_init_sysint_credential(sm_p->cred_p, credential);
    sm_p->u.geteattr.nkey = nkey;
    sm_p->u.geteattr.key_array = key_array;
    sm_p->u.geteattr.resp_p = resp_p;
    sm_p->error_code = 0;
    sm_p->object_ref = ref;
    PVFS_hint_copy(hints, &sm_p->hints);

    return PINT_client_state_machine_post(
            smcb,  op_id, user_ptr);
}

PVFS_error PVFS_sys_geteattr_list(
        PVFS_object_ref ref,
        const PVFS_credential *credential,
        int32_t nkey,
        PVFS_ds_keyval *key_array,
        PVFS_sysresp_geteattr *resp_p,
        PVFS_hint hints)
{
    PVFS_error ret = -PVFS_EINVAL, error = 0;
    PVFS_sys_op_id op_id;

    gossip_debug(GOSSIP_CLIENT_DEBUG, "PVFS_sys_geteattr entered\n");

    ret = PVFS_isys_geteattr_list(ref, credential,
            nkey, key_array, resp_p, &op_id, hints, NULL);

    if (ret)
    {
        PVFS_perror_gossip("PVFS_isys_geteattr call", ret);
        error = ret;
    }
    else if (!ret && op_id != -1)
    {
        ret = PVFS_sys_wait(op_id, "geteattr", &error);
        if (ret)
        {
             PVFS_perror_gossip("PVFS_sys_wait call", ret);
             error = ret;
        }
        PINT_sys_release(op_id);
    }
    return error;
}

PVFS_error PVFS_sys_geteattr(
        PVFS_object_ref ref,
        const PVFS_credential *credential,
        PVFS_ds_keyval *key_p,
        PVFS_ds_keyval *val_p,
        PVFS_hint hints)
{
    PVFS_sysresp_geteattr resp_p;
    PVFS_error tmp_err;
    resp_p.val_array = val_p;
    resp_p.err_array = &tmp_err;
    return PVFS_sys_geteattr_list(ref, credential, 1, key_p, &resp_p, hints);
}

static PINT_sm_action get_eattr_setup_msgpair(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    int ret = -PVFS_EINVAL;
    int i;
    PINT_sm_msgpair_state *msg_p;
    PVFS_capability capability;

    gossip_debug(GOSSIP_CLIENT_DEBUG,
		"get_eattr state: get_eattr_setup_msgpair\n");

    PINT_null_capability(&capability);

    /* this will be the array of value buffer sizes */
    sm_p->u.geteattr.size_array =
             (PVFS_size *)malloc(sm_p->u.geteattr.nkey *
                                 sizeof (PVFS_size));

    /* fill the array pulling out of the value array */
    /* we don't want to send the unused fields in this array */
    for (i = 0; i < sm_p->u.geteattr.nkey; i++)
        sm_p->u.geteattr.size_array[i] =
            sm_p->u.geteattr.resp_p->val_array[i].buffer_sz;

    PINT_msgpair_init(&sm_p->msgarray_op);
    msg_p = &sm_p->msgarray_op.msgpair;

    PINT_SERVREQ_GETEATTR_FILL(
        msg_p->req,
        capability,
        sm_p->object_ref.fs_id,
        sm_p->object_ref.handle,
        sm_p->u.geteattr.nkey,
        sm_p->u.geteattr.key_array,
        sm_p->u.geteattr.size_array,
        sm_p->hints);

    PINT_cleanup_capability(&capability);

    msg_p->fs_id = sm_p->object_ref.fs_id;
    msg_p->handle = sm_p->object_ref.handle;
    msg_p->retry_flag = PVFS_MSGPAIR_RETRY;
    msg_p->comp_fn = get_eattr_comp_fn;

    ret = PINT_cached_config_map_to_server(
            &msg_p->svr_addr,
            msg_p->handle,
            msg_p->fs_id);

    if (ret)
    {
        gossip_err("Failed to map meta server address\n");
        js_p->error_code = 0;
    }

    PINT_sm_push_frame(smcb, 0, &sm_p->msgarray_op);
    return SM_ACTION_COMPLETE;
}

static PINT_sm_action get_eattr_cleanup(
        struct PINT_smcb *smcb, job_status_s *js_p)
{
    struct PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_FRAME_CURRENT);
    gossip_debug(GOSSIP_CLIENT_DEBUG,
		"get_eattr state: get_eattr_cleanup\n");
    /* free the array malloc'd above */
    free(sm_p->u.geteattr.size_array);
    sm_p->error_code  = js_p->error_code;
    if(sm_p->error_code != 0)
    {
        PINT_acache_invalidate(sm_p->object_ref);
    }

    PINT_SET_OP_COMPLETE;
    return SM_ACTION_TERMINATE;
}

static int get_eattr_comp_fn(
    void *v_p,
    struct PVFS_server_resp *resp_p,
    int i)
{
    int j = 0;
    int ret = 0;
    int decode_ret;
    PINT_smcb *smcb = v_p;
    PINT_client_sm *sm_p = PINT_sm_frame(smcb, PINT_MSGPAIR_PARENT_SM);

    gossip_debug(GOSSIP_CLIENT_DEBUG,
		"get_eattr completion fn: get_eattr_comp_fn\n");

    /* only posted one msgpair */
    assert(i==0);

    /* if this particular request was successful, then store the server
     * response and let the caller sort it out */

    /* recall that the returned val only has buffer and buffer_sz
     * encoded across the wire ... so the buffer_sz is actually the
     * read_sz ... the original buffer_sz is still in the original
     * val struct the user passed in
     */

    if ((sm_p->msgarray_op.msgarray[i].op_status == 0) ||
        (sm_p->msgarray_op.msgarray[i].op_status == -PVFS_ERANGE))
    {
        int k, mink;
        mink = sm_p->u.geteattr.nkey;
        if (resp_p->u.geteattr.nkey < mink)
        {
            mink = resp_p->u.geteattr.nkey;
            gossip_err("Successful call returned fewer values than requested\n");
        }
        if (!resp_p->u.geteattr.val)
        {
            gossip_err("Successful call returned NULL value list\n");
            return -1;
        }
        gossip_debug(GOSSIP_GETEATTR_DEBUG,"returned %d values\n",
                sm_p->u.geteattr.nkey);
        for (k = 0; k < sm_p->u.geteattr.nkey; k++)
        {
            gossip_debug(GOSSIP_GETEATTR_DEBUG,"resp_read_sz = %d\n",
                    resp_p->u.geteattr.val[k].read_sz);
            gossip_debug(GOSSIP_GETEATTR_DEBUG,"resp_buff_sz = %d\n",
                    resp_p->u.geteattr.val[k].buffer_sz);
            gossip_debug(GOSSIP_GETEATTR_DEBUG,"sm_buff_sz = %d\n",
                    sm_p->u.geteattr.resp_p->val_array[k].buffer_sz);
            gossip_debug(GOSSIP_GETEATTR_DEBUG,"resp_buff_ = %s\n",
                    (char *)resp_p->u.geteattr.val[k].buffer);
            /* out the actual read_sz in its place */
            sm_p->u.geteattr.resp_p->val_array[k].read_sz =
                    resp_p->u.geteattr.val[k].buffer_sz;
            sm_p->u.geteattr.resp_p->err_array[k] =
                    resp_p->u.geteattr.err[k];
            /* check for too big a return message */
            if(resp_p->u.geteattr.err[k] == 0)
            {
                if (sm_p->u.geteattr.resp_p->val_array[k].read_sz <=
                        sm_p->u.geteattr.resp_p->val_array[k].buffer_sz)
                {
                    gossip_debug(GOSSIP_GETEATTR_DEBUG,"copying\n");
                    memcpy(sm_p->u.geteattr.resp_p->val_array[k].buffer,
                            resp_p->u.geteattr.val[k].buffer,
                            sm_p->u.geteattr.resp_p->val_array[k].read_sz);
                }
                else /* oops, error! returned too much data */
                {
                    gossip_debug(GOSSIP_GETEATTR_DEBUG,"clearing on error\n");
                    memset(sm_p->u.geteattr.resp_p->val_array[k].buffer,0,
                            sm_p->u.geteattr.resp_p->val_array[k].buffer_sz);
                    /* record an error, but keep going */
                    ret = -PVFS_EMSGSIZE;
                }

                /* seems ok - decode the eattrs we know about */
                decode_ret = PINT_eattr_decode(
                    &sm_p->u.geteattr.key_array[k],
                    &sm_p->u.geteattr.resp_p->val_array[k]);
                if(decode_ret != 0)
                {
                    gossip_debug(GOSSIP_GETATTR_DEBUG, 
                                 "failed decode of eattr: %s\n",
                                 (char *)sm_p->u.geteattr.key_array[k].buffer);
                    return decode_ret;
                }
            }
        }
    }
    else
    {
        ret = sm_p->msgarray_op.msgarray[i].op_status;
    }

    /* if this is the last response, check all of the status values
     * and return error code if any requests failed
     */
    if (i == (sm_p->msgarray_op.count - 1))
    {
	for (j=0; j < sm_p->msgarray_op.count; j++)
	{
	    if (sm_p->msgarray_op.msgarray[j].op_status != 0)
	    {
		return(sm_p->msgarray_op.msgarray[j].op_status);
	    }
	}
    }
    return ret;
}

/*
 * Local variables:
 *  mode: c
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
 */