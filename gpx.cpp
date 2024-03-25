#include <cstdio>
#include "nmemfile.h"

#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_STDIO
#include "miniz.c"

#define CURRENT_GPX_FILE_VERSION 4

struct saver : public nmemfile
{
    virtual ~saver(){}

    void metaint(const char *name, int n)
    {
        char tmp[64];
        short stmp[64];
        sprintf(tmp, "%d", n);

        int r = 0;
        for(;;r++)
            if(!(stmp[r] = tmp[r]))
                break;
        r++;

        write(name, strlen(name)+1);
        write(stmp, sizeof(short)*r);
    }
};

void printhelp()
{
    fprintf(stderr, "Usage: gpx <w> <h> <map> <scr> <col> [<map> <scr> <col> [...]]\n");
}

int main(int arga, const char *argc[])
{
    //x y map scr col map scr col

    if(arga < 6)
    {
        printhelp();
        return -1;
    }

    int sx,sy;

    sscanf(argc[1], "%d", &sx);
    sscanf(argc[2], "%d", &sy);

    if(sx <= 0 || sy <= 0)
    {
        printhelp();
        return -1;
    }

    int expected_arguments = 3 + 3 * sx * sy;
    if(arga != expected_arguments)
    {
        printhelp();
        return -1;
    }


    int xsize = 160 * sx;
    int ysize = 200 * sy;

    const int cx = sx * 40;
    const int cy = sy * 25;

    int rmapsize = cx * cy * 8;
    int rcolorsize = cx * cy;
    int rscreensize = cx * cy;
	int rbackbuffers = 1;

	int buffersize = 64 + (64 + rmapsize + rscreensize + rcolorsize) * rbackbuffers;

    saver file;

    file << int32_t(CURRENT_GPX_FILE_VERSION);	//Version

    int mode = 1;   //1 Multi
    file << int32_t(mode);

    int32_t metacount=9;
    file << metacount;

    file.metaint("xsize",xsize);
    file.metaint("ysize",ysize);
    file.metaint("mapsize",rmapsize);
    file.metaint("colorsize",rcolorsize);
    file.metaint("screensize",rscreensize);
    file.metaint("backbuffers",1);
    file.metaint("backbufsel",0);
    file.metaint("par",0);        //PC
    file.metaint("overflow",0);   //Replace

    //Buffers
    uint8_t *buffer = new uint8_t[buffersize];

	uint8_t *p = buffer + 64 + (64 + rmapsize + rscreensize + rcolorsize) * 0/*backbuffer*/;

	p+=47;	//Extra bytes

	uint8_t *crippled = p;
	p+=6;

	uint8_t *lock = p;
	p+=6;

	uint8_t *border = p;
	p++;

	uint8_t *background = p;
	p++;

	uint8_t *ext = p;
	uint8_t *ext0 = p;
	p++;
	uint8_t *ext1 = p;
	p++;
	uint8_t *ext2 = p;
	p++;


	uint8_t *map = p;
	p += rmapsize;

	uint8_t *color = p;
	p += rcolorsize;

	uint8_t *screen = p;

    *border = 0xe;

    // Loop all screens and copy data to buffer
    int carg = 3;

    for(int isy = 0; isy < sy; isy++)
    {
        for(int isx = 0; isx < sx; isx++)
        {
            //Copy map
            {
                nmemfile src;
                if(!src.load(argc[carg]))
                {
                    fprintf(stderr, "Error loading %s\n", argc[carg]);
                    return -1;
                }
                carg++;

                for(int y = 0; y < 25; y++)
                {
                    for(int x = 0; x < 40; x++)
                    {
                        for(int b = 0; b < 8; b++)
                        {
                            map[((isy * 25 + y) * cx + isx * 40 + x) * 8 + b] =
                                src[(y * 40 + x) * 8 + b];
                        }
                    }
                }
            }
            //Copy scr
            {
                nmemfile src;
                if(!src.load(argc[carg]))
                {
                    fprintf(stderr, "Error loading %s\n", argc[carg]);
                    return -1;
                }
                carg++;

                for(int y = 0; y < 25; y++)
                {
                    for(int x = 0; x < 40; x++)
                    {
                        screen[(isy * 25 + y) * cx + isx * 40 + x] =
                            src[y * 40 + x];
                    }
                }
            }
            //Copy col
            {
                nmemfile src;
                if(!src.load(argc[carg]))
                {
                    fprintf(stderr, "Error loading %s\n", argc[carg]);
                    return -1;
                }
                carg++;

                for(int y = 0; y < 25; y++)
                {
                    for(int x = 0; x < 40; x++)
                    {
                        color[(isy * 25 + y) * cx + isx * 40 + x] =
                            src[y * 40 + x];
                    }
                }
            }
        }
    }

    file.write(buffer, buffersize);
    delete [] buffer;


    // Undo buffers
    file << int32_t(0);
    file << int32_t(0);
    file << int32_t(0);

    uLongf destsize = uLongf(file.len()+16);
    p = (uint8_t *)malloc(destsize);
    if(compress2(p,&destsize,file.buf(),(uLong)file.len(),Z_BEST_COMPRESSION) != Z_OK)
    {
        fprintf(stderr, "Compression error\n");
        free(p);
        return -1;
    }

    free(file.detach());
    file.attach(p,destsize,true);

    file.save(stdout);
    return 0;
}
