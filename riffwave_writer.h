#ifdef __WAND__
target[name[riffwave_writer.h] type[include]]
dependency[riffwave_writer.o]
#endif

#ifndef WAVEIO_RIFFWAVEWRITER_H
#define WAVEIO_RIFFWAVEWRITER_H

#include "sample.h"
#include <cstddef>
#include <cstdint>

namespace Herbs
	{
	class StreamOut;
	}

namespace WaveIO
	{
	struct Streaminfo;

	class RiffwaveWriter
		{
		public:
			RiffwaveWriter(Herbs::StreamOut& dest);
			void infoWrite(const Streaminfo& info);
			size_t write(const sample_t* samples,size_t length);

			~RiffwaveWriter();

		private:
			uintmax_t length_written;
			Herbs::StreamOut& m_dest;
			uint32_t header_size;
			uint32_t channel_count;

			bool chunkIDWrite(const char* header_name);
			bool chunkSizeWrite(uint32_t size);
			size_t dataWrite(const void* buffer,size_t buffer_size);
		};
	}

#endif
