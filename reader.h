#ifdef __WAND__
target[name[reader.h] type[include]]
#endif

#ifndef WAVEIO_READER_H
#define WAVEIO_READER_H

#include "sample.h"
#include "riffwave_reader.h"
#include <cstddef>
#include <cstdint>

namespace WaveIO
	{
	class Reader
		{
		public:
			Reader(Herbs::StreamIn& source,Streaminfo& info):
				reader(source)
				{reader.infoRead(info);}

			size_t read(sample_t* buffer,size_t n_samples)
				{return reader.read(buffer,n_samples);}

			uintmax_t samplesCountGet() const
				{return reader.samplesCountGet();}

		private:
			RiffwaveReader reader;
		};
	}

#endif
