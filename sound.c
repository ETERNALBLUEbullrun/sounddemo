#include <stdio.h>	/*size_t*/
#include <ctype.h>	/*uint16_t uint32_t*/
#include <limits.h>	/*CHAR_BIT*/
#include "sound.h"	/*RiffFormat MicrosoftRiff SubchunkId SubchunkSz MicrosoftPcm MicrosoftWave
*					  headerPcm headerWave headerRiff headerRifx*/
#include "io.h"		/*IO_CAST_TO Io IoSz IoRet IoHead IoHeader ioTest ioOpen ioClose ioToBuff*/
const IoRet headerPcm(Io *io) {
	const MicrosoftWave *ioWave = *io->buff;
	printf(" %u == wave.subchunk2Sz, ", ioWave->pcm.subchunk2Sz);
	if(subchunkSzPcm != ioWave->subchunk1Sz) {
		printf("\nError: %u == wave.subchunk1Id, should == 16 for PCM.\n", ioWave->subchunk1Sz);
		return -1;
	}
	if(subchunkIdPcm != ioWave->pcm.subchunk2Id) {
		printf("\nError: %x == wave.subchunk2Id, should == 'data'.\n", ioWave->pcm.subchunk2Id);
		return -1;
	}
	return 0;
}
const IoRet headerWave(Io *io) {
	const MicrosoftWave *ioWave = NULL;
	IoRet ioRet = 0;
	printf(" == 'WAVE'; Microsoft (WAV) sound.\n");
	if(0 > (ioRet = ioToBuff(io, sizeof(*ioWave), 0L, SEEK_SET))) {
		printf("\nError: 0 > ioToBuff(%p, sizeof(*ioWave), 0L, SEEK_SET)\n", io);
		return ioRet;
	}
	ioWave = *io->buff;
	if(subchunkIdWave != ioWave->subchunk1Id) {
		printf("\nError: %x == wave.subchunk1Id, should == 'fmt '.\n", ioWave->subchunk1Id);
		return -1;
	}
	printf("%u == wave.subchunk1Sz", ioWave->subchunk1Sz);
	/*printf(" %u == wave.AudioFormat", ioWave->audioFormat);*/
	printf(", %u == wave.numChannels", ioWave->numChannels);
	printf(", %u == wave.samplePs", ioWave->samplePs);
	printf(", %u == wave.bytePs", ioWave->bytePs);
	printf(", %u == wave.blockAlign", ioWave->blockAlign);
	printf(", %u == wave.bitsPerSample", ioWave->bitsPerSample);
	{
		const IoSz bitPs = ioWave->numChannels * ioWave->samplePs * ioWave->bitsPerSample / CHAR_BIT;
		if(bitPs != ioWave->bytePs) {
			printf("\nError: bitPs == %u != wave.bytePs * CHAR_BIT.\n", bitPs);
			return -1;
		}
	}
	printf(", %u == wave.audioFormat", ioWave->audioFormat);
	switch(ioWave->audioFormat) {
	case microsoftAudioPcm:
		printf("; PCM.");
		return headerPcm(io);
	default:
		printf("; unknown.");
		printf(" %u == wave.extraParamSz, ", ioWave->extraParamSz);
		return -1;
	}
	return 0;
}
const IoRet headerRiff(Io *io) {
	const MicrosoftRiff *ioRiff = NULL;
	IoRet ioRet = 0;
	printf(" == 'RIFF'; Microsoft RIFF.");
	if(0 > (ioRet = ioToBuff(io, sizeof(*ioRiff), 0L, SEEK_SET))) {
		printf("\nError: 0 > ioToBuff(%p, sizeof(*ioRiff), 0L, SEEK_SET)\n", io);
		return -1;;
	}
	ioRiff = *io->buff;
	printf(" %u == buffSz, ", ioRiff->chunkSz);
	printf("0x%x == format", ioRiff->format);
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
		printf("\nError: 0 > ioToBuff(%p, %u, 0L, SEEK_SET)\n", io, io->ioSz);
		return ioRet;
	}
	return ioRet;
}
const IoRet headerRifx(Io *io) {
	printf(" == 'RIFX'; Microsoft RIFX big-endian, TODO (error).\n");
	return -1;
}
const IoRet main(int argc, char **argv) {
	Io io;
	char *input = NULL;
	const IoHead *inputHead = NULL;
	IoRet ioRet = 0;
	if(0 > (ioRet = ioTest("out.swp"))) {
		printf("Error: `%i == ioTest(\"out.swp\")`", ioRet);
	}
	if(2 != argc) {
		printf("Should '%s input.wav'. Input audio: ", argv[0]);
		size_t inputSz = 0;
		ioRet = getline(&input, &inputSz, stdin);
		if(2 > ioRet) {
			printf("Error: No input.");
			return -1;
		}
		input[ioRet - 1] = '\0';
	} else {
		input = argv[1];
	}
	printf("input: \"%s\"", input);
	ioOpen(&io, input, "ro");
	printf(", %u == sz", io.ioSz);
	ioRet = ioToBuff(&io, sizeof(*inputHead), 0L, SEEK_SET);
	inputHead = *io.buff;
	if(0 > ioRet) {
		printf("\nError: 0 > ioToBuff(%p, sizeof(*inputHead), 0L, SEEK_SET)\n", &io);
		goto exit;
	}
	printf(", 0x%x == inputHead", *inputHead);
	switch(*inputHead) {
	case ioHeaderRiff:	/*'RIFF'*/
		ioRet = headerRiff(&io);
	break;
	case ioHeaderRifx:	/*'RIFX'*/
		ioRet = headerRifx(&io);
	break;
	default:
		printf("; unknown (error).\n");
		ioRet = -1;
	}
	if(0 <= ioRet) {
		if(40<io.ioSz) io.ioSz=40;
		for(IoSz i = 0; io.ioSz > i; ++i) {
			printf("%x ", (IoSz)IO_CAST_TO(&io, char *)[i]);
		}
	}
exit:
	ioClose(&io);
	return ioRet;
}

