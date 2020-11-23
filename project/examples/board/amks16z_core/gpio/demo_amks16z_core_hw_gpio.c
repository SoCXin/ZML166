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
 * \brief KS16xx GPIO��PORT HW������
 *
 * \brief GPIO ���������������������
 * - ������
 *   1.��J14��key��PIOC_0�̽���һ��;
 *   2.��J9��LED0��PIOC_4�̽���һ��;
 *   3.��J10��LED1��PIOC_8�̽���һ��;
 *
 * -ʵ�����󣺳������ص�demo��󣬰���һ�ΰ���LED0�����һ�η�ת����������demo���ϵ�LED0������
 *
 * \par Դ����
 * \par Դ����
 * \snippet demo_amks16z_core_hw_gpio.c src_amks16z_core_hw_gpio
 *
 * \internal
 * \par Modification History
 * - 1.01 16-09-18  nwt, make some changes.
 * - 1.00 15-10-16  wxj, first implementation.
 * \endinternal
 */
 
/**
 * \addtogroup demo_amks16z_core_if_hw_gpio
 * \copydoc demo_amks16z_core_hw_gpio.c
 */
 
/** [src_amks16z_core_hw_gpio] */
#include "ametal.h"
#include "am_vdebug.h"
#include "demo_fsl_entrys.h"
#include "demo_amks16z_core_all_entries.h"

/**
 * \brief �������
 */
void demo_amks16z_core_hw_gpio_entry (void)
{
    AM_DBG_INFO("demo amks16z_core hw gpio!\r\n");

    demo_kl26_hw_gpio_entry();
}

/** [src_amks16z_core_hw_gpio] */
 
/* end of file */