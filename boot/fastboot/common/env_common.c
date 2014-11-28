/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>

 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <malloc.h>
#ifdef AUTELAN
#include <mmc.h>
#endif
DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_AMIGAONEG3SE
	extern void enable_nvram(void);
	extern void disable_nvram(void);
#endif

#undef DEBUG_ENV
#ifdef DEBUG_ENV
#define DEBUGF(fmt,args...) printf(fmt ,##args)
#else
#define DEBUGF(fmt,args...)
#endif

extern env_t *env_ptr;

extern int env_relocate_spec (unsigned int);
extern uchar env_get_char_spec(int);

static uchar env_get_char_init (int index);

/************************************************************************
 * Default settings to be used when no valid environment is found
 */
#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)

#ifdef CONFIG_SUPPORT_CA_RELEASE
uchar default_environment[] = {
	"\0"
};
#else

#ifdef AUTELAN
#ifdef CONFIG_BOOTARGS
#undef CONFIG_BOOTARGS
#endif
#define CONFIG_BOOTARGS         \
    "mem=2G"                    \
        " "                     \
    "console=ttyAMA0,115200"    \
        " "                     \
    "root=/dev/mmcblk0p8"       \
        " "                     \
    "rootfstype=ext4"           \
        " "                     \
    "rootwait"                  \
        " "                     \
    "ro"                        \
        " "                     \
    "blkdevparts="              \
        "mmcblk0:"              \
        "512K(boot),"           \
        "512K(argv),"           \
        "4M(baseparam),"        \
        "4M(pqparam),"          \
        "4M(logo),"             \
        "8M(kernel),"           \
        "200M(rootfs0),"        \
        "200M(rootfs1),"        \
        "200M(rootfs2),"        \
        "3000M(rootfs_data),"   \
        "-(others)"             \
        " "                     \
    "mmz=ddr,0,0,300M"

#ifdef CONFIG_BOOTCOMMAND
#undef CONFIG_BOOTCOMMAND
#endif
#define CONFIG_BOOTCOMMAND "mmc read 0 0x1000000 0x6800 0x4000;bootm 0x1000000"
#endif

