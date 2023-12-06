/*Allow: GPLv2.0, BSD, C.Commons, or unlicensed*/
/*Purpose: demo for sound/audio,
 * should have just took an hour to do,
 * and would go on to construct sound/audio,
 * but failed to parse small sample .wav inputs,
 * so must wav format was misunderstood somehow.*/
#include <stdio.h>	/*size_t FILE*/
#include <ctype.h>	/*uint16_t uint32_t*/
typedef unsigned IoSz;
typedef int IoRet;
typedef uint32_t IoHead;
typedef enum IoHeader : uint32_t {
	ioHeaderRiff = 'FFIR',		/*'RIFF'*/
	ioHeaderRifx = 'RIFX'		/*'RIFX' big-endian*/
} IoHeader;
typedef enum RiffFormat : uint32_t {
	riffFormatWave = 'EVAW',	/*'WAVE'*/
	riffFormatList = 'TSIL',	/*'LIST'*/
	riffFormatIsft = 'fvaL',	/*'Lavf'*/
	riffFormatJunk = 0			/*''*/
} RiffFormat;
typedef struct MicrosoftRiff {
	IoHeader header;			/*'RIFF'*/
	uint32_t chunkSz;
	RiffFormat format;			/*'WAVE'*/
} MicrosoftRiff;
typedef enum SubchunkId : uint32_t {
	subchunkIdWave = ' tmf',	/*'fmt '*/
	subchunkIdPcm = 'atad'		/*'data'*/
} SubchunkId;
typedef enum SubchunkSz : uint32_t {
	subchunkSzPcm = 16,
	subchunkSzList = 18,
	subchunkSzIsft = 12,
	subchunkSzJunk = 1016
} SubchunkSz;
typedef enum MicrosoftAudio : uint16_t {
	microsoftAudioPcm = 1
} MicrosoftAudio;
typedef struct MicrosoftPcm {
	SubchunkId subchunk2Id;		/*'data'*/
	SubchunkSz subchunk2Sz;
} MicrosoftPcm;
typedef struct MicrosoftWave {
	SubchunkId subchunk1Id;		/*'fmt '*/
	SubchunkSz subchunk1Sz;
	MicrosoftAudio audioFormat;
	uint16_t numChannels;
	uint32_t samplePs;
	uint32_t bytePs;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	union {
		uint16_t extraParamSz;	/*+ char [ExtraParamSz]ExtraParams, not for PCM*/
		MicrosoftPcm pcm;
	};
} MicrosoftWave;
typedef struct MicrosoftRiffList {
	SubchunkId subchunk2Id;		/*'LIST'*/
	SubchunkSz subchunk2Sz;		/*18*/
	SubchunkId subchunk1Id;		/*'fmt '*/
	SubchunkSz subchunk1Sz;
	MicrosoftAudio audioFormat;
} MicrosoftRiffList;
const IoSz ioSz(FILE *io);
const IoRet headerPcm(FILE *io, IoSz inputSz, const MicrosoftWave *ioWave, uint8_t **inputBuff);
const IoRet headerWave(FILE *io, IoSz inputSz, const MicrosoftRiff *ioRiff, uint8_t **inputBuff);
const IoRet headerRiff(FILE *io, IoSz inputSz, uint8_t **inputBuff);
const IoRet headerRifx(FILE *io, IoSz inputSz, uint8_t **inputBuff);
const IoRet main(int argc, char **argv);

