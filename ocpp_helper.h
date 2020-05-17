#ifndef __OCPP_HELPER_H
#define __OCPP_HELPER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "ocpp_ws_client.h"
#include "cJSON.h"

//Message length defined for OCPP Payload
#define MESSAGE_LEN 1000

//Declaring buffer for OCPP Payload
char ocppMessagePayload[MESSAGE_LEN];

//Declaring GUID 16 octects according to RFC4122 https://tools.ietf.org/html/rfc4122
typedef unsigned char uuid_t[16];

//UUID structure defined as mentioned in RFC4122
struct uuid
{
    uint32_t     time_low;
    uint16_t     time_mid;
    uint16_t     time_hi_and_version;
    uint16_t     clock_seq;
    uint8_t      node[6];
};

//UUID comes in lower and upper cases, to convert the GUID returned and changing it to lower format
static const char *fmt_lower =
	"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x";

//OCPP frame structure defined
typedef struct _ocpp_frame
{
    char *messageCode;     //messageCode of character type
    char *messageID;       //messageID or UUID for uniquely identifying each message
    char *action;          //What action is to be used
    cJSON *jsonPacket;     //JSON packet associated with that particular action 
}ocpp_frame;

/**
 * Fucntion prototypes defined here
*/
static int random_get_fd(void);
static void random_get_bytes(void *buf, size_t nbytes);
static void __uuid_generate_random(uuid_t out, int *num);
static void uuid_unpack(const uuid_t in, struct uuid *uu);
static void uuid_pack(const struct uuid *uu, uuid_t ptr);
static void uuid_unparse_lower(const uuid_t uu, char *out);
static void uuid_unparse_x(const uuid_t uu, char *out, const char *fmt);
ocpp_frame *formOCPPFrame(char *messagecode, char *actiontocms, cJSON *jsonpacket);
cJSON *readConfigFile(const char *filePath);
cJSON *bootNotificationRequest(cJSON *config);
int sendFrameToCMS(wsclient *c,ocpp_frame *packet);
ocpp_frame *parseOCPPFrame(char *payload);
static int getSubString(char *source, char *target,int from, int to);
int changeTimeOfMachine(char *isoTime);

#endif
