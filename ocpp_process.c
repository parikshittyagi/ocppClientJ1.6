/**
*@file            ocpp_process.c
*@author          Parikshit Tyagi     
*@version         1.4
*@date            20 August 2019
*@brief           This library is the entry point of OCPP Process, all the states are handled here.
*                 With the help of ocpp_helper and ocpp_ws_client, it follows https://www.oasis-open.org/committees/download.php/58944/ocpp-1.6.pdf 
*                 for ocpp client implementation. Please read the above link briefly for more understanding between CP and CMS
*/

#include "ocpp_process.h"

/**
 *  These are the message codes when any RPC call is made from CMS or CP 
*/
char *ocpp_message_call_code = "2";          //This message code is used when any RPC call is to be made
char *ocpp_message_response_code = "3";      //This message code is used when response of any RPC call is to be given
char *ocpp_message_error_code = "4";        //This message code is used when there is some error in sending packet or Transport layer

//This is path given for the configuration file
const char *configfilePath = "config.json";

//OCPP buffer used with index matching to ENUM of OCPPStates such that OCPPStates name can be used instead of its ENUM values
char *ocpp_states_Buffer[] = {
    "Authorize",
    "BootNotification",
    "DataTransfer",
    "DiagnosticsStatusNotification",
    "FirmwareStatusNotification",
    "Heartbeat",
    "MeterValues",
    "StartTransaction",
    "StatusNotification",
    "StopTransaction"
};


OCPP_States ocppstates = BootNotification;          //According to OCPP1.6, BootNotification is the first state which registers itself to network
bool isMessageSend = false;                  //Flag used if message is send from CP to CMS
bool isWSEstablished = false;               //Flag used if connection is established between CP and CMS

ocpp_frame *ocppSendFrame = NULL;          //OCPP Send Frame defined as per ocpp_frame structure in ocpp_helper.h
ocpp_frame *ocppReceiveFrame = NULL;       //OCPP Receive Frame defined as per ocpp_frame structure in ocpp_helper.h

/**
 * This function is called when close connection event is called from either client or Server 
 */
int onclose(wsclient *c)
{
    fprintf(stderr, "onclose called: %d\n", c->sockfd);
    return 0;
}


/**
 * This function is called when any error event has occured in the connection
 */

int onerror(wsclient *c, wsclient_error *err)
{
    fprintf(stderr, "onerror: (%d): %s\n", err->code, err->str);
    if (err->extra_code)
    {
        errno = err->extra_code;
        perror("recv");
    }
    return 0;
}


/**
 * This function is called whenever the client receives any message from server 
 */
int onmessage(wsclient *c, wsclient_message *msg)
{
    fprintf(stderr, "onmessage: (%llu): %s\n", msg->payload_len, msg->payload);
    
    //This condition checks if message was send by CP or CMS
    if(isMessageSend)
    {
        //Switch condition to check which state the current machine is
        switch (ocppstates)
        {

        //BootNotfication case
        case BootNotification:
                ocppReceiveFrame = parseOCPPFrame(msg->payload);   //calling the function which parses the char *payload to ocpp_frame
                if(!strcmp(ocppReceiveFrame->messageCode, (char *)ocpp_message_response_code)) //As mentioned by OCPP, respose code should be checked for RPC call
                {
                    fprintf(stderr, "Response code matched\n");
                    //As per OCPP1.6J if the request is made by CP then CMS should send same messageID which is GUID for us
                    if(!strcmp(ocppReceiveFrame->messageID, ocppSendFrame->messageID))
                    {
                        //Once it is confirmed that it is response from CMS, then checking conditions according to BootNotification.response.req
                        fprintf(stderr, "MessageID matched with the messageID sent\n");

                        if(!strcmp(cJSON_GetObjectItem(ocppReceiveFrame->jsonPacket, "status")->valuestring,"Accepted"))   //If CP is accepted in the network
                        {
                            fprintf(stderr, "Charging station accepted by CMS\n");
                            //This functions gets the time from CMS in response and then change the time of client for time synchronization
                            if(0 != changeTimeOfMachine(cJSON_GetObjectItem(ocppReceiveFrame->jsonPacket, "currentTime")->valuestring)) 
                            {
                                fprintf(stderr, "Failed to change the time of the system\n");
                            }
                            fprintf(stderr, "System time changed successfully\n");
                            heartBeatInterval = cJSON_GetObjectItem(ocppReceiveFrame->jsonPacket, "interval")->valueint; //Parsing heartbeat interval
                            ocppstates = Heartbeat;      //Changing OCPPState to heartbeat
                        }
                        else if(!strcmp(cJSON_GetObjectItem(ocppReceiveFrame->jsonPacket, "status")->valuestring,"Pending"))  //If status is still pending 
                        {
                            fprintf(stderr, "assigned retry interval in pending state\n");
                            retryInterval = cJSON_GetObjectItem(ocppReceiveFrame->jsonPacket, "interval")->valueint;     //Assigning retryInterval
                        }
                        else   //Third condition when CMS rejects the CP
                        {
                            fprintf(stderr,"Charging station is not accepted, Please register this Charging Station\n");
                        }
                    }
                }
                else
                {
                    fprintf(stderr, "Received error while getting the data\n");
                }
            break;
        
        default:
            break;
        }
    }
    return 0;
}

