#include <stdio.h>	/*size_t FILE fopen fclose fseek ftell fread*/
#include <stdlib.h>	/*malloc free*/
#include <ctype.h>	/*uint16_t uint32_t*/
#include <limits.h>	/*CHAR_BIT*/
#include "sound.h"	/*IoSz IoRet IoHead IoHeader RiffFormat MicrosoftRiff SubchunkId SubchunkSz MicrosoftPcm MicrosoftWave ioSz headerPcm headerWave headerRiff headerRifx*/
IoSz ioSz(FILE *io) {
	fseek(io, 0L, SEEK_END);
	IoSz inputSz = ftell(io);
	fseek(io, 0L, SEEK_SET);
	return inputSz;
}
IoRet headerPcm(FILE *io, IoSz inputSz, const MicrosoftWave *ioWave, uint8_t **inputBuff) {
	printf(" %u == wave.subchunk2Sz, ", ioWave->pcm.subchunk2Sz);
	if(subchunkSzPcm != ioWave->subchunk1Sz) {
		printf("\n%u == wave.subchunk1Id, should == 16 for PCM.\n", ioWave->subchunk1Sz);
		return -1;
	}
	if(subchunkIdPcm != ioWave->pcm.subchunk2Id) {
		printf("\n%x == wave.subchunk2Id, should == 'data'.\n", ioWave->pcm.subchunk2Id);
		return -1;
	}
	return 0;
}
IoRet headerWave(FILE *io, IoSz inputSz, const MicrosoftRiff *ioRiff, uint8_t **inputBuff) {
	printf(" == 'WAVE'; Microsoft (WAV) sound.\n");
	MicrosoftWave ioWave;
	IoRet ioRet;
	ioRet = fread(&ioWave, sizeof(ioWave), 1, io);
	if(-1 == ioRet) {
		printf("\n-1 == fread(&ioWave, sizeof(ioWave), 1, %p)\n", io);
		return -1;
	}
	if(subchunkIdWave != ioWave.subchunk1Id) {
		printf("\n%x == wave.subchunk1Id, should == 'fmt '.\n", ioWave.subchunk1Id);
		return -1;
	}
	printf("%u == wave.subchunk1Sz", ioWave.subchunk1Sz);
	/*printf(" %u == wave.AudioFormat", ioWave.audioFormat);*/
	printf(", %u == wave.numChannels", ioWave.numChannels);
	printf(", %u == wave.samplePs", ioWave.samplePs);
	printf(", %u == wave.bytePs", ioWave.bytePs);
	printf(", %u == wave.blockAlign", ioWave.blockAlign);
	printf(", %u == wave.bitsPerSample", ioWave.bitsPerSample);
	const IoSz bitPs = ioWave.numChannels * ioWave.samplePs * ioWave.bitsPerSample / CHAR_BIT;
	if(bitPs != ioWave.bytePs) {
		printf("\nError: bitPs == %u != wave.bytePs * CHAR_BIT.\n", bitPs);
		return -1;
	}
	printf(", %u == wave.audioFormat", ioWave.audioFormat);
	switch(ioWave.audioFormat) {
	case microsoftAudioPcm:
		printf("; PCM.");
		return headerPcm(io, inputSz, &ioWave, inputBuff);
	default:
		printf("; unknown.");
		printf(" %u == wave.extraParamSz, ", ioWave.extraParamSz);
		return -1;
	}
	return 0;
}
IoRet headerRiff(FILE *io, IoSz inputSz, uint8_t **inputBuff) {
	printf(" == 'RIFF'; Microsoft RIFF.");
	MicrosoftRiff ioRiff;
	IoRet ioRet;
	ioRet = fread(&ioRiff, sizeof(ioRiff), 1, io);
	if(-1 == ioRet) {
		printf("\n-1 == fread(&ioRiff, sizeof(ioRiff), 1, %p)\n", io);
		return -1;
	}
	printf(" %u == buffSz, ", ioRiff.chunkSz);
	printf("0x%x == format", ioRiff.format);
	switch(ioRiff.format) {
	case riffFormatWave:	/*'WAVE'*/
		ioRet = headerWave(io, inputSz, &ioRiff, inputBuff);
	break;
	default:
		printf("unknown.\n");
		return -1;
	}
	if(-1 != ioRet) {
		fseek(io, 0L, SEEK_SET);
		ioRet = fread(*inputBuff, inputSz, 1, io);
	}
	if(-1 == ioRet) {
		printf("\n-1 == fread(inputBuff, %u, 1, %p)\n", inputSz, io);
		free(*inputBuff);
		*inputBuff = NULL;
		return -1;
	}
	return 0;
}
IoRet headerRifx(FILE *io, IoSz inputSz, uint8_t **inputBuff) {
	printf(" == 'RIFX'; Microsoft RIFX big-endian, TODO.\n");
	return -1;
}
IoRet main(int argc, char **argv) {
	char *input = NULL;
	if(2 != argc) {
		printf("Should '%s input.wav'. Input audio: ", argv[0]);
		size_t inputSz = 0;
		IoRet ioRet = getline(&input, &inputSz, stdin);
		if(2 > ioRet) {
			printf("Error: No input.");
			return -1;
		}
		input[ioRet - 1] = '\0';
	} else {
		input = argv[1];
	}
	printf("input: \"%s\"", input);
	FILE *io = fopen(input, "ro");
	if(NULL == io) {
		printf("\nNULL == fopen(%s, \"ro\")\n", input);
		return -1;
	}
	IoSz inputSz = ioSz(io);
	printf(", %u == sz", inputSz);
	IoHead inputHead;
	IoRet ioRet = fread(&inputHead, sizeof(inputHead), 1, io);
	fseek(io, 0L, SEEK_SET);
	if(-1 == ioRet) {
		printf("\n-1 == fread(&inputHead, sizeof(inputHead), 1, %p)", io);
		return -1;
	}
	uint8_t *inputBuff = malloc(inputSz);
	if(NULL == inputBuff) {
		printf("NULL == malloc(%u)\n ", inputSz);
		return -1;
	}
	printf(", 0x%x == inputHead", inputHead);
	switch(inputHead) {
	case ioHeaderRiff:	/*'RIFF'*/
		headerRiff(io, inputSz, &inputBuff);
	break;
	case ioHeaderRifx:	/*'RIFX'*/
		headerRifx(io, inputSz, &inputBuff);
	break;
	default:
		printf("; unknown.\n");
		return -1;
	}
	if(NULL == inputBuff) {
		return -1;
	}
if(40<inputSz)inputSz=40;
	for(IoSz i = 0; inputSz > i; ++i) {
		printf("%x ", (IoSz)inputBuff[i]);
	}
	free(inputBuff);
	return 0;
}

