#pragma once
#include <cstdint>

class nstream  
{
public:
	nstream(){};
	virtual ~nstream(){};

	virtual size_t write(const void *p, size_t l)=0;
	virtual size_t read(void *p, size_t l)=0;

	typedef enum
	{
		pos_begin=0,
		pos_current,
		pos_end
	}pos_type;

	virtual int64_t setpos(int64_t delta, pos_type how=pos_begin){return -1;}
	int64_t getpos(void){return setpos(0,pos_current);}

	inline nstream &operator<<(const bool &d){write(&d,sizeof(d));return *this;}
	inline nstream &operator<<(const float &d){write(&d,sizeof(d));return *this;}
	inline nstream &operator<<(const double &d){write(&d,sizeof(d));return *this;}
	inline nstream &operator<<(const wchar_t &d){write(&d,sizeof(d));return *this;}

	inline nstream &operator<<(const char &d){write(&d,sizeof(d));return *this;}
	inline nstream &operator<<(const short &d){write(&d,sizeof(d));return *this;}
	inline nstream &operator<<(const int32_t &d){write(&d,sizeof(d));return *this;}
	inline nstream &operator<<(const int64_t &d){write(&d,sizeof(d));return *this;}

	inline nstream &operator<<(const unsigned char &d){write(&d,sizeof(d));return *this;}
	inline nstream &operator<<(const unsigned short &d){write(&d,sizeof(d));return *this;}
	inline nstream &operator<<(uint32_t &d){write(&d,sizeof(d));return *this;}
	inline nstream &operator<<(uint64_t &d){write(&d,sizeof(d));return *this;}

#ifdef _NEWSTR_CLASS
	inline nstream &operator<<(const nstrc &o){writestring(o);return *this;}
	inline nstream &operator<<(const nstrw &o){writestring(o);return *this;}
#endif

	inline nstream &operator>>(bool &d){read(&d,sizeof(d));return *this;}
	inline nstream &operator>>(float &d){read(&d,sizeof(d));return *this;}
	inline nstream &operator>>(double &d){read(&d,sizeof(d));return *this;}
	inline nstream &operator>>(wchar_t &d){read(&d,sizeof(d));return *this;}

	inline nstream &operator>>(char &d){read(&d,sizeof(d));return *this;}
	inline nstream &operator>>(short &d){read(&d,sizeof(d));return *this;}
	inline nstream &operator>>(int32_t &d){read(&d,sizeof(d));return *this;}
	inline nstream &operator>>(int64_t &d){read(&d,sizeof(d));return *this;}

	inline nstream &operator>>(unsigned char &d){read(&d,sizeof(d));return *this;}
	inline nstream &operator>>(unsigned short &d){read(&d,sizeof(d));return *this;}
	inline nstream &operator>>(uint32_t &d){read(&d,sizeof(d));return *this;}
	inline nstream &operator>>(uint64_t &d){read(&d,sizeof(d));return *this;}

#ifdef _NEWSTR_CLASS
	inline nstream &operator>>(nstrc &o){readstring(o);return *this;}
	inline nstream &operator>>(nstrw &o){readstring(o);return *this;}
#endif

private:

#ifdef _NEWSTR_CLASS
	void writestring(const nstrc &o)
	{
		write(o,(o.len()+1)*sizeof(char));
	}
	void writestring(const nstrw &o)
	{
		write(o,(o.len()+1)*sizeof(unsigned short));
	}

	void readstring(nstrc &o)
	{
		o="";
		char b;
		for(;;)
		{
			read(&b,sizeof(char));
			if(!b)break;
			o+=b;
		}
	}
	void readstring(nstrw &o)
	{
		o=L"";
		unsigned short b;
		for(;;)
		{
			read(&b,sizeof(unsigned short));
			if(!b)break;
			o+=b;
		}
	}
#endif

};
