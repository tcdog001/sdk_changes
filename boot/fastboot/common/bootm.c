
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bootm.h"

#define BOOTENV_MMCBLK          "/dev/mmcblk0p2"

#define ENV_NAME_LEN            15
#define ENV_CHANGED             0x01
#define ENV_INIT(_idx, _name)   [_idx] = { .name = _name, .flag = 0 }

static struct {
    char name[1+ENV_NAME_LEN];
    int flag;
} envctl[AT_ENV_COUNT] = {
    ENV_INIT(AT_ENV_INIT,       AT_NAME_INIT),
    ENV_INIT(AT_ENV_ROOTFS,     AT_NAME_ROOTFS),
    ENV_INIT(AT_ENV_ROOTFS1,    AT_NAME_ROOTFS1),
    ENV_INIT(AT_ENV_ROOTFS1ERR, AT_NAME_ROOTFS1ERR),
    ENV_INIT(AT_ENV_ROOTFS2,    AT_NAME_ROOTFS2),
    ENV_INIT(AT_ENV_ROOTFS2ERR, AT_NAME_ROOTFS2ERR),
    ENV_INIT(AT_ENV_MAC,        AT_NAME_MAC),
    ENV_INIT(AT_ENV_SN,         AT_NAME_SN),
    ENV_INIT(AT_ENV_BOARDTYPE,  AT_NAME_BOARDTYPE),
    ENV_INIT(AT_ENV_BOARDVERSION, AT_NAME_BOARDVERSION),
};

static char bootenv[AT_ENV_COUNT][AT_ENV_LINE_SIZE];

#define BLKSIZE     512
#define MULTIPLE    (BLKSIZE/AT_ENV_LINE_SIZE)

static int
init_private(void)
{
    int i;

    for (i=AT_ENV_PRIVATE; i<AT_ENV_COUNT; i++) {
        sprintf(envctl[i].name, AT_NAME_PRIVATE, i);
    }
}

static int
env_write(FILE *f)
{
    int i, k, idx, ret = 0;

#if 0
    for (i=0; i<AT_ENV_COUNT/MULTIPLE; i++) {
        int changed = 0;
        
        for (k=0; k<MULTIPLE; k++) {
            idx = i*MULTIPLE + k;

            if (ENV_CHANGED & envctl[k].flag) {
                changed = 1;

                break;
            }
        }

        if (changed) {
            int err = fseek(f, BLKSIZE*i, SEEK_SET);
            if (err) {
                println("seek error(%d), skip it", errno);

                ret = -1; continue;
            }
            
            int count = fwrite(bootenv[i*MULTIPLE], BLKSIZE, 1, f);
            if (1 != count) {
                println("write error(%d), skip it", errno);

                ret = -1; continue;
            }
        }
    }
#else
    for (i=0; i<AT_ENV_COUNT; i++) {
        if (ENV_CHANGED & envctl[i].flag) {
            int err = fseek(f, AT_ENV_LINE_SIZE*i, SEEK_SET);
            if (err) {
                println("seek %s error(%d), skip it", envctl[i].name, errno);

                ret = -1; continue;
            }
            
            int count = fwrite(bootenv[i], AT_ENV_LINE_SIZE, 1, f);
            if (1 != count) {
                println("write %s=%s error(%d), skip it", envctl[i].name, bootenv[i], errno);

                ret = -1; continue;
            }
        }
    }
#endif

    return ret;
}

static int 
env_read(FILE *f)
{
    int count;
    
    count = fread((void *)bootenv, AT_ENV_LINE_SIZE, AT_ENV_COUNT, f);
    if (AT_ENV_COUNT != count) {
        println("read " BOOTENV_MMCBLK " error");

        return -1;
    }
    
    return 0;
}

void usage(int argc, char *argv[])
{
    println("%s ==> get all env", argv[0]);
    println("%s name ==> get env by name", argv[0]);
    println("%s name1=value1 name2=value2 ... ==> set env by name and value", argv[0]);
}

static int
getidx_byname(char *name)
{
    int i;

    for (i=0; i<AT_ENV_COUNT; i++) {
        if (0==strcmp(name, envctl[i].name)) {
            return i;
        }
    }

    return -1;
}

static char *
getvalue_byname(char *name)
{
    int idx = getidx_byname(name);

    if (idx<0) {
        return NULL;
    } else {
        return bootenv[idx];
    }
}

int main(int argc, char *argv[])
{
    FILE *f = NULL;
    int count;
    int err = 0;
    int len;
    int idx;
    int i;
    char *name;
    char *value;
    
    init_private();
    
    f = fopen(BOOTENV_MMCBLK, "rb+");
    if (NULL==f) {
        println("can not open " BOOTENV_MMCBLK);
        
        err = -1; goto exit;
    }
    
    if (env_read(f)) {
        err = -1; goto exit;
    }
    
    /*
    * display all
    */
    if (1==argc) {
        for (i=1; i<AT_ENV_COUNT; i++) {
            if (bootenv[i][0]) {
                println("%s=%s", envctl[i].name, bootenv[i]);
            }
        }

        err = 0; goto exit;
    }
    else if (2==argc) {
        char *name = argv[1];
        /*
        * help
        */
        if (0==strcmp("-h", name) || 0==strcmp("--help", name)) {
            usage(argc, argv);
            err = 0; goto exit;
        }
        /*
        * get by name
        */
        else if (NULL==strchr(name, '=')) {
            value = getvalue_byname(name);
            if (NULL==value) {
                println("argv[1](%s) bad name", name);

                err = -1;
            }
            else if (0==value[0]) {
                /* empty */
                err = 0;
            }
            else {
                println("%s", value);
                
                err = 0;
            }

            goto exit;
        }
    }
    
    /*
    * set by name
    */
    for (i=1; i<argc; i++) {
        char line[2*AT_ENV_LINE_SIZE] = {0};
        len = strlen(argv[i]);
        
        /*
        * check input length
        */
        if (len > (2*AT_ENV_LINE_SIZE-1)) {
            println("argv[%d](%s) too long", i, argv[i]);

            err = -1; goto exit;
        }
        
        strcpy(line, argv[i]);

        /*
        * get name
        */
        name = line;
        value = strchr(line, '=');
        if (NULL==value) {
            println("argv[%d](%s) should as xxx=xxxx", i, argv[i]);

            err = -1; goto exit;
        }
        *value = 0; value++;
        
        /*
        * check name
        */
        idx = getidx_byname(name);
        if (idx<0) {
            println("argv[%d](%s) bad name(%s)", i, argv[i], name);
            
            err = -1; goto exit;
        }
        
        /*
        * check value
        */
        if (0==value[0]) {
            bootenv[idx][0] = 0;
        }
        else if (strlen(value) > (AT_ENV_LINE_SIZE-1)) {
            println("argv[%d](%s) value length max %d", i, argv[i], (AT_ENV_LINE_SIZE-1));

            err = -1; goto exit;
        }
        else {
            strcpy(bootenv[idx], value);
        }
        envctl[idx].flag |= ENV_CHANGED;
    }
    
    if (env_write(f)) {
        err = -1; goto exit;
    }
    
exit:
    fclose(f);

    return err;
}


