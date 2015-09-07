#ifndef __BOOTM_H__
#define __BOOTM_H__
/******************************************************************************/
#ifndef AT_ENV_LINE_SIZE
#define AT_ENV_LINE_SIZE        128
#endif

#ifndef AT_ENV_COUNT
#define AT_ENV_COUNT            100
#endif

#define AT_ROOTFS_OK            'o'
#define AT_ROOTFS_FAIL          'f'
#define AT_ROOTFS_VERFY         'v'

#define AT_ROOTFS_BASE          '7'

/*
* 只能新增表项，禁止修改已有表项的索引
*/
#define AT_ENV_INIT             0
#define AT_ENV_ROOTFS           1
#define AT_ENV_ROOTFS1          2
#define AT_ENV_ROOTFS1ERR       3
#define AT_ENV_ROOTFS2          4
#define AT_ENV_ROOTFS2ERR       5

#define AT_ENV_MAC              6
#define AT_ENV_SN               7
#define AT_ENV_BOARDTYPE        8
#define AT_ENV_BOARDVERSION     9
#define AT_ENV_PSN              10
#define AT_ENV_MID              11
#if 0
#define AT_ENV_ROOTFS0VER       12
#define AT_ENV_ROOTFS1VER       13
#define AT_ENV_ROOTFS2VER       14
#endif
#define AT_ENV_BOOTVER          15

#define AT_ENV_OEM_MAC          36
#define AT_ENV_OEM_SN           37
#define AT_ENV_OEM_BOARDTYPE    38
#define AT_ENV_OEM_BOARDVERSION 39

#define AT_ENV_PRIVATE          40

#define AT_ENV_PTEST            99

#define AT_DEFT_INIT            "f00d1e"
#define AT_DEFT_BOOTVER         "1.2"
#define AT_DEFT_ROOTFS          "1"
#define AT_DEFT_ROOTFS0         "o"
#define AT_DEFT_ROOTFS1         "o"
#define AT_DEFT_ROOTFS2         "o"
#define AT_DEFT_ROOTFS0ERR      "0"
#define AT_DEFT_ROOTFS1ERR      "0"
#define AT_DEFT_ROOTFS2ERR      "0"

#define AT_NAME_INIT                "init"
#define AT_NAME_BOOTVER             "bootver"
#define AT_NAME_ROOTFS              "rootfs"
#define AT_NAME_ROOTFS1             "rootfs1"
#define AT_NAME_ROOTFS2             "rootfs2"
#define AT_NAME_ROOTFS1ERR          "rootfs1err"
#define AT_NAME_ROOTFS2ERR          "rootfs2err"
#if 0
#define AT_NAME_ROOTFS0VER          "rootfs0ver"
#define AT_NAME_ROOTFS1VER          "rootfs1ver"
#define AT_NAME_ROOTFS2VER          "rootfs2ver"
#endif
#define AT_NAME_PSN                 "psn"
#define AT_NAME_MID                 "mid"
#define AT_NAME_MAC                 "pcba.mac"
#define AT_NAME_SN                  "pcba.sn"
#define AT_NAME_BOARDTYPE           "pcba.type"
#define AT_NAME_BOARDVERSION        "pcba.version"
#define AT_NAME_OEM_MAC             "oem.mac"
#define AT_NAME_OEM_SN              "oem.sn"
#define AT_NAME_OEM_BOARDTYPE       "oem.type"
#define AT_NAME_OEM_BOARDVERSION    "oem.version"
#define AT_NAME_PRIVATE             "idx%d"

#define println(fmt, args...)   printf(fmt "\n", ##args)
#define dprintln println
#define countof_array(x)        (sizeof(x)/sizeof((x)[0]))
/******************************************************************************/
#endif
