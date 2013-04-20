#include "tll_common.h"
#include "uartRx.h"
#include "uartTx.h"
#include "bufferPool.h"
#include "wire.h"

int Wire_Init(wire_t *pThis, uartRx_t *rx, uartTx_t *tx, bufferPool_t *bp)
{
    pThis->rx = rx;
    pThis->tx = tx;
    pThis->bp = bp;

    return PASS;
}

int Wire_SendMessage(wire_t *pThis, chunk_t *pChunk)
{
    chunk_t *new_chunk;
    while (pChunk->bytesUsed > 0)
    {
        if (FAIL == bufferPool_acquire(pThis->bp, &new_chunk))
        {
            return FAIL;
        }

        if (FAIL == Wire_PackMessage(pChunk, new_chunk))
        {
            return FAIL;
        }

        if (FAIL == uartTx_send(pThis->tx, new_chunk))
        {
            return FAIL;
        }
    }
    bufferPool_release(pThis->bp, pChunk);
    return PASS;
}

int Wire_GetMessageAbs(wire_t *pThis, chunk_t *pChunk)
{
    if (FAIL == Wire_UnpackMessage(pChunk))
    {
        pChunk->bytesUsed = 0;
        return FAIL;
    }
    return PASS;
}

int Wire_GetMessage(wire_t *pThis, chunk_t *pChunk)
{
    if (FAIL == uartRx_get(pThis->rx, pChunk))
    {
        pChunk->bytesUsed = 0;
        return FAIL;
    }
    return Wire_GetMessageAbs(pThis, pChunk);
}

int Wire_GetMessageNb(wire_t *pThis, chunk_t *pChunk)
{
    if (FAIL == uartRx_getNb(pThis->rx, pChunk))
    {
        pChunk->bytesUsed = 0;
        return FAIL;
    }
    return Wire_GetMessageAbs(pThis, pChunk);
}

int Wire_PackMessage(chunk_t *pChunk, chunk_t *new_chunk)
{
    int packet_length = (pChunk->bytesUsed <= 100) ? pChunk->bytesUsed : 100;
    int packed_length;
    unsigned char payload_crc = 0;

    new_chunk->u08_buff[0] = 0x7e;
    new_chunk->u08_buff[1] = (packet_length >> 8) & 0xff;
    new_chunk->u08_buff[2] = packet_length & 0xff;

    for (packed_length = 0; packed_length < packet_length; packed_length++)
    {
        new_chunk->u08_buff[packed_length + 3] = pChunk->u08_buff[packed_length];
        payload_crc += new_chunk->u08_buff[packed_length + 3];
    }

    if (pChunk->bytesUsed > 100)
    {
        for (packed_length = 0; packed_length < pChunk->bytesUsed - 100; packed_length++)
        {
            pChunk->u08_buff[packed_length] = pChunk->u08_buff[packed_length + 100];
        }
        pChunk->bytesUsed = pChunk->bytesUsed - 100;
    }

    new_chunk->u08_buff[packet_length + 3] = 0xff - payload_crc;
	new_chunk->bytesUsed = packet_length + 4;

    return PASS;
}

int Wire_UnpackMessage(chunk_t *pChunk)
{
    int packet_length = (pChunk->u08_buff[1] << 8) | pChunk->u08_buff[2];
    int unpacked_length;
    unsigned char payload_crc = 0;

    if (0x7e != pChunk->u08_buff[0] || 0 >= packet_length)
    {
        return FAIL;
    }

    for (unpacked_length = 0; unpacked_length < packet_length; unpacked_length++)
    {
        pChunk->u08_buff[unpacked_length] = pChunk->u08_buff[unpacked_length + 3];
        payload_crc += pChunk->u08_buff[unpacked_length];
    }

    if (pChunk->u08_buff[unpacked_length + 3] != (0xff - payload_crc))
    {
        return FAIL;
    }

    pChunk->bytesUsed = packet_length;

    return PASS;
}
