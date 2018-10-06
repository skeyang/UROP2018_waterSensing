/*
 * Sensor connection:
 * Temperature sensor   DIO23
 * Pressure sensor      DIO24
 * Flow rate sensor     DIO25
 * Conductivity sensor  DIO26
 * Turbidity sensor     DIO27
 */
/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <xdc/std.h>

#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file
#include <xdc/runtime/Log.h>

#include <ti/sysbios/knl/Clock.h>

#include <ti/drivers/ADC.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

#include "custom_fmt.h"

// Sensor Controller Interface
#include "scif.h"

// BLuetooth Developer Studio
#include <ble_service.h>

#include "project_zero.h"

#include <stdio.h>


/*********************************************************************
 * GLOBAL VARIABLES
 */
static uint32_t     g_sensorLastTick = 0;
static Clock_Struct g_sensorClock;

/*********************************************************************
 * LOCAL FUNCTION DECLARATIONS
 */
// HWI
static void SC_ctrlReadyHwiCb(void);
static void SC_taskAlertHwiCb(void);

// SWI
static void SC_rangerClockSwiFxn(UArg a0);

// TASK
//static void SC_processAdc(void);
static void SC_processSensor(void);


/*********************************************************************
 * HWI CALLBACKS
 */

/*
 * @brief   Callback from Scif driver on Control READY interrupt.
 *
 *          Signals main task with empty msg APP_MSG_SC_CTRL_READY.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SC_ctrlReadyHwiCb(void)
{
    // Signal main loop
    user_enqueueRawAppMsg(APP_MSG_SC_CTRL_READY, NULL, 0);
} // SC_ctrlReadyHwiCb


/*
 * @brief   Callback from Scif driver on Task ALERT interrupt.
 *
 *          Signals main task with empty msg APP_MSG_SC_TASK_ALERT.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SC_taskAlertHwiCb(void)
{
    // Signal main loop
    user_enqueueRawAppMsg(APP_MSG_SC_TASK_ALERT, NULL, 0);
} // SC_taskAlertHwiCb


/*********************************************************************
 * SWI CALLBACKS
 */

