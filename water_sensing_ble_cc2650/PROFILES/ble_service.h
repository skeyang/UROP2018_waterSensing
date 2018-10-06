/**********************************************************************************************
 * Filename:       ble_service.h
 *
 * Description:    This file contains the bleService service definitions and
 *                 prototypes.
 *
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************************************/


#ifndef _BLESERVICE_H_
#define _BLESERVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
* CONSTANTS
*/
// Service UUID
#define BLESERVICE_SERV_UUID 0x11AA

//  Characteristic defines
#define BLESERVICE_TEMPERATUREVALUE      0
#define BLESERVICE_TEMPERATUREVALUE_UUID 0xA11A
#define BLESERVICE_TEMPERATUREVALUE_LEN  3

//  Characteristic defines
#define BLESERVICE_PRESSUREVALUE      1
#define BLESERVICE_PRESSUREVALUE_UUID 0xB11B
#define BLESERVICE_PRESSUREVALUE_LEN  4

//  Characteristic defines
#define BLESERVICE_FLOWVALUE      2
#define BLESERVICE_FLOWVALUE_UUID 0xC11C
#define BLESERVICE_FLOWVALUE_LEN  4

//  Characteristic defines
#define BLESERVICE_CONDUCTIVITYVALUE      3
#define BLESERVICE_CONDUCTIVITYVALUE_UUID 0xD11D
#define BLESERVICE_CONDUCTIVITYVALUE_LEN  5

//  Characteristic defines
#define BLESERVICE_TURBIDITYVALUE      4
#define BLESERVICE_TURBIDITYVALUE_UUID 0xE11E
#define BLESERVICE_TURBIDITYVALUE_LEN  4

//  Characteristic defines
#define BLESERVICE_PHVALUE      5
#define BLESERVICE_PHVALUE_UUID 0xF11F
#define BLESERVICE_PHVALUE_LEN  6

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*bleServiceChange_t)( uint8 paramID );

typedef struct
{
  bleServiceChange_t        pfnChangeCb;  // Called when characteristic value changes
} bleServiceCBs_t;



/*********************************************************************
 * API FUNCTIONS
 */


/*
 * BleService_AddService- Initializes the AdcService service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t BleService_AddService( void );

/*
 * BleService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t BleService_RegisterAppCBs( bleServiceCBs_t *appCallbacks );

/*
 * BleService_SetParameter - Set a BleService parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t BleService_SetParameter( uint8 param, uint8 len, void *value );

/*
 * BleService_GetParameter - Get a BleService parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t BleService_GetParameter( uint8 param, void *value );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _BLESERVICE_H_ */
