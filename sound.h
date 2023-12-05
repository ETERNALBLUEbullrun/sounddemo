/*Allow: GPLv2.0, BSD, C.Commons, or unlicensed*/
/*Purpose: demo for sound/audio,
 * should have just took an hour to do,
 * and would go on to construct sound/audio,
 * but failed to parse small sample .wav inputs,
 * so must wav format was misunderstood somehow.*/
#include <stdio.h> /*size_t FILE*/
#include <ctype.h> /*uint16_t uint32_t*/
static char *input;
typedef unsigned IoSz;
typedef int IoRet;
typedef uint32_t IoHead;
typedef enum IoHeader : uint32_t {
	ioHeaderRIFF = 'FFIR',		/*'RIFF'*/
	ioHeaderRIFX = 'RIFX'		/*'RIFX' big-endian*/
} IoHeader;
typedef enum RiffFormat : uint32_t {
	riffFormatWAVE = 'EVAW'		/*'WAVE'*/
} RiffFormat;
typedef struct MicrosoftRiff {
	IoHeader header;		/*'RIFF'*/
	uint32_t chunkSz;
	RiffFormat format;		/*'WAVE'*/
} MicrosoftRiff;
typedef enum Subchunk1ID : uint32_t {
	subchunk1ID = ' tmf'		/*'fmt '*/
} Subchunk1ID;
typedef enum Subchunk1Size : uint32_t {
	subchunk1SizePCM = 16
} Subchunk1Size;
typedef enum MicrosoftAudio : uint16_t {
	microsoftAudioPCM = 1
} MicrosoftAudio;
typedef enum Subchunk2ID : uint32_t {
	subchunk2ID = 'atad'		/*'data'*/
} Subchunk2ID;
typedef struct MicrosoftPCM {
	Subchunk2ID subchunk2ID;	/*'data'*/
	uint32_t subchunk2Size;
} MicrosoftPCM;
typedef struct MicrosoftWave {
	Subchunk1ID subchunk1ID;	/*'fmt '*/
	uint32_t subchunk1Size;
	MicrosoftAudio audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	union {
		uint16_t extraParamSize;	/*+ char [ExtraParamSize]ExtraParams, not for PCM*/
		MicrosoftPCM pcm;
	};
} MicrosoftWave;
IoSz ioSz(FILE *io);
IoRet headerPCM(FILE *io, IoSz inputSz, const MicrosoftWave *ioWave, uint8_t **inputBuff);
IoRet headerWAVE(FILE *io, IoSz inputSz, const MicrosoftRiff *ioRiff, uint8_t **inputBuff);
IoRet headerRIFF(FILE *io, IoSz inputSz, uint8_t **inputBuff);
IoRet headerRIFX(FILE *io, IoSz inputSz, uint8_t **inputBuff);
IoRet main(int argc, char **argv);

