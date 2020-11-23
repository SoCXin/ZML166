/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief SPI 驱动层实现函数
 *
 * \internal
 * \par Modification history
 * - 1.00 19-07-19  htf, first implementation
 * \endinternal
 */

/*******************************************************************************
includes
*******************************************************************************/

#include "am_zlg237_spi_poll.h"
#include "ametal.h"
#include "am_int.h"
#include "am_gpio.h"
#include "am_delay.h"
#include "am_clk.h"
#include "hw/amhw_zlg237_spi.h"

/*******************************************************************************
  SPI 状态和事件定义
*******************************************************************************/

/**
 * SPI 控制器状态
 */

#define __SPI_ST_IDLE               0                   /* 空闲状态 */
#define __SPI_ST_MSG_START          1                   /* 消息开始 */
#define __SPI_ST_TRANS_START        2                   /* 传输开始 */
#define __SPI_ST_M_SEND_DATA        3                   /* 主机发送 */
#define __SPI_ST_M_RECV_DATA        4                   /* 主机接收 */

/**
 * SPI 控制器事件
 *
 * 共32位，低16位是事件编号，高16位是事件参数
 */

#define __SPI_EVT_NUM_GET(event)    ((event) & 0xFFFF)
#define __SPI_EVT_PAR_GET(event)    ((event >> 16) & 0xFFFF)
#define __SPI_EVT(evt_num, evt_par) (((evt_num) & 0xFFFF) | ((evt_par) << 16))

#define __SPI_EVT_NONE              __SPI_EVT(0, 0)     /* 无事件 */
#define __SPI_EVT_TRANS_LAUNCH      __SPI_EVT(1, 0)     /* 传输就绪 */
#define __SPI_EVT_M_SEND_DATA       __SPI_EVT(2, 0)     /* 发送数据 */
#define __SPI_EVT_M_RECV_DATA       __SPI_EVT(3, 0)     /* 接收数据 */

/*******************************************************************************
  模块内函数声明
*******************************************************************************/
am_local void __spi_default_cs_ha    (am_spi_device_t *p_dev, int state);
am_local void __spi_default_cs_la    (am_spi_device_t *p_dev, int state);
am_local void __spi_default_cs_dummy (am_spi_device_t *p_dev, int state);

am_local void __spi_cs_on  (am_zlg237_spi_poll_dev_t *p_this,
                            am_spi_device_t          *p_dev);
am_local void __spi_cs_off (am_zlg237_spi_poll_dev_t *p_this,
                            am_spi_device_t          *p_dev);

am_local void __spi_write_data (am_zlg237_spi_poll_dev_t *p_dev);
am_local void __spi_read_data (am_zlg237_spi_poll_dev_t *p_dev);

am_local int  __spi_hard_init (am_zlg237_spi_poll_dev_t *p_this);
am_local int  __spi_config (am_zlg237_spi_poll_dev_t *p_this);

am_local int  __spi_mst_sm_event (am_zlg237_spi_poll_dev_t *p_dev);
/*******************************************************************************
  SPI驱动函数声明
*******************************************************************************/
am_local int __spi_info_get (void *p_arg, am_spi_info_t   *p_info);
am_local int __spi_setup    (void *p_arg, am_spi_device_t *p_dev );
am_local int __spi_msg_start (void              *p_drv,
                              am_spi_device_t   *p_dev,
                              am_spi_message_t  *p_msg);

/**
 * \brief SPI 驱动函数
 */
am_local am_const struct am_spi_drv_funcs __g_spi_drv_funcs = {
    __spi_info_get,
    __spi_setup,
    __spi_msg_start,
};

/******************************************************************************/

/* 获取SPI的输入频率 */
#define __SPI_FRQIIN_GET(p_hw_spi)    am_clk_rate_get(p_this->p_devinfo->clk_id)

/* 获取SPI支持的最大速度 */
#define __SPI_MAXSPEED_GET(p_hw_spi) (__SPI_FRQIIN_GET(p_hw_spi) / 2)

