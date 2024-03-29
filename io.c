#include <stdio.h>	/*FILE fopen fclose fseek ftell fread fwrite*/
#include <stdlib.h>	/*realloc free*/
#include <assert.h>	/*rassert*/
#include "io.h"		/*Io IoSz IoRet IoHead IoHeader
*					  ioSz ioLoad ioUnload ioToBuff ioFromBuff*/
const IoSz ioSz(FILE *io) {
	assert(NULL != io);
	fseek(io, 0L, SEEK_END);
	IoSz inputSz = ftell(io);
	fseek(io, 0L, SEEK_SET);
	return inputSz;
}
void ioSetBuff(Io *io, void *buff, IoSz buffSz) {
	assert(NULL != io);
	assert(NULL != buff || 0 == buffSz);
	io->buff_ = buff;
	io->buff = &io->buff_;
	io->buffSz = buffSz;
	io->buffMax = buffSz;
}
const IoRet ioLoad(Io *io, const char *restrict fPath, const char *restrict mode /*= "ro"*/) {
	assert(NULL != io);
	ioSetBuff(io, NULL, 0);
	io->io = fopen(fPath, mode);
	if(NULL == io->io) {
		printf("\nError: (NULL == fopen(\"%s\", \"%s\"))\n", fPath, mode);
		io->ioSz = 0;
		return -1;
	}
	io->ioSz = ioSz(io->io);
	return 0;
}
/*const IoRet*/void ioUnload(Io *io) {
	assert(NULL != io);
	/*IoRet ioRet = 0;*/
	if(NULL != io->buff_) {
		free(io->buff_);
	} else if(NULL != io->buff && NULL != *io->buff) {
		free(*io->buff);
	}
	if(0 < io->io) {
		fclose(io->io);
	}/* else {
		ioRet = -1;
	}*/
	ioSetBuff(io, NULL, 0);
	io->io = 0;
	/*return ioRet;*/
}
const IoRet ioToBuff(Io *io, IoSz rSz, long offset /*= 0L*/, int whence /*= SEEK_CUR*/) {
	assert(NULL != io);
	void *buff = realloc(*io->buff, rSz);
	if(NULL == buff) {
		printf("\nError: (NULL == realloc(%p, %u))\n", *io->buff, rSz);
		return -2;
	}
	*io->buff = buff;
	io->buffMax = rSz;
	fseek(io->io, offset, whence);
	io->buffSz = fread(buff, 1, rSz, io->io);
	if(rSz != io->buffSz) {
		printf("\nError: (%u == fread(%p, 1, %u, %p))\n", io->buffSz, buff, rSz, io->io);
		return -1;
	}
	return 0;
}
const IoRet ioFromBuff(Io *io, IoSz wSz, long offset /*= 0L*/, int whence /*= SEEK_CUR*/) {
	assert(NULL != io);
	const void *buff = io->buff_;
	if(NULL != io->buff && NULL != *io->buff) {
		buff = *io->buff;
	}
	if(NULL == buff) {
		printf("\nError: (NULL == *%p->buff)\n", io);
		return -2;
	}
	if(wSz > io->buffSz) {
		printf("\nError: (ioFromBuff(%p, %u, %lu, %i)) but (%u == *%p->buffSz)\n", io, wSz, offset, whence, io->buffSz, io);
		return -2;
	}
	fseek(io->io, offset, whence);
	IoSz ioSz = fwrite(buff, 1, wSz, io->io);
	if(wSz != ioSz) {
		printf("\nError: (%u == fwrite(%p, 1, %u, %p))\n", ioSz, buff, wSz, io->io);
		return -1;
	}
	io->ioSz = ioSz;
	return 0;
}
const IoRet ioTest(const char *fPath) {
	Io io;
	IoRet ioRet = 0;
	int fromBuff = 'UwUs';
	int *toBuff = NULL;
	IoSz fromBuffSz = sizeof(fromBuff);
	ioRet = ioLoad(&io, fPath, "a+");
	if(0 > ioRet) {
		printf("\nError: (%i == ioLoad(%p, %s, %s))\n", ioRet, &io, fPath, "a+");
		goto exit;
	}
	if(0 < io.ioSz) {
		printf("\nError: ioTest(\"%s\"), but \"%s\" exists (%u == io.ioSz)\n", fPath, fPath, io.ioSz);
		ioUnload(&io);
		return ioRet;
	}
	ioSetBuff(&io, &fromBuff, fromBuffSz);
	ioRet = ioFromBuff(&io, io.buffSz, 0, SEEK_SET);
	if(0 > ioRet) {
		printf("\nError: (%i == ioFromBuff(%p, %u, 0, SEEK_SET))\n", ioRet, &io, io.buffSz);
		goto exit;
	}
	if(fromBuffSz != io.ioSz) {
		printf("\nError: (0 == ioToBuff(%p, %u, 0, SEEK_SET)), but (%u == %p->ioSz).\n", &io, fromBuffSz, io.ioSz, &io);
		goto exit;
	}
	ioSetBuff(&io, NULL, 0);
	ioRet = ioToBuff(&io, io.ioSz, 0, SEEK_SET);
	if(0 > ioRet) {
		printf("\nError: (%i == ioToBuff(%p, %u, 0, SEEK_SET))\n", ioRet, &io, fromBuffSz);
		goto exit;
	}
	toBuff = io.buff_;
	if(fromBuff != *toBuff) {
		printf("\nError: wrote %u but read %u.\n", fromBuff, *toBuff);
		goto exit;
	}
exit:
	ioUnload(&io);
	ioRet = ioLoad(&io, fPath, "w");
	if(0 > ioRet) {
		printf("\nError: (%i == ioLoad(%p, %s, %s))\n", ioRet, &io, fPath, "w");
		return ioRet;
	}
	if(0 < io.ioSz) {
		printf("\nError: (%p == fopen(\"%s\",\"w\")) but (0 < io.ioSz)\n", io.io, fPath);
		ioRet = -1;
	}
	ioUnload(&io);
	return ioRet;
}

