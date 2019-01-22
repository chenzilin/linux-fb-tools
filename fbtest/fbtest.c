#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

struct fb_dev
{
    int fb; //for frame buffer
    void *fb_mem;   //frame buffer mmap
    int fb_width, fb_height, fb_line_len, fb_size;
    int fb_bpp;
};
static struct fb_dev fbdev;

static void draw(int color)
{
        int i, j;
        uint32_t *p=(uint32_t*)fbdev.fb_mem;

        for(i=0; i<fbdev.fb_height*2; i++, p+=fbdev.fb_line_len/4){
                for(j=0; j<fbdev.fb_width; j++)
                        p[j]=color;
        }
}

static void usage()
{
    printf(
            "usage: fbtest [option] \n"
            "   -h: this message\n"
            "   -m: display mode.\n"
            "       0: auto display rgb.\n"
            "       1: control increase range one(0x00-->0x01).\n"
            "       2: control increase range double(0x02-->0x04).\n"
            "   -d: switch play on fb1 or fb0.\n"
            "       default /dev/fb0.\n"
    );
}

int framebuffer_open(const char* fbfilename)
{
    int fb;
    struct fb_var_screeninfo fb_vinfo;
    struct fb_fix_screeninfo fb_finfo;
    char realname[80];
    strcpy(realname,"/dev/");
    strcat(realname, fbfilename);

    fb = open (realname, O_RDWR);
    if(fb<0){
        printf("device %s open failed\n", realname);
        return -1;
    }
    
    if (ioctl(fb, FBIOGET_VSCREENINFO, &fb_vinfo)) {
        printf("Can't get VSCREENINFO: %s\n", strerror(errno));
        close(fb);
        return -1;
    }

    if (ioctl(fb, FBIOGET_FSCREENINFO, &fb_finfo)) {
        printf("Can't get FSCREENINFO: %s\n", strerror(errno));
        return 1;
    }

    fbdev.fb_bpp = fb_vinfo.red.length + fb_vinfo.green.length +
        fb_vinfo.blue.length + fb_vinfo.transp.length;

    fbdev.fb_width = fb_vinfo.xres;
    fbdev.fb_height = fb_vinfo.yres;
    fbdev.fb_line_len = fb_finfo.line_length;
    fbdev.fb_size = fb_finfo.smem_len;

    printf("frame buffer: %d(%d)x%d,  %dbpp, 0x%xbyte\n", 
        fbdev.fb_width, fbdev.fb_line_len, fbdev.fb_height, fbdev.fb_bpp, fbdev.fb_size);
        
    if(fbdev.fb_bpp!=32){
        printf("frame buffer must be 32bpp mode\n");
        exit(0);
    }

    fbdev.fb_mem = mmap (NULL, fbdev.fb_size, PROT_READ|PROT_WRITE,MAP_SHARED,fb,0);
    if(fbdev.fb_mem==NULL || (int)fbdev.fb_mem==-1){
        fbdev.fb_mem=NULL;
        printf("mmap failed\n");
        close(fb);
        return -1;
    }
    
    fbdev.fb=fb;
    memset (fbdev.fb_mem, 0x0, fbdev.fb_size);

    return 0;
}

void framebuffer_close()
{
    if(fbdev.fb_mem){
        munmap(fbdev.fb_mem, fbdev.fb_size);
        fbdev.fb_mem=NULL;
    }

    if(fbdev.fb){
        close(fbdev.fb);
        fbdev.fb=0;
    }
}

int main(int argc, char* argv[])
{
    int i,c;
    char fbfilename[16] = "fb0";
    int display_mode = 0;
    
    while ((c = getopt(argc, argv, "d:m:")) != -1) {
        switch (c) {
        case 'd':
            strcpy(fbfilename, optarg);
            break;
        case 'm':
            display_mode = atoi(optarg);
            break;
        case '?':
        case 'h':
            usage();
            return 1;
        }
        if(display_mode < 0 || display_mode > 2)
        {
            usage();
        }
    }

    framebuffer_open(fbfilename);

    if (display_mode == 2) {
        for(i=0;i<24;i++) {
        printf("%d:color=0x%x",i, 1<<i);
        draw(1<<i);
        getchar();
        }
    }
    else {
        // blue
        for(i=0;i<=0xff;i++){
            printf("%d:color=0x%x\n",i, i);
            draw(i);
            if (display_mode == 1) getchar();
            else usleep(500000);
        }
        // green
        for(i=0;i<=0xffff;i+=0x100){
            printf("%d:color=0x%x\n",i, i);
            draw(i);
            if (display_mode == 1) getchar();
            else usleep(500000);
        }
        // red
        for(i=0;i<=0xffffff;i+=0x10000){
            printf("%d:color=0x%x\n",i, i);
            draw(i);
            if (display_mode == 1) getchar();
            else usleep(500000);
        }
    }

    framebuffer_close();
    return 0;
}

