#ifdef __WAND__
target[name[audioclip.h] type[include]]
dependency[audioclip.o]
#endif

#ifndef WAVEIO_AUDIOCLIP_H
#define WAVEIO_AUDIOCLIP_H

#include "sample.h"
#include "streaminfo.h"
#include <herbs/matrixstorage/matrixstorage.h>

namespace Herbs
	{
	class StreamIn;
	class StreamOut;
	}

namespace WaveIO
	{
	class AudioClip
		{
		public:
			AudioClip(Herbs::StreamIn& source);
			AudioClip(const sample_t* data,size_t length,uint32_t fs);

			bool store(Herbs::StreamOut& dest);
			bool load(Herbs::StreamIn& source);

			sample_t* dataGet(size_t channel)
				{return data.rowGet(channel);}

			const sample_t* dataGet(size_t channel) const
				{return data.rowGet(channel);}

			uint32_t sampleRateGet() const
				{return info.sample_rate;}

			uint32_t channelMaskGet() const
				{return info.channel_mask;}

			size_t channelCountGet() const
				{return data.nRowsGet();}

			size_t lengthGet() const
				{return data.nColsGet();}

		private:
			Herbs::MatrixStorage<sample_t> data;
			Streaminfo info;
		};
	}

#endif
