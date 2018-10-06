/**********************************************************************************************
 * Filename:       ble_service.c
 *
 * Description:    This file contains the implementation of the service.
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


/*********************************************************************
 * INCLUDES
 */
#include <ble_service.h>
#include <string.h>

#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
* GLOBAL VARIABLES
*/

// bleService Service UUID
CONST uint8_t bleServiceUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BLESERVICE_SERV_UUID)
};

// temperatureValue UUID
CONST uint8_t bleService_TemperatureValueUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BLESERVICE_TEMPERATUREVALUE_UUID)
};
// pressureValue UUID
CONST uint8_t bleService_PressureValueUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BLESERVICE_PRESSUREVALUE_UUID)
};
// flowValue UUID
CONST uint8_t bleService_FlowValueUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BLESERVICE_FLOWVALUE_UUID)
};
// conductivityValue UUID
CONST uint8_t bleService_ConductivityValueUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BLESERVICE_CONDUCTIVITYVALUE_UUID)
};
// turbidityValue UUID
CONST uint8_t bleService_TurbidityValueUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BLESERVICE_TURBIDITYVALUE_UUID)
};
// phValue UUID
CONST uint8_t bleService_PhValueUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BLESERVICE_PHVALUE_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static bleServiceCBs_t *pAppCBs = NULL;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t bleServiceDecl = { ATT_UUID_SIZE, bleServiceUUID };

// Characteristic "TemperatureValue" Properties (for declaration)
static uint8_t bleService_TemperatureValueProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic "TemperatureValue" Value variable
static uint8_t bleService_TemperatureValueVal[BLESERVICE_TEMPERATUREVALUE_LEN] = {0};

// Characteristic "TemperatureValue" CCCD
static gattCharCfg_t *bleService_TemperatureValueConfig;
// Characteristic "PressureValue" Properties (for declaration)
static uint8_t bleService_PressureValueProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic "PressureValue" Value variable
static uint8_t bleService_PressureValueVal[BLESERVICE_PRESSUREVALUE_LEN] = {0};

// Characteristic "PressureValue" CCCD
static gattCharCfg_t *bleService_PressureValueConfig;
// Characteristic "FlowValue" Properties (for declaration)
static uint8_t bleService_FlowValueProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic "FlowValue" Value variable
static uint8_t bleService_FlowValueVal[BLESERVICE_FLOWVALUE_LEN] = {0};

// Characteristic "FlowValue" CCCD
static gattCharCfg_t *bleService_FlowValueConfig;
// Characteristic "ConductivityValue" Properties (for declaration)
static uint8_t bleService_ConductivityValueProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic "ConductivityValue" Value variable
static uint8_t bleService_ConductivityValueVal[BLESERVICE_CONDUCTIVITYVALUE_LEN] = {0};

// Characteristic "ConductivityValue" CCCD
static gattCharCfg_t *bleService_ConductivityValueConfig;
// Characteristic "TurbidityValue" Properties (for declaration)
static uint8_t bleService_TurbidityValueProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic "TurbidityValue" Value variable
static uint8_t bleService_TurbidityValueVal[BLESERVICE_TURBIDITYVALUE_LEN] = {0};

// Characteristic "TurbidityValue" CCCD
static gattCharCfg_t *bleService_TurbidityValueConfig;

// Characteristic "PhValue" Value variable
static uint8_t bleService_PhValueVal[BLESERVICE_PHVALUE_LEN] = {0};

// Characteristic "PhValue" CCCD
static gattCharCfg_t *bleService_PhValueConfig;

