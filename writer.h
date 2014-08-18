#ifdef __WAND__
target[name[writer.h] type[include]]
#endif

#ifndef WAVEIO_WRITER_H
#define WAVEIO_WRITER_H

#include "sample.h"
#include "riffwave_writer.h"
#include <cstddef>
#include <cstdint>

namespace WaveIO
	{
	class Writer
		{
		public:
			Writer(Herbs::StreamOut& dest,const Streaminfo& info):
				writer(dest)
				{writer.infoWrite(info);}

			size_t write(const sample_t* buffer,size_t n_samples)
				{return writer.write(buffer,n_samples);}

		private:
			RiffwaveWriter writer;
		};
	}

#endif