/* 获取SPI支持的最小速度 */
#define __SPI_MINSPEED_GET(p_hw_spi) (__SPI_FRQIIN_GET(p_hw_spi) / 256)

/**
 * \brief 位数转字节
 */
am_local
uint8_t __bits_to_bytes (uint8_t bits)
{
       uint8_t bytes = 0;
       bytes = (bits >> 3) + ((bits & 0x7) ? 1 : 0);
       return bytes;
}
/**
 * \brief 默认CS脚控制函数，高电平有效
 */
am_local
void __spi_default_cs_ha (am_spi_device_t *p_dev, int state)
{
    am_gpio_set(p_dev->cs_pin, state ? 1 : 0);
}

/**
 * \brief 默认CS脚控制函数，低电平有效
 */
am_local
void __spi_default_cs_la (am_spi_device_t *p_dev, int state)
{
    am_gpio_set(p_dev->cs_pin, state ? 0 : 1);
}

/**
 * \brief 默认CS脚控制函数，由硬件自行控制
 */
am_local
void __spi_default_cs_dummy (am_spi_device_t *p_dev, int state)
{
    return;
}

/**
 * \brief CS引脚激活
 */
am_local
void __spi_cs_on (am_zlg237_spi_poll_dev_t *p_this, am_spi_device_t *p_dev)
{
    /* if last device toggled after message */
    if (p_this->p_tgl_dev != NULL) {

        /* last message on defferent device */
        if (p_this->p_tgl_dev != p_dev) {
            p_this->p_tgl_dev->pfunc_cs(p_this->p_tgl_dev, 0);
        }
        p_this->p_tgl_dev = NULL;
    }

    p_dev->pfunc_cs(p_dev, 1);
}

/**
 * \brief CS引脚去活
 */
am_local
void __spi_cs_off (am_zlg237_spi_poll_dev_t *p_this,
                   am_spi_device_t      *p_dev)
{
    if (p_this->p_tgl_dev == p_dev) {
        p_this->p_tgl_dev = NULL;
    }

    p_dev->pfunc_cs(p_dev, 0);
}

/******************************************************************************/

am_local
void __spi_write_data (am_zlg237_spi_poll_dev_t *p_dev)
{
    amhw_zlg237_spi_t    *p_hw_spi = (amhw_zlg237_spi_t *)
                                     (p_dev->p_devinfo->spi_reg_base);
    am_spi_transfer_t *p_trans  = p_dev->p_cur_trans;

    while(amhw_zlg237_spi_status_flag_check (
        p_hw_spi, AMHW_ZLG237_SPI_TX_EMPTY_FLAG) == AM_FALSE);

    /* tx_buf 有效 */
    if (p_trans->p_txbuf != NULL) {
        /** \brief 待发送数据的基址+偏移 */
        uint8_t *ptr = (uint8_t *)(p_trans->p_txbuf) + p_dev->data_ptr;
        amhw_zlg237_spi_tx_put(p_hw_spi, *ptr);
    /* tx_buf 无效 */
    } else {
        /* 待发送数据无效 直接发0 */
        amhw_zlg237_spi_tx_put(p_hw_spi, 0x00);
    }
    /** \brief 需接收数据的byte数 */
    p_dev->nbytes_to_recv = __bits_to_bytes(
        p_dev->p_cur_spi_dev->bits_per_word); //todo
    p_dev->p_cur_msg->actual_length += p_dev->nbytes_to_recv;
}

