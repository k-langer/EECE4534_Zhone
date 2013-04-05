 /******************************************************************************
 *@file: xbee.h
 *
 *@brief: 
 *  - Allows communications to and from an Xbee module
 *
 * Current Target: Arduino
 * Future Target: TLL6527v1-0
 * Compiler: Arduino     Output format: None, sent directly to Arduino
 *
 * @author:  Todd Lunter
 * @created: March 24, 2013
 *
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "inc/xbee.h"

int xbee_fd = -1;

void Xbee_Init( int fd )
{
    xbee_fd = fd;
}

/** Transmit message through the Xbee module
 *
 * @param pMessage    the message to be sent to the Xbee module to be transmitted
 * @param length    the length of the message to be sent
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_SendMessage( unsigned char *pMessage, unsigned char length )
{
    if (length != write(xbee_fd, pMessage, length))
    {
        return FAIL;
    }
    return PASS;
}

/** Gets messages from the Xbee module
 *
 * @param pMessage    the message to be sent to the Xbee module to be transmitted
 * @param pLength   the length of the message received in bytes
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_GetMessage( unsigned char *pMessage, unsigned char *pLength )
{
    int length = 0;
    int count = 0;
    while (count < 3)
    {
        length = read(xbee_fd, pMessage+(length++), 120);
        if (length < 0)
        {
            count++;
            usleep(100000);
        }
        else
        {
            break;
        }
    }
    *pLength = length;
    return (length > 0) ? PASS : FAIL;
}

/** Pack an API Message
 *
 * @param msg           xbee message structure to be made into a byte value
 * @param pMsg          message contents to be replaced with built messagee
 * @param pLength       length of the built message
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_PackMessage( xbee_message_t *msg, unsigned char *pMsg, unsigned char *pLength )
{
    unsigned char length = 0;
    unsigned char payload_crc = 0;

    pMsg[length++] = 0x7e;
    pMsg[length++] = (msg->length >> 8);
    pMsg[length++] = (msg->length & 0x00ff);

    unsigned char i;
    for (i = 0; i < msg->length; i++)
    {
        pMsg[length++] = msg->payload[i];
        payload_crc += msg->payload[i];
    }

    pMsg[length++] = (0xff - payload_crc);

    *pLength = length;

    return PASS;
}

int Xbee_UnpackMessage( unsigned char *pMsg, unsigned char *pLength, xbee_message_t *msg )
{
}

/** Sends a transmit message to the desired address with the given data
 *
 * @param to            short address to send the message to
 * @param pData         data to include
 * @param length        length of data
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_SendTransmitMessage( unsigned short to, unsigned char *pData, unsigned char length )
{
    xbee_transmit_message_t *transmit_msg = (xbee_transmit_message_t *)malloc(sizeof(xbee_transmit_message_t));

    transmit_msg->to = to;
    memcpy(transmit_msg->pData, pData, length);
    transmit_msg->length = length;

    unsigned char builtMsg[120] = {0};
    unsigned char builtLength = 0;

    if (FAIL == Xbee_PackTransmitMessage(transmit_msg, builtMsg, &builtLength))
    {
        return FAIL;
    }

    if (FAIL == Xbee_SendMessage(builtMsg, builtLength))
    {
        return FAIL;
    }
    
    return PASS;
}

/** Pack Transmit Message
 * 
 * @param transmit_msg  transmit message structure of data
 * @param pMsg          message to overwrite with data
 * @param pLength       length to overwrite
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_PackTransmitMessage( xbee_transmit_message_t *transmit_msg, unsigned char *pMsg, unsigned char *pLength)
{
    xbee_message_t *xbee_msg = (xbee_message_t *)malloc(sizeof(xbee_message_t));
    xbee_msg->length = 0;

    xbee_msg->payload[xbee_msg->length++] = 0x01;
    xbee_msg->payload[xbee_msg->length++] = 0x01;
    xbee_msg->payload[xbee_msg->length++] = (transmit_msg->to >> 8);
    xbee_msg->payload[xbee_msg->length++] = (transmit_msg->to & 0xff);
    xbee_msg->payload[xbee_msg->length++] = 0x00;
    
    unsigned char i;
    for (i = 0; i < transmit_msg->length; i++)
    {
        xbee_msg->payload[xbee_msg->length++] = transmit_msg->pData[i];
    }

    return Xbee_PackMessage(xbee_msg, pMsg, pLength);
}

/** Unpack Receive Message
 *
 * @param pMsg      xbee message to parse
 * @param pMessage  pointer to xbee receive message to replace
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_UnpackReceiveMessage( xbee_message_t *pMsg, xbee_receive_message_t *pMessage )
{
    unsigned char parsed_length = 0;
    xbee_receive_message_state_t state = XBEE_RECEIVE_API_ID;

    if (pMsg->payload[parsed_length++] == XBEE_RECEIVE_API_BYTE)
    {
        state = XBEE_RECEIVE_SOURCE_MSB;
    } else {
        return FAIL;
    }

    pMessage->from = ((pMsg->payload[parsed_length++] << 8) | (pMsg->payload[parsed_length++] & 0xff));
    pMessage->signal_strength = pMsg->payload[parsed_length++];
    pMessage->options = pMsg->payload[parsed_length++];
    while (parsed_length < pMsg->length)
    {
        pMessage->pData[pMessage->length++] = pMsg->payload[parsed_length++];
    }

    return PASS;
}
    
/** Put the Xbee module into command mode
 *
 * @param guard_time    refers to the length of time it takes for the Xbee module
 *                        to wait for entering command mode
 *
 * @return Zero on success, positive otherwise 
 */
