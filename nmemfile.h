#pragma once
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "nstream.h"

class nmemfile : public nstream  
{
public:
	nmemfile(size_t initsize=0, size_t growsize=-1)
	{
		pos=leng=0;
		alloc=initsize;
		grow=growsize;

		if(alloc)
			ptr=(unsigned char *)malloc(alloc);
		else
			ptr=NULL;

		del=true;
	}

	nmemfile(void *p, size_t l, bool delp=false)
	{
		attach(p,l,delp);
	}

	virtual ~nmemfile()
	{
		if(del)
			free(ptr);
	}

	operator unsigned char *(){return ptr;}
	operator char *(){return (char *)ptr;}
	unsigned char &operator[](int i)
	{
		return ptr[i];
	}
	size_t len(void)const{return leng;}

	unsigned char *buf(void){return ptr;}
	void setlen(size_t l){leng=l;if(leng<pos)pos=leng;}

	void attach(void *p, size_t l, bool delp=false)
	{
		pos=0;
		alloc=leng=l;
		ptr=(unsigned char *)p;
		grow=-1;
		del=delp;
	}

	void *detach(void)
	{
		del=false;
		return ptr;
	}

	size_t write(const void *p, size_t l) override
	{
		if(pos+l>alloc)
		{
			if(!del)
			{
				//Buffer full, cant write all of it
				memcpy(ptr+pos,p,l=alloc-pos);
				return l;
			}

			size_t needed=(pos+l)-alloc;
			size_t nalloc=grow!=-1?grow:(alloc?alloc:256);
			alloc+=needed>nalloc?needed:nalloc;

			ptr=(unsigned char *)realloc(ptr,alloc);
			//unsigned char *n=new unsigned char[alloc];
			//memcpy(n,ptr,leng);	//Move old buffer
			//delete [] ptr;
			//ptr=n;
		}

		memcpy(ptr+pos,p,l);
		pos+=l;

		if(pos>leng)leng=pos;

		return l;
	}

	size_t read(void *p, size_t l) override
	{
		if(pos+l>leng)
			l=leng-pos;

		memcpy(p,ptr+pos,l);
		pos+=l;

		return l;
	}

	void *read(size_t l)
	{
		if(pos+l>leng)
			l=leng-pos;

		void *r=(void *)(ptr+pos);
		pos+=l;
		return r;
	}

	size_t sizeleft(void)const
	{
		return leng-pos;
	}

	int64_t setpos(int64_t delta, pos_type how=pos_begin)
	{
		int64_t newpos;

		switch(how)
		{
		case nstream::pos_begin:
			newpos=delta;
			break;
		case nstream::pos_current:
			newpos=pos+delta;
			break;
		case nstream::pos_end:
			newpos=leng+delta;
			break;
		default:
			return -1;
		}

		if(newpos<0 || newpos > int64_t(leng))return -1;

		pos=(size_t)newpos;
		return newpos;
	}

	bool load(const char *file)
	{
		FILE *fp=fopen(file, "rb");

		if(!fp)
			return false;

		if(del)free(ptr);

		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		ptr=(unsigned char *)malloc(size);
		fread(ptr, 1, size, fp);

		fclose(fp);

		pos=0;
		alloc=leng=(size_t)size;
		grow=-1;
		del=true;

		return true;
	}

	bool save(FILE *fp)
	{
		return fwrite(ptr,1,leng,fp)==leng?true:false;
	}


private:
	unsigned char *ptr;
	size_t leng,alloc,pos,grow;
	bool del;

};