/**
 *  This function is called when for the first time connection is maintained between server and client
 */
int onopen(wsclient *c)
{
    fprintf(stderr, "onopen called: %d\n", c->sockfd);
    isWSEstablished = true;      //Changing the flag to true once connection is established
    return 0;
}

/**
 * This is the main entry point for OCPP-Start process and all the OCPP States are handled here only
 */
void ocpp_process_start(void)
{
    printf("\nStarted the OCPP Client\n");
    cJSON *configJson = NULL;              //assigning JSON pointer for config.json
    cJSON *developmentPacket = NULL;       //assigning JSON pointer for development information in config.json
    configJson = readConfigFile(configfilePath);   //This function reads the config.json file and returns cJSON * packet
    developmentPacket = cJSON_GetObjectItem(configJson, "development");   //Getting development JSON packet from configJson
    if(developmentPacket == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);    
        } 
    }
   
    /**
     * OCPP WebURL --> ws://<IPAddressOfServer>/<ChargePoint ID>  
     * Rightnow we are using port 8080 for development but in production, for ws it becomes 80 and for wss it becomes 443
     * change the port accordingly in ocpp_ws_client void *libwsclient_handshake_thread(void *ptr) function
     */

    char *url = NULL;               //assigning URL pointer of char type for the URL of the server i.e websocket connection address
    //dynamically allocating memory equivalent to URL size 
    //Total size of URL --> sizeof(WS_URL)+sizeof(chargePointID)+1
    url = (char *)malloc(strlen(cJSON_GetObjectItem(developmentPacket, "WS_URL")->valuestring)+(strlen(cJSON_GetObjectItem(developmentPacket, "chargePointID")->valuestring))+ 1);
    if(url == NULL)
    {
        fprintf(stderr, "Unable to allocate memory to the URL\n");
    }
    memset(url, 0, sizeof(url));       //initialising URL memory with zero
    strcpy(url, cJSON_GetObjectItem(developmentPacket, "WS_URL")->valuestring);     //copying the WS_URL from development Packet
    strcat(url, cJSON_GetObjectItem(developmentPacket, "chargePointID")->valuestring);   //concatinating chargePoint ID after the URL
    fprintf(stderr,"URL after concatenation -> %s\n", url);
   
    /**
     * This section is used in connecting OCPP Client with server
     */
   
    wsclient *myclient = NULL;
    myclient = libwsclient_new(url);                //Calling function from ocpp_ws_client to initiate connection
    libwsclient_onopen(myclient, &onopen);          //Functions to bind Open Event in Websockets with function --> int onopen(wsclient *c)
    libwsclient_onmessage(myclient, &onmessage);    ///Functions to bind onMessage Event in Websockets with function --> int onmessage(wsclient *c, wsclient_message *msg)
    libwsclient_onerror(myclient, &onerror);        //Functions to bind onerror Event in Websockets with function --> int onerror(wsclient *c, wsclient_error *err)
    libwsclient_onclose(myclient, &onclose);        //Functions to bind onclose Event in Websockets with function --> int onclose(wsclient *c)
    libwsclient_run(myclient);                      //Functions binds myclient returned from libwsclient(url) and starts communicating
   
   /**
    *  Once the connection is established between server and client, OCPP state machine starts
    *  isWSEstablished becomes true once the CP connection accepted by CMS 
    */

    while(isWSEstablished)
    {
        switch (ocppstates)
        {
        /**
         * BootNotification is the first state that comes in OCPP which shares the information of the CP to CMS
         */
        case BootNotification:
            if(!isMessageSend)       //Checking if the BootNotification packet is sent or not 
            {
                fprintf(stderr, "Sending BootNotification Packet\n");
                //calling function to form OCPP Packet for bootNotification
                ocppSendFrame = formOCPPFrame(ocpp_message_call_code, ocpp_states_Buffer[ocppstates], bootNotificationRequest(configJson));    
                //Once OCPP packet is made, it is passed to sendFrameToCMS function, which takecare of OCPP payload to be made and then send it to server
                if(0 != sendFrameToCMS(myclient, ocppSendFrame))
                {
                    fprintf(stderr, "Not able to send packet, please check websocket connection\n");
                }       
                //On successful sending data flag is made true
                isMessageSend = true; 
            }
            //Waiting for the server to respond
            if(isMessageSend){
                printf("Back to while loop\n");
                sleep(1);
            }
            break;
        
        case Heartbeat:
            printf("Inside HeartBeat function\n");
            sleep(1);
            break;

        default:
            printf("Inside default switch case\n");
            sleep(1);
            break;
        }
    }

    //Freeing the memory allocation assigned dynamically
    free(url);
    free(ocppReceiveFrame);
    free(ocppSendFrame);
    cJSON_Delete(developmentPacket);
    libwsclient_finish(myclient);       //closing the websocket connection while finishing connection

}