#ifdef __WAND__
target[name[audioclip.o] type[object]]
#endif

#include "audioclip.h"
#include "reader.h"
#include "writer.h"
#include <herbs/array_fixedsize/array_fixedsize.h>
#include <herbs/exceptionmissing/exceptionmissing.h>

WaveIO::AudioClip::AudioClip(Herbs::StreamIn& source):data(1,1)
	{
	if(!load(source))
		{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}
	}

WaveIO::AudioClip::AudioClip(const sample_t* data,size_t length,uint32_t fs):
	data(1,length),info{fs,1,SPEAKER_FRONT_CENTER}
	{
	std::copy(data,data+length,dataGet(0));
	}

bool WaveIO::AudioClip::load(Herbs::StreamIn& source)
	{
	Reader reader(source,info);
	auto n_samples=reader.samplesCountGet();

	Herbs::ArrayFixedsize<sample_t> buffer(n_samples);
	if(reader.read(buffer.begin(),buffer.length())!=buffer.length())
		{return 0;}

	data.resize(info.channel_count,n_samples/info.channel_count);
	for(size_t k=0;k<data.nRowsGet();++k)
		{
		auto ptr=dataGet(k);
		auto ptr_end=ptr+data.nColsGet();
		auto src=buffer.begin()+k;
		while(ptr!=ptr_end)
			{
			*ptr=*src;
			src+=data.nRowsGet();
			++ptr;
			}
		}
	return 1;
	}

bool WaveIO::AudioClip::store(Herbs::StreamOut& dest)
	{
	Writer writer(dest,info);
	Herbs::ArrayFixedsize<sample_t> buffer(data.nColsGet()*data.nRowsGet());
	for(size_t k=0;k<data.nRowsGet();++k)
		{
		auto ptr=dataGet(k);
		auto ptr_end=ptr+data.nColsGet();
		auto dst=buffer.begin()+k;
		while(ptr!=ptr_end)
			{
			*dst=*ptr;
			dst+=data.nRowsGet();
			++ptr;
			}
		}
	if(writer.write(buffer.begin(),buffer.length())!=buffer.length())
		{return 0;}
	return 1;
	}
