 /******************************************************************************
 *@file: xbee.c
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
 * @todo:   fix get message blocking
 *          complete message types
 *          smoother transition between message types
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tll_common.h"
#include "xbee.h"
#include "chunk.h"
#include "bufferPool.h"
#include "queue.h"
#include "uartRx.h"
#include "uartTx.h"

unsigned int Xbee_ParseHex( char *str )
{
    unsigned int value = 0;

    for(;; str++ )
	{
		switch( *str )
		{
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				value = (value << 4) | (*str & 0xf);
				break;
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
				value = (value << 4) | (9 + (*str & 0xf));
				break;
			default:
				return value;
		}
	}
}

void Xbee_Sleep( unsigned int time )
{
	unsigned int i = 0;
	while (i++ < time * 1000000);
}

int Xbee_Init(xbee_t *pThis, uartRx_t *rx, uartTx_t *tx, bufferPool_t *pBufPool)
{
	pThis->rx = rx;
	pThis->tx = tx;
    pThis->bp = pBufPool;

	return PASS;
}

/** Transmit message through the Xbee module
 *
 * @param pMessage    the message to be sent to the Xbee module to be transmitted
 * @param length    the length of the message to be sent
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_SendMessage( xbee_t *pThis, chunk_t *pChunk )
{
    if (FAIL == uartTx_send(pThis->tx, pChunk))
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
int Xbee_ParseMessage( xbee_t *pThis, chunk_t *pChunk)
{
    int return_type = PASS;
    if (pChunk->s08_buff[0] == 0x7e )
    {
        xbee_message_t *msg = (xbee_message_t *)malloc(sizeof(xbee_message_t));
        if (FAIL == Xbee_UnpackMessage(pChunk->u08_buff, pChunk->bytesUsed, msg))
        {
            return_type = FAIL;
        }
        else
        {
            xbee_receive_message_t *receive_msg = (xbee_receive_message_t *)malloc(sizeof(xbee_receive_message_t));
            switch (msg->payload[0])
            {
                case XBEE_RECEIVE_API_BYTE:
                    Xbee_UnpackReceiveMessage(msg, receive_msg);
                    int i;
                    for (i = 0; i < receive_msg->length; i++)
                    {
                        pChunk->u08_buff[i] = receive_msg->pData[i];
                    }
                    pChunk->bytesUsed = receive_msg->length;
                    break;
                case XBEE_TRANSMIT_STATUS_API_BYTE:
                    return_type = FAIL;
                    break;
                case XBEE_AT_RESPONSE_API_BYTE:
                    return_type = FAIL;
                    break;
                default:
                    return_type = FAIL;
                    break;
            }
        }
    }

    if (FAIL == return_type)
    {
        pChunk->bytesUsed = 0;
    }

    return return_type;
}

int Xbee_GetMessage( xbee_t *pThis, chunk_t *pChunk )
{
    //do
    //{
        if ( FAIL == uartRx_get(pThis->rx, pChunk))
        {
            return FAIL;
        }
    //}
    //while (FAIL == Xbee_ParseMessage(pThis, pChunk));

    return PASS;
}


int Xbee_GetMessageNb( xbee_t *pThis, chunk_t *pChunk )
{
    //do
    //{
        if ( FAIL == uartRx_getNb(pThis->rx, pChunk))
        {
            return FAIL;
        }
    //}
    //while (FAIL == Xbee_ParseMessage(pThis, pChunk));

    return PASS;
}

/** Pack an API Message
 *
 * @param pXbee_msg     xbee message structure to be made into a byte value
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

/** Unpacks a raw byte message
 * 
 * @param pMsg      initial message byte store
 * @param length    length of the byte message
 * @param pXbee_msg message that will contain the value from the raw byte message
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_UnpackMessage( unsigned char *pMsg, unsigned char length, xbee_message_t *pXbee_msg )
{
    unsigned char unpacked_length = 0;
    unsigned char payload_crc = 0;

    unsigned char payload_length;

    if (pMsg[unpacked_length++] != 0x7e)
    {
        return FAIL;
    }
    unpacked_length++;  // Messages have a given payload length of two bytes but will only
                        // ever be one byte in length max (between 0-255)
    pXbee_msg->length = pMsg[unpacked_length++];

    for (payload_length = 0; payload_length < pXbee_msg->length; payload_length++)
    {
        pXbee_msg->payload[payload_length] = pMsg[unpacked_length++];
        payload_crc += pXbee_msg->payload[payload_length];
    }

    if ((0xff - payload_crc) != pMsg[unpacked_length++] || unpacked_length != length)
    {
        return FAIL;
    }
    return PASS;
}

/** Prints an Xbee message in a friendly format
 *
 * @param pMsg  the xbee message to print
 *
 * @return Zero on sucess, positive otherwise
 */
