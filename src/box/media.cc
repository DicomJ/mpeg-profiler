#include <bitstream/opstream.h>
#include <mpeg-profiler/box/media.h>


namespace mpeg {
namespace profiler {


void Media::operator >> (OPrintStream &stream) const  {
    stream.iout(stream.branching) << "Media [";
    stream << (*this)['mdhd'];
    stream.out << "]" << std::endl;

    OPrintStream::Indent indent(stream); {
        stream << (*this)['minf']['stbl'];
    }
}



}} // namespace mpeg::profiler