/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t bleServiceAttrTbl[] =
{
  // bleService Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&bleServiceDecl
  },
    // TemperatureValue Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &bleService_TemperatureValueProps
    },
      // TemperatureValue Characteristic Value
      {
        { ATT_UUID_SIZE, bleService_TemperatureValueUUID },
        0,
        0,
        bleService_TemperatureValueVal
      },
      // TemperatureValue CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&bleService_TemperatureValueConfig
      },
    // PressureValue Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &bleService_PressureValueProps
    },
      // PressureValue Characteristic Value
      {
        { ATT_UUID_SIZE, bleService_PressureValueUUID },
        0,
        0,
        bleService_PressureValueVal
      },
      // PressureValue CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&bleService_PressureValueConfig
      },
    // FlowValue Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &bleService_FlowValueProps
    },
      // FlowValue Characteristic Value
      {
        { ATT_UUID_SIZE, bleService_FlowValueUUID },
        0,
        0,
        bleService_FlowValueVal
      },
      // FlowValue CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&bleService_FlowValueConfig
      },
    // ConductivityValue Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &bleService_ConductivityValueProps
    },
      // ConductivityValue Characteristic Value
      {
        { ATT_UUID_SIZE, bleService_ConductivityValueUUID },
        0,
        0,
        bleService_ConductivityValueVal
      },
      // ConductivityValue CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&bleService_ConductivityValueConfig
      },
    // TurbidityValue Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &bleService_TurbidityValueProps
    },
      // TurbidityValue Characteristic Value
      {
        { ATT_UUID_SIZE, bleService_TurbidityValueUUID },
        0,
        0,
        bleService_TurbidityValueVal
      },
      // TurbidityValue CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&bleService_TurbidityValueConfig
      },
    // PhValue Characteristic Declaration
      {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &bleService_TurbidityValueProps
      },
        // PhValue Characteristic Value
        {
          { ATT_UUID_SIZE, bleService_PhValueUUID },
          0,
          0,
          bleService_PhValueVal
        },
        // PhValue CCCD
        {
          { ATT_BT_UUID_SIZE, clientCharCfgUUID },
          GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0,
          (uint8 *)&bleService_PhValueConfig
        },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t bleService_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                           uint8 *pValue, uint16 *pLen, uint16 offset,
                                           uint16 maxLen, uint8 method );