am_local
void __spi_read_data (am_zlg237_spi_poll_dev_t *p_dev)
{
    amhw_zlg237_spi_t     *p_hw_spi = (amhw_zlg237_spi_t *)
                                      (p_dev->p_devinfo->spi_reg_base);

    am_spi_transfer_t  *p_trans  = p_dev->p_cur_trans;

    uint8_t  *p_buf8;

    p_buf8  = (uint8_t *)p_trans->p_rxbuf + p_dev->data_ptr;


    if (amhw_zlg237_spi_status_flag_check (
        p_hw_spi, AMHW_ZLG237_SPI_RX_NOT_EMPTY_FLAG) == AM_TRUE) {

        /* rx_buf 有效 */
        if (p_trans->p_rxbuf != NULL && p_dev->nbytes_to_recv) {
             *p_buf8 = amhw_zlg237_spi_rx_get(p_hw_spi);
        /* rx_buf 无效或者不需要接收数据 */
        } else {
               (void)amhw_zlg237_spi_rx_get(p_hw_spi);
        }

        /* 已经发送或接收的数据byte数 */
        p_dev->data_ptr += p_dev->nbytes_to_recv;
        p_dev->nbytes_to_recv = 0;

    }
}

/******************************************************************************/
/**
 * \brief 从message列表表头取出一条 transfer
 * \attention 调用此函数必须锁定控制器
 */
am_static_inline
struct am_spi_transfer *__spi_trans_out (am_spi_message_t *msg)
{
    if (am_list_empty_careful(&(msg->transfers))) {
        return NULL;
    } else {
        struct am_list_head *p_node = msg->transfers.next;
        am_list_del(p_node);
        return am_list_entry(p_node, struct am_spi_transfer, trans_node);
    }
}

/******************************************************************************/
am_local
int __spi_setup (void *p_arg, am_spi_device_t *p_dev)
{
    am_zlg237_spi_poll_dev_t *p_this = (am_zlg237_spi_poll_dev_t *)p_arg;

    uint32_t max_speed, min_speed;

    if (p_dev == NULL) {
        return -AM_EINVAL;
    }

    /* 默认数据为8位，最大不超过16位 */
    if (p_dev->bits_per_word == 0) {
        p_dev->bits_per_word = 8;
    } else if (p_dev->bits_per_word > 32) {
        return -AM_ENOTSUP;
    }

    /* 最大SPI速率不能超过主时钟，最小不能小于主时钟256分频 */
    max_speed = __SPI_MAXSPEED_GET(p_hw_spi);
    min_speed = __SPI_MINSPEED_GET(p_hw_spi);

    if (p_dev->max_speed_hz > max_speed) {
        p_dev->max_speed_hz = max_speed;
    } else if (p_dev->max_speed_hz < min_speed) {
        return -AM_ENOTSUP;
    }

    /* 无片选函数 */
    if (p_dev->mode & AM_SPI_NO_CS) {
        p_dev->pfunc_cs = __spi_default_cs_dummy;

    /* 有片选函数 */
    }  else {

        /* 不提供则默认片选函数 */
        if (p_dev->pfunc_cs == NULL) {

            /* 片选高电平有效 */
            if (p_dev->mode & AM_SPI_CS_HIGH) {
                am_gpio_pin_cfg(p_dev->cs_pin, AM_GPIO_OUTPUT_INIT_LOW);
                p_dev->pfunc_cs = __spi_default_cs_ha;

            /* 片选低电平有效 */
            } else {
                am_gpio_pin_cfg(p_dev->cs_pin, AM_GPIO_OUTPUT_INIT_HIGH);
                p_dev->pfunc_cs = __spi_default_cs_la;
            }
        }
    }

    /* 解除片选信号 */
    __spi_cs_off(p_this, p_dev);

    return AM_OK;
}

am_local
int __spi_info_get (void *p_arg, am_spi_info_t *p_info)
{
    am_zlg237_spi_poll_dev_t  *p_this   = (am_zlg237_spi_poll_dev_t *)p_arg;

    if (p_info == NULL) {
        return -AM_EINVAL;
    }

    /* 最大速率等于 PCLK */
    p_info->max_speed = __SPI_MAXSPEED_GET(p_hw_spi);
    p_info->min_speed = __SPI_MINSPEED_GET(p_hw_spi);
    p_info->features  = AM_SPI_MODE_0    |
                        AM_SPI_MODE_1    |
                        AM_SPI_MODE_2    |
                        AM_SPI_MODE_3;   /* features */
    return AM_OK;
}

