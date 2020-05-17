#ifndef __OCPP_PROCESS_H
#define __OCPP_PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "ocpp_ws_client.h"
#include "ocpp_helper.h"
#include "cJSON.h"
#include "main.h"


//OCPP Machine States, as per defined in the documentation
typedef enum{
    Authorize,
    BootNotification,
    CancelReservation,
    ChangeAvailability,
    ChangeConfiguration,
    ClearCache,
    ClearChargingProfile,
    DataTransfer,
    DiagnosticsStatusNotification,
    FirmwareStatusNotification,
    GetConfiguration,
    GetDiagnostics,
    GetLocalListVersion,
    Heartbeat,
    MeterValues,
    RemoteStartTransaction,
    RemoteStopTransaction,
    GetCompositeSchedule,
    ReserveNow,
    Reset,
    StartTransaction,
    StatusNotification,
    StopTransaction,
    SendLocalList,
    SetChargingProfile,
    TriggerMessage,
    UnlockConnector,
    UpdateFirmware
}OCPP_States;

//HeartBeat Interval used for pinging to CMS for mainting the connection
uint64_t heartBeatInterval;
//If the CP is rejected or state becomes pending, it is the interval when it will again retry 
uint64_t retryInterval;

//OCPP Process starting point
void ocpp_process_start();
int onclose(wsclient *c);               // This function is called when close event occurs in websocket connection
int onerror(wsclient *c, wsclient_error *err);          // This function is called when error event occurs in websocket connection
int onmessage(wsclient *c, wsclient_message *msg);         // This function is called when message event occurs in websocket connection
int onopen(wsclient *c);                // This function is called webSocket is just open

#endif