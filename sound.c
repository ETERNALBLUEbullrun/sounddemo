#include <stdio.h>	/*size_t*/
#include <stdbool.h>/*bool*/
#include <errno.h>	/*errno*/
#include <ctype.h>	/*uint16_t uint32_t*/
#include <limits.h>	/*CHAR_BIT*/
#include "sound.h"	/*headerPcm headerWave headerRiff headerRifx*/
#include "MsRiff.h"	/*RiffFormat MicrosoftRiff SubchunkId SubchunkSz MicrosoftPcm MicrosoftWave MsHead MsHeader*/
#include "io.h"		/*IO_CAST_TO Io IoSz IoRet ioTest ioLoad ioUnload ioToBuff*/
const IoRet headerPcm(Io *io) {
	const MicrosoftWave *ioWave = *io->buff;
	IoRet ioRet = 0;
	if(subchunkSzPcm != ioWave->subchunk1Sz) {
		printf("\nError: (%u == MicrosoftWave.subchunk1Sz), but PCM should: (%u == MicrosoftWave.subchunk1Sz).\n", ioWave->subchunk1Sz, subchunkSzPcm);
		ioRet = -1;
	}
	if(subchunkIdPcm != ioWave->pcm.subchunk2Id) {
		printf("\nError: (0x%x == MicrosoftWave.subchunk2Id), but PCM should: ('data' == 0x%x == MicrosoftWave.subchunk2Id)).\n", ioWave->pcm.subchunk2Id, subchunkIdPcm);
		ioRet = -1;
	}
	printf(" (%u == MicrosoftWave.subchunk2Sz), ", ioWave->pcm.subchunk2Sz);
	return ioRet;
}
const IoRet headerWave(Io *io) {
	const MicrosoftWave *ioWave = NULL;
	IoRet ioRet = 0;
	printf(" == 'WAVE'; Microsoft (WAV) sound.\n");
	if(0 > (ioRet = ioToBuff(io, sizeof(*ioWave), 0L, SEEK_SET))) {
		printf("\nError: (%i == ioToBuff(%p, sizeof(*ioWave), 0L, SEEK_SET))\n", ioRet, io);
		return ioRet;
	}
	ioWave = *io->buff;
	if(subchunkIdWave != ioWave->subchunk1Id) {
		printf("\nError: (0x%x == MicrosoftWave.subchunk1Id), but WAVE should: ('fmt ' == 0x%x == MicrosoftWave.subchunk1Id)).\n", ioWave->subchunk1Id, subchunkIdWave);
		return -1;
	}
	printf("(%u == MicrosoftWave.subchunk1Sz)", ioWave->subchunk1Sz);
	/*printf(" (%u == MicrosoftWave.AudioFormat)", ioWave->audioFormat);*/
	printf(", (%u == MicrosoftWave.numChannels)", ioWave->numChannels);
	printf(", (%u == MicrosoftWave.samplePs)", ioWave->samplePs);
	printf(", (%u == MicrosoftWave.bytePs)", ioWave->bytePs);
	printf(", (%u == MicrosoftWave.blockAlign)", ioWave->blockAlign);
	printf(", (%u == MicrosoftWave.bitsPerSample)", ioWave->bitsPerSample);
	{
		const IoSz bitPs = ioWave->numChannels * ioWave->samplePs * ioWave->bitsPerSample / CHAR_BIT;
		if(bitPs != ioWave->bytePs) {
			printf("\nError: (bitPs == %u != MicrosoftWave.bytePs * CHAR_BIT).\n", bitPs);
			return -1;
		}
	}
	printf(", (%u == MicrosoftWave.audioFormat)", ioWave->audioFormat);
	switch(ioWave->audioFormat) {
	case microsoftAudioPcm:
		printf("; PCM.");
		return headerPcm(io);
	default:
		printf("; unknown.");
		printf(" (%u == MicrosoftWave.extraParamSz), ", ioWave->extraParamSz);
		return -1;
	}
	return 0;
}
const IoRet headerRiff(Io *io) {
	const MicrosoftRiff *ioRiff = NULL;
	IoRet ioRet = 0;
	printf(" == 'RIFF'; Microsoft RIFF.");
	if(0 > (ioRet = ioToBuff(io, sizeof(*ioRiff), 0L, SEEK_SET))) {
		printf("\nError: (%i == ioToBuff(%p, sizeof(*ioRiff), 0L, SEEK_SET))\n", ioRet, io);
		return -1;
	}
	ioRiff = *io->buff;
	printf(" (%u == MicrosoftRiff.buffSz), ", ioRiff->chunkSz);
	printf("(0x%x == MicrosoftRiff.format)", ioRiff->format);
	switch(ioRiff->format) {
	case riffFormatWave:	/*'WAVE'*/
		ioRet = headerWave(io);
	break;
	default:
		printf("; unknown (Error).\n");
		return -1;
	}
	if(0 > ioRet) {
		return ioRet;
	}
	ioRet = ioToBuff(io, io->ioSz, 0L, SEEK_SET);
	if(0 > ioRet) {
		printf("\nError: (%i == ioToBuff(%p, %u, 0L, SEEK_SET))\n", ioRet, io, io->ioSz);
		return ioRet;
	}
	return ioRet;
}
const IoRet headerRifx(Io *io) {
	printf(" == 'RIFX'; Microsoft RIFX big-endian, TODO (error).\n");
	return -1;
}
const IoRet main(int argc, char **argv) {
	const MsHead *msHead = NULL;
	const bool inputMalloc = (2 != argc);
	char *input = NULL;
	Io io;
	IoRet ioRet = 0;
	if(0 > (ioRet = ioTest("out.swp"))) {
		printf("Error: (%i == ioTest(\"out.swp\"))", ioRet);
		return ioRet;
	}
	if(inputMalloc) {
		printf("Should run '%s input.wav'. Input audio: ", argv[0]);
		size_t inputSz = 0;
		ioRet = getline(&input, &inputSz, stdin);
		if(2 > ioRet) {
			printf("Error: No input. (getline, %i == errno)\n", errno);
			return -1;
		}
		input[ioRet - 1] = '\0';
	} else {
		input = argv[1];
	}
	printf("(\"%s\" == fPath)", input);
	ioRet = ioLoad(&io, input, "ro");
	if(0 > ioRet) {
		printf("\nError: (%i == ioLoad(%p, \"%s\", \"ro\"))\n", ioRet, &io, input);
		goto exit;
	}
	printf(", (%u == fPath.sz)", io.ioSz);
	ioRet = ioToBuff(&io, sizeof(*msHead), 0L, SEEK_SET);
	msHead = *io.buff;
	if(0 > ioRet) {
		printf("\nError: (%i == ioToBuff(%p, sizeof(*msHead), 0L, SEEK_SET))\n", ioRet, &io);
		goto exit;
	}
	printf(", (0x%x == fPath.head)", *msHead);
	switch(*msHead) {
	case msHeaderRiff:	/*'RIFF'*/
		ioRet = headerRiff(&io);
	break;
	case msHeaderRifx:	/*'RIFX'*/
		ioRet = headerRifx(&io);
	break;
	default:
		printf("; unknown (error).\n");
		ioRet = -1;
	}
	if(0 <= ioRet) {
		if(40<io.ioSz) io.ioSz=40;
		for(IoSz i = 0; io.ioSz > i; ++i) {
			printf("0x%x ", (IoSz)IO_CAST_TO(&io, char *)[i]);
		}
	}
exit:
	if(inputMalloc) {
		free(input);
	}
	ioUnload(&io);
	return ioRet;
}

