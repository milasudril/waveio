#ifdef __WAND__
target[name[riffwave_reader.h] type[include]]
dependency[riffwave_reader.o]
#endif

#ifndef WAVEIO_RIFFWAVEREADER_H
#define WAVEIO_RIFFWAVEREADER_H

namespace Herbs
	{
	class StreamIn;

	template<class T>
	class Stringbase;
	}

#include "sample.h"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace WaveIO
	{
	struct Streaminfo;

	class RiffwaveReader
		{
		public:
			RiffwaveReader(Herbs::StreamIn& source);
			void infoRead(Streaminfo& info);
			size_t read(sample_t* samples,size_t n_samples);
			uintmax_t samplesCountGet() const
				{return length;}

		private:
			typedef void (*Converter)(const void* buffer_src,void* buffer_dest
				,size_t length);

			uintmax_t current_chunk_size;
			uintmax_t size_tot;
			uintmax_t length;
			Herbs::StreamIn& m_source;
			Converter converter;
			std::unique_ptr<uint8_t> m_buffer;
			size_t m_buffer_size;
			uint32_t bit_depth;
			uint32_t flags;


			bool headerRead(Herbs::Stringbase<char>& name);
			void skip();
			size_t dataRead(void* buffer,size_t buffer_size);
		};
	}

#endif
