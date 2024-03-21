#include "iis_pcm_defs.h"
// exampl
// int16_t res = map(rawData[count],0,255,-32768,32767);
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// Convert binary format to BCD
uint8_t dec2bcd(uint8_t dec)
{
    return ( (dec/10)<<4) | (dec % 10);
}
// Convert BCD format to BCD binary
uint8_t bcd2dec(uint8_t bcd)
{
    return ((((bcd)>>4) & 0x0F) * 10) + ((bcd) & 0x0F);
}
// Process 24 bit sample to 16 bit
int16_t ProcessSample24bitTo16(uint16_t *samples)
{
//     unsigned int bw;
    uint8_t gain = 2;

    int32_t full_sample = (((int32_t)samples[0] << 16) | samples[1]);
    uint8_t sign = samples[0] >> 15;
    
    full_sample = full_sample << gain;
    int16_t sample = (sign << 15) ;

    int16_t temp_sample = 0x7FFF & (full_sample >> 16);

    // samples_avg += abs(sample | temp_sample);
    return sample | temp_sample;
}

