#include <stdio.h>	/*FILE fopen fclose fseek ftell fread fwrite*/
#include <stdlib.h>	/*realloc free*/
#include "io.h"	/*Io IoSz IoRet IoHead IoHeader
*					  ioSz ioOpen ioClose ioToBuff ioFromBuff*/
const IoSz ioSz(FILE *io) {
	fseek(io, 0L, SEEK_END);
	IoSz inputSz = ftell(io);
	fseek(io, 0L, SEEK_SET);
	return inputSz;
}
void ioSetBuff(Io *io, void *buff, IoSz buffSz) {
	io->buff_ = buff;
	io->buff = &io->buff_;
	io->buffSz = buffSz;
	io->buffMax = buffSz;
}
const IoRet ioOpen(Io *io, const char *restrict fPath, const char *restrict mode /*= "ro"*/) {
	ioSetBuff(io, NULL, 0);
	io->io = fopen(fPath, mode);
	if(NULL == io->io) {
		printf("\nError: NULL == fopen(\"%s\", \"%s\")\n", fPath, mode);
		return -1;
	}
	io->ioSz = ioSz(io->io);
	return 0;
}
const IoRet ioClose(Io *io) {
	IoRet ioRet = 0;
	if(NULL != io->buff_) {
		free(io->buff_);
		io->buff_ = NULL;
	}
	if(NULL != io->buff && NULL != *io->buff) {
		free(*io->buff);
	}
	io->buff = NULL;
	if(0 < io->io) {
		fclose(io->io);
	} else {
		ioRet = -1;
	}
	io->io = 0;
	return ioRet;
}
const IoRet ioToBuff(Io *io, IoSz rSz, long offset /*= 0L*/, int whence /*= SEEK_CUR*/) {
	void *buff = realloc(*io->buff, rSz);
	if(NULL == buff) {
		printf("\nError: `NULL == realloc(%p, %u)`\n", *io->buff, rSz);
		return -2;
	}
	*io->buff = buff;
	io->buffMax = rSz;
	fseek(io->io, offset, whence);
	io->buffSz = fread(buff, 1, rSz, io->io);
	if(rSz != io->buffSz) {
		printf("\nError: `%u == fread(%p, 1, %u, %p)`\n", io->buffSz, buff, rSz, io->io);
		return -1;
	}
	return 0;
}
const IoRet ioFromBuff(Io *io, IoSz wSz, long offset /*= 0L*/, int whence /*= SEEK_CUR*/) {
	const void *buff = io->buff_;
	if(NULL != io->buff && NULL != *io->buff) {
		buff = *io->buff;
	}
	if(NULL == buff) {
		printf("\nError: `NULL == *%p->buff`\n", io);
		return -2;
	}
	if(wSz > io->buffSz) {
		printf("\nError: `ioToBuff(%p, %u, %lu, %i)` but `%u == *%p->buffSz`\n", io, wSz, offset, whence, io->buffSz, io);
		return -2;
	}
	fseek(io->io, offset, whence);
	IoSz ioSz = fwrite(buff, 1, wSz, io->io);
	if(wSz != ioSz) {
		printf("\nError: `%u == fwrite(%p, 1, %u, %p)`\n", ioSz, buff, wSz, io->io);
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
	ioRet = ioOpen(&io, fPath, "a+");
	if(0 < io.ioSz) {
		printf("\nError: `%s` exists, plus `0 < ioSz`\n", fPath);
		ioClose(&io);
		return ioRet;
	}
	if(0 > ioRet) {
		printf("\nError: `%i == ioOpen(%p, %s, %s)`\n", ioRet, &io, fPath, "a+");
		goto exit;
	}
	ioSetBuff(&io, &fromBuff, fromBuffSz);
	ioRet = ioFromBuff(&io, io.buffSz, 0, SEEK_SET);
	if(0 > ioRet) {
		printf("\nError: `%i == ioFromBuff(%p, %u, 0, SEEK_SET)`\n", ioRet, &io, io.buffSz);
		goto exit;
	}
	if(fromBuffSz != io.ioSz) {
		printf("\nError: `0 == ioToBuff(%p, %u, 0, SEEK_SET)`, but `%u == %p->ioSz`.\n", &io, fromBuffSz, io.ioSz, &io);
		goto exit;
	}
	ioSetBuff(&io, NULL, 0);
	ioRet = ioToBuff(&io, io.ioSz, 0, SEEK_SET);
	if(0 > ioRet) {
		printf("\nError: `%i == ioToBuff(%p, %u, 0, SEEK_SET)`\n", ioRet, &io, fromBuffSz);
		goto exit;
	}
	toBuff = io.buff_;
	if(fromBuff != *toBuff) {
		printf("\nError: wrote %u but read %u`\n", fromBuff, *toBuff);
		goto exit;
	}
exit:
	ioClose(&io);
	ioRet = ioOpen(&io, fPath, "w");
	if(0 > ioRet) {
		printf("\nError: `%i == ioOpen(%p, %s, %s)`\n", ioRet, &io, fPath, "w");
		return ioRet;
	}
	if(0 < io.ioSz) {
		printf("\nError: `fopen(`%s`,\"w\")` but `0 < ioSz`\n", fPath);
		ioRet = -1;
	}
	ioClose(&io);
	return ioRet;
}