/**
 * \brief SPI 硬件初始化
 */
am_local
int __spi_hard_init (am_zlg237_spi_poll_dev_t *p_this)
{
    amhw_zlg237_spi_t *p_hw_spi = (amhw_zlg237_spi_t *)
                                  (p_this->p_devinfo->spi_reg_base);

    if (p_this == NULL) {
        return -AM_EINVAL;
    }

    /* NSS采用软件管理*/
    amhw_zlg237_spi_ssm_enable(p_hw_spi);

    /* 关闭硬件SPI的NSS引脚输出*/
    amhw_zlg237_spi_ssout_disable(p_hw_spi);

    /* 软件管理下，NSS内部为高，与引脚无关。特别注意，无论硬件、软件管理，配置成主机模式，NSS必须为高*/
    amhw_zlg237_spi_ssi_set(p_hw_spi,AMHW_ZLG237_SPI_SSI_TO_NSS_ENABLE);

    /* 配置为主机模式 */
    amhw_zlg237_spi_master_salver_set(p_hw_spi, AMHW_ZLG237_SPI_MASTER);

    /* SPI使能 */
    amhw_zlg237_spi_enable(p_hw_spi);

    return AM_OK;
}

am_local
int __spi_config (am_zlg237_spi_poll_dev_t *p_this)
{
    const am_zlg237_spi_poll_devinfo_t *p_devinfo = p_this->p_devinfo;
    amhw_zlg237_spi_t                  *p_hw_spi  = (amhw_zlg237_spi_t *)
                                                    (p_devinfo->spi_reg_base);
    am_spi_transfer_t               *p_trans      = p_this->p_cur_trans;

    uint32_t mode_flag = 0;

    /* 如果为0，使用默认参数值 */
    if (p_trans->bits_per_word == 0) {
        p_trans->bits_per_word = p_this->p_cur_spi_dev->bits_per_word;
    }

    if (p_trans->speed_hz == 0) {
        p_trans->speed_hz = p_this->p_cur_spi_dev->max_speed_hz;
    }

    /* 设置字节数有效性检查 */
    if (p_trans->bits_per_word > 16 || p_trans->bits_per_word < 1) {
        return -AM_EINVAL;
    }

    /* 设置分频值有效性检查 */
    if (p_trans->speed_hz > __SPI_MAXSPEED_GET(p_hw_spi) ||
        p_trans->speed_hz < __SPI_MINSPEED_GET(p_hw_spi)) {
        return -AM_EINVAL;
    }

    /* 发送和接收缓冲区有效性检查 */
    if ((p_trans->p_txbuf == NULL) && (p_trans->p_rxbuf == NULL)) {
        return -AM_EINVAL;
    }

    /* 发送字节数检查 */
    if (p_trans->nbytes == 0) {
        return -AM_ELOW;
    }

    /**
     * 配置当前设备模式
     */
    mode_flag = 0;

    if (p_this->p_cur_spi_dev->mode & AM_SPI_LSB_FIRST) {
        mode_flag |= AMHW_ZLG237_SPI_LSB_FIRST_SEND_LSB;
    }

    /* 关闭spi外设*/
    amhw_zlg237_spi_disable(p_hw_spi);

    /* NSS采用软件管理*/
    amhw_zlg237_spi_ssm_enable(p_hw_spi);

    /* 关闭硬件SPI的NSS引脚输出*/
    amhw_zlg237_spi_ssout_disable(p_hw_spi);

    /* 软件管理下，NSS内部为高，与引脚无关。特别注意，无论硬件、软件管理，配置成主机模式，NSS必须为高*/
    amhw_zlg237_spi_ssi_set(p_hw_spi,AMHW_ZLG237_SPI_SSI_TO_NSS_ENABLE);

    /* 设置为主机模式*/
    amhw_zlg237_spi_master_salver_set(p_hw_spi, AMHW_ZLG237_SPI_MASTER);

    /* 配置SPI模式（时钟相位和极性） */
    amhw_zlg237_spi_clk_mode_set(p_hw_spi, 0x3 & p_this->p_cur_spi_dev->mode);

    /* 配置SPI数据长度和帧格式 */
    if (p_this->p_cur_spi_dev->bits_per_word == 8) {
        amhw_zlg237_spi_lsbfirst_set(p_hw_spi, mode_flag);
        amhw_zlg237_spi_data_length_set(p_hw_spi, AMHW_ZLG237_SPI_DATA_8BIT);
    }
    else if (p_this->p_cur_spi_dev->bits_per_word == 16) {
        amhw_zlg237_spi_lsbfirst_set(p_hw_spi, mode_flag);
        amhw_zlg237_spi_data_length_set(p_hw_spi, AMHW_ZLG237_SPI_DATA_16BIT);
    }
    else if ((p_this->p_cur_spi_dev->bits_per_word >= 1) ||
             (p_this->p_cur_spi_dev->bits_per_word <= 32)) {
        amhw_zlg237_spi_lsbfirst_set(p_hw_spi, mode_flag);
        amhw_zlg237_spi_flex_length_set(p_hw_spi,
                                        p_this->p_cur_spi_dev->bits_per_word);
        amhw_zlg237_spi_flexlength_enable(p_hw_spi);
    }
    else {
        amhw_zlg237_spi_lsbfirst_set(p_hw_spi, mode_flag);
        amhw_zlg237_spi_data_length_set(p_hw_spi, AMHW_ZLG237_SPI_DATA_8BIT);
    }

    /* 设置SPI速率 */
    amhw_zlg237_spi_baudratefre_set(p_hw_spi, p_this->p_devinfo->baud_div);

    amhw_zlg237_spi_enable(p_hw_spi);

    return AM_OK;
}

