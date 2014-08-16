#ifdef __WAND__
target[name[waveio-test] type[application]]
#endif

#include "reader.h"
#include "writer.h"
#include "streaminfo.h"
#include <herbs/filein/filein.h>
#include <herbs/fileout/fileout.h>
#include <herbs/array_fixedsize/array_fixedsize.h>


int main()
	{
	try
		{
		WaveIO::Streaminfo info;

		Herbs::FileIn src(Herbs::Path(STR("wavex-f32.wav")));
		WaveIO::Reader reader(src,info);

		Herbs::ArrayFixedsize<WaveIO::sample_t> buffer(reader.samplesCountGet());

		if(reader.read(buffer.begin(),buffer.length())
			!=buffer.length())
			{
			printf("Error\n");
			return -1;
			}

		Herbs::FileOut dest(Herbs::Path(STR("file-out.wav")));
		WaveIO::Writer writer(dest,info);
		writer.write(buffer.begin(),buffer.length());
		}
	catch(...)
		{
		printf("Something got wrong!\n");
		}

	return 0;
	}
