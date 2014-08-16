#ifdef __WAND__
target[name[streaminfo.h] type[include]]
#endif

#ifndef WAVEIO_STREAMINFO_H
#define WAVEIO_STREAMINFO_H

#include <cstdint>

namespace WaveIO
	{
	const uint32_t SPEAKER_FRONT_LEFT=0x1;
	const uint32_t SPEAKER_FRONT_RIGHT=0x2;
	const uint32_t SPEAKER_FRONT_CENTER=0x4;
	const uint32_t SPEAKER_LOW_FREQUENCY=0x8;
	const uint32_t SPEAKER_BACK_LEFT=0x10;
	const uint32_t SPEAKER_BACK_RIGHT=0x20;
	const uint32_t SPEAKER_FRONT_LEFT_OF_CENTER=0x40;
	const uint32_t SPEAKER_FRONT_RIGHT_OF_CENTER=0x80;
	const uint32_t SPEAKER_BACK_CENTER=0x100;
	const uint32_t SPEAKER_SIDE_LEFT=0x200;
	const uint32_t SPEAKER_SIDE_RIGHT=0x400;
	const uint32_t SPEAKER_TOP_CENTER=0x800;
	const uint32_t SPEAKER_TOP_FRONT_LEFT=0x1000;
	const uint32_t SPEAKER_TOP_FRONT_CENTER=0x2000;
	const uint32_t SPEAKER_TOP_FRONT_RIGHT=0x4000;
	const uint32_t SPEAKER_TOP_BACK_LEFT=0x8000;
	const uint32_t SPEAKER_TOP_BACK_CENTER=0x10000;
	const uint32_t SPEAKER_TOP_BACK_RIGHT=0x20000;
	const uint32_t SPEAKER_STEREO_LEFT=0x40000000;
	const uint32_t SPEAKER_STEREO_RIGHT=0x80000000;

	struct Streaminfo
		{
		uint32_t sample_rate;
		uint32_t channel_count;
		uint32_t channel_mask;
		};
	}

#endif
