#include "vfs.h"
#include "errno-base.h"


#define LOG_TAG_CONST       NORM
#define LOG_TAG             "[normal]"
#include "debug.h"



extern struct vfs_operations vfs_ops_begin[];
extern struct vfs_operations vfs_ops_end[];

#define list_for_each_vfs_operation(ops) \
	for (ops=vfs_ops_begin; ops<vfs_ops_end; ops++)



void vfs_init(void)
{
    struct vfs_operations *ops;
    list_for_each_vfs_operation(ops) {
        if (NULL != ops->init) {
            log_info("ops_init!!!\n");
            ops->init();
        }
    }
}
u32 vfs_mount(void **ppvfs, void *device, void *type)
{
    if ((void *)NULL == *ppvfs) {
        *ppvfs = vfs_hdl_malloc();
        if ((void *)NULL == *ppvfs) {
            return E_NO_VFS;
        }
    }

    struct imount *pvfs = *ppvfs;
    struct vfs_operations *ops;
    list_for_each_vfs_operation(ops) {
        if (NULL != ops->mount) {
            if (NULL != type) {
                if (0 != strcmp(ops->fs_type, type)) {
                    continue;
                }
            } else {
                if (0 == strcmp(ops->fs_type, "norfs")) {
                    continue;
                }

            }
            if (0 == ops->mount(&(pvfs->pfs), device)) {
                pvfs->ops = ops;
                return 0;
            } else {
                if (NULL == ops->close_fs) {
                    ops->close_fs(pvfs->pfs);
                }
            }
        }
    }
    vfs_fhdl_free(*ppvfs);
    return E_NO_FS;
}

u32 vfs_openbypath(void *pvfs, void **ppvfile, const char *path)
{

    if ((void *)NULL == *ppvfile) {
        *ppvfile = vfs_hdl_malloc();
        if ((void *)NULL == *ppvfile) {
            return E_NO_VFS;
        }
    }

    u32 err;

    struct vfs_operations *ops;
    struct imount *p_vfs = pvfs;
    struct imount *p_vfile = *ppvfile;
    if ((void *)NULL == p_vfs) {
        err =  E_VFS_HDL;
        goto __vfs_openbypath;
    }
    p_vfile->ops = p_vfs->ops;
    ops = p_vfs->ops;

    if (NULL != ops->openbypath) {
        err = ops->openbypath(p_vfs->pfs, &p_vfile->pfile, path);
        /* if (0 != err) { */
        /* *ppvfile = vfs_fhdl_free(*ppvfile); */
        /* } */
        /* return err; */
    } else {
        err = E_VFS_OPS;
    }
__vfs_openbypath:
    if (0 != err) {
        vfs_file_close(ppvfile);
    }
    return err;
}

u32 vfs_openbyindex(void *pvfs, void **ppvfile, u32 index)
{

    if ((void *)NULL == *ppvfile) {
        *ppvfile = vfs_hdl_malloc();
        if ((void *)NULL == *ppvfile) {
            return E_NO_VFS;
        }
    }

    u32 err;
    struct vfs_operations *ops;
    struct imount *p_vfs = pvfs;
    struct imount *p_vfile = *ppvfile;
    if ((void *)NULL == p_vfs) {
        err =  E_VFS_HDL;
    }
    p_vfile->ops = p_vfs->ops;
    ops = p_vfs->ops;

    if (NULL != ops->openbyindex) {
        err = ops->openbyindex(p_vfs->pfs, &p_vfile->pfile, index);
    } else {
        err = E_VFS_OPS;
    }
__vfs_openbyindex:
    if (0 != err) {
        vfs_file_close(ppvfile);
    }
    return err;
}

u32 vfs_openbyfile(void *pcvfile, void **ppvfile, void *ext_name)
{

    if ((void *)NULL == *ppvfile) {
        *ppvfile = vfs_hdl_malloc();
        if ((void *)NULL == *ppvfile) {
            return E_NO_VFS;
        }
    }

    u32 err;
    struct vfs_operations *ops;

    struct imount *p_cvfile = pcvfile;
    struct imount *p_vfile = *ppvfile;
    if ((void *)NULL == p_cvfile) {
        err =  E_VFS_HDL;
    }
    p_vfile->ops = p_cvfile->ops;

    ops = p_vfile->ops;

    if (NULL != ops->openbyfile) {
        err = ops->openbyfile(p_cvfile->pfile, &p_vfile->pfile, ext_name);
    } else {
        err = E_VFS_OPS;
    }
__vfs_openbyindex:
    if (0 != err) {
        log_info("openbyfile : 0x%x\n", err);
        vfs_file_close(ppvfile);
    }
    return err;
}

