/**
 * @file                utils.c
 * @author               Parikshit Tyagi          
 * @version             1.4
 * @date                3 July 2019
 * @brief               This is the utility library used for sha1 digest and base64 encoding.
 *                      Mainly used in packet framing of the payload as mention in websockets 
 *                      (https://tools.ietf.org/html/rfc6455) official rfc, for more details
 *                      please refer to the above link.                     
 */


#include "utils.h"

/*  
 *  SHA1Reset
 *
 *  Description:
 *      This function will initialize the SHA1Context in preparation
 *      for computing a new message digest.
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to reset.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1Reset(SHA1Context *context)
{
    context->Length_Low = 0;
    context->Length_High = 0;
    context->Message_Block_Index = 0;

    context->Message_Digest[0] = 0x67452301;
    context->Message_Digest[1] = 0xEFCDAB89;
    context->Message_Digest[2] = 0x98BADCFE;
    context->Message_Digest[3] = 0x10325476;
    context->Message_Digest[4] = 0xC3D2E1F0;

    context->Computed = 0;
    context->Corrupted = 0;
}

/*  
 *  SHA1Input
 *
 *  Description:
 *      This function accepts an array of octets as the next portion of
 *      the message.
 *
 *  Parameters:
 *      context: [in/out]
 *          The SHA-1 context to update
 *      message_array: [in]
 *          An array of characters representing the next portion of the
 *          message.
 *      length: [in]
 *          The length of the message in message_array
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1Input(SHA1Context *context,
               const unsigned char *message_array,
               unsigned length)
{
    if (!length)
    {
        return;
    }

    if (context->Computed || context->Corrupted)
    {
        context->Corrupted = 1;
        return;
    }

    while (length-- && !context->Corrupted)
    {
        context->Message_Block[context->Message_Block_Index++] =
            (*message_array & 0xFF);

        context->Length_Low += 8;
        /* Force it to 32 bits */
        context->Length_Low &= 0xFFFFFFFF;
        if (context->Length_Low == 0)
        {
            context->Length_High++;
            /* Force it to 32 bits */
            context->Length_High &= 0xFFFFFFFF;
            if (context->Length_High == 0)
            {
                /* Message is too long */
                context->Corrupted = 1;
            }
        }

        if (context->Message_Block_Index == 64)
        {
            SHA1ProcessMessageBlock(context);
        }

        message_array++;
    }
}


/*  
 *  SHA1ProcessMessageBlock
 *
 *  Description:
 *      This function will process the next 512 bits of the message
 *      stored in the Message_Block array.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      Many of the variable names in the SHAContext, especially the
 *      single character names, were used because those were the names
 *      used in the publication.
 *         
 *
 */
void SHA1ProcessMessageBlock(SHA1Context *context)
{
    const unsigned K[] = /* Constants defined in SHA-1   */
        {
            0x5A827999,
            0x6ED9EBA1,
            0x8F1BBCDC,
            0xCA62C1D6};
    int t;                  /* Loop counter                 */
    unsigned temp;          /* Temporary word value         */
    unsigned W[80];         /* Word sequence                */
    unsigned A, B, C, D, E; /* Word buffers                 */

    /*
     *  Initialize the first 16 words in the array W
     */
    for (t = 0; t < 16; t++)
    {
        W[t] = ((unsigned)context->Message_Block[t * 4]) << 24;
        W[t] |= ((unsigned)context->Message_Block[t * 4 + 1]) << 16;
        W[t] |= ((unsigned)context->Message_Block[t * 4 + 2]) << 8;
        W[t] |= ((unsigned)context->Message_Block[t * 4 + 3]);
    }

    for (t = 16; t < 80; t++)
    {
        W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
    }

    A = context->Message_Digest[0];
    B = context->Message_Digest[1];
    C = context->Message_Digest[2];
    D = context->Message_Digest[3];
    E = context->Message_Digest[4];

    for (t = 0; t < 20; t++)
    {
        temp = SHA1CircularShift(5, A) +
               ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 20; t < 40; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 40; t < 60; t++)
    {
        temp = SHA1CircularShift(5, A) +
               ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 60; t < 80; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    context->Message_Digest[0] =
        (context->Message_Digest[0] + A) & 0xFFFFFFFF;
    context->Message_Digest[1] =
        (context->Message_Digest[1] + B) & 0xFFFFFFFF;
    context->Message_Digest[2] =
        (context->Message_Digest[2] + C) & 0xFFFFFFFF;
    context->Message_Digest[3] =
        (context->Message_Digest[3] + D) & 0xFFFFFFFF;
    context->Message_Digest[4] =
        (context->Message_Digest[4] + E) & 0xFFFFFFFF;

    context->Message_Block_Index = 0;
}


/*  
 *  SHA1Result
 *
 *  Description:
 *      This function will return the 160-bit message digest into the
 *      Message_Digest array within the SHA1Context provided
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to use to calculate the SHA-1 hash.
 *
 *  Returns:
 *      1 if successful, 0 if it failed.
 *
 *  Comments:
 *
 */
int SHA1Result(SHA1Context *context)
{

    if (context->Corrupted)
    {
        return 0;
    }

    if (!context->Computed)
    {
        SHA1PadMessage(context);
        context->Computed = 1;
    }

    return 1;
}


/*  
 *  SHA1PadMessage
 *
 *  Description:
 *      According to the standard, the message must be padded to an even
 *      512 bits.  The first padding bit must be a '1'.  The last 64
 *      bits represent the length of the original message.  All bits in
 *      between should be 0.  This function will pad the message
 *      according to those rules by filling the Message_Block array
 *      accordingly.  It will also call SHA1ProcessMessageBlock()
 *      appropriately.  When it returns, it can be assumed that the
 *      message digest has been computed.
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to pad
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1PadMessage(SHA1Context *context)
{
    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second
     *  block.
     */
    if (context->Message_Block_Index > 55)
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while (context->Message_Block_Index < 64)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }

        SHA1ProcessMessageBlock(context);

        while (context->Message_Block_Index < 56)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }
    else
    {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while (context->Message_Block_Index < 56)
        {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }
}

/**
 * Nonce is 16 bit randow key used for Sec-WebSocket-Key in the handshaking 
 * mechanism
 */

/* reference from https://tools.ietf.org/html/rfc4648#section-4 */
int get_nonce(uint8_t *dest, int len)
{
    FILE *fp;
    size_t n;

    fp = fopen("/dev/urandom", "r");
    if (fp)
    {
        n = fread(dest, len, 1, fp);
        fclose(fp);
        return n;
    }

    return -1;
}


int base64_encode(const void *src, size_t srclen, void *dest, size_t destsize)
{
    char *Base64 =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    const uint8_t *input = src;
    char *output = dest;

    while (srclen > 0)
    {
        int skip = 1;
        int i0 = input[0] >> 2;
        int i1 = (input[0] & 0x3) << 4;
        int i2 = 64;
        int i3 = 64;

        if (destsize < 5)
            return -1;

        if (srclen > 1)
        {
            skip++;
            i1 += input[1] >> 4;
            i2 = (input[1] & 0xF) << 2;

            if (srclen > 2)
            {
                i2 += input[2] >> 6;
                i3 = input[2] & 0x3F;
                skip++;
            }
        }

        *output++ = Base64[i0];
        *output++ = Base64[i1];
        *output++ = Base64[i2];
        *output++ = Base64[i3];

        input += skip;
        srclen -= skip;
        destsize -= 4;
    }

    *output++ = 0;
    return output - (char *)dest - 1;
}
