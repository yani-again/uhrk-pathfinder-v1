#include <string.h>
#include "ublox.h"
#include "cmsis_gcc.h"

/* UBX functions */

uint16_t UBX_CalculateChecksum(uint8_t *pData, uint16_t length)
{
    uint8_t CK_A, CK_B;
    uint8_t checksum;
    CK_A = CK_B = 0;    // in case auto-initialisation isn't 0

    for (uint16_t i = 0; i < length; ++i)
    {
        CK_A = CK_A + pData[i];
        CK_B = CK_B + CK_A;
    }

    checksum = CK_A | (CK_B << 8);
    return checksum;
}


/* NMEA functions */

/*
 * NOTES
 * - No VTG parsing for now - speed/course isn't part of the beacon's responsibility, unless it's
 *   used for launch/land detection (in which case I'll work on it)
 */

/*
 * @brief Lets you know if an NMEA message is an ANTSTATUS message or not
 *
 * @param A single NMEA message starting with '$' and ending with the checksum
 */
bool NMEA_In_AntStatus(uint8_t *pMsg)
{
    if (memcmp(&pMsg[ANTSTATUS_START_INDEX], ANTSTATUS_PREFIX, ANTSTATUS_PREFIX_LEN) != 0)
        return false;
    return true;
}

/*
 * @brief Reports the ANTSTATUS of the relevant TXT message
 *
 * @param A valid ANTSTATUS NMEA message
 */
NMEA_AntStatus NMEA_Parse_AntStatus(uint8_t *pMsg)
{
    uint8_t first_char = pMsg[ANTSTATUS_START_INDEX + ANTSTATUS_PREFIX_LEN];
    uint8_t second_char = pMsg[ANTSTATUS_START_INDEX + ANTSTATUS_PREFIX_LEN + 1];

    switch (first_char)
    {
        case 'I':
            // INIT status
            return NMEA_ANT_INIT;
            break;
        case 'D':
            // DONTKNOW status
            return NMEA_ANT_DONTKNOW;
            break;
        case 'S':
            // SHORT status
            return NMEA_ANT_SHORT;
            break;
        case 'O':
            if (second_char == 'K')
                return NMEA_ANT_OK;     // OK status
            else
                return NMEA_ANT_OPEN;   // OPEN status
            break;
        default:
            // not a real status
            return NMEA_ANT_ERR;
            break;
    }
}

void uBLOX_BufferInit(uBLOX_BufferTypeDef* buffer)
{
    buffer->head = 0;
    buffer->tail = 0;
}

/*
 * @brief Returns the value of the buffer at position of `tail`
 *
 * @param Pointer to buffer typedef
 *
 * @return 0 if buffer is empty, the data otherwise
 *
 * @warn It's impossible to tell if the return value 0 is due to buffer being empty
 *       of the data actually holding that value, so the user must also check if
 *       the buffer is empty before calling this
 */
uint8_t uBLOX_BufferPop(uBLOX_BufferTypeDef* buffer_t)
{
    uint8_t ret_val;

    if (buffer_t->head == buffer_t->tail)
    {
        ret_val = 0;
    }
    else
    {
        ret_val = buffer_t->buffer[buffer_t->tail];
        __COMPILER_BARRIER();
        buffer_t->tail = (buffer_t->tail + 1) & (sizeof(buffer_t->buffer) - 1);
    }

    return ret_val;
}

/*
 * @brief Adds new value to the buffer at the position of `head`
 *
 * @param Pointer to buffer typedef and the 1-byte data
 *
 * @return 0 if data was added successfully, 1 otherwise (buffer full)
 */
uint8_t uBLOX_BufferPush(uBLOX_BufferTypeDef* buffer_t, uint8_t data)
{
    uint16_t new_head = (buffer_t->head + 1) & (sizeof(buffer_t->buffer) - 1);

    if (new_head == buffer_t->tail)
    {
        // no space for new data
        return 1;
    }
    else
    {
        buffer_t->buffer[buffer_t->head] = data;
        __COMPILER_BARRIER();
        buffer_t->head = new_head;
        return 0;
    }
}
