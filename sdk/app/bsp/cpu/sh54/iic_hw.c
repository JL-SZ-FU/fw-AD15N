#include "iic_hw.h"
#include "clock.h"
/*
    [[  注意!!!  ]]
    * 适用于带cfg_done的硬件IIC，另一种硬件IIC另作说明
    * 硬件IIC的START / ACK(NACK)必须在发送或接收字节cfg_done前设置，且不能
      接cfg_done单独发送；而STOP则应在发送或接收字节cfg_done后设置，必须接
      cfg_done单独发送
*/
#define LOG_TAG_CONST       IIC
#define LOG_TAG             "[iic]"
#include "debug.h"

//硬件IIC设备数据初始化
const struct hw_iic_config hw_iic_cfg[] = {
    {
        //hw_iic0 data
        .port = 'B',
        .baudrate = 200000,
        .hdrive = 0,
        .io_pu = 1,
        .role = IIC_MASTER,
        .slave_addr = 0x54,
        .isr_en = 0
    },
};



#define EINVAL 22
struct iic_iomapping {
    u8 scl;
    u8 sda;
};

static const struct iic_iomapping hwiic_iomap[IIC_HW_NUM][IIC_PORT_GROUP_NUM] = {
    {
        //scl    ,   sda
        {IO_PORT_DP, IO_PORT_DM},    //group a
        {IO_PORTA_01, IO_PORTA_02},  //group b
        {IO_PORTA_04, IO_PORTA_05},  //group c
        {IO_PORTA_08, IO_PORTA_09},  //group d
    },
};

static JL_IIC_TypeDef *const iic_regs[IIC_HW_NUM] = {
    JL_IIC,
};

#define iic_get_id(iic)         (iic)

#define iic_info_port(iic)      (hw_iic_cfg[iic_get_id(iic)].port - 'A')
#define iic_info_baud(iic)      (hw_iic_cfg[iic_get_id(iic)].baudrate)
#define iic_info_slave_addr(iic)      (hw_iic_cfg[iic_get_id(iic)].slave_addr)
#define iic_info_hdrive(iic)    (hw_iic_cfg[iic_get_id(iic)].hdrive)
#define iic_info_io_filt(iic)   (hw_iic_cfg[iic_get_id(iic)].io_filter)
#define iic_info_io_pu(iic)     (hw_iic_cfg[iic_get_id(iic)].io_pu)
#define iic_info_role(iic)      (hw_iic_cfg[iic_get_id(iic)].role)

static inline u32 iic_get_scl(hw_iic_dev iic)
{
    u8 port = iic_info_port(iic);
    return hwiic_iomap[iic_get_id(iic)][port].scl;
}

static inline u32 iic_get_sda(hw_iic_dev iic)
{
    u8 port = iic_info_port(iic);
    return hwiic_iomap[iic_get_id(iic)][port].sda;
}

