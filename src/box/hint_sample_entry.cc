#include <bitstream/opstream.h>
#include <mpeg-isobase/box/hint_sample_entry.h>
#include <mpeg-profiler/box/hint_sample_entry.h>


namespace mpeg {
namespace profiler {


void HintSampleEntry::operator >> (isobase::HintSampleEntry &box) const {
    SampleEntry::operator >> (box);
}

void HintSampleEntry::event(const bitstream::Parser::Event::Header &event) {
    SampleEntry::event(event);
    const auto &box = cast_header<isobase::HintSampleEntry>(event);
}

void HintSampleEntry::operator >> (OPrintStream &stream) const {
    SampleEntry::operator >> (stream);
}


}} // namespace mpeg::profiler

