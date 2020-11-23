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
 * \brief fm175xx�����ݶ�д������ͨ�������ӿ�ʵ��
 *
 * - �������裺
 *   1. ��ȷ���Ӳ����úô��ڡ�
 *   2. ��ȷ���Ӻ����ߡ�
 *   3. ��A�࿨�������߸�Ӧ����
 *
 * - ʵ������
 *   1. ������ԿA�Կ�Ƭ�Ŀ�2������֤�������֤��ͨ�����ӡkey A authent failed��
 *   2. �����֤�ɹ������ȡ��0��������Ƭ���кţ�����3��������ԿA����ȡ���Ʋ��ֺ���ԿB������4���Ϳ�2
 *      ����ͬһ��������������ӡ������
 *
 * - ע�⣺
 *   1. ÿ����������4���飬���е�0����2����Ϊ���ݿ飬��3����Ϊ���ƿ顣��֤��ʵ�Ƕ��������Եģ����Ǹ�
 *      am_fm175xx_picca_m1_authent()������������һ������Ϊ���ַ����˸ú���������
 *      ��ʵ����֤����Ŀ����ڵ���������֤�ɹ������Ը��ݿ��ƿ��еĿ���Ȩ�޶Ա��������еĿ���ж�д��
 *   2. ��0�е������ǿ�Ƭ���кţ��ÿ�ֻ����
 *   3. ��3λ���ƿ飬��Ȼ���������A��ԿΪȫF�����Ƕ�ȡ������A��ԿȴΪȫ0����ΪA��Կ���κ�����¶�û��
 *      ����Ȩ�ޡ�
 *   4. ��4����֤�Ŀ�2����ͬһ����������˶������������ǲ���ȷ�ģ����ǶԿ�4���ڵ�����������֤��
 *
 * \par Դ����
 * \snippet demo_amks16rfid8_fm175xx_picca_block.c src_amks16rfid8_fm175xx_picca_block
 *
 * \internal
 * \par Modification history
 * - 1.00 17-11-21  sdq, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_kl26_if_dr_fm175xx_picca_block
 * \copydoc demo_amks16rfid8_fm175xx_picca_block.c
 */

/** [src_amks16rfid8_fm175xx_picca_block] */
#include "am_fm175xx.h"
#include "am_fm175xx_reg.h"
#include "am_vdebug.h"
#include "am_delay.h"
#include "kl26_pin.h"
#include "am_kl26_inst_init.h"
#include "am_fm175xx_ant.h"
#include "am_hwconf_fm175xx.h"
#include "demo_components_entries.h"
#include "demo_amks16rfid8_entries.h"

/**
 * \name ������İ���ع̶�����
 * @{
 */
#define __CD4051_PIN_EN  PIOB_19         /**< \brief   CD4051ͨ������оƬ ʹ������  */
#define __CD4051_PIN_S2  PIOB_18         /**< \brief   CD4051ͨ������оƬS2����  */
#define __CD4051_PIN_S1  PIOB_17         /**< \brief   CD4051ͨ������оƬS1����  */
#define __CD4051_PIN_S0  PIOB_16         /**< \brief   CD4051ͨ������оƬS0����  */

#define __ANT_ENABLE_PIN PIOE_29         /**<\brief   ������ѹоƬʹ������  */

/* ���� fm175xx �����л���Ϣ */
static am_antenna_info_t  __g_antenna_info = {
    {
        __CD4051_PIN_EN,
        __CD4051_PIN_S0,
        __CD4051_PIN_S1,
        __CD4051_PIN_S2
    },
    AM_FM175XX_ANTENNA_EIGHT
};

/**
 * \brief A�࿨���д����
 */
void demo_amks16rfid8_fm175xx_picca_write_block (void)
{
    am_fm175xx_handle_t handle;

    /* B�汾���İ���ڴ�����   ��Ҫ����ʹ������
     * ��ΪA�汾���İ���ɶԸ��д������ע��
     */
    am_gpio_pin_cfg(__ANT_ENABLE_PIN, AM_GPIO_OUTPUT_INIT_HIGH);

    am_cd4051_pin_init(&__g_antenna_info);
    /* ѡ������1 */
    am_cd4051_channel_selected(&__g_antenna_info, 0);

    handle = am_fm175xx_inst_init();

    demo_fm175xx_picca_write_block(handle);
}

/** [src_amks16rfid8_fm175xx_picca_block] */

/* end of file */