static bStatus_t bleService_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                            uint8 *pValue, uint16 len, uint16 offset,
                                            uint8 method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t bleServiceCBs =
{
  bleService_ReadAttrCB,  // Read callback function pointer
  bleService_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * BleService_AddService- Initializes the BleService service by registering
 *          GATT attributes with the GATT server.
 *
 */
bStatus_t BleService_AddService( void )
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  bleService_TemperatureValueConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( bleService_TemperatureValueConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, bleService_TemperatureValueConfig );
  // Allocate Client Characteristic Configuration table
  bleService_PressureValueConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( bleService_PressureValueConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, bleService_PressureValueConfig );
  // Allocate Client Characteristic Configuration table
  bleService_FlowValueConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( bleService_FlowValueConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, bleService_FlowValueConfig );
  // Allocate Client Characteristic Configuration table
  bleService_ConductivityValueConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( bleService_ConductivityValueConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, bleService_ConductivityValueConfig );
  // Allocate Client Characteristic Configuration table
  bleService_TurbidityValueConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( bleService_TurbidityValueConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, bleService_TurbidityValueConfig );
  // Allocate Client Characteristic Configuration table
  bleService_PhValueConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( bleService_PhValueConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, bleService_PhValueConfig );
  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( bleServiceAttrTbl,
                                        GATT_NUM_ATTRS( bleServiceAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &bleServiceCBs );

  return ( status );
}

/*
 * BleService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t BleService_RegisterAppCBs( bleServiceCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    pAppCBs = appCallbacks;

    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

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
bStatus_t BleService_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case BLESERVICE_TEMPERATUREVALUE:
      if ( len == BLESERVICE_TEMPERATUREVALUE_LEN )
      {
        memcpy(bleService_TemperatureValueVal, value, len);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( bleService_TemperatureValueConfig, (uint8_t *)&bleService_TemperatureValueVal, FALSE,
                                    bleServiceAttrTbl, GATT_NUM_ATTRS( bleServiceAttrTbl ),
                                    INVALID_TASK_ID,  bleService_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case BLESERVICE_PRESSUREVALUE:
      if ( len == BLESERVICE_PRESSUREVALUE_LEN )
      {
        memcpy(bleService_PressureValueVal, value, len);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( bleService_PressureValueConfig, (uint8_t *)&bleService_PressureValueVal, FALSE,
                                    bleServiceAttrTbl, GATT_NUM_ATTRS( bleServiceAttrTbl ),
                                    INVALID_TASK_ID,  bleService_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case BLESERVICE_FLOWVALUE:
      if ( len == BLESERVICE_FLOWVALUE_LEN )
      {
        memcpy(bleService_FlowValueVal, value, len);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( bleService_FlowValueConfig, (uint8_t *)&bleService_FlowValueVal, FALSE,
                                    bleServiceAttrTbl, GATT_NUM_ATTRS( bleServiceAttrTbl ),
                                    INVALID_TASK_ID,  bleService_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case BLESERVICE_CONDUCTIVITYVALUE:
      if ( len == BLESERVICE_CONDUCTIVITYVALUE_LEN )
      {
        memcpy(bleService_ConductivityValueVal, value, len);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( bleService_ConductivityValueConfig, (uint8_t *)&bleService_ConductivityValueVal, FALSE,
                                    bleServiceAttrTbl, GATT_NUM_ATTRS( bleServiceAttrTbl ),
                                    INVALID_TASK_ID,  bleService_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case BLESERVICE_TURBIDITYVALUE:
      if ( len == BLESERVICE_TURBIDITYVALUE_LEN )
      {
        memcpy(bleService_TurbidityValueVal, value, len);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( bleService_TurbidityValueConfig, (uint8_t *)&bleService_TurbidityValueVal, FALSE,
                                    bleServiceAttrTbl, GATT_NUM_ATTRS( bleServiceAttrTbl ),
                                    INVALID_TASK_ID,  bleService_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case BLESERVICE_PHVALUE:
      if ( len == BLESERVICE_PHVALUE_LEN )
      {
        memcpy(bleService_PhValueVal, value, len);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( bleService_PhValueConfig, (uint8_t *)&bleService_PhValueVal, FALSE,
                                    bleServiceAttrTbl, GATT_NUM_ATTRS( bleServiceAttrTbl ),
                                    INVALID_TASK_ID,  bleService_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*
 * BleService_GetParameter - Get a AdcService parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t BleService_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*********************************************************************
 * @fn          bleService_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t bleService_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                       uint8 *pValue, uint16 *pLen, uint16 offset,
                                       uint16 maxLen, uint8 method )
{
  bStatus_t status = SUCCESS;

  // See if request is regarding the TemperatureValue Characteristic Value
if ( ! memcmp(pAttr->type.uuid, bleService_TemperatureValueUUID, pAttr->type.len) )
  {
    if ( offset > BLESERVICE_TEMPERATUREVALUE_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, BLESERVICE_TEMPERATUREVALUE_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the PressureValue Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, bleService_PressureValueUUID, pAttr->type.len) )
  {
    if ( offset > BLESERVICE_PRESSUREVALUE_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, BLESERVICE_PRESSUREVALUE_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the FlowValue Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, bleService_FlowValueUUID, pAttr->type.len) )
  {
    if ( offset > BLESERVICE_FLOWVALUE_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, BLESERVICE_FLOWVALUE_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the ConductivityValue Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, bleService_ConductivityValueUUID, pAttr->type.len) )
  {
    if ( offset > BLESERVICE_CONDUCTIVITYVALUE_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, BLESERVICE_CONDUCTIVITYVALUE_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the TurbidityValue Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, bleService_TurbidityValueUUID, pAttr->type.len) )
  {
    if ( offset > BLESERVICE_TURBIDITYVALUE_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, BLESERVICE_TURBIDITYVALUE_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
    // See if request is regarding the PhValue Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, bleService_PhValueUUID, pAttr->type.len) )
  {
    if ( offset > BLESERVICE_PHVALUE_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, BLESERVICE_PHVALUE_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has READ permissions.
    *pLen = 0;
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  return status;
}


/*********************************************************************
 * @fn      bleService_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t bleService_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                        uint8 *pValue, uint16 len, uint16 offset,
                                        uint8 method )
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;

  // See if request is regarding a Client Characterisic Configuration
  if ( ! memcmp(pAttr->type.uuid, clientCharCfgUUID, pAttr->type.len) )
  {
    // Allow only notifications.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY);
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has WRITE permissions.
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  // Let the application know something changed (if it did) by using the
  // callback it registered earlier (if it did).
  if (paramID != 0xFF)
    if ( pAppCBs && pAppCBs->pfnChangeCb )
      pAppCBs->pfnChangeCb( paramID ); // Call app function from stack task context.

  return status;
}
