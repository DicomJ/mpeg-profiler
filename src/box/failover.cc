#include <bitstream/opstream.h>
#include <mpeg-profiler/box/failover.h>


namespace mpeg {
namespace profiler {


void FailoverBox::operator >> (OPrintStream &stream) const {
    Box::output_caption(stream, false);
    OPrintStream::Indent indent(stream, std::string(4,' ')); {
        stream.iout() << ">>> ERROR: Profiler has failed to handle this box!!!";
    }
}


}} // namespace mpeg::profiler

