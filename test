//
// Created by yufang on 17-5-23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <liblightnvm.h>
#include <time.h>

static char nvm_dev_path[NVM_DEV_PATH_LEN] = "/dev/nvme0n1";
static struct nvm_dev *dev;
static const struct nvm_geo *geo;
static struct nvm_addr *addr;

int setup(void) {
    dev = nvm_dev_open(nvm_dev_path);
    if (!dev) {
        perror("nvm_dev_open");
    }
    geo = nvm_dev_get_geo(dev);
    return 0;
}

int teardown(void) {
    nvm_dev_close(dev);

    return 0;
}

int test_write(struct nvm_addr *addr, char *str)
{
    char *buf_w = NULL;
    struct nvm_ret ret;
    int pmode = NVM_FLAG_PMODE_SNGL;
    ssize_t res;

    size_t buf_w_nbytes = geo->sector_nbytes;
    buf_w = nvm_buf_alloc(geo,buf_w_nbytes);
    if(!buf_w)
    {
        printf("nvm_buf_alloc failed \n");
        //goto exit_naddr;
    }
    //nvm_buf_fill(buf_w, buf_w_nbytes);
    strcpy(buf_w, str);

    //erase
    res = nvm_addr_erase(dev, addr, 1, pmode, &ret);
    if (res < 0) {
        printf("fail to erase \n");
        goto exit_naddr;
    }
    printf("success to erase \n");

    //write
    res = nvm_addr_write(dev, addr, 1, buf_w, NULL, pmode, &ret);
    if (res < 0) {
        printf("fail to write \n");
        goto exit_naddr;
    }
    printf("success to write \n");
    exit_naddr:
    free(buf_w);

    return 0;
}

int test_read(struct nvm_addr *addr)
{
    char *buf_r = NULL;
    struct nvm_ret ret;
    int pmode = NVM_FLAG_PMODE_SNGL;
    ssize_t res;

    size_t buf_r_nbytes = geo->sector_nbytes;
    buf_r = nvm_buf_alloc(geo, buf_r_nbytes);
    if(!buf_r)
    {
        printf("nvm_buf_alloc failed \n");
        goto exit_naddr;
    }

    //read
    res = nvm_addr_read(dev, addr, 1, buf_r, NULL, pmode, &ret);
    if (res < 0) {
        printf("fail to read \n");
        nvm_ret_pr(&ret);
        goto exit_naddr;
    }
//    char *str1 = malloc(strlen("hello world!"));
//    strcpy(str1, buf_r);
    printf("success to read ,content is %s\n",buf_r);

    exit_naddr:
    free(buf_r);

    return 0;
}

//int get_bbt (struct nvm_addr lun_addr)
//{
//    struct nvm_bbt *bbt;
//    struct nvm_ret ret;
//
//    bbt = nvm_bbt_get(dev, lun_addr, &ret);	// Verify that we can call it
//    if (!bbt)
//    {
//        printf("fail to get bbt\n");
//        goto exit_naddr;
//    }
//
//    nvm_bbt_set(dev, bbt, &ret);
//    nvm_bbt_pr(bbt);
//
//    exit_naddr:
//
//
//    return 0;
//
//}

int main() {
    clock_t start, finish;
    double duration;
    struct nvm_addr test_addr[1];
    addr = test_addr;
    //写入的地址0 block, 0 page,
    test_addr[0].ppa = 0;
    test_addr[0].g.ch = 0;
    test_addr[0].g.lun = 0;
    test_addr[0].g.blk = 0;
    test_addr[0].g.pg = 1;
    test_addr[0].g.sec = 0;
    char *str = "hello world!";
    setup();
    nvm_geo_pr(geo);
    nvm_addr_pr(*addr);
    //test write
    start = clock();
    test_write(addr, str);
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf("elapsed_time is %lf \n",duration);
    //test read
    test_read(addr);
    teardown();

}