int Xbee_PrintMessage(xbee_message_t *pMsg)
{
    printf("Length: %d\n", pMsg->length);
    printf("Payload:");
    unsigned char length;
    for (length = 0; length < pMsg->length; length++)
        printf(" %02x", pMsg->payload[length]);
    printf("\n");

    return PASS;
}

/** Sends a transmit message to the desired address with the given data
 *
 * @param to            short address to send the message to
 * @param pData         data to include
 * @param length        length of data
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_SendTransmitMessage( xbee_t *pThis, unsigned short to, chunk_t *pChunk )
{
    xbee_transmit_message_t *transmit_msg = (xbee_transmit_message_t *)malloc(sizeof(xbee_transmit_message_t));

    transmit_msg->to = to;
	int i;
	for (i = 0; i < pChunk->bytesUsed; i++)
	{
		transmit_msg->pData[i] = pChunk->u08_buff[i];
	}
    transmit_msg->length = pChunk->bytesUsed;

    unsigned char builtMsg[120] = {0};
    unsigned char builtLength = 0;

    if (FAIL == Xbee_PackTransmitMessage(transmit_msg, builtMsg, &builtLength))
    {
        return FAIL;
    }

	for (i = 0; i < builtLength; i++)
	{
		pChunk->u08_buff[i] = builtMsg[i];
	}
	pChunk->bytesUsed = builtLength;

    if (FAIL == Xbee_SendMessage(pThis, pChunk))
    {
        return FAIL;
    }
    
    return PASS;
}

/** Pack Transmit Message
 * 
 * @param pTransmit_msg transmit message structure of data
 * @param pMsg          message to overwrite with data
 * @param pLength       length to overwrite
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_PackTransmitMessage( xbee_transmit_message_t *pTransmit_msg, unsigned char *pMsg, unsigned char *pLength )
{
	static unsigned char frame = 1;
	if (frame == 0)
		frame++;

    xbee_message_t *xbee_msg = (xbee_message_t *)malloc(sizeof(xbee_message_t));
    xbee_msg->length = 0;

    xbee_msg->payload[xbee_msg->length++] = XBEE_TRANSMIT_API_BYTE;
    xbee_msg->payload[xbee_msg->length++] = frame;
    xbee_msg->payload[xbee_msg->length++] = (pTransmit_msg->to >> 8);
    xbee_msg->payload[xbee_msg->length++] = (pTransmit_msg->to & 0xff);
    xbee_msg->payload[xbee_msg->length++] = 0x00;
    
    unsigned char i;
    for (i = 0; i < pTransmit_msg->length; i++)
    {
        xbee_msg->payload[xbee_msg->length++] = pTransmit_msg->pData[i];
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

    if (pMsg->payload[parsed_length++] != XBEE_RECEIVE_API_BYTE)
    {
        return FAIL;
    }

    pMessage->from = (pMsg->payload[parsed_length++] << 8) & 0xff00;
    pMessage->from |= (pMsg->payload[parsed_length++] & 0x00ff);
    pMessage->signal_strength = pMsg->payload[parsed_length++];
    pMessage->options = pMsg->payload[parsed_length++];
	pMessage->length = 0;
    while (parsed_length < pMsg->length)
    {
        pMessage->pData[pMessage->length++] = pMsg->payload[parsed_length++];
    }

    return PASS;
}

/** Prints an Xbee receive message in a friendly format
 *
 * @param pMsg  the xbee receive message to print
 *
 * @return Zero on sucess, positive otherwise
 */
