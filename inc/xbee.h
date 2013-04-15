/******************************************************************************
 *@file: xbee.h
 *
 *@brief: 
 *  - This is the header file for xbee communications 
 *
 * Current Target: Arduino
 * Future Target: TLL6527v1-0
 * Compiler: Arduino     Output format: None, sent directly to Arduino
 *
 * @author:  Todd Lunter
 * @created: March 24, 2013
 *
 *******************************************************************************/

#ifndef XBEE_H_
#define XBEE_H_

#include "chunk.h"
#include "bufferPool.h"
#include "queue.h"
#include "uartRx.h"
#include "uartTx.h"

#define GUARD_TIME 1000

#define XBEE_MAX_PAYLOAD_LENGTH 120
#define XBEE_MAX_DATA_LENGTH    100

#define XBEE_RECEIVE_API_BYTE           0x81
#define XBEE_TRANSMIT_API_BYTE          0x01
#define XBEE_TRANSMIT_STATUS_API_BYTE   0x89
#define XBEE_AT_API_BYTE                0x08
#define XBEE_AT_RESPONSE_API_BYTE       0x88

typedef struct {
	uartRx_t *rx;
	uartTx_t *tx;
	bufferPool_t *bp;
} xbee_t;

typedef struct {
    unsigned char payload[XBEE_MAX_PAYLOAD_LENGTH];
    unsigned char length;
} xbee_message_t;

typedef struct {
    unsigned char at_command;
    unsigned short value;
} xbee_at_message_t;

typedef struct {
    unsigned char at_command;
    unsigned char status;
    unsigned short value;
} xbee_at_r_message_t;

typedef struct {
    unsigned short to;
    unsigned char option;
    unsigned char pData[XBEE_MAX_DATA_LENGTH];
    unsigned char length;
} xbee_transmit_message_t;

typedef struct {
    unsigned char status;
} xbee_t_status_message_t;

typedef struct {
    unsigned short from;
    unsigned char signal_strength;
    unsigned char options;
    unsigned char pData[XBEE_MAX_DATA_LENGTH];
    unsigned char length;
} xbee_receive_message_t;

int Xbee_Init(xbee_t *pThis, uartRx_t *rx, uartTx_t *tx, bufferPool_t *pBufPool);

/** Transmit message through the Xbee module
 *
 * @param pMessage	the message to be sent to the Xbee module to be transmitted
 * @param length    the length of the message to be sent
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_SendMessage( xbee_t *pThis, chunk_t *pChunk );

/** Gets messages from the Xbee module
 *
 * @param pMessage    the message to be sent to the Xbee module to be transmitted
 * @param pLength   the length of the message received in bytes
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_GetMessage( xbee_t *pThis, chunk_t *pChunk );
int Xbee_GetMessageNb( xbee_t *pThis, chunk_t *pChunk );

/** Pack an API Message
 *
 * @param msg           message to convert to an xbee message
 * @param pMsg          message contents to be replaced with built messagee
 * @param pLength       length of the built message
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_PackMessage( xbee_message_t *msg, unsigned char *pMsg, unsigned char *pLength );

/** Unpacks a raw byte message
 * 
 * @param pMsg      initial message byte store
 * @param length    length of the byte message
 * @param pXbee_msg message that will contain the value from the raw byte message
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_UnpackMessage( unsigned char *pMsg, unsigned char length, xbee_message_t *pXbee_msg );

/** Prints an Xbee message in a friendly format
 *
 * @param pMsg  the xbee message to print
 *
 * @return Zero on sucess, positive otherwise
 */
int Xbee_PrintMessage(xbee_message_t *pMsg);

/** Sends a transmit message to the desired address with the given data
 *
 * @param to            short address to send the message to
 * @param pData         data to include
 * @param length        length of data
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_SendTransmitMessage( xbee_t *pThis, unsigned short to, chunk_t *pChunk );

/** Pack Transmit Message
 * 
 * @param to            short address to send the message to
 * @param pDataPointer  pointer to the data
 * @param dataLength    length of the data
 * @param pMsg          message to overwrite with data
 * @param pLength       length to overwrite
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_PackTransmitMessage( xbee_transmit_message_t *pTransmit_msg, unsigned char *pMsg, unsigned char *pLength);

/** Unpack Receive Message
 *
 * @param pMsg      byte message to parse
 * @param length    length of byte message
 * @param pMessage  pointer to xbee receive message to replace
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_UnpackReceiveMessage( xbee_message_t *pMsg, xbee_receive_message_t *pMessage );

/** Prints an Xbee receive message in a friendly format
 *
 * @param pMsg  the xbee receive message to print
 *
 * @return Zero on sucess, positive otherwise
 */
int Xbee_PrintReceiveMessage( xbee_receive_message_t *pMsg );

int Xbee_UnpackTransmitStatusMessage( xbee_message_t *pMsg, xbee_t_status_message_t *pStatus_message );

int Xbee_PackApiAtCommandMessage( xbee_at_message_t *pAt_msg, unsigned char *pMsg, unsigned char *pLength );

int Xbee_UnpackAtApiResponseMessage( xbee_message_t *pMsg, xbee_at_r_message_t *pAt_message );

/** Put the Xbee module into command mode
 *
 * @param guard_time	refers to the length of time it takes for the Xbee module
 						to wait for entering command mode
 *
 * @return Zero on success, positive otherwise 
 */
int Xbee_StartCommandMode( xbee_t *pThis, unsigned short guardTime );

/** Force exits command mode
 *  Only call if you know you're in command mode, otherwise message will be sent
 *  To current destination!
 *
 * @return Zero on success
 */
int Xbee_ExitCommandMode( xbee_t *pThis );

/** Get the current address from the module
 *
 * @param pAddr		The destination to overwrite with the current address
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_CommandModeGetAddress( xbee_t *pThis, unsigned short *pAddr );

/** Set the current address for the module
 *
 * @param pAddr		The address to set for the Xbee module
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_CommandModeSetAddress( xbee_t *pThis, unsigned short addr );

/** Gets if the module is in API mode or not
 *
 * @param pApiMode  Returns the level of API mode (0, 1 or 2);
 *
 * @return Zero on sucess, positive otherwise
 */
int Xbee_CommandModeGetApiMode( xbee_t *pThis, unsigned short *pApiMode );

/** Sets the module to the given API Mode
 *
 * @param apiMode   The given API mode level to set the module to
 *
 * @return Zero on sucess, positive otherwise
 */
int Xbee_CommandModeSetApiMode ( xbee_t *pThis, unsigned char apiMode );

#endif