/**
 * \brief SPI传输数据函数
 */
am_local
int __spi_msg_start (void              *p_drv,
                     am_spi_device_t   *p_dev,
                     am_spi_message_t  *p_msg)
{
    am_zlg237_spi_poll_dev_t *p_this = (am_zlg237_spi_poll_dev_t *)p_drv;

    p_this->p_cur_spi_dev  = p_dev;                         /* 将当前设备参数信息存入 */
    p_this->p_cur_msg      = p_msg;                         /* 将当前设备传输消息存入 */
    p_this->nbytes_to_recv = 0;                             /* 待接收字符数清0 */
    p_this->data_ptr       = 0;                             /* 已接收字符数清0 */

    p_this->busy = AM_TRUE;

    __spi_mst_sm_event(p_this);

    return AM_OK;
}

/******************************************************************************/


/**
 * \brief SPI 使用状态机传输
 */
am_local
int __spi_mst_sm_event (am_zlg237_spi_poll_dev_t *p_dev)
{
    am_spi_message_t  *p_cur_msg   = NULL;

    p_cur_msg = p_dev->p_cur_msg;

    while (!am_list_empty(&(p_cur_msg->transfers))) {
        /* 获取到一个传输，正确处理该传输即可 */
        am_spi_transfer_t *p_cur_trans = __spi_trans_out(p_cur_msg);
        p_dev->p_cur_trans             = p_cur_trans;

        /* reset current data pointer */
        p_dev->data_ptr       = 0;
        p_dev->nbytes_to_recv = 0;

        /* 如果该传输指定SPI字节数传输  则进入该判断 用户自定义设置  */
        if(p_dev->p_cur_trans->bits_per_word != 0 ||

           p_dev->p_cur_trans->speed_hz != 0) {
           __spi_config(p_dev);
           p_dev->bef_bits_per_word = p_dev->p_cur_spi_dev->bits_per_word;
           p_dev->bef_speed_hz = p_dev->p_cur_trans->speed_hz;

        /* 如果该传输默认SPI字节数传输  且上次传输保存的SPI字节数与 SPI 设备不同  则进行重新设置  */
        }else if(p_dev->p_cur_trans->bits_per_word == 0 &&

            p_dev->bef_bits_per_word != p_dev->p_cur_spi_dev->bits_per_word){

            /* 判断使用何种传输方式   改变时进行重新配置 */
            __spi_config(p_dev);
            p_dev->bef_bits_per_word = p_dev->p_cur_spi_dev->bits_per_word;
        }
       if(p_dev->p_cur_trans->speed_hz == 0 &&

            p_dev->bef_speed_hz != p_dev->p_cur_spi_dev->max_speed_hz ){

           /* 配置SPI传输参数    改变时进行重新配置     */
            __spi_config(p_dev);
            p_dev->bef_speed_hz = p_dev->p_cur_spi_dev->max_speed_hz;
        }

        /* CS选通 */
        __spi_cs_on(p_dev, p_dev->p_cur_spi_dev);

        /* CS选通后，至少需要等待100ms，等待从机(单片机)完成相关配置
         * 才能正确传输数据，否则部分数据会丢失、错乱。
         * 假如是其他从机单位，响应较快，可考虑取消此处延时。
         */
        if(p_dev->p_devinfo->cs_mdelay != 0) {
            am_mdelay(p_dev->p_devinfo->cs_mdelay);
        }

        while(p_dev->data_ptr < p_cur_trans->nbytes){
            __spi_write_data(p_dev);
            while(p_dev->nbytes_to_recv != 0){
                __spi_read_data(p_dev);
            }
        }
    }
    /* notify the caller */
    p_cur_msg->pfn_complete(p_cur_msg->p_arg);

    /* 片选关闭 */
    __spi_cs_off(p_dev, p_dev->p_cur_spi_dev);

    return AM_OK;
}

