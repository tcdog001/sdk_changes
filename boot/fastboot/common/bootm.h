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
#define AT_ENV_PRIVATE          10

#define AT_DEFT_INIT            "f00d1e"
#define AT_DEFT_ROOTFS          "1"
#define AT_DEFT_ROOTFS1         "o"
#define AT_DEFT_ROOTFS1ERR      "0"
#define AT_DEFT_ROOTFS2         "o"
#define AT_DEFT_ROOTFS2ERR      "0"

#define AT_NAME_INIT            "init"
#define AT_NAME_ROOTFS          "rootfs"
#define AT_NAME_ROOTFS1         "rootfs1"
#define AT_NAME_ROOTFS1ERR      "rootfs1err"
#define AT_NAME_ROOTFS2         "rootfs2"
#define AT_NAME_ROOTFS2ERR      "rootfs2err"
#define AT_NAME_MAC             "pcba.mac"
#define AT_NAME_SN              "pcba.sn"
#define AT_NAME_BOARDTYPE       "pcba.type"
#define AT_NAME_BOARDVERSION    "pcba.version"
#define AT_NAME_PRIVATE         "idx%d"

#define println(fmt, args...)   printf(fmt "\n", ##args)
#define dprintln println

/******************************************************************************/
#endif
