/*Allow: GPLv2.0, BSD, C.Commons, or unlicensed*/
/*Purpose: demo for sound/audio,
 * should have just took an hour to do,
 * and would go on to construct sound/audio,
 * but failed to parse small sample .wav inputs,
 * so must wav format was misunderstood somehow.*/
#include <stdio.h> /*FILE*/
//#include <types.h>
static char *input;
typedef unsigned IoSz;
typedef int IoRet;
typedef uint32_t IoHead;
typedef struct MicrosoftRiff {
	uint32_t header;	//'RIFF'
	uint32_t chunkSz;
	uint32_t format;	//'WAVE'
} MicrosoftRiff;
typedef enum Subchunk1ID : uint32_t {
	subchunk1ID = ' tmf'		/*'fmt '*/
} Subchunk1ID;
typedef enum Subchunk2ID : uint32_t {
	subchunk2ID = 'atad'		/*'data'*/
} Subchunk2ID;
typedef enum MicrosoftAudio : uint16_t {
	MicrosoftAudioPCM = 1
} MicrosoftAudio;
typedef struct MicrosoftPCM {
	Subchunk2ID Subchunk2ID;	//'data'
	uint32_t Subchunk2Size;
} MicrosoftPCM;
typedef struct MicrosoftWave {
	Subchunk1ID Subchunk1ID;	//'fmt '
	uint32_t Subchunk1Size;
	MicrosoftAudio AudioFormat;
	uint16_t NumChannels;
	uint32_t SampleRate;
	uint32_t ByteRate;
	uint16_t BlockAlign;
	uint16_t BitsPerSample;
	union {
		uint16_t ExtraParamSize;	/*Not for PCM*/
/*		[] ExtraParams;*/
		MicrosoftPCM pcm;
	};
} MicrosoftWave;
IoSz ioSz(FILE *io);
IoRet headerWAVE(FILE *io, IoSz inputSz, MicrosoftRiff *ioRiff, uint8_t **inputBuff);
IoRet headerRIFF(FILE *io, IoSz inputSz, uint8_t **inputBuff);
IoRet main(int argc, char **argv);