/*
 * @brief   Callback from Clock module on timeout.
 *
 *          Stores current clock tick, and signals main task with
 *          empty msg APP_MSG_SC_EXEC_SENSOR.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SC_sensorClockSwiFxn(UArg a0)
{

} // SC_rangerClockSwiFxn


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*
 * @brief   Processing function for the ADC SC task.
 *
 *          Is called whenever the APP_MSG_SC_TASK_ALERT msg is sent
 *          and ADC SC task has generated an alert.
 *
 *          Retrieves ADC value from SC, and calculates and sets new period and
 *          timeout for Clock object.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SC_processSensor(void)
{
    // Retrieve sensor values and and send to phone via bluetooth

    //// Temperature //////////////////////////////////////////////////////////////////
    float adcTemp = scifTaskData.adc.output.adcTempValue;
    float Temp = adcTemp*430/4096; // convert analog value to temperature in celcius
    // Notify the change to the BLE service
    char pTempLine[10];
    if(Temp < 10) itoaAppendStr(pTempLine, Temp, "  ");
    else if(Temp < 100 & Temp >=10) itoaAppendStr(pTempLine, Temp, " ");
    else itoaAppendStr(pTempLine, Temp, "");
    user_enqueueCharDataMsg(APP_MSG_UPDATE_CHARVAL, 0,
                            BLESERVICE_SERV_UUID, BLESERVICE_TEMPERATUREVALUE,
                            (uint8_t *)pTempLine, strlen(pTempLine));

    //// Pressure /////////////////////////////////////////////////////////////////////
    uint16_t adcPress = scifTaskData.adc.output.adcPressureValue;
    // Notify the change to the BLE service
    char pPressLine[20];
    itoaAppendStr(pPressLine, adcPress, "");
    user_enqueueCharDataMsg(APP_MSG_UPDATE_CHARVAL, 0,
                            BLESERVICE_SERV_UUID, BLESERVICE_PRESSUREVALUE,
                            (uint8_t *)pPressLine, strlen(pPressLine));

    //// Flow /////////////////////////////////////////////////////////////////////////
    uint16_t adcFlow = scifTaskData.adc.output.adcFlowValue;
    // Notify the change to the BLE service
    char pFlowLine[20];
    itoaAppendStr(pFlowLine, adcFlow, "");
    user_enqueueCharDataMsg(APP_MSG_UPDATE_CHARVAL, 0,
                            BLESERVICE_SERV_UUID, BLESERVICE_FLOWVALUE,
                            (uint8_t *)pFlowLine, strlen(pFlowLine));


    //// Conductivity /////////////////////////////////////////////////////////////////
    float adcConductivity = scifTaskData.adc.output.adcConductivityValue;
    // convert analog value to conductivity, includes temperature compensation
    float TempCoefficient=1.0+0.0185*(Temp-25.0);
    float inputVoltage = adcConductivity*4300/4096;
    float CoefficientVoltage = inputVoltage/TempCoefficient;
    uint16_t Conductivity = 0;
    if(CoefficientVoltage<=448){
        Conductivity=6.84*CoefficientVoltage-64.32;
    }
    else if(CoefficientVoltage<=1457){
        Conductivity=6.98*CoefficientVoltage-127;
    }
    else{
        Conductivity=5.3*CoefficientVoltage+2278;
    }
    // Notify the change to the BLE service
    char pConductLine[20];
    if (Conductivity < 10) itoaAppendStr(pConductLine, Conductivity, "    ");
    else if (Conductivity < 100 & Conductivity >= 10) itoaAppendStr(pConductLine, Conductivity, "   ");
    else if (Conductivity < 1000 & Conductivity >= 100) itoaAppendStr(pConductLine, Conductivity, "  ");
    else if (Conductivity < 10000 & Conductivity >= 1000) itoaAppendStr(pConductLine, Conductivity, " ");
    else itoaAppendStr(pConductLine, Conductivity, "");
    user_enqueueCharDataMsg(APP_MSG_UPDATE_CHARVAL, 0,
                            BLESERVICE_SERV_UUID, BLESERVICE_CONDUCTIVITYVALUE,
                            (uint8_t *)pConductLine, strlen(pConductLine));

    //// Turbidity ////////////////////////////////////////////////////////////////////////
    float adcTurbidity = scifTaskData.adc.output.adcTurbidityValue;
    float voltTurbidity = adcTurbidity*4300/4096; // Convert analog value to millivolts
    // Notify the change to the BLE service
    char pTurbLine[20];
    if(voltTurbidity < 10) itoaAppendStr(pTurbLine, voltTurbidity, "   ");
    else if(voltTurbidity < 100 & voltTurbidity >= 10) itoaAppendStr(pTurbLine, voltTurbidity, "  ");
    else if(voltTurbidity < 1000 & voltTurbidity >= 100) itoaAppendStr(pTurbLine, voltTurbidity, " ");
    else itoaAppendStr(pTurbLine, voltTurbidity, "");
    user_enqueueCharDataMsg(APP_MSG_UPDATE_CHARVAL, 0,
                            BLESERVICE_SERV_UUID, BLESERVICE_TURBIDITYVALUE,
                            (uint8_t *)pTurbLine, strlen(pTurbLine));

    /////// pH //////////////////////////////////////////////////////////////////////////////

    /* Initialise UART communication for the pH sensor
     * Make sure that the jumpers for the RXD and TXD pins are removed
     * or else the sensor can't send data via UART
     */
    char rxBuffer[6];
    UART_init();
    UART_Handle uart;
    UART_Params uartParams;
    UART_Params_init(&uartParams);
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.writeMode = UART_MODE_BLOCKING;
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_NEWLINE;
    uartParams.baudRate = 9600;
    uart = UART_open(0, &uartParams);
    if(uart){
        UART_read(uart, &rxBuffer, sizeof(rxBuffer));
    }
    // Notify the change to the BLE service
    user_enqueueCharDataMsg(APP_MSG_UPDATE_CHARVAL, 0,
                               BLESERVICE_SERV_UUID, BLESERVICE_PHVALUE,
                               (uint8_t *)rxBuffer, strlen(rxBuffer));
    UART_close(uart);

    user_toggleLED(0);
} // SC_processAdc