int Xbee_PrintReceiveMessage( xbee_receive_message_t *pMsg )
{
    printf("From: %02x %02x\n", pMsg->from >> 8, pMsg->from & 0xff);
    printf("Signal: %02x\n", pMsg->signal_strength);
    printf("Length: %d\n", pMsg->length);
    printf("Data:");
    unsigned char length;
    for (length = 0; length < pMsg->length; length++)
        printf(" %02x", pMsg->pData[length]);
    printf("\n");

    return PASS;
}

int Xbee_UnpackTransmitStatusMessage( xbee_message_t *pMsg, xbee_t_status_message_t *pStatus_message )
{
    unsigned char parsed_length = 0;

    if (pMsg->payload[parsed_length++] != XBEE_TRANSMIT_STATUS_API_BYTE)
    {
        return FAIL;
    }
    parsed_length++; // Don't care about the frame

    pStatus_message->status = pMsg->payload[parsed_length++];

    return PASS;
}

int Xbee_PackApiAtCommandMessage( xbee_at_message_t *pAt_msg, unsigned char *pMsg, unsigned char *pLength )
{
	static unsigned char frame = 1;
	if (frame == 0)
		frame++;

    xbee_message_t *xbee_msg = (xbee_message_t *)malloc(sizeof(xbee_message_t));
    xbee_msg->length = 0;

    xbee_msg->payload[xbee_msg->length++] = XBEE_AT_API_BYTE;
    xbee_msg->payload[xbee_msg->length++] = frame;
    xbee_msg->payload[xbee_msg->length++] = pAt_msg->at_command >> 8;
    xbee_msg->payload[xbee_msg->length++] = pAt_msg->at_command & 0xff;

    char msg[4] = {0};
    sprintf(msg, "%04x", pAt_msg->value);
    xbee_msg->payload[xbee_msg->length++] = msg[0];
    xbee_msg->payload[xbee_msg->length++] = msg[1];
    xbee_msg->payload[xbee_msg->length++] = msg[2];
    xbee_msg->payload[xbee_msg->length++] = msg[3];

    return Xbee_PackMessage(xbee_msg, pMsg, pLength);
}

int Xbee_UnpackAtApiResponseMessage( xbee_message_t *pMsg, xbee_at_r_message_t *pAt_message )
{
    unsigned char parsed_length = 0;

    if (pMsg->payload[parsed_length++] != XBEE_AT_RESPONSE_API_BYTE)
    {
        return FAIL;
    }
    parsed_length++; // Don't care about the frame

    pAt_message->at_command = pMsg->payload[parsed_length++] << 8;
    pAt_message->at_command |= pMsg->payload[parsed_length++] & 0xff;
    pAt_message->status = pMsg->payload[parsed_length++];

    pMsg->payload[pMsg->length] = 0;
    pAt_message->value = Xbee_ParseHex((char*)pMsg->payload+parsed_length);

    return PASS;
}
    
/** Put the Xbee module into command mode
 *
 * @param guard_time    refers to the length of time it takes for the Xbee module
 *                        to wait for entering command mode
 *
 * @return Zero on success, positive otherwise 
 */