/******************************************************************************/

/**
 * \brief SPI初始化
 */
am_spi_handle_t am_zlg237_spi_poll_init (
    am_zlg237_spi_poll_dev_t           *p_dev,
    const am_zlg237_spi_poll_devinfo_t *p_devinfo)
{
    if (NULL == p_devinfo || NULL == p_dev ) {
        return NULL;
    }

    if (p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }

    p_dev->spi_serve.p_funcs = (struct am_spi_drv_funcs *)&__g_spi_drv_funcs;
    p_dev->spi_serve.p_drv   = p_dev;

    p_dev->p_devinfo         = p_devinfo;

    p_dev->p_cur_spi_dev     = NULL;
    p_dev->p_tgl_dev         = NULL;
    p_dev->busy              = AM_FALSE;
    p_dev->p_cur_msg         = NULL;
    p_dev->p_cur_trans       = NULL;
    p_dev->data_ptr          = 0;
    p_dev->nbytes_to_recv    = 0;
    p_dev->state             = __SPI_ST_IDLE;     /* 初始化为空闲状态 */

    if (__spi_hard_init(p_dev) != AM_OK) {
        return NULL;
    }

    return &(p_dev->spi_serve);
}

/**
 * \brief SPI 去除初始化
 */
void am_zlg237_spi_poll_deinit (am_spi_handle_t handle)
{
    am_zlg237_spi_poll_dev_t *p_dev    = (am_zlg237_spi_poll_dev_t *)handle;
    amhw_zlg237_spi_t        *p_hw_spi = (amhw_zlg237_spi_t *)
                                         (p_dev->p_devinfo->spi_reg_base);

    if (NULL == p_dev) {
        return ;
    }

    p_dev->spi_serve.p_funcs = NULL;
    p_dev->spi_serve.p_drv   = NULL;

    /* 禁能SPI */
    amhw_zlg237_spi_disable(p_hw_spi);

    if (p_dev->p_devinfo->pfn_plfm_deinit) {
        p_dev->p_devinfo->pfn_plfm_deinit();
    }
}

