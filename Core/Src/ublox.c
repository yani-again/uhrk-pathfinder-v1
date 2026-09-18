#import "ublox.h"

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
