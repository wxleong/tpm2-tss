/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2017-2018, Fraunhofer SIT sponsored by Infineon Technologies AG
 * All rights reserved.
 *******************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h" // IWYU pragma: keep
#endif

#include <stdlib.h>           // for NULL, EXIT_FAILURE, EXIT_SUCCESS

#include "esys_int.h"         // for RSRC_NODE_T
#include "esys_iutil.h"       // for esys_GetResourceObject
#include "esys_types.h"       // for IESYS_RESOURCE
#include "tss2_common.h"      // for TSS2_RC, TSS2_RC_SUCCESS
#include "tss2_esys.h"        // for Esys_Free, ESYS_TR_NONE, Esys_FlushContext
#include "tss2_tpm2_types.h"  // for TPM2_ALG_SHA256, TPM2B_AUTH, TPM2B_DATA

#define LOGMODULE test
#include "util/log.h"         // for goto_if_error, LOG_INFO, LOG_ERROR

/** This test is intended to test the quote command with password
 *  authentication.
 *
 * We create a RSA primary signing key which will be used
 * for signing.
 *
 * Tested ESYS commands:
 *  - Esys_CreatePrimary() (M)
 *  - Esys_FlushContext() (M)
 *  - Esys_Quote() (M)
 *
 * @param[in,out] esys_context The ESYS_CONTEXT.
 * @retval EXIT_FAILURE
 * @retval EXIT_SUCCESS
 */

int
test_esys_quote(ESYS_CONTEXT * esys_context)
{
    TSS2_RC r;
    ESYS_TR primaryHandle = ESYS_TR_NONE;

    TPM2B_PUBLIC *outPublic = NULL;
    TPM2B_CREATION_DATA *creationData = NULL;
    TPM2B_DIGEST *creationHash = NULL;
    TPMT_TK_CREATION *creationTicket = NULL;

    TPM2B_ATTEST *attest = NULL;
    TPMT_SIGNATURE *signature = NULL;

    TPM2B_AUTH authValuePrimary = {
        .size = 5,
        .buffer = {1, 2, 3, 4, 5}
    };

    TPM2B_SENSITIVE_CREATE inSensitivePrimary = {
        .size = 0,
        .sensitive = {
            .userAuth = {
                 .size = 0,
                 .buffer = {0},
             },
            .data = {
                 .size = 0,
                 .buffer = {0},
             },
        },
    };

    inSensitivePrimary.sensitive.userAuth = authValuePrimary;

    TPM2B_PUBLIC inPublic = {
            .size = 0,
            .publicArea = {
                .type = TPM2_ALG_RSA,
                .nameAlg = TPM2_ALG_SHA256,
                .objectAttributes = (
                    TPMA_OBJECT_USERWITHAUTH |
                    TPMA_OBJECT_RESTRICTED |
                    TPMA_OBJECT_SIGN_ENCRYPT |
                    TPMA_OBJECT_FIXEDTPM |
                    TPMA_OBJECT_FIXEDPARENT |
                    TPMA_OBJECT_SENSITIVEDATAORIGIN
                    ),
                .authPolicy = {
                        .size = 0,
                    },
                .parameters.rsaDetail = {
                    .symmetric = {
                        .algorithm = TPM2_ALG_NULL,
                        .keyBits.aes = 128,
                        .mode.aes = TPM2_ALG_CFB,
                        },
                    .scheme = {
                         .scheme = TPM2_ALG_RSASSA,
                         .details = { .rsassa = { .hashAlg = TPM2_ALG_SHA256 }},

                    },
                    .keyBits = 2048,
                    .exponent = 0,
                },
                .unique.rsa = {
                        .size = 0,
                        .buffer = {},
                    },
            },
        };

    TPM2B_AUTH authValue = {
                .size = 0,
                .buffer = {}
    };


    TPM2B_DATA outsideInfo = {
            .size = 0,
            .buffer = {},
    };


    TPML_PCR_SELECTION creationPCR = {
        .count = 0,
    };

    LOG_INFO("\nRSA key will be created.");

    r = Esys_TR_SetAuth(esys_context, ESYS_TR_RH_OWNER, &authValue);
    goto_if_error(r, "Error: TR_SetAuth", error);

    RSRC_NODE_T *primaryHandle_node;

    r = Esys_CreatePrimary(esys_context, ESYS_TR_RH_OWNER, ESYS_TR_PASSWORD,
                           ESYS_TR_NONE, ESYS_TR_NONE, &inSensitivePrimary,
                           &inPublic, &outsideInfo, &creationPCR,
                           &primaryHandle, &outPublic, &creationData,
                           &creationHash, &creationTicket);
    goto_if_error(r, "Error esys create primary", error);

    r = esys_GetResourceObject(esys_context, primaryHandle,
                               &primaryHandle_node);
    goto_if_error(r, "Error Esys GetResourceObject", error);

    LOG_INFO("Created Primary with handle 0x%08x...",
             primaryHandle_node->rsrc.handle);

    r = Esys_TR_SetAuth(esys_context, primaryHandle,
                        &authValuePrimary);
    goto_if_error(r, "Error: TR_SetAuth", error);

    TPM2B_DATA qualifyingData  = { .size = 0, .buffer = {}};
    TPMT_SIG_SCHEME sig_scheme = { .scheme = TPM2_ALG_NULL };

    TPML_PCR_SELECTION pcr_selection = {
        .count = 2,
        .pcrSelections = {
            {
                .hash = TPM2_ALG_SHA256,
                .sizeofSelect = 3,
                .pcrSelect = { 0,4,0 } },
            {
                .hash = TPM2_ALG_SHA256,
                .sizeofSelect = 3,
                .pcrSelect = { 0,4,0 } },
        }};

    r = Esys_Quote(esys_context, primaryHandle,
                   ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE,
                   &qualifyingData, &sig_scheme, &pcr_selection,
                   &attest, &signature);
    goto_if_error(r, "Error Esys Quote", error);

    r = Esys_FlushContext(esys_context, primaryHandle);
    goto_if_error(r, "Error: FlushContext", error);

    Esys_Free(outPublic);
    Esys_Free(creationData);
    Esys_Free(creationHash);
    Esys_Free(creationTicket);

    Esys_Free(attest);
    Esys_Free(signature);
    return EXIT_SUCCESS;

 error:

    if (primaryHandle != ESYS_TR_NONE) {
        if (Esys_FlushContext(esys_context, primaryHandle) != TSS2_RC_SUCCESS) {
            LOG_ERROR("Cleanup primaryHandle failed.");
        }
    }

    return EXIT_FAILURE;
}

int
test_invoke_esys(ESYS_CONTEXT * esys_context) {
    return test_esys_quote(esys_context);
}
