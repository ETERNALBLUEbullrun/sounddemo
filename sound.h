/*Allow: GPLv2.0, BSD, C.Commons, or unlicensed*/
/*Purpose: demo for sound/audio,
 * should have just took an hour to do,
 * and would go on to construct sound/audio,
 * but failed to parse small sample .wav inputs,
 * so must wav format was misunderstood somehow.*/
#ifndef SOUND_H_
#define SOUND_H_
#include "io.h"		/*IoRet Io*/
const IoRet headerPcm(Io *io);
const IoRet headerWave(Io *io);
const IoRet headerRiff(Io *io);
const IoRet headerRifx(Io *io);
const IoRet main(int argc, char **argv);
#endif /*ndef SOUND_H_*/