static int iic_port_init(hw_iic_dev iic)
{
    u32 reg;
    int ret = 0;
    u8 port;
    u8 id = iic_get_id(iic);
    u32 scl, sda;
    /*log_info("id=%d\n",id);*/
    port = iic_info_port(iic);
    /*log_info("port=%d\n",port);*/
    if (port >= IIC_PORT_GROUP_NUM) {
        return -EINVAL;
    }
    scl = iic_get_scl(iic);
    sda = iic_get_sda(iic);
    if (id == 0) {
        reg = JL_IOMC->IOMC1;
        reg &= ~(0x3 << 8);
        reg |= (port << 8);
        JL_IOMC->IOMC1 = reg;
#if 0
        if (port == 0) {
            //USB_DP, USB_DM init
            log_info("warning!!!  iic overwrite usb configuration\n");
            JL_USB_IO->CON0 |= (BIT(11) | BIT(8)); //IO MODE
            /* JL_USB_IO->CON0 &= ~BIT(2);  //DP DIR OUT */
            /* JL_USB_IO->CON0 &= ~BIT(3);  //DM DIR OUT */
            /* JL_USB_IO->CON0 |= BIT(0);  //DP output 1 */
            /* JL_USB_IO->CON0 |= BIT(1);  //DM output 1 */
            if (iic_info_io_pu(iic)) {
                JL_USB_IO->CON0 |= BIT(6);  //DP PU
                JL_USB_IO->CON0 |= BIT(7);  //DM PU
            } else {
                JL_USB_IO->CON0 &= ~BIT(6);  //DP PU
                JL_USB_IO->CON0 &= ~BIT(7);  //DM PU
            }
        } else {
            //gpio_direction_output(sca, 1);
            //gpio_direction_output(sda, 1);
            if (iic_info_hdrive(iic)) {
                gpio_set_hd(scl, 1);
                gpio_set_hd(sda, 1);
            } else {
                gpio_set_hd(scl, 0);
                gpio_set_hd(sda, 0);
            }
            if (iic_info_io_pu(iic)) {
                gpio_set_pull_up(scl, 1);
                gpio_set_pull_up(sda, 1);
            } else {
                gpio_set_pull_up(scl, 0);
                gpio_set_pull_up(sda, 0);
            }
        }
#else
        if (port == 0) {
            //usb_iomode(1);/////////////////////////////////////////////////////////
        }
        if (iic_info_hdrive(iic)) {
            gpio_set_hd(scl, 1);
            gpio_set_hd(sda, 1);
        } else {
            gpio_set_hd(scl, 0);
            gpio_set_hd(sda, 0);
        }
        if (iic_info_io_pu(iic)) {
            gpio_set_pull_up(scl, 1);
            gpio_set_pull_up(sda, 1);
            gpio_set_pull_down(scl, 0);
            gpio_set_pull_down(sda, 0);
            /*log_info("pullup\n");*/
        } else {
            gpio_set_pull_up(scl, 0);
            gpio_set_pull_up(sda, 0);
        }
#endif
        //} else if (fh->id == 1) {
    } else {
        ret = -EINVAL;
    }
    return ret;
}

int hw_iic_set_baud(hw_iic_dev iic, u32 baud)
{
    //f_iic = f_sys / ((IIC_BAUD + 1) * 4)
    //=> IIC_BAUD = f_sys / (4 * f_iic) - 1
    u32 sysclk;
    u8 id = iic_get_id(iic);

    sysclk = clk_get("lsb");
    if (sysclk < 2 * baud) {
        return -EINVAL;
    }
    iic_baud_reg(iic_regs[id]) = sysclk / (4 * baud) - 1;
    return 0;
}

static void hw_iic_set_die(hw_iic_dev iic, u8 en)
{
    u8 id = iic_get_id(iic);
    u8 port = iic_info_port(iic);
    u32 scl, sda;

    if (port >= IIC_PORT_GROUP_NUM) {
        return ;
    }
    scl = iic_get_scl(iic);
    sda = iic_get_sda(iic);
    if (id == 0) {
#if 0
        if (port == 0) {
            if (en) {
                JL_USB_IO->CON0 |= BIT(9);  //DP 1.2V digital input en
                JL_USB_IO->CON0 |= BIT(10);  //DM 1.2V digital input en
            } else {
                JL_USB_IO->CON0 &= ~BIT(9);
                JL_USB_IO->CON0 &= ~BIT(10);
            }
        } else {
            gpio_set_die(scl, en);  //!!!must set
            gpio_set_die(sda, en);  //!!!must set
        }
#else
        gpio_set_die(scl, en);
        gpio_set_die(sda, en);
        /*log_info("set--die\n");*/
#endif
    } else {
        //undefined
    }
}

void hw_iic_suspend(hw_iic_dev iic)
{
    hw_iic_set_die(iic, 0);
}

void hw_iic_resume(hw_iic_dev iic)
{
    hw_iic_set_die(iic, 1);
}

void iic_isr();
int hw_iic_init(hw_iic_dev iic)
{
    int ret;
    u8 id = iic_get_id(iic);

    if ((ret = iic_port_init(iic))) {
        log_error("invalid hardware iic port\n");
        return ret;
    }
    hw_iic_set_die(iic, 1);
    if (iic_info_role(iic) == IIC_MASTER) {
        iic_role_host(iic_regs[id]);
        /*log_info("master\n");*/
        if ((ret = hw_iic_set_baud(iic, iic_info_baud(iic)))) {
            log_error("iic baudrate is invalid\n");
            return ret ;
        }
    } else {
        iic_role_slave(iic_regs[id]);
        hw_iic_slave_set_addr(iic, iic_info_slave_addr(iic), 1);
//        iic_si_mode_en(iic_regs[id]);
    }


    if (hw_iic_cfg[iic].isr_en) {
        iic_set_end_ie(iic_regs[id]);
        iic_set_ie(iic_regs[id]);
        iic_buf_reg(iic_regs[id]) = 0xff;
        HWI_Install(IRQ_IIC_IDX, (u32)iic_isr, 0) ;
    } else {
        iic_clr_ie(iic_regs[id]);
    }
    iic_pnd_clr(iic_regs[id]);
    iic_end_pnd_clr(iic_regs[id]);
    iic_start_pnd_clr(iic_regs[id]);
    iic_enable(iic_regs[id]);
#if 0
    log_info("info->scl = %d\n", iic_get_scl(iic));
#endif
    return 0;
}

