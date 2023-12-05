#include <stdio.h> /*size_t FILE fopen fclose fseek ftell fread*/
#include <stdlib.h> /*malloc free*/
#include <ctype.h> /*uint16_t uint32_t*/
#include "sound.h" /*IoSz IoRet IoHead IoHeader RiffFormat MicrosoftRiff Subchunk1ID Subchunk1Size Subchunk2ID MicrosoftPCM MicrosoftWAVE ioSz headerPCM headerWAVE headerRIFF headerRIFX*/
/*static char *input = "input.wav";*/
IoSz ioSz(FILE *io) {
	fseek(io, 0L, SEEK_END);
	IoSz inputSz = ftell(io);
	fseek(io, 0L, SEEK_SET);
	return inputSz;
}
IoRet headerPCM(FILE *io, IoSz inputSz, const MicrosoftWave *ioWave, uint8_t **inputBuff) {
	printf(" %u == wave.subchunk2Size, ", ioWave->pcm.subchunk2Size);
	if(subchunk1SizePCM !=  ioWave->subchunk1Size) {
		printf("\n%u == wave.subchunk1ID, should == 16 for PCM.\n", ioWave->subchunk1Size);
		return -1;
	}
	if(subchunk2ID != ioWave->pcm.subchunk2ID) {
		printf("\n%x == wave.subchunk2ID, should == 'data'.\n", ioWave->pcm.subchunk2ID);
		return -1;
	}
	return 0;
}
IoRet headerWAVE(FILE *io, IoSz inputSz, const MicrosoftRiff *ioRiff, uint8_t **inputBuff) {
	printf(" == 'WAVE'; Microsoft (WAV) sound.\n");
        MicrosoftWave ioWave;
	IoRet ioRet;
	ioRet = fread(&ioWave, sizeof(ioWave), 1, io);
	if(-1 == ioRet) {
		printf("\n-1 == fread(&ioWave, sizeof(ioWave), 1, %p)\n", io);
		return -1;
	}
	if(subchunk1ID !=  ioWave.subchunk1ID) {
		printf("\n%x == wave.subchunk1ID, should == 'fmt '.\n", ioWave.subchunk1ID);
		return -1;
	}
	printf("%u == wave.subchunk1Size", ioWave.subchunk1Size);
	/*printf(" %u == wave.audioFormat", ioWave.audioFormat);*/
	printf(", %u == wave.numChannels", ioWave.numChannels);
	printf(", %u == wave.sampleRate", ioWave.sampleRate);
	printf(", %u == wave.byteRate", ioWave.byteRate);
	printf(", %u == wave.blockAlign", ioWave.blockAlign);
	printf(", %u == wave.bitsPerSample", ioWave.bitsPerSample);
	printf(", %u == wave.audioFormat", ioWave.audioFormat);
	switch(ioWave.audioFormat) {
	case microsoftAudioPCM:
		printf("; PCM.");
		return headerPCM(io, inputSz, &ioWave, inputBuff);
	default:
		printf("; unknown.");
		printf(" %u == wave.extraParamSize, ", ioWave.extraParamSize);
		return -1;
	}
	return 0;
}
IoRet headerRIFF(FILE *io, IoSz inputSz, uint8_t **inputBuff) {
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
	case riffFormatWAVE:	/*'WAVE'*/
		ioRet = headerWAVE(io, inputSz, &ioRiff, inputBuff);
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
IoRet headerRIFX(FILE *io, IoSz inputSz, uint8_t **inputBuff) {
	printf(" == 'RIFX'; Microsoft RIFX big-endian, TODO.\n");
	return -1;
}
IoRet main(int argc, char **argv) {
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
	case ioHeaderRIFF:	/*'RIFF'*/
		headerRIFF(io, inputSz, &inputBuff);
	break;
	case ioHeaderRIFX:	/*'RIFX'*/
		headerRIFX(io, inputSz, &inputBuff);
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

