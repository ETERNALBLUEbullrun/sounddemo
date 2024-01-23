/*Allow: GPLv2.0, BSD, C.Commons, or unlicensed*/
/*Purpose: demo for sound/audio,
 * should have just took an hour to do,
 * and would go on to construct sound/audio,
 * but failed to parse small sample .wav inputs,
 * so must wav format was misunderstood somehow.*/
#ifndef IO_H_
#define IO_H_
#include <stdio.h>	/*FILE*/
#include <ctype.h>	/*uint32_t*/
typedef unsigned IoSz;
typedef int IoRet;			/*`assert(0 <= ioRet);`. Convention; -1 for IO errors, -2 for mem errors.*/
typedef struct Io {
	FILE *io;
	IoSz ioSz;
	void **buff;
	void *buff_;
	IoSz buffSz;
	IoSz buffMax;
} Io;
#define IO_CAST_TO(io, CAST_TO) ((CAST_TO)(*(io)->buff))
typedef uint32_t IoHead;	/*PE/ELF "magic word"/file-type/head*/
const IoSz ioSz(FILE *io);
void ioSetBuff(Io *io, void *buff, IoSz buffSz);
const IoRet ioLoad(Io *io, const char *restrict fPath, const char *restrict mode /*= "ro"*/);
void ioUnload(Io *io);
const IoRet ioToBuff(Io *io, IoSz rSz, long offset /*= 0L*/, int whence /*= SEEK_CUR*/);
const IoRet ioFromBuff(Io *io, IoSz wSz, long offset /*= 0L*/, int whence /*= SEEK_CUR*/);
const IoRet ioTest(const char *restrict fPath);
#endif/*ndef IO_H_*/

