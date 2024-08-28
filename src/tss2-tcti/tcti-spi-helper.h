/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright 2020 Fraunhofer SIT. All rights reserved.
 */
#ifndef TCTI_SPI_HELPER_HELPER_H
#define TCTI_SPI_HELPER_HELPER_H
#include "tcti-common.h"           // for TSS2_TCTI_COMMON_CONTEXT
#include "tcti-helper-common.h"    // for TSS2_TCTI_HELPER_COMMON_CONTEXT
#include "tss2_tcti_spi_helper.h"  // for TSS2_TCTI_SPI_HELPER_PLATFORM

#define TCTI_SPI_HELPER_MAGIC 0x4D5C6E8BD4811477ULL

#define TCTI_SPI_HELPER_HEAD_LOCALITY_0    0x00D40000
#define TCTI_SPI_HELPER_HEAD_TPM_ACCESS    (TCTI_SPI_HELPER_HEAD_LOCALITY_0 + 0)
#define TCTI_SPI_HELPER_HEAD_TPM_STS       (TCTI_SPI_HELPER_HEAD_LOCALITY_0 + 0x18)
#define TCTI_SPI_HELPER_HEAD_TPM_DATA_FIFO (TCTI_SPI_HELPER_HEAD_LOCALITY_0 + 0x24)
#define TCTI_SPI_HELPER_HEAD_TPM_DID_VID   (TCTI_SPI_HELPER_HEAD_LOCALITY_0 + 0xF00)
#define TCTI_SPI_HELPER_HEAD_TPM_RID       (TCTI_SPI_HELPER_HEAD_LOCALITY_0 + 0xF04)

enum TCTI_SPI_HELPER_REGISTER_ACCESS_TYPE {
    TCTI_SPI_HELPER_REGISTER_WRITE = 0,
    TCTI_SPI_HELPER_REGISTER_READ = 1
};

typedef struct {
    TSS2_TCTI_COMMON_CONTEXT common;
    TCTI_HELPER_COMMON_CONTEXT helper_common;
    TSS2_TCTI_SPI_HELPER_PLATFORM platform;
} TSS2_TCTI_SPI_HELPER_CONTEXT;

#endif /* TCTI_SPI_HELPER_HELPER_H */
