#ifndef WIRE_H_
#define WIRE_H_

#include "tll_common.h"
#include "uartRx.h"
#include "uartTx.h"
#include "bufferPool.h"

#define WIRE_MAX_PAYLOAD_LENGTH 100

typedef struct {
    uartRx_t *rx;
    uartTx_t *tx;
    bufferPool_t *bp;
} wire_t;

typedef struct {
    unsigned char payload[WIRE_MAX_PAYLOAD_LENGTH];
    unsigned char length;
} wire_message_t;

int Wire_Init(wire_t *pThis, uartRx_t *rx, uartTx_t *tx, bufferPool_t *bp);

int Wire_SendMessage(wire_t *pThis, chunk_t *pChunk);

int Wire_GetMessage(wire_t *pThis, chunk_t *pChunk);
int Wire_GetMessageNb(wire_t *pThis, chunk_t *pChunk);

int Wire_PackMessage(chunk_t *pChunk, chunk_t *new_chunk);
int Wire_UnpackMessage(chunk_t *pChunk);

#endif
