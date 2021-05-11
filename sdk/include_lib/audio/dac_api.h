#ifndef __DAC_API_H__
#define __DAC_API_H__
#include "typedef.h"
#include "dac.h"
//#include "typedef.h"

typedef enum {
    SR_NORMAL = 1,
    SR_CONSTANT,
} SR_WORKMODE;

#define DAC_PACKET_SIZE     (32*8)

#define DAC_DECODER_KICK_SIZE     (DAC_PACKET_SIZE * 4)

#define DAC_DECODER_BUF_SIZE     (DAC_PACKET_SIZE * 5)



bool dac_mute(bool mute);
void dac_mode_init(void);
void dac_init_api(u32 sr);
void dac_sr_api(u32 sr);
void dac_off_api(void);
// void dac_sr_api(u32 sr);
void fill_dac_fill(u8 *buf, u32 len);
u8 dac_vol(char set, u8 vol);
void dac_usb_vol(u8 vol_r, u8 vol_l);
bool regist_dac_channel(void *sound, void *kick);
bool unregist_dac_channel(void *dec_hld);

#endif