void hw_iic_uninit(hw_iic_dev iic)
{
    u8 id = iic_get_id(iic);
    u8 port = iic_info_port(iic);
    u32 scl, sda;

    scl = iic_get_scl(iic);
    sda = iic_get_sda(iic);
    hw_iic_set_die(iic, 0);
    if (id == 0) {
#if 0
        if (port == 0) {
            //JL_USB_IO->CON0 |= BIT(2);  //DP DIR IN
            //JL_USB_IO->CON0 |= BIT(3);  //DM DIR IN
            JL_USB_IO->CON0 &= ~(BIT(11) | BIT(8)); //disable IO_MODE
            JL_USB_IO->CON0 &= ~BIT(6);  //DP PU
            JL_USB_IO->CON0 &= ~BIT(7);  //DM PU
        } else {
            /* gpio_set_direction(scl, 1); */
            /* gpio_set_direction(sda, 1); */
            gpio_set_hd(scl, 0);
            gpio_set_hd(sda, 0);
            gpio_set_pull_up(scl, 0);
            gpio_set_pull_up(sda, 0);
        }
#else
        gpio_set_hd(scl, 0);
        gpio_set_hd(sda, 0);
        gpio_set_pull_up(scl, 0);
        gpio_set_pull_up(sda, 0);
        if (port == 0) {
            //usb_iomode(0);////////////////////////////////////////////////////////////////
        }
#endif
    }
    iic_disable(iic_regs[id]);
}

void hw_iic_start(hw_iic_dev iic)//必须字节发送前设置好。硬件无法清除，需手动清除
{
    u8 id = iic_get_id(iic);
    iic_preset_restart(iic_regs[id]);
}

void hw_iic_stop(hw_iic_dev iic) //必须最后一字节发送前设置好
{
    u8 id = iic_get_id(iic);
    iic_preset_end(iic_regs[id]);
}

u8 hw_iic_tx_byte(hw_iic_dev iic, u8 byte)
{
    u8 id = iic_get_id(iic);
    iic_buf_reg(iic_regs[id]) = byte;
    /*log_info("iic-pnd=%x\n",iic_pnd(iic_regs[id]));*/
    while (!iic_pnd(iic_regs[id]));
    /*log_info("iic-pnd=%x\n",iic_pnd(iic_regs[id]));*/
    iic_pnd_clr(iic_regs[id]);
    iic_no_restart(iic_regs[id]);
    iic_no_end(iic_regs[id]);
    return iic_send_is_ack(iic_regs[id]);
}

u8 hw_iic_rx_byte(hw_iic_dev iic, u8 ack)//如需接收后结束，先设置结束位
{
    u8 id = iic_get_id(iic);
    if (ack) {
        iic_recv_ack(iic_regs[id]);
    } else {
        iic_recv_nack(iic_regs[id]);
    }
    iic_buf_reg(iic_regs[id]) = 0xff;
    while (!iic_pnd(iic_regs[id]));
    iic_pnd_clr(iic_regs[id]);
    iic_no_end(iic_regs[id]);
    return iic_buf_reg(iic_regs[id]);
}

int hw_iic_read_buf(hw_iic_dev iic, void *buf, int len)////////////////////////////////////////////
{
    u8 id = iic_get_id(iic);
    int i;

    if (!buf || !len) {
        return -1;
    }
    iic_recv_ack(iic_regs[id]);
    for (i = 0; i < len; i++) {
        if (i == len - 1) {
            iic_recv_nack(iic_regs[id]);
        }
        iic_buf_reg(iic_regs[id]) = 0xff;
        while (!iic_pnd(iic_regs[id]));
        iic_pnd_clr(iic_regs[id]);
        ((u8 *)buf)[i] = iic_buf_reg(iic_regs[id]);
    }
    return len;
}