u32 vfs_openbyclust(void *pvfs, void **ppvfile, u32 clust)
{

    if ((void *)NULL == *ppvfile) {
        *ppvfile = vfs_hdl_malloc();
        if ((void *)NULL == *ppvfile) {
            return E_NO_VFS;
        }
    }

    u32 err;
    struct vfs_operations *ops;
    struct imount *p_vfs = pvfs;
    struct imount *p_vfile = *ppvfile;
    if ((void *)NULL == p_vfs) {
        err =  E_VFS_HDL;
    }
    p_vfile->ops = p_vfs->ops;
    ops = p_vfs->ops;

    if (NULL != ops->openbyclust) {
        err = ops->openbyclust(p_vfs->pfs, &p_vfile->pfile, clust);
    } else {
        err = E_VFS_OPS;
    }
__vfs_openbyclust:
    if (0 != err) {
        vfs_file_close(ppvfile);
    }
    return err;
}

u32 vfs_createfile(void *pvfs, void **ppvfile, u32 *pindex)
{

    if ((void *)NULL == *ppvfile) {
        *ppvfile = vfs_hdl_malloc();
        if ((void *)NULL == *ppvfile) {
            return E_NO_VFS;
        }
    }

    u32 err;
    struct vfs_operations *ops;
    struct imount *p_vfs = pvfs;
    struct imount *p_vfile = *ppvfile;
    if ((void *)NULL == p_vfs) {
        err =  E_VFS_HDL;
    }
    p_vfile->ops = p_vfs->ops;
    ops = p_vfs->ops;

    if (NULL != ops->createfile) {
        err = ops->createfile(p_vfs->pfs, &p_vfile->pfile, pindex);
    } else {
        err = E_VFS_OPS;
    }
__vfs_openbyindex:
    if (0 != err) {
        vfs_file_close(ppvfile);
    }
    return err;
}

u32 vfs_read(void *pvfile, void *buf, u32 len)
{
    struct imount *p_vfile = pvfile;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfile) {
        return 0;
    }
    ops = p_vfile->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->read)) {
        u32 res;
        res = ops->read(p_vfile->pfile, buf, len);
        return res;
    }
    return  0;
}

u32 vfs_write(void *pvfile, void *buf, u32 len)
{
    struct imount *p_vfile = pvfile;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfile) {
        return 0;
    }
    ops = p_vfile->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->write)) {
        u32 res;
        res = ops->write(p_vfile->pfile, buf, len);
        return res;
    }
    return  0;
}

u32 vfs_seek(void *pvfile, u32 offset, u32 mode)
{
    struct imount *p_vfile = pvfile;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfile) {
        return E_VFILE_HDL;
    }
    ops = p_vfile->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->seek)) {
        u32 res;
        res = ops->seek(p_vfile->pfile, offset, mode);
        return res;
    }
    return  E_VFS_OPS;
}
u32 vfs_file_close(void **ppvfile)
{
    if ((void *)NULL == *ppvfile) {
        return E_VFILE_HDL;
    }
    struct imount *p_vfile = *ppvfile;
    struct vfs_operations *ops;
    ops = p_vfile->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->close_file)) {
        u32 res = E_VFS_OPS;
        if (NULL != (p_vfile->pfile)) {
            res = ops->close_file(&p_vfile->pfile);
            if (0 != res) {
                return E_FS_PFILE;
            }
        }
        *ppvfile = vfs_fhdl_free(*ppvfile);
        return res;
    }
    return  E_VFS_OPS;
}

u32 vfs_fs_close(void **ppvfs)
{
    if ((void *)NULL == *ppvfs) {
        return E_VFS_HDL;
    }
    struct imount *p_vfs = *ppvfs;
    struct vfs_operations *ops;
    u32 res  = E_VFS_OPS;
    ops = p_vfs->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->close_fs)) {
        if (NULL != p_vfs->pfs) {
            res = ops->close_fs(&p_vfs->pfs);
        }
        /* if (0 != res) { */
        /* return E_FS_PFILE; */
        /* } */
        /* return res; */
    }

    *ppvfs = vfs_fhdl_free(*ppvfs);
    return  res;
}

u32 vfs_file_name(void *pvfile, void *name, u32 len)
{
    struct imount *p_vfile = pvfile;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfile) {
        return 0;
    }
    ops = p_vfile->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->name)) {
        u32 res;
        return ops->name(p_vfile->pfile, name, len);
    }
    return 0;
}

