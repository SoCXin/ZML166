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
 * \brief MiniPort-View + MiniPort-595 �¶���ʾ��ʹ�õװ�LM75BD��ȡ�¶ȣ� 
 *        ���̣�ͨ����׼�ӿ�ʵ��
 *
 * - ��������
 *   1. Ҫʹ�õװ壨AM116-Core���ϵ� LM75 �¶ȴ���������Ҫ�̽� J11 �� J13��
 *   2. �Ƚ� MiniPort-595 ����ֱ���� AM116-Core �� MiniPort �����ӣ�
 *   3. �� MiniPort-View ������ MiniPort-595 �ӿ������ӡ�
 *
 * - ʵ������
 *   1. ���Կ����������ʾ��ǰ�¶ȣ���J14��ðѡ��̽� KEY �� RES�����µװ��ϵ�
 *      KEY/RES �������ʵ���ᷢ�ȣ������¶ȴ������ɼ������¶����ߡ�
 *
 * - ����˵��
 *   1. �������ʹ�� I2C ��ȡ LM75 ���¶ȣ�����ʹ�ò鿴 I2C �µ��������
 *
 * \par Դ����
 * \snippet demo_am116_core_miniport_hc595_digitron_temp.c src_am116_core_miniport_hc595_digitron_temp
 *
 * \internal
 * \par Modification history
 * - 1.00 15-07-21  tee, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_am116_core_miniport_hc595_digitron_temp
 * \copydoc demo_am116_core_miniport_hc595_digitron_temp.c
 */

/** [src_am116_core_miniport_hc595_digitron_temp] */
#include "ametal.h"
#include "am_vdebug.h"
#include "am_zlg116_inst_init.h"
#include "demo_std_entries.h"
#include "demo_am116_core_entries.h"

/**
 * \brief ������� 
 */
void demo_am116_core_miniport_hc595_digitron_temp_entry (void)
{
    AM_DBG_INFO("demo am116_core miniport hc595 digitron temp!\r\n");

    am_miniport_view_595_inst_init();

    demo_std_digitron_temp_entry(0, am_temp_lm75_inst_init());
}
/** [src_am116_core_miniport_hc595_digitron_temp] */

/* end of file */