int hw_iic_write_buf(hw_iic_dev iic, const void *buf, int len)///////////////////////////////////////
{
    u8 id = iic_get_id(iic);
    int i = 0;

    if (!buf || !len) {
        return -1;
    }
    for (i = 0; i < len; i++) {
        iic_buf_reg(iic_regs[id]) = ((u8 *)buf)[i];
        while (!iic_pnd(iic_regs[id]));
        iic_pnd_clr(iic_regs[id]);
        if (!iic_send_is_ack(iic_regs[id])) {
            break;
        }
    }
    return i;
}

void hw_iic_set_ie(hw_iic_dev iic, u8 en)
{
    u8 id = iic_get_id(iic);

    if (en) {
        iic_set_ie(iic_regs[id]);
    } else {
        iic_clr_ie(iic_regs[id]);
    }
}

u8 hw_iic_get_pnd(hw_iic_dev iic)
{
    u8 id = iic_get_id(iic);

    return !!iic_pnd(iic_regs[id]);
}

void hw_iic_clr_pnd(hw_iic_dev iic)
{
    u8 id = iic_get_id(iic);

    iic_pnd_clr(iic_regs[id]);
}

void hw_iic_set_end_ie(hw_iic_dev iic, u8 en)
{
    u8 id = iic_get_id(iic);

    if (en) {
        iic_set_end_ie(iic_regs[id]);
    } else {
        iic_clr_end_ie(iic_regs[id]);
    }
}

u8 hw_iic_get_end_pnd(hw_iic_dev iic)
{
    u8 id = iic_get_id(iic);

    return !!iic_end_pnd(iic_regs[id]);
}

void hw_iic_clr_end_pnd(hw_iic_dev iic)
{
    u8 id = iic_get_id(iic);

    iic_end_pnd_clr(iic_regs[id]);
}

void hw_iic_slave_set_addr(hw_iic_dev iic, u8 addr, u8 addr_ack)//addr_ack=1:应答
{
    u8 id = iic_get_id(iic);

    iic_baud_reg(iic_regs[id]) = (addr & 0xfe) | !!addr_ack;
}

void hw_iic_slave_rx_prepare(hw_iic_dev iic, u8 ack)
{
    u8 id = iic_get_id(iic);

    /*iic_dir_in(iic_regs[id]);*/
    if (ack) {
        iic_recv_ack(iic_regs[id]);
    } else {
        iic_recv_nack(iic_regs[id]);
    }
    iic_buf_reg(iic_regs[id]) = 0xff;
}

u8 hw_iic_slave_rx_byte(hw_iic_dev iic, bool *is_start_addr)///////////////////////////////////////////
{
    u8 id = iic_get_id(iic);
    if (iic_start_pnd(iic_regs[id])) {
        iic_start_pnd_clr(iic_regs[id]);
        is_start_addr ? (*is_start_addr = 1) : 0;
    } else {
        is_start_addr ? (*is_start_addr = 0) : 0;
    }
    return iic_buf_reg(iic_regs[id]);
}

void hw_iic_slave_tx_byte(hw_iic_dev iic, u8 byte)
{
    u8 id = iic_get_id(iic);

    iic_buf_reg(iic_regs[id]) = byte;
}

u8 hw_iic_slave_tx_check_ack(hw_iic_dev iic)
{
    u8 id = iic_get_id(iic);

    return iic_send_is_ack(iic_regs[id]);
}

u8 iic_slave_rxdata;
SET(interrupt(""))
void iic_isr()
{
    if (iic_pnd(iic_regs[0])) {
        if (iic_start_pnd(iic_regs[0])) {
            /*log_info(" start!\n");*/
            iic_slave_rxdata = iic_buf_reg(iic_regs[0]);
            iic_buf_reg(iic_regs[0]) = 0xff;
            iic_start_pnd_clr(iic_regs[0]);
        } else {
            /*log_info("no start!\n");*/
            iic_slave_rxdata = iic_buf_reg(iic_regs[0]);
            iic_buf_reg(iic_regs[0]) = 0xff;
            iic_pnd_clr(iic_regs[0]);
        }
        iic_pnd_clr(iic_regs[0]);
    }
    if (iic_end_pnd(iic_regs[0])) {
        /*log_info("end!\n");*/
        iic_start_pnd_clr(iic_regs[0]);
        iic_pnd_clr(iic_regs[0]);
        iic_end_pnd_clr(iic_regs[0]);
    }
}

