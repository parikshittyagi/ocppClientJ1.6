/**
 *@file            main.c
 *@author          Parikshit Tyagi
 *@version         1.4
 *@date            19 August 2019
 *@brief           The Code Structure is below stated -->
 *                 --> main.c                     @entry point of the code, it initiates the OCPP1.6 Process
 *                 --> ocpp_process.c/.h          @This library handles the events occurring w.r.t to OCPP, the machine states can be found
 *                                                 here. Moreover this library start Websockets connection with the CMS URL mentioned in config.json
 *                 --> ocpp_helper.c/.h           @This library is a helper library for OCPP1.6 Process, it has functions which helps in achieving 
 *                                                 OCPP client functionalities.
 *                                                 For generating UUID the reference is taken from here -> https://github.com/karelzak/util-linux/tree/master/libuuid
 *                 --> ocpp_ws_client.c/.h        @This library is responsible for connecting OCPP client to CMS with the help of WebSockets. It has 
 *                                                 process of handshaking according to OCPPJ1.6 and once accepted it has events which helps in knowing 
 *                                                 the nature of communication over Transport Layer
 *                                                 Currently the communication over Websockets is unsecured, soon planned to move on TLS1.2
 *                                                 The main refrence of this library can be found here https://github.com/payden/libwsclient .
 *                 --> utils.c/.h                 @This library is the helper library of ocpp_ws_client.
 *                 --> cJSON.c/.h                 @This library is taken from https://github.com/DaveGamble/cJSON. Since OCPP1.6J is mostly based on JSON
 *                                                 packets over sockets, hence heavy use of this library is done in complete client. This library is written
 *                                                 in C and does not require any other dependecy and can be easily ported.
 *                 --> config.json                @This is configuration file, all the OCPP1.6J configuration to be set here in JSON format
 *                 --> compile.sh                 @This compiles the complete source code and generate executable with name of "main"
 *
 *                 Abbreviations Used -->
 *                 --> CP                         @Charge Point
 *                 --> CMS                        @Charging Management System
 *                 --> OCPP                       @Open Charge Point Protocol
 *                 --> WS                         @WebSockets
 *                 --> WSS                        @WebSockets Secured
 *                 --> TLS                        @Transport Layer Security
 *
 *@references      --> https://www.oasis-open.org/committees/download.php/58944/ocpp-1.6.pdf 
 *                 --> https://github.com/karelzak/util-linux/tree/master/libuuid
 *                 --> https://github.com/payden/libwsclient
 *                 --> https://github.com/DaveGamble/cJSON
 *                 --> https://tools.ietf.org/html/rfc6455
 *                 --> https://tools.ietf.org/html/rfc4122
 *                  
 *                 Functionalities -->
 *                 @BootNotification done with "Accepted" case         
*/


#include "main.h"


int main(int argc, char const *agrv[])
{
    //Main Entry of OCPP Client
    fprintf(stderr,"Device Started Successfully\n");
    //Calling ocpp process to start
    ocpp_process_start();

    return 0;

}