int Xbee_StartCommandMode( unsigned short apiMode )
{
    sleep(apiMode);
    Xbee_SendMessage((unsigned char *)"+++", 3);
    sleep(apiMode);
    sleep(10);
    unsigned char msg[100] = {0};
    unsigned char length = 0;

    if (PASS == Xbee_GetMessage(msg, &length))
    {
        if (length == 3)
        {
            return PASS;
        }
    }
    return FAIL;
}

/** Force exits command mode
 *  Only call if you know you're in command mode, otherwise message will be sent
 *  To current destination!
 *
 * @return Zero on success
 */
int Xbee_ExitCommandMode( void )
{
    Xbee_SendMessage((unsigned char*)"ATCN\r", 5);
    sleep(10);
    unsigned char msg[100] = {0};
    unsigned char length = 0;
    if (PASS == Xbee_GetMessage(msg, &length))
    {
        if (length == 3)
        {
            return PASS;
        }
    }
    return FAIL;
}

/** Get the current address from the module
 *
 * @param pAddr        The destination to overwrite with the current address
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_CommandModeGetAddress( unsigned short *pAddr )
{
    Xbee_SendMessage((unsigned char*)"ATMY\r", 5);
    sleep(10);
    unsigned char msg[100] = {0};
    unsigned char length = 0;
    if (PASS == Xbee_GetMessage(msg, &length))
    {
        if (length > 0)
        {
            *pAddr = 0;
            unsigned char i;
            for (i = 0; i < length; i++)
            {
                int power = 1;
                unsigned char j;
                for (j = 0; j < (length - 1 - j); j++)
                    power *= 16;

                *pAddr += msg[i] * power;
            }
            return PASS;
        }
    }
    return FAIL;
}

/** Set the current address for the module
 *
 * @param pAddr        The address to set for the Xbee module
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_CommandModeSetAddress( unsigned short addr )
{
    unsigned char tempStr[30] = "ATMY";
    char addrStr[30] = {0};
    sprintf((char*)addrStr, "%04x\r", addr);
    strcat((char*)tempStr, addrStr);
    Xbee_SendMessage(tempStr, 9);
    sleep(10);
    unsigned char msg[100] = {0};
    unsigned char length = 0;
    if (PASS == Xbee_GetMessage(msg, &length))
    {
        if (length == 3)
        {
            return PASS;
        }
    }
    return FAIL;
}

/** Gets if the module is in API mode or not
 *
 * @param pApiMode  Returns the level of API mode (0, 1 or 2)
 *
 * @return Zero on sucess, positive otherwise
 */
int Xbee_CommandModeGetApiMode( unsigned short *pApiMode )
{
    Xbee_SendMessage((unsigned char*)"ATAP\r", 5);
    sleep(10);
    unsigned char msg[100] = {0};
    unsigned char length = 0;
    if (PASS == Xbee_GetMessage(msg, &length))
    {
        if (length > 0)
        {
            *pApiMode = 0;
            unsigned char i;
            for (i = 0; i < length; i++)
            {
                int power = 1;
                unsigned char j;
                for (j = 0; j < (length - 1 - j); j++)
                    power *= 16;

                *pApiMode += msg[i] * power;
            }
            return PASS;
        }
    }
    return FAIL;
}

/** Sets the module to the given API Mode
 *
 * @param apiMode   The given API mode level to set the module to
 *
 * @return Zero on sucess, positive otherwise
 */
int Xbee_CommandModeSetApiMode ( unsigned short apiMode )
{
    unsigned char tempStr[30] = "ATAP";
    char apiModeStr[30] = {0};
    sprintf((char*)apiModeStr, "%01x\r", apiMode);
    strcat((char*)tempStr, apiModeStr);
    Xbee_SendMessage(tempStr, 6);
    sleep(10);
    unsigned char msg[100] = {0};
    unsigned char length = 0;
    if (PASS == Xbee_GetMessage(msg, &length))
    {
        if (length == 3)
        {
            return PASS;
        }
    }
    return FAIL;
}
