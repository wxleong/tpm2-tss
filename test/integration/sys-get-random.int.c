/* SPDX-License-Identifier: BSD-2-Clause */
/***********************************************************************
 * Copyright (c) 2017-2018, Intel Corporation
 *
 * All rights reserved.
 ***********************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h" // IWYU pragma: keep
#endif

#include <stdlib.h>           // for exit
#include <string.h>           // for memcmp

#include "tss2_common.h"      // for TSS2_RC_SUCCESS, TSS2_RC
#include "tss2_sys.h"         // for Tss2_Sys_GetRandom, TSS2_SYS_CONTEXT
#include "tss2_tpm2_types.h"  // for TPM2B_DIGEST

#define LOGMODULE test
#include "test.h"             // for test_invoke
#include "util/log.h"         // for LOG_ERROR, LOG_INFO

/**
 * This program contains integration test for SYS Tss2_Sys_GetRandom.
 * First, this test is checking the return code to make sure the
 * SYS is executed correctly(return code should return TPM2_RC_SUCCESS).
 * Second, the SYS is called twice to make sure the return randomBytes
 * are different by comparing the two randomBytes through memcmp.
 * It might not be the best test for random bytes generator but
 * at least this test shows the return randomBytes are different.
 */
int
test_invoke (TSS2_SYS_CONTEXT *sys_context)
{
    TSS2_RC rc;
    TPM2B_DIGEST randomBytes1 = {sizeof (TPM2B_DIGEST) - 2,};
    TPM2B_DIGEST randomBytes2 = {sizeof (TPM2B_DIGEST) - 2,};
    int bytes = 20;

    LOG_INFO("GetRandom tests started.");
    rc = Tss2_Sys_GetRandom(sys_context, 0, bytes, &randomBytes1, 0);
    if (rc != TSS2_RC_SUCCESS) {
        LOG_ERROR("GetRandom FAILED! Response Code : %x", rc);
        exit(1);
    }
    rc = Tss2_Sys_GetRandom(sys_context, 0, bytes, &randomBytes2, 0);
    if (rc != TSS2_RC_SUCCESS) {
        LOG_ERROR("GetRandom FAILED! Response Code : %x", rc);
        exit(1);
    }
    if(memcmp(&randomBytes1, &randomBytes2, bytes) == 0) {
        LOG_ERROR("Comparison FAILED! randomBytes 0x%p & 0x%p are the same.", &randomBytes1, &randomBytes2);
        exit(1);
    }
    LOG_INFO("GetRandom Test Passed!");
    return 0;
}
