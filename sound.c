#include <stdio.h> /*FILE fopen fclose fseek ftell fread*/
#include <malloc.h> /*malloc free*/
//#include <types.h>
#include "sound.h" /*ioSz headerWAVE headerRIFF*/
//static char *input = "input.wav";
IoSz ioSz(FILE *io) {
	fseek(io, 0L, SEEK_END);
	IoSz inputSz = ftell(io);
	fseek(io, 0L, SEEK_SET);
	return inputSz;
}
IoRet headerWAVE(FILE *io, IoSz inputSz, MicrosoftRiff *ioRiff, uint8_t **inputBuff) {
	printf(" == 'WAVE'; Microsoft (WAV) sound.\n");
	MicrosoftAudio AudioFormat;
        MicrosoftWave ioWave;
	IoRet ioRet;
	ioRet = fread(&ioWave, sizeof(ioWave), 1, io);
	if(-1 == ioRet) {
		printf("\n-1 == fread(&ioWave, sizeof(ioWave), 1, %p)\n", io);
		return -1;
	}
	if(subchunk1ID !=  ioWave.Subchunk1ID) {
		printf("\n%x == wave.Subchunk1ID, should == 'fmt '\n", ioWave.Subchunk1ID);
		return -1;
	}
	printf(" %u == wave.Subchunk1Size, ", ioWave.Subchunk1Size);
	printf(" %u == wave.AudioFormat", ioWave.AudioFormat);
	switch(ioWave.AudioFormat) {
	case MicrosoftAudioPCM:
		printf("; PCM.");
		break;
	default:
		printf("; unknown.");
		return -1;
	}
	printf(" %u == wave.NumChannels, ", ioWave.NumChannels);
	printf(" %u == wave.SampleRate, ", ioWave.SampleRate);
	printf(" %u == wave.ByteRate, ", ioWave.ByteRate);
	printf(" %u == wave.BlockAlign, ", ioWave.BlockAlign);
	printf(" %u == wave.BitsPerSample, ", ioWave.BitsPerSample);
	switch(ioWave.AudioFormat) {
	case MicrosoftAudioPCM:
		if(subchunk2ID !=  ioWave.pcm.Subchunk2ID) {
			printf("\n%x == wave.Subchunk2ID, should == 'data'\n", ioWave.pcm.Subchunk2ID);
			return -1;
		}
		printf(" %u == wave.Subchunk2Size, ", ioWave.pcm.Subchunk2Size);
		break;
	default:
		printf(" %u == wave.ExtraParamSize, ", ioWave.ExtraParamSize);
	}
	return 0;
}
IoRet headerRIFF(FILE *io, IoSz inputSz, uint8_t **inputBuff) {
	printf(" == 'RIFF'; Microsoft RIFF.");
        MicrosoftRiff ioRiff;
	IoRet ioRet;
	ioRet = fread(&ioRiff, sizeof(MicrosoftRiff), 1, io);
	if(-1 == ioRet) {
		printf("\n-1 == fread(&ioRiff, sizeof(ioRiff), 1, %p)\n", io);
		return -1;
	}
	printf(" %u == buffSz, ", ioRiff.chunkSz);
	printf("0x%x == format", ioRiff.format);
	switch(ioRiff.format) {
	case 'EVAW':	//'WAVE'
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
		printf("\n-1 == fread(buffer, %u, 1, %p)\n", inputSz, io);
		free(*inputBuff);
		*inputBuff = NULL;
		return -1;
	}
	return 0;
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
	case 'FFIR':	//'RIFF'
		headerRIFF(io, inputSz, &inputBuff);
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
