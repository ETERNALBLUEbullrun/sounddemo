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
	ioHeaderRiff = 'FFIR',		/*'RIFF'*/
	ioHeaderRifx = 'RIFX'		/*'RIFX' big-endian*/
} IoHeader;
typedef enum RiffFormat : uint32_t {
	riffFormatWave = 'EVAW'		/*'WAVE'*/
} RiffFormat;
typedef struct MicrosoftRiff {
	IoHeader header;		/*'RIFF'*/
	uint32_t chunkSz;
	RiffFormat format;		/*'WAVE'*/
} MicrosoftRiff;
typedef enum SubchunkId : uint32_t {
	subchunkIdWave = ' tmf',	/*'fmt '*/
	subchunkIdPcm = 'atad'		/*'data'*/
} SubchunkId;
typedef enum SubchunkSize : uint32_t {
	subchunkSizePcm = 16
} SubchunkSize;
typedef enum MicrosoftAudio : uint16_t {
	microsoftAudioPcm = 1
} MicrosoftAudio;
typedef struct MicrosoftPcm {
	SubchunkId subchunk2Id;		/*'data'*/
	SubchunkSize subchunk2Size;
} MicrosoftPcm;
typedef struct MicrosoftWave {
	SubchunkId subchunk1Id;		/*'fmt '*/
	SubchunkSize subchunk1Size;
	MicrosoftAudio audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	union {
		uint16_t extraParamSize;	/*+ char [ExtraParamSize]ExtraParams, not for PCM*/
		MicrosoftPcm pcm;
	};
} MicrosoftWave;
IoSz ioSz(FILE *io);
IoRet headerPcm(FILE *io, IoSz inputSz, const MicrosoftWave *ioWave, uint8_t **inputBuff);
IoRet headerWave(FILE *io, IoSz inputSz, const MicrosoftRiff *ioRiff, uint8_t **inputBuff);
IoRet headerRiff(FILE *io, IoSz inputSz, uint8_t **inputBuff);
IoRet headerRifx(FILE *io, IoSz inputSz, uint8_t **inputBuff);
IoRet main(int argc, char **argv);

