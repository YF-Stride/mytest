//
// Created by yufang on 17-5-25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <liblightnvm.h>
#include <time.h>

static char nvm_dev_path[NVM_DEV_PATH_LEN] = "/dev/nvme0n1";
static struct nvm_dev *dev;
static const struct nvm_geo *geo;
static struct nvm_addr blk_addr;

int setup(void) {
    dev = nvm_dev_open(nvm_dev_path);
    if (!dev) {
        perror("nvm_dev_open");
    }
    geo = nvm_dev_get_geo(dev);
    printf("dev imfor\n");
    nvm_dev_pr(dev);
    blk_addr.ppa = 0;
    blk_addr.g.ch = 0;
    blk_addr.g.lun = 0;
    blk_addr.g.blk = 0;
    return 0;
}

int teardown(void) {
    nvm_dev_close(dev);

    return 0;
}

int test_write1() {
    char *buf_w = NULL;
    struct nvm_ret ret;
    int pmode = NVM_FLAG_PMODE_SNGL;
    int naddrs = geo->nplanes * geo->nsectors;
    printf("naddrs is %d \n ", naddrs);
    struct nvm_addr addrs[naddrs];
    ssize_t res;
    size_t pg;
    int i;
    clock_t start, finish;
    double duration;


    size_t buf_w_nbytes =  naddrs * geo->sector_nbytes;
    buf_w = nvm_buf_alloc(geo, buf_w_nbytes);
    if (!buf_w) {
        printf("nvm_buf_alloc failed \n");
        goto exit_naddr;
    }
    nvm_buf_fill(buf_w, naddrs*geo->sector_nbytes);

    //erase
    for( int i = 0; i < geo -> nplanes; i++){
        addrs[i].ppa = blk_addr.ppa;
        addrs[i].g.pl = 0;
        addrs[i].g.blk =i;
        nvm_addr_pr(addrs[i]);
    }
    printf("geo -> nplanes is %zd \n",geo -> nplanes);

    res = nvm_addr_erase(dev, addrs, geo->nplanes, pmode, &ret);
    if (res < 0) {
        printf("fail to erase \n");
        goto exit_naddr;
    }
    printf("success to erase \n");
    //one plane write1
    addrs[geo -> nplanes].g.blk = 0;
    for( int i = 0; i < naddrs ; i++){
        addrs[i].ppa = blk_addr.ppa;
        addrs[i].g.pg = 0;
        addrs[i].g.sec = i % geo->nsectors;
        addrs[i].g.pl = 0;
        addrs[i].g.blk = i/geo->nsectors;
        nvm_addr_pr(addrs[i]);
    }
    start = clock();
    res = nvm_addr_write(dev, addrs, naddrs, buf_w, NULL, pmode, &ret );
    finish = clock();
    if (res < 0) {
        printf("fail to erase \n");
        goto exit_naddr;
    }
    duration = (double) (finish - start) / CLOCKS_PER_SEC;
    printf("one plane elapsed_time is %lf \n", duration);


    exit_naddr:
    free(buf_w);

    return 0;
}

int test_write2()
{
    char *buf_w = NULL;
    struct nvm_ret ret;
    int pmode;
    if(geo->nplanes == 1)
        pmode = NVM_FLAG_PMODE_SNGL;
    else if(geo->nplanes == 2)
        pmode = NVM_FLAG_PMODE_QUAD;
    else
        pmode = NVM_FLAG_PMODE_DUAL;
    int naddrs = geo->nplanes * geo->nsectors;
    printf("naddrs is %d \n",naddrs);
    struct nvm_addr addrs[naddrs];
    ssize_t res;
    size_t pg;
    int i;
    clock_t start, finish;
    double duration;


    size_t buf_w_nbytes = naddrs * geo->sector_nbytes;
    buf_w = nvm_buf_alloc(geo,buf_w_nbytes);
    if(!buf_w)
    {
        printf("nvm_buf_alloc failed \n");
        goto exit_naddr;
    }
    nvm_buf_fill(buf_w, naddrs * geo->sector_nbytes);

    //erase
    for( int i = 0; i < geo -> nplanes; i++){
        addrs[i].ppa = blk_addr.ppa;
        addrs[i].g.pl = i;
        addrs[i].g.blk = blk_addr.g.blk;
        nvm_addr_pr(addrs[i]);
    }
    res = nvm_addr_erase(dev, addrs, geo->nplanes, pmode, &ret);
    if (res < 0) {
        printf("fail to erase \n");
        goto exit_naddr;
    }
    printf("success to erase \n");
    //two plane write2
    start = clock();
    for( int i = 0; i < naddrs; i++){
        addrs[i].ppa = blk_addr.ppa;
        addrs[i].g.pg = 0;
        addrs[i].g.sec = i % geo->nsectors;
        addrs[i].g.pl = (i/geo->nsectors)%geo->nplanes;
        addrs[i].g.blk = 0;
        nvm_addr_pr(addrs[i]);
    }
    res = nvm_addr_write(dev, addrs, naddrs, buf_w, NULL, pmode, &ret);
    if (res < 0) {
        printf("fail to write \n");
        goto exit_naddr;
    }
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf("two plane elapsed_time is %lf \n",duration);

    exit_naddr:
    free(buf_w);

    return 0;
}

int main() {
    setup();
    test_write1();
    test_write2();
    teardown();
}