uchar default_environment[] = {
#ifdef	CONFIG_BOOTARGS
	"bootargs="	CONFIG_BOOTARGS			"\0"
#endif
#ifdef	CONFIG_BOOTCOMMAND
	"bootcmd="	CONFIG_BOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_RAMBOOTCOMMAND
	"ramboot="	CONFIG_RAMBOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_NFSBOOTCOMMAND
	"nfsboot="	CONFIG_NFSBOOTCOMMAND		"\0"
#endif
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	"bootdelay="	MK_STR(CONFIG_BOOTDELAY)	"\0"
#endif
#if defined(CONFIG_BAUDRATE) && (CONFIG_BAUDRATE >= 0)
	"baudrate="	MK_STR(CONFIG_BAUDRATE)		"\0"
#endif
#ifdef	CONFIG_LOADS_ECHO
	"loads_echo="	MK_STR(CONFIG_LOADS_ECHO)	"\0"
#endif
#ifdef	CONFIG_MDIO_INTF
	"mdio_intf="	CONFIG_MDIO_INTF	        "\0"
#endif
#ifdef	CONFIG_ETHADDR
	"ethaddr="	MK_STR(CONFIG_ETHADDR)		"\0"
#endif
#ifdef	CONFIG_ETH1ADDR
	"eth1addr="	MK_STR(CONFIG_ETH1ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH2ADDR
	"eth2addr="	MK_STR(CONFIG_ETH2ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH3ADDR
	"eth3addr="	MK_STR(CONFIG_ETH3ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH4ADDR
	"eth4addr="	MK_STR(CONFIG_ETH4ADDR)		"\0"
#endif
#ifdef	CONFIG_ETH5ADDR
	"eth5addr="	MK_STR(CONFIG_ETH5ADDR)		"\0"
#endif
#ifdef	CONFIG_IPADDR
	"ipaddr="	MK_STR(CONFIG_IPADDR)		"\0"
#endif
#ifdef	CONFIG_SERVERIP
	"serverip="	MK_STR(CONFIG_SERVERIP)		"\0"
#endif
#ifdef	CONFIG_SYS_AUTOLOAD
	"autoload="	CONFIG_SYS_AUTOLOAD			"\0"
#endif
#ifdef	CONFIG_PREBOOT
	"preboot="	CONFIG_PREBOOT			"\0"
#endif
#ifdef	CONFIG_ROOTPATH
	"rootpath="	MK_STR(CONFIG_ROOTPATH)		"\0"
#endif
#ifdef	CONFIG_GATEWAYIP
	"gatewayip="	MK_STR(CONFIG_GATEWAYIP)	"\0"
#endif
#ifdef	CONFIG_NETMASK
	"netmask="	MK_STR(CONFIG_NETMASK)		"\0"
#endif
#ifdef	CONFIG_HOSTNAME
	"hostname="	MK_STR(CONFIG_HOSTNAME)		"\0"
#endif
#ifdef	CONFIG_BOOTFILE
	"bootfile="	MK_STR(CONFIG_BOOTFILE)		"\0"
#endif
#ifdef	CONFIG_LOADADDR
	"loadaddr="	MK_STR(CONFIG_LOADADDR)		"\0"
#endif
#ifdef  CONFIG_CLOCKS_IN_MHZ
	"clocks_in_mhz=1\0"
#endif
#if defined(CONFIG_PCI_BOOTDELAY) && (CONFIG_PCI_BOOTDELAY > 0)
	"pcidelay="	MK_STR(CONFIG_PCI_BOOTDELAY)	"\0"
#endif
#ifdef  CONFIG_EXTRA_ENV_SETTINGS
	CONFIG_EXTRA_ENV_SETTINGS
#endif
	"\0"
};
#endif

void env_crc_update (void)
{
	env_ptr->crc = crc32(0, env_ptr->data, ENV_SIZE);
}

static uchar env_get_char_init (int index)
{
	uchar c;

	/* if crc was bad, use the default environment */
	if (gd->env_valid)
	{
		c = env_get_char_spec(index);
	} else {
		c = default_environment[index];
	}

	return (c);
}

#ifdef CONFIG_AMIGAONEG3SE
uchar env_get_char_memory (int index)
{
	uchar retval;
	enable_nvram();
	if (gd->env_valid) {
		retval = ( *((uchar *)(gd->env_addr + index)) );
	} else {
		retval = ( default_environment[index] );
	}
	disable_nvram();
	return retval;
}
#else
uchar env_get_char_memory (int index)
{
	if (gd->env_valid) {
		return ( *((uchar *)(gd->env_addr + index)) );
	} else {
		return ( default_environment[index] );
	}
}
#endif

uchar env_get_char (int index)
{
	uchar c;

	/* if relocated to RAM */
	if (gd->flags & GD_FLG_RELOC)
		c = env_get_char_memory(index);
	else
		c = env_get_char_init(index);

	return (c);
}

uchar *env_get_addr (int index)
{
	if (gd->env_valid) {
		return ( ((uchar *)(gd->env_addr + index)) );
	} else {
		return (&default_environment[index]);
	}
}

void set_default_env(void)
{
	if (sizeof(default_environment) > ENV_SIZE) {
		puts ("*** Error - default environment is too large\n\n");
		return;
	}

	memset(env_ptr, 0, sizeof(env_t));
	memcpy(env_ptr->data, default_environment,
	       sizeof(default_environment));
#ifdef CONFIG_SYS_REDUNDAND_ENVIRONMENT
	env_ptr->flags = 0xFF;
#endif
	/* 
	 * optimize uboot startup time, only do_saveenv command update CRC, 
	 * so if you want do saveenv, you should call function env_crc_update() 
	 * before call function saveenv()
	 * modified by baijinying KF39160 2011-04-22
	 */
	/* env_crc_update (); */
	gd->env_valid = 1;
}

#ifdef AUTELAN

static char *
get_first_env(void)
{
    char *first = (char *)env_ptr->data;

    if (first[0]) {
//        println("get first env:%s", first);
        
        return first;
    } else {
        println("no found first env");
        
        return NULL;
    }
}

static char *
get_next_env(char *env)
{
    int len = strlen(env);
    char *next = env + len + 1; /* skip '\0' */

    if (next[0]) {
//        println("get next env:%s", next);
        
        return next;
    } else {
//        println("no found next env");
        
        return NULL;
    }
}

static char *
get_env_byname(char *name)
{
    char *env = get_first_env();

    while(env) {
        if (0==memcmp(env, name, strlen(name))) {
            return env;
        }
        
        env = get_next_env(env);
    }

    println("no found env:%s", name);
    
    return NULL;    
}

static int 
read_emmc(unsigned int begin, void *buf, int size)
{
    struct mmc *mmc = find_mmc_device(0);
    int ret;

    if (!mmc) {
        println("init mmc error");
        return -1;
    }

    ret = mmc->block_dev.block_read(0, begin >> 9,
        size >> 9, buf);
    if (ret != (size >> 9)){
        println("read emmc error, begin:0x%x, size:0x%x", begin, size);
        return -1;
    }

    return ret << 9;
}

static int 
write_emmc(unsigned int begin, void *buf, int size)
{
    struct mmc *mmc = find_mmc_device(0);
    int ret;

    if (!mmc) {
        println("init mmc error");
        return -1;
    }

    ret = mmc->block_dev.block_write(0, begin >> 9,
        size >> 9, buf);
    if (ret != (size >> 9)) {
        println("write emmc error, begin:0x%x, size:0x%x", begin, size);
        return -1;
    }
    
    return ret << 9;
}

static char *
change_bootargs(int rootfs)
{
    char *env, *root, *mmcblk;
    
    rootfs += AT_ROOTFS_BASE;
    env = get_env_byname("bootargs");
    if (NULL==env) {
        return NULL;
    }
    
#define BOOTARGS_ROOT "root=/dev/mmcblk0p"
    root = strstr(env, BOOTARGS_ROOT);
    if (NULL==root) {
        return NULL;
    }
    
    mmcblk = root + strlen(BOOTARGS_ROOT);
    if (*mmcblk != rootfs) {
        println("rootfs changed from %c to %c", *mmcblk, rootfs);
        *mmcblk = rootfs;
    }
    
    return env;
}

#define ATENV(x)        ((char *)env_ptr->atenv[x])
#define ATENV_0(x)      ATENV(x)[0]

static inline int 
get_rootfs_current(void)
{
    return ATENV_0(AT_ENV_ROOTFS) - '0';
}

static inline void 
set_rootfs_current(int rootfs)
{
    ATENV_0(AT_ENV_ROOTFS) = '0' + rootfs;
}

static inline int
get_rootfs_buddy(int rootfs)
{
    switch(rootfs) {
    case 1:
        return 2;
    case 2:
        return 1;
    default:
        return 1;
    }
}

static inline int
get_rootfs_state(int rootfs)
{
    switch(rootfs) {
    case 1:
        return ATENV_0(AT_ENV_ROOTFS1);
    case 2:
        return ATENV_0(AT_ENV_ROOTFS2);
    case 0: /* down */
    default:
        /*
        * rootfs0's state is ok
        */
        return AT_ROOTFS_OK;
    }
}

static inline void
set_rootfs_state(int rootfs, int state)
{
    switch(rootfs) {
    case 1:
        ATENV_0(AT_ENV_ROOTFS1) = state;
        break;
    case 2:
        ATENV_0(AT_ENV_ROOTFS2) = state;
        break;
    default:
        break;
    }
}


static inline void
set_rootfs_error(int rootfs, int error)
{
    switch(rootfs) {
    case 1:
        ATENV_0(AT_ENV_ROOTFS1ERR) = error + '0';
    
        break;
    case 2:
        ATENV_0(AT_ENV_ROOTFS2ERR) = error + '0';
    
        break;
    default:
        println("rootfs0 error allways 0");
        
        break;
    }
}

static inline int
get_rootfs_error(int rootfs)
{
    int error;
    
    switch(rootfs) {
    case 1:
        error = ATENV_0(AT_ENV_ROOTFS1ERR) - '0';
        
        break;
    case 2:
        error = ATENV_0(AT_ENV_ROOTFS2ERR) - '0';
        
        break;
    default:
        return 0;
    }
    
    if (error<0) {
        println("rootfs%d error(%d) < 0, reset to 0", 
            rootfs, 
            error);

        set_rootfs_error(rootfs, 0);
        
        return 0;
    }

    return error;
}

static inline int
add_rootfs_error(int rootfs)
{
    int error = get_rootfs_error(rootfs) + 1;

    set_rootfs_error(rootfs, error);

    println("add rootfs%d error from %d to %d", 
        rootfs, 
        error - 1, 
        error);
    
    return error;
}


#define ROOTFS_DUMP(_name) \
    println(#_name ":%c", ATENV_0(_name))

static void 
rootfs_dump(void) 
{
    
    println("=======rootfs dump begin==========");
    println("AT_ENV_INIT:%s", ATENV(AT_ENV_INIT));
    ROOTFS_DUMP(AT_ENV_ROOTFS);
    ROOTFS_DUMP(AT_ENV_ROOTFS1);
    ROOTFS_DUMP(AT_ENV_ROOTFS1ERR);
    ROOTFS_DUMP(AT_ENV_ROOTFS2);
    ROOTFS_DUMP(AT_ENV_ROOTFS2ERR);
    println("=======rootfs dump end============");
}

#define atenv_init(idx, deft) do{ \
    memset(ATENV(idx), 0, AT_ENV_LINE_SIZE); \
    strcpy(ATENV(idx), deft); \
    println(#idx " init to %s", deft); \
}while(0)

static void 
rootfs_init(void) 
{
    atenv_init(AT_ENV_INIT,         AT_DEFT_INIT);
    atenv_init(AT_ENV_BOOTVER,      AT_DEFT_BOOTVER);
    atenv_init(AT_ENV_ROOTFS,       AT_DEFT_ROOTFS);
    
    atenv_init(AT_ENV_ROOTFS0,      AT_DEFT_ROOTFS0);
    atenv_init(AT_ENV_ROOTFS1,      AT_DEFT_ROOTFS1);
    atenv_init(AT_ENV_ROOTFS2,      AT_DEFT_ROOTFS2);
    
    atenv_init(AT_ENV_ROOTFS0ERR,   AT_DEFT_ROOTFS0ERR);
    atenv_init(AT_ENV_ROOTFS1ERR,   AT_DEFT_ROOTFS1ERR);
    atenv_init(AT_ENV_ROOTFS2ERR,   AT_DEFT_ROOTFS2ERR);
}

static void 
rootfs_try_init(void) 
{
    rootfs_dump();
    
    if (0!=memcmp(ATENV(AT_ENV_INIT), AT_DEFT_INIT, strlen(AT_DEFT_INIT))) {
        println("atenv need to init...");

        rootfs_init();
    }
}

static void 
rootfs_select() {
    int current = get_rootfs_current();
    int buddy = get_rootfs_buddy(current);
    int rootfs = current;
    int error;
    int state;
    char *statestring;
    
    println("current rootfs is %d", current);
    println("buddy rootfs is %d", buddy);
    
    /*
    * try current rootfs
    *   if is rootfs0, state is ok
    */
    state = get_rootfs_state(rootfs);
    switch(state) {
    case AT_ROOTFS_OK: /* down */
    case AT_ROOTFS_VERFY:
        error = get_rootfs_error(rootfs);
        
        if (error < 3) {
            /*
            * try again
            */
            println("current rootfs:%d state:%c error:%d, go on", 
                rootfs,
                state,
                error);
            
            goto start_up;
        } else {
            set_rootfs_state(rootfs, AT_ROOTFS_FAIL);

            println("current rootfs:%d state:%c error:%d, set state to failed and try buddy", 
                rootfs,
                state,
                error);
            /*
            * max error, try buddy
            */            
            goto try_buddy;
        }
    case AT_ROOTFS_FAIL:
        println("current rootfs is failed(%d), try buddy", error);
        
        goto try_buddy;
    default:
        /*
        * fix state to fail, try buddy
        */
        set_rootfs_state(rootfs, AT_ROOTFS_FAIL);
        println("current rootfs state is unknow(%d), reset to failed and try buddy", state);
        
        goto try_buddy;
    }

try_buddy:
    rootfs = buddy;
    state = get_rootfs_state(rootfs);
    switch(state) {
    case AT_ROOTFS_OK: /* down */
    case AT_ROOTFS_VERFY:
        error = get_rootfs_error(rootfs);        
        if (error < 3) {
            /*
            * try again
            */
            println("buddy rootfs:%d state:%c error:%d, go on", 
                rootfs,
                state,
                error);
        } else {
            /*
            * max error, try rootfs0
            */
            rootfs = 0;
            set_rootfs_state(rootfs, AT_ROOTFS_FAIL);

            println("buddy rootfs:%d state:%c error:%d, set state to failed and try rootfs0",
                rootfs,
                state,
                error);
        }

        break;
    case AT_ROOTFS_FAIL:
        rootfs = 0;
        println("buddy rootfs is failed(%d), try rootfs0", error);
        break;
    default:
        /*
        * fix state to fail, try buddy
        */
        set_rootfs_state(rootfs, AT_ROOTFS_FAIL);
        rootfs = 0;
        
        println("buddy rootfs state is unknow(%d), reset to failed and try rootfs0", state);
        
        break;
    }

start_up:
    set_rootfs_current(rootfs);
    add_rootfs_error(rootfs);
    change_bootargs(rootfs);
    env_crc_update();
    saveenv();
}

#endif

void env_relocate (void)
{
#ifndef CONFIG_RELOC_FIXUP_WORKS
	DEBUGF ("%s[%d] offset = 0x%lx\n", __FUNCTION__,__LINE__,
		gd->reloc_off);
#endif

#ifdef CONFIG_AMIGAONEG3SE
	enable_nvram();
#endif

#ifdef ENV_IS_EMBEDDED
    println("ENV_IS_EMBEDDED...");
	/*
	 * The environment buffer is embedded with the text segment,
	 * just relocate the environment pointer
	 */
#ifndef CONFIG_RELOC_FIXUP_WORKS
	env_ptr = (env_t *)((ulong)env_ptr + gd->reloc_off);
#endif
	DEBUGF ("%s[%d] embedded ENV at %p\n", __FUNCTION__,__LINE__,env_ptr);
#else
    println("ENV_IS_NOT_EMBEDDED...");
	/*
	 * We must allocate a buffer for the environment
	 */
	env_ptr = (env_t *)malloc (CONFIG_ENV_SIZE);
	DEBUGF ("%s[%d] malloced ENV at %p\n", __FUNCTION__,__LINE__,env_ptr);
#endif

	if (gd->env_valid == 0) {
#if defined(CONFIG_GTH)	|| defined(CONFIG_ENV_IS_NOWHERE)	/* Environment not changable */
		puts ("Using default environment\n\n");
#else
		puts ("*** Warning - bad CRC, using default environment\n\n");
		show_boot_progress (-60);
#endif
		set_default_env();
	} else {
		int rel;
		rel = env_relocate_spec(CONFIG_ENV_ADDR);

#ifdef CONFIG_ENV_BACKUP
		if (rel) {
			printf("Read Env form %s addr(0x%08X) fail, "
			       "try to read from Backup Env.\n",
			       env_get_media(NULL),
			       CONFIG_ENV_ADDR);

			rel = env_relocate_spec(CONFIG_ENV_BACKUP_ADDR);
			/*
			 * the saveenv() will not calculate crc, the crc value
			 * come from env_relocate_spec().
			 */
			if (!rel)
				rel = saveenv();
		}
#endif /* CONFIG_ENV_BACKUP */
		if (rel) {
			printf("\n*** Warning - bad CRC or %s, "
			      "using default environment\n\n",
			      env_get_media(NULL));
			set_default_env();
		}
	}
	gd->env_addr = (ulong)&(env_ptr->data);

#ifdef AUTELAN
    rootfs_try_init();
    rootfs_select();
#endif

#ifdef CONFIG_AMIGAONEG3SE
	disable_nvram();
#endif
}

#ifdef CONFIG_AUTO_COMPLETE
int env_complete(char *var, int maxv, char *cmdv[], int bufsz, char *buf)
{
	int i, nxt, len, vallen, found;
	const char *lval, *rval;

	found = 0;
	cmdv[0] = NULL;

	len = strlen(var);
	/* now iterate over the variables and select those that match */
	for (i=0; env_get_char(i) != '\0'; i=nxt+1) {

		for (nxt=i; env_get_char(nxt) != '\0'; ++nxt)
			;

		lval = (char *)env_get_addr(i);
		rval = strchr(lval, '=');
		if (rval != NULL) {
			vallen = rval - lval;
			rval++;
		} else
			vallen = strlen(lval);

		if (len > 0 && (vallen < len || memcmp(lval, var, len) != 0))
			continue;

		if (found >= maxv - 2 || bufsz < vallen + 1) {
			cmdv[found++] = "...";
			break;
		}
		cmdv[found++] = buf;
		memcpy(buf, lval, vallen); buf += vallen; bufsz -= vallen;
		*buf++ = '\0'; bufsz--;
	}

	cmdv[found] = NULL;
	return found;
}
#endif
