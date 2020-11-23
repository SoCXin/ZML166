/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      ametal.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief USBģ��U�����̣�ͨ��driver��Ľӿ�ʵ��
 *
 * - �������裺
 *   1. ��usbͨ�������ļ�����������user_config\am_hwconf_usrcfg\am_usb_common_config.h
 *   #define USB_DEVICE_CLASS_MSC_ON   1
 *   //#define USB_DEVICE_CLASS_HID_MOUSE   1
 *   //#define USB_DEVICE_CLASS_HID_CUSTOM   1
 *   2. ��USB�����ϵ��Ժ����س���
 *   3. �ڵ����ϻ���ʾ��һ���̷���
 *
 * - ʵ������
 *   1. ���̷������Կ���������һ��README.TXT�ļ���
 *   2. ������U���������϶��ļ�,���ڻ���ʾ���϶��ļ�����Ϣ��
 *
 * \par Դ����
 * \snippet demo_hc32f460_core_usbd_msc.c src_hc32f460_core_usbd_msc
 *
 * \internal
 * \par Modification History
 * - 1.00 19-05-28  htf, first implementation
 * \endinternal
 */

/**
 * \addtogroup demo_if_demo_hc32f460_core_usbd_msc
 * \copydoc demo_hc32f460_core_usbd_msc.c
 */

/** [src_demo_hc32f460_core_usbd_msc] */

#include "demo_hc32f460_entries.h"
#include "am_hc32f460_inst_init.h"
#include "demo_hc32f460_core_entries.h"
#include "am_usbd_msc.h"
#include "am_vdebug.h"

void demo_hc32f460_usbd_msc_entry (void)
{

    AM_DBG_INFO("demo hc32f460_core usbd msc!\r\n");

    /* usb U��ʵ����ʼ��*/
    am_usbd_msc_handle handle = am_hc32f460_usb_msc_inst_init();

    demo_usbd_msc_entry(handle);
}

/** [src_demo_hc32f460_core_usbd_msc] */

/* end of file */