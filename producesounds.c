#include <stdio.h>	/*size_t scanf*/
#include <stdbool.h>/*bool*/
#include <errno.h>	/*errno*/
#include <ctype.h>	/*uint16_t uint32_t*/
#include <limits.h>	/*CHAR_BIT*/
#include "sound.h"	/*headerPcm headerWave headerRiff headerRifx*/
#include "MsRiff.h"	/*RiffFormat MicrosoftRiff SubchunkId SubchunkSz MicrosoftPcm MicrosoftWave MsHead MsHeader*/
#include "io.h"		/*IO_CAST_TO Io IoSz IoRet ioTest ioLoad ioUnload ioToBuff*/
enum : unsigned {
	bitsPerSampleDefault = 16,
	msHeaderDefault = msHeaderRiff
};
const IoRet headerPcm(Io *io) {
	const MicrosoftWave *ioWave = *io->buff;
	IoRet ioCode = 0;
	if(subchunkSzPcm != ioWave->subchunk1Sz) {
		printf("\nError: (%u == MicrosoftWave.subchunk1Sz), but PCM should: (%u == MicrosoftWave.subchunk1Sz).\n", ioWave->subchunk1Sz, subchunkSzPcm);
		ioCode = -1;
	}
	if(subchunkIdPcm != ioWave->pcm.subchunk2Id) {
		printf("\nError: (0x%x == MicrosoftWave.subchunk2Id), but PCM should: ('data' == 0x%x == MicrosoftWave.subchunk2Id)).\n", ioWave->pcm.subchunk2Id, subchunkIdPcm);
		ioCode = -1;
	}
	printf(" (%u == MicrosoftWave.subchunk2Sz), ", ioWave->pcm.subchunk2Sz);
	return ioCode;
}
const IoRet headerWave(Io *io) {
	const MicrosoftWave *ioWave = NULL;
	MicrosoftWave wave;
	IoRet ioCode = 0;
	printf(" == 'WAVE'; Microsoft (WAV) sound.\n");
	if(0 > (ioCode = ioToBuff(io, sizeof(*ioWave), 0L, SEEK_SET))) {
		printf("\nError: (%i == ioToBuff(%p, sizeof(*ioWave), 0L, SEEK_SET))\n", ioCode, io);
		return ioCode;
		printf("Please enter wave.bitsPerSample: [%u]", bitsPerSampleDefault);
		ioCode = scanf("%hu", &wave.bitsPerSample);
		if(1 != ioCode) {
			wave.bitsPerSample = bitsPerSampleDefault;
		}
		printf("(%u == wave.bitsPerSamplen)", wave.bitsPerSample);
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
	IoRet ioCode = 0;
	printf(" == 'RIFF'; Microsoft RIFF.");
	if(0 > (ioCode = ioToBuff(io, sizeof(*ioRiff), 0L, SEEK_SET))) {
		printf("\nError: (%i == ioToBuff(%p, sizeof(*ioRiff), 0L, SEEK_SET))\n", ioCode, io);
		return -1;
	}
	ioRiff = *io->buff;
	printf(" (%u == MicrosoftRiff.buffSz), ", ioRiff->chunkSz);
	printf("(0x%x == MicrosoftRiff.format)", ioRiff->format);
	switch(ioRiff->format) {
	case riffFormatWave:	/*'WAVE'*/
		ioCode = headerWave(io);
	break;
	default:
		printf("; unknown (Error).\n");
		return -1;
	}
	if(0 > ioCode) {
		return ioCode;
	}
	ioCode = ioToBuff(io, io->ioSz, 0L, SEEK_SET);
	if(0 > ioCode) {
		printf("\nError: (%i == ioToBuff(%p, %u, 0L, SEEK_SET))\n", ioCode, io, io->ioSz);
		return ioCode;
	}
	return ioCode;
}
const IoRet headerRifx(Io *io) {
	printf(" == 'RIFX'; Microsoft RIFX big-endian, TODO (error).\n");
	return -1;
}
const IoRet main(int argc, char **argv) {
	const MsHead *msHead = NULL;
	MsHead msHead_;
	const bool outputMalloc = (2 != argc);
	char *output = NULL;
	Io io;
	IoRet ioCode = 0;
	if(0 > (ioCode = ioTest("out.swp"))) {
		printf("Error: (%i == ioTest(\"out.swp\"))", ioCode);
		return ioCode;
	}
	if(outputMalloc) {
		printf("Should run '%s output.wav'. Output path: ", argv[0]);
		size_t outputSz = 0;
		ioCode = getline(&output, &outputSz, stdin);
		if(2 > ioCode) {
			printf("Error: No output. (getline, %i == errno)\n", errno);
			return -1;
		}
		output[ioCode - 1] = '\0';
	} else {
		output = argv[1];
	}
	printf("(\"%s\" == fPath)", output);
	ioCode = ioLoad(&io, output, "ro");
	if(0 > ioCode || 0 == io.ioSz) {
		ioUnload(&io);
		ioCode = ioLoad(&io, output, "w+");
		if(0 > ioCode) {
			printf("\nError: (%i == ioLoad(%p, \"%s\", \"ro\"))\n", ioCode, &io, output);
			goto exit;
		}
/*		printf("\nError: (%i == ioLoad(%p, \"%s\", \"ro\"))\n", ioCode, &io, output);
		goto exit;*/
		printf("\nPlease enter fPath.head:as hex [0x%x for MS-RIFF]", msHeaderDefault);
		ioCode = scanf("%x", &msHead_);
		if(1 != ioCode) {
			msHead_ = msHeaderDefault;
		}
		ioSetBuff(&io, &msHead_, sizeof(*msHead));
		ioCode = ioFromBuff(&io, sizeof(*msHead), 0L, SEEK_SET);
	return 0;
		if(0 > ioCode) {
			printf("\nError: (%i == ioFromBuff(%p, sizeof(*msHead), 0L, SEEK_SET))\n", ioCode, &io);
			goto exit;
		}
	} else {
		printf(", (%u == fPath.sz)", io.ioSz);
		ioCode = ioToBuff(&io, sizeof(*msHead), 0L, SEEK_SET);
		if(0 > ioCode) {
			printf("\nError: (%i == ioToBuff(%p, sizeof(*msHead), 0L, SEEK_SET))\n", ioCode, &io);
			goto exit;
		}
	}
	msHead = *io.buff;
	printf(", (0x%x == fPath.head)", *msHead);
	switch(*msHead) {
	case msHeaderRiff:	/*'RIFF'*/
		ioCode = headerRiff(&io);
	break;
	case msHeaderRifx:	/*'RIFX'*/
		ioCode = headerRifx(&io);
	break;
	default:
		printf("; unknown (error).\n");
		ioCode = -1;
	}
	if(0 <= ioCode) {
		if(40<io.ioSz) io.ioSz=40;
		for(IoSz i = 0; io.ioSz > i; ++i) {
			printf("0x%x ", (IoSz)IO_CAST_TO(&io, char *)[i]);
		}
	}
exit:
	if(outputMalloc) {
		free(output);
	}
	ioUnload(&io);
	return ioCode;
}

