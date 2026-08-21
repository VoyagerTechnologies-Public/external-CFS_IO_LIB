/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

#include "io_lib.h"
#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

static int32 TMTF_Status;
static int32 TM_SYNC_Status;
static int32 TC_SYNC_Status;

int32 TMTF_LibInit(void)
{
    return TMTF_Status;
}

int32 TM_SYNC_LibInit(void)
{
    return TM_SYNC_Status;
}

int32 TC_SYNC_LibInit(void)
{
    return TC_SYNC_Status;
}

static void IO_LIB_Test_Setup(void)
{
    UT_ResetState(0);
    TMTF_Status    = CFE_SUCCESS;
    TM_SYNC_Status = CFE_SUCCESS;
    TC_SYNC_Status = CFE_SUCCESS;
}

static void IO_LIB_Init_Nominal(void)
{
    UtAssert_INT32_EQ(IO_LibInit(), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 0);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);
}

static void IO_LIB_Init_ProtocolFailure(void)
{
    TM_SYNC_Status = IO_LIB_ERROR;

    UtAssert_INT32_EQ(IO_LibInit(), IO_LIB_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 0);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);
}

void UtTest_Setup(void)
{
    UtTest_Add(IO_LIB_Init_Nominal, IO_LIB_Test_Setup, NULL, "IO_LIB_Init_Nominal");
    UtTest_Add(IO_LIB_Init_ProtocolFailure, IO_LIB_Test_Setup, NULL, "IO_LIB_Init_ProtocolFailure");
}
