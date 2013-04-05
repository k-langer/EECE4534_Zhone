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

#ifndef PASS
#define PASS 0
#endif

#ifndef FAIL
#define FAIL 1
#endif

#define GUARD_TIME 1000

#define XBEE_MAX_PAYLOAD_LENGTH 120
#define XBEE_MAX_DATA_LENGTH 100

#define XBEE_RECEIVE_API_BYTE 0x81

typedef struct {
    unsigned char payload[XBEE_MAX_PAYLOAD_LENGTH];
    unsigned char length;
} xbee_message_t;

typedef struct {
    unsigned short to;
    unsigned char option;
    unsigned char pData[XBEE_MAX_DATA_LENGTH];
    unsigned char length;
} xbee_transmit_message_t;

typedef struct {
    unsigned short from;
    unsigned char signal_strength;
    unsigned char options;
    unsigned char pData[XBEE_MAX_DATA_LENGTH];
    unsigned char length;
} xbee_receive_message_t;

typedef enum {
    XBEE_START = 0,
    XBEE_LENGTH_MSB,
    XBEE_LENGTH_LSB,
    XBEE_PAYLOAD,
    XBEE_CRC,
    XBEE_END
} xbee_message_state_t;

typedef enum {
    XBEE_RECEIVE_API_ID = 0,
    XBEE_RECEIVE_SOURCE_MSB,
    XBEE_RECEIVE_SOURCE_LSB,
    XBEE_RECEIVE_SIGNAL_LEVEL,
    XBEE_RECEIVE_OPTIONS,
    XBEE_RECEIVE_DATA
} xbee_receive_message_state_t;

void Xbee_Init( int fd );

/** Transmit message through the Xbee module
 *
 * @param pMessage	the message to be sent to the Xbee module to be transmitted
 * @param length    the length of the message to be sent
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_SendMessage( unsigned char *pMessage, unsigned char length );

/** Gets messages from the Xbee module
 *
 * @param pMessage    the message to be sent to the Xbee module to be transmitted
 * @param pLength   the length of the message received in bytes
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_GetMessage( unsigned char *pMessage, unsigned char *pLength );

/** Pack an API Message
 *
 * @param msg           message to convert to an xbee message
 * @param pMsg          message contents to be replaced with built messagee
 * @param pLength       length of the built message
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_PackMessage( xbee_message_t *msg, unsigned char *pMsg, unsigned char *pLength  );

/** Sends a transmit message to the desired address with the given data
 *
 * @param to            short address to send the message to
 * @param pData         data to include
 * @param length        length of data
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_SendTransmitMessage( unsigned short to, unsigned char *pData, unsigned char length );

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
int Xbee_PackTransmitMessage( xbee_transmit_message_t *msg, unsigned char *pMsg, unsigned char *pLength);

/** Unpack Receive Message
 *
 * @param pMsg      byte message to parse
 * @param length    length of byte message
 * @param pMessage  pointer to xbee receive message to replace
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_UnpackReceiveMessage( xbee_message_t *pMsg, xbee_receive_message_t *pMessage );

/** Put the Xbee module into command mode
 *
 * @param guard_time	refers to the length of time it takes for the Xbee module
 						to wait for entering command mode
 *
 * @return Zero on success, positive otherwise 
 */
int Xbee_StartCommandMode( unsigned short guardTime );

/** Force exits command mode
 *  Only call if you know you're in command mode, otherwise message will be sent
 *  To current destination!
 *
 * @return Zero on success
 */
int Xbee_ExitCommandMode( void );

/** Get the current address from the module
 *
 * @param pAddr		The destination to overwrite with the current address
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_CommandModeGetAddress( unsigned short *pAddr );

/** Set the current address for the module
 *
 * @param pAddr		The address to set for the Xbee module
 *
 * @return Zero on success, positive otherwise
 */
int Xbee_CommandModeSetAddress( unsigned short addr );

/** Gets if the module is in API mode or not
 *
 * @param pApiMode  Returns the level of API mode (0, 1 or 2)
 *
 * @return Zero on sucess, positive otherwise
 */
int Xbee_CommandModeGetApiMode( unsigned short *pApiMode );

/** Sets the module to the given API Mode
 *
 * @param apiMode   The given API mode level to set the module to
 *
 * @return Zero on sucess, positive otherwise
 */
int Xbee_CommandModeSetApiMode ( unsigned short apiMode );

#endif