int vfs_get_attrs(void *pvfile, void *pvfs_attr)
{
    struct imount *p_vfile = pvfile;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfile) {
        return 0;
    }
    ops = p_vfile->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->fget_attr)) {
        u32 res;
        return ops->fget_attr(p_vfile->pfile, pvfs_attr);
    }
    return 0;
}

int vfs_get_fsize(void *pvfile, void *parm)
{
    struct imount *p_vfile = pvfile;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfile) {
        return 0;
    }
    ops = p_vfile->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->flen)) {
        u32 res;
        return ops->flen(p_vfile->pfile, (u32 *)parm);
    }
    return 0;
}

int vfs_ftell(void *pvfile, void *parm)
{
    struct imount *p_vfile = pvfile;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfile) {
        return 0;
    }
    ops = p_vfile->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->ftell)) {
        u32 res;
        return ops->ftell(p_vfile->pfile, (u32 *)parm);
    }
    return 0;
}

int vfs_get_folderinfo(void *pvfile, int *start_num, int *end_num)
{
    struct imount *p_vfile = pvfile;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfile) {
        return 0;
    }
    ops = p_vfile->ops;
    int arg[2] = {0};
    arg[0] = (int)start_num;
    arg[1] = (int)end_num;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->ioctl)) {
        u32 res;
        return ops->ioctl(p_vfile->pfile, FS_IOCTL_GET_FOLDER_INFO, (int)arg);
    }
    return 0;
}

int vfs_fscan(void *pvfs, const char *file_type, u8 max_deepth, u32 clust, u32 *ret_p)
{
    struct imount *p_vfs = pvfs;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfs) {
        return E_VFILE_HDL;
    }
    ops = p_vfs->ops;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->fscan)) {
        return ops->fscan(p_vfs->pfs, file_type, max_deepth, clust, ret_p);
    }
    return  E_VFS_OPS;
}

int vfs_ioctl(void *pvfile, int cmd, int arg)
{
    struct imount *p_vfile = pvfile;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfile) {
        return E_VFILE_HDL;
    }
    ops = p_vfile->ops;
    /* log_info("ops->ioctl : 0x%x  0x%lx!!\n",ops->ioctl, (u32)&ops->ioctl); */
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->ioctl)) {
        return ops->ioctl(p_vfile->pfile, cmd, arg);
    }
    return  E_VFS_OPS;
}

int vfs_mk_dir(void *pvfs, char *folder, u8 mode)
{
    struct imount *p_vfs = pvfs;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfs) {
        return 0;
    }
    ops = p_vfs->ops;
    int arg[2] = {0};
    arg[0] = (int)folder;
    arg[1] = (int)mode;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->ioctl)) {
        u32 res;
        return ops->ioctl(p_vfs->pfs, FS_IOCTL_MK_DIR, (int)arg);
    }
    return 0;
}

int vfs_get_encfolder_info(void *pvfs, char *folder, char *ext, u32 *last_num, u32 *total_num)
{
    struct imount *p_vfs = pvfs;
    struct vfs_operations *ops;
    if ((void *)NULL == p_vfs) {
        return 0;
    }
    ops = p_vfs->ops;
    int arg[4] = {0};
    arg[0] = (int)folder;
    arg[1] = (int)ext;
    arg[2] = (int)last_num;
    arg[3] = (int)total_num;
    if (((void *)NULL != ops)  && ((void *)NULL !=  ops->ioctl)) {
        u32 res;
        return ops->ioctl(p_vfs->pfs, FS_IOCTL_GET_ENCFOLDER_INFO, (int)arg);
    }
    return 0;
}

#if 0
#define vfs_demo_show(ptr,err)              \
    log_info("Demo 0x%x : %s",err,ptr);       \
    if(0 != err){                           \
        return ;                            \
    }


static u8 demo_buff[512];
void vfs_demo(void)
{
    void *pvfs = 0;
    void *pvfile = 0;
    u32 err;
    log_info("vfs init !!!\n");
    vfs_init();
    log_info("vfs init finish !!!\n");
    err = vfs_mount(&pvfs, (void *)NULL, (void *)NULL);
    vfs_demo_show("vfs_mount\n", err);
    err = vfs_openbypath(pvfs, &pvfile, "/dir_bin_01/file1.txt");
    vfs_demo_show("vfs_openbypath\n", err);
    err = vfs_read(pvfile, demo_buff, 512);
    log_info("vfs read len : %d\n", err);
    log_info_hexdump(demo_buff, 512);
}
#endif