int Xbee_StartCommandMode( xbee_t *pThis, unsigned short guardTime )
{
	chunk_t *command_chunk;
	bufferPool_acquire(pThis->bp, &command_chunk);
	unsigned char msg[] = { '+', '+', '+' };
	int i;
	for (i = 0; i < 3; i++)
	{
		command_chunk->u08_buff[i] = msg[i];
	}
	command_chunk->bytesUsed = 3;

    Xbee_Sleep(guardTime);
    Xbee_SendMessage(pThis, command_chunk);
    Xbee_Sleep(guardTime);

	bufferPool_acquire(pThis->bp, &command_chunk);
    if (PASS == Xbee_GetMessage(pThis, command_chunk))
    {
        if (3 == command_chunk->bytesUsed)
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
int Xbee_ExitCommandMode( xbee_t *pThis )
{
	chunk_t *command_chunk;
	bufferPool_acquire(pThis->bp, &command_chunk);
	unsigned char msg[] = { 'A', 'T', 'C', 'N', '\r' };
	int i;
	for (i = 0; i < 5; i++)
	{
		command_chunk->u08_buff[i] = msg[i];
	}
	command_chunk->bytesUsed = 5;
    Xbee_SendMessage(pThis, command_chunk);

	bufferPool_acquire(pThis->bp, &command_chunk);
    if (PASS == Xbee_GetMessage(pThis, command_chunk))
    {
        if (3 == command_chunk->bytesUsed)
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
int Xbee_CommandModeGetAddress( xbee_t *pThis, unsigned short *pAddr )
{
	chunk_t *command_chunk;
	bufferPool_acquire(pThis->bp, &command_chunk);
	unsigned char msg[] = { 'A', 'T', 'M', 'Y', '\r' };
	int i;
	for (i = 0; i < 5; i++)
	{
		command_chunk->u08_buff[i] = msg[i];
	}
	command_chunk->bytesUsed = 5;
    Xbee_SendMessage(pThis, command_chunk);

	bufferPool_acquire(pThis->bp, &command_chunk);
    if (PASS == Xbee_GetMessage(pThis, command_chunk))
    {
        if (0 < command_chunk->bytesUsed)
        {
			command_chunk->u08_buff[command_chunk->bytesUsed] = 0;
			*pAddr = Xbee_ParseHex((char*)command_chunk->s08_buff);

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
int Xbee_CommandModeSetAddress( xbee_t *pThis, unsigned short addr )
{
	chunk_t *command_chunk;
	bufferPool_acquire(pThis->bp, &command_chunk);

    char addrStr[4] = {0};
    sprintf((char*)addrStr, "%04x\r", addr);

    unsigned char msg[] = { 'A', 'T', 'M', 'Y', addrStr[0], addrStr[1], addrStr[2], addrStr[3], '\r' };
	int i;
	for (i = 0; i < 9; i++)
	{
		command_chunk->u08_buff[i] = msg[i];
	}
	command_chunk->bytesUsed = 9;
    Xbee_SendMessage(pThis, command_chunk);

	bufferPool_acquire(pThis->bp, &command_chunk);
    if (PASS == Xbee_GetMessage(pThis, command_chunk))
    {
        if (3 == command_chunk->bytesUsed)
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
int Xbee_CommandModeGetApiMode( xbee_t *pThis, unsigned short *pApiMode )
{
	chunk_t *command_chunk;
	bufferPool_acquire(pThis->bp, &command_chunk);
	unsigned char msg[] = { 'A', 'T', 'A', 'P', '\r' };
	int i;
	for (i = 0; i < 5; i++)
	{
		command_chunk->u08_buff[i] = msg[i];
	}
	command_chunk->bytesUsed = 5;
    Xbee_SendMessage(pThis, command_chunk);

	bufferPool_acquire(pThis->bp, &command_chunk);
    if (PASS == Xbee_GetMessage(pThis, command_chunk))
    {
        if (0 < command_chunk->bytesUsed)
        {
			command_chunk->u08_buff[command_chunk->bytesUsed] = 0;
			*pApiMode = Xbee_ParseHex((char*)command_chunk->s08_buff);

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
int Xbee_CommandModeSetApiMode ( xbee_t *pThis, unsigned char apiMode )
{
	chunk_t *command_chunk;
	bufferPool_acquire(pThis->bp, &command_chunk);
    unsigned char msg[] = { 'A', 'T', 'A', 'P', apiMode + 0x30, '\r' };
	int i;
	for (i = 0; i < 6; i++)
	{
		command_chunk->u08_buff[i] = msg[i];
	}
	command_chunk->bytesUsed = 6;
    Xbee_SendMessage(pThis, command_chunk);

	bufferPool_acquire(pThis->bp, &command_chunk);
    if (PASS == Xbee_GetMessage(pThis, command_chunk))
    {
        if (3 == command_chunk->bytesUsed)
        {
            return PASS;
        }
    }
    return FAIL;
}