/*
 * @brief   Processing function for the Ranger SC task.
 *
 *          Is called whenever the APP_MSG_SC_TASK_ALERT msg is sent
 *          and Ranger SC task has generated an alert.
 *
 *          Retrieves both high and low tdc values from SC, extends into 32-bit,
 *          converts into cm, and prints out the value on Log.
 *
 * @param   None.
 *
 * @return  None.
 */
//static void SC_processSensor(void)
//{
//    // Toggle LED1 (Green) on LP
//    user_toggleLED(1);
//} // SC_processRanger


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/*
 * @brief   Called before main loop.
 *
 *          Initializes Scif driver, registers callbacks, configures RTC, and
 *          starts SC tasks.
 *
 * @param   None.
 *
 * @return  None.
 */
void SC_init(void)
{
    // Insert default params
    Clock_Params clockParams;
    Clock_Params_init(&clockParams);
    // Set period to 0 ms
    clockParams.period = 0;
    // Initialize the clock object / Clock_Struct previously added globally.
    Clock_construct(&g_sensorClock,        // global clock struct
                    SC_sensorClockSwiFxn,  // callback from clock
                    0,                     // Initial delay before first timeout
                    &clockParams);         // clock parameters

    // Initialize the Sensor Controller
    scifOsalInit();
    scifOsalRegisterCtrlReadyCallback(SC_ctrlReadyHwiCb);
    scifOsalRegisterTaskAlertCallback(SC_taskAlertHwiCb);
    scifInit(&scifDriverSetup);

    uint32_t rtcHz = 1; // 3Hz RTC
    scifStartRtcTicksNow(0x00010000 / rtcHz);

    // Configure SC Tasks here, if any

    // Start Sensor Controller
    scifStartTasksNbl(BV(SCIF_ADC_TASK_ID));

    //Log_info0("scTask initialization done");
} // SC_init


/*
 * @brief   Processing function for the APP_MSG_SC_CTRL_READY event.
 *
 *          Is called from main loop whenever the APP_MSG_SC_CTRL_READY msg is
 *          sent.
 *
 *          Currently does nothing.
 *
 * @param   None.
 *
 * @return  None.
 */
void SC_processCtrlReady(void)
{
  // Do nothing

} // SC_processCtrlReady


/*
 * @brief   Processing function for the APP_MSG_SC_TASK_ALERT event.
 *
 *          Is called from main loop whenever the APP_MSG_SC_TASK_ALERT msg is
 *          sent.
 *
 *          Checks which SC tasks are active, and calls their corresponding
 *          processing function. Also clears and ACKs the interrupts to the
 *          Scif driver.
 *
 * @param   None.
 *
 * @return  None.
 */
void SC_processTaskAlert(void)
{
    // Clear the ALERT interrupt source
    scifClearAlertIntSource();

    // Do SC Task processing here

    // Check which task called and do process
    SC_processSensor();

    // Acknowledge the ALERT event
    scifAckAlertEvents();
} // SC_processTaskAlert

/*
 * @brief   Processing function for the APP_MSG_SC_EXEC_SENSOR event.
 *
 *          Is called from main loop whenever the APP_MSG_SC_EXEC_SENSOR msg is
 *          sent.
 *
 *          Executes the Ranger SC task once.
 *
 * @param   None.
 *
 * @return  None.
 */

void SC_execSensor(void)
{
} // SC_execRanger
