## Code structure

**main.c**  -->  Entry point of the code, it initiates the OCPP1.6 Process

**ocpp_process.c/.h**  -->    This library handles the events occurring w.r.t to OCPP, the machine states can be found
here. Moreover this library start Websockets connection with the CMS URL mentioned in config.json

**ocpp_helper.c/.h** -->     This library is a helper library for OCPP1.6 Process, it has functions which helps in achieving OCPP client functionalities.For generating UUID the reference is taken from [here](https://github.com/karelzak/util-linux/tree/master/libuuid)

**ocpp_ws_client.c/.h** -->  This library is responsible for connecting OCPP client to CMS with the help of WebSockets. It has 
process of handshaking according to OCPPJ1.6 and once accepted it has events which helps in knowing the nature of communication over Transport Layer.The main refrence of this library can be found [here](https://github.com/payden/libwsclient)

**utils.c/.h** -->           This library is the helper library of ocpp_ws_client.

**cJSON.c/.h** -->          This library is taken from [here](https://github.com/DaveGamble/cJSON). Since OCPP1.6J is mostly based on JSON packets over sockets, hence heavy use of this library is done in complete client. This library is written in C and does not require any other dependecy and can be easily ported.

**config.json** -->         This is configuration file, all the OCPP1.6J configuration to be set here in JSON format

**compile.sh** -->          This compiles the complete source code and generate executable with name of "main"

---

## Setting up connection

This code is tested with Steve server for OCPPJ1.6. The steve server code can be found [here](https://github.com/RWTH-i5-IDSG/steve).To start connection with your respective server below mentioned are the steps to be followed.

1. In config.json file, put the WS URL of server in **WS_URL** field.
2. Put the client ID in **chargePointID** field.
3. The **chargePointProfile** is used in boot notification message from client side for providing charger information to the server.
4. If the dev platform supports gcc, then simply run **compile.sh** file and you will get an executable with name of **test**
6. Use this executable to run and connect with ocpp complaint CMS server

---

## How to take up further development

Currently this client only supports bootNotification message and it's response, since OCPP is completely functionality dependent, it's better to use your own use cases for further developement, but there are already functions that can be used to create OCPP message or parse occpp messages.

1. Use **parseOCPPFrame** function to parse any message from CMS and it will return a structure of **ocpp_frame** type from which further JSON packet can be retrieved.
2. To form any OCPP frame, use **formOCPPFrame** function and to send it to server use **sendFrameToCMS** function.

---

If further support is required, you can write me at parikshit04tyagi@gmail.com. Also if this source codes helps you in some way, don't forget to award a star to this repo. Happy Coding !!

