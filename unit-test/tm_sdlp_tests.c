/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

#include "crypto.h"
#include "tm_sdlp.h"
#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include <string.h>

static SaInterfaceStruct      TestSaInterface;
static SecurityAssociation_t  TestSa;
static int32                  TestSaStatus;
static int32                  TestSecurityHeaderLength;
static int32                  TestSecurityTrailerLength;

SaInterface sa_if;

static int32 Test_GetOperationalSa(uint8 tfvn, uint16 scid, uint16 vcid, uint8 mapid,
                                   SecurityAssociation_t **saPtr)
{
    UtAssert_UINT32_EQ(tfvn, 0);
    UtAssert_UINT32_EQ(scid, 3);
    UtAssert_UINT32_EQ(vcid, 1);
    UtAssert_UINT32_EQ(mapid, 0);

    if (TestSaStatus == CRYPTO_LIB_SUCCESS)
    {
        *saPtr = &TestSa;
    }

    return TestSaStatus;
}

int32_t Crypto_Get_Security_Header_Length(SecurityAssociation_t *saPtr)
{
    UtAssert_ADDRESS_EQ(saPtr, &TestSa);
    return TestSecurityHeaderLength;
}

int32_t Crypto_Get_Security_Trailer_Length(SecurityAssociation_t *saPtr)
{
    UtAssert_ADDRESS_EQ(saPtr, &TestSa);
    return TestSecurityTrailerLength;
}

static void TM_SDLP_Test_Setup(void)
{
    memset(&TestSaInterface, 0, sizeof(TestSaInterface));
    memset(&TestSa, 0, sizeof(TestSa));

    UT_ResetState(0);
    TestSaStatus                       = CRYPTO_LIB_SUCCESS;
    TestSecurityHeaderLength          = 14;
    TestSecurityTrailerLength         = 0;
    TestSaInterface.sa_get_operational_sa_from_gvcid = Test_GetOperationalSa;
    sa_if                              = &TestSaInterface;
}

static void TM_SDLP_InitChannel_ReservesSdlsFields(void)
{
    TM_SDLP_FrameInfo_t     frameInfo;
    TM_SDLP_GlobalConfig_t  globalConfig;
    TM_SDLP_ChannelConfig_t channelConfig;
    uint8                   frame[1786];
    uint8                   overflow[1786];

    memset(&frameInfo, 0, sizeof(frameInfo));
    memset(&globalConfig, 0, sizeof(globalConfig));
    memset(&channelConfig, 0, sizeof(channelConfig));

    globalConfig.scId          = 3;
    globalConfig.frameLength   = sizeof(frame);
    channelConfig.vcId         = 1;
    channelConfig.overflowSize = sizeof(overflow);
    TestSecurityTrailerLength  = 16;

    UtAssert_INT32_EQ(TM_SDLP_InitChannel(&frameInfo, frame, overflow, &globalConfig, &channelConfig),
                      TM_SDLP_SUCCESS);
    UtAssert_UINT32_EQ(frameInfo.dataFieldOffset, TMTF_PRIHDR_LENGTH + 14);
    UtAssert_UINT32_EQ(frameInfo.dataFieldLength, sizeof(frame) - TMTF_PRIHDR_LENGTH - 14 - 16);
    UtAssert_UINT32_EQ(frameInfo.freeOctets, frameInfo.dataFieldLength);
    UtAssert_UINT32_EQ(frameInfo.currentDataOffset, frameInfo.dataFieldOffset);
}

static void TM_SDLP_InitChannel_RequiresSaInterface(void)
{
    TM_SDLP_FrameInfo_t     frameInfo;
    TM_SDLP_GlobalConfig_t  globalConfig;
    TM_SDLP_ChannelConfig_t channelConfig;
    uint8                   frame[100];
    uint8                   overflow[100];

    memset(&frameInfo, 0, sizeof(frameInfo));
    memset(&globalConfig, 0, sizeof(globalConfig));
    memset(&channelConfig, 0, sizeof(channelConfig));

    globalConfig.scId          = 3;
    globalConfig.frameLength   = sizeof(frame);
    channelConfig.vcId         = 1;
    channelConfig.overflowSize = sizeof(overflow);
    sa_if                      = NULL;

    UtAssert_INT32_EQ(TM_SDLP_InitChannel(&frameInfo, frame, overflow, &globalConfig, &channelConfig),
                      CRYPTO_LIB_ERR_NO_INIT);
}

static void TM_SDLP_InitChannel_RejectsInvalidSdlsLength(void)
{
    TM_SDLP_FrameInfo_t     frameInfo;
    TM_SDLP_GlobalConfig_t  globalConfig;
    TM_SDLP_ChannelConfig_t channelConfig;
    uint8                   frame[100];
    uint8                   overflow[100];

    memset(&frameInfo, 0, sizeof(frameInfo));
    memset(&globalConfig, 0, sizeof(globalConfig));
    memset(&channelConfig, 0, sizeof(channelConfig));

    globalConfig.scId          = 3;
    globalConfig.frameLength   = sizeof(frame);
    channelConfig.vcId         = 1;
    channelConfig.overflowSize = sizeof(overflow);
    TestSecurityHeaderLength   = CRYPTO_LIB_ERR_NULL_SA;

    UtAssert_INT32_EQ(TM_SDLP_InitChannel(&frameInfo, frame, overflow, &globalConfig, &channelConfig),
                      TM_SDLP_INVALID_LENGTH);
}

void UtTest_Setup(void)
{
    UtTest_Add(TM_SDLP_InitChannel_ReservesSdlsFields, TM_SDLP_Test_Setup, NULL,
               "TM_SDLP_InitChannel_ReservesSdlsFields");
    UtTest_Add(TM_SDLP_InitChannel_RequiresSaInterface, TM_SDLP_Test_Setup, NULL,
               "TM_SDLP_InitChannel_RequiresSaInterface");
    UtTest_Add(TM_SDLP_InitChannel_RejectsInvalidSdlsLength, TM_SDLP_Test_Setup, NULL,
               "TM_SDLP_InitChannel_RejectsInvalidSdlsLength");
}
