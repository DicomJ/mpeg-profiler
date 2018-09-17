#include <bitstream/opstream.h>
#include <bitstream/sstream.h>
#include <mpeg-isobase/box/sample_entry.h>
#include <mpeg-profiler/profiler.h>
#include <mpeg-profiler/box/sample_entry.h>
#include <mpeg-profiler/box/data_entry.h>


namespace mpeg {
namespace profiler {


using bitstream::SStream;
using bitstream::output::print::fourcc;


void SampleEntry::operator >> (isobase::SampleEntry &box) const {
    box.data_reference_index = data_reference_index + 1;
}

void SampleEntry::event(const bitstream::Parser::Event::Header &event) {
    Box::event(event);
    const auto &box = cast_header<isobase::SampleEntry>(event);

    data_reference_index = box.data_reference_index;
    if (0 == data_reference_index--) {
        throw Profiler::Exception("data_reference_index=0 cloud larger or equal to 1");
    }
}

void SampleEntry::seal() {
    const Box &dref = parent.parent.parent['dinf']['dref'];
    if (data_reference_index >= dref.all().size()) {
        throw Profiler::Exception(SStream()
                << "data_reference_index=" << data_reference_index + 1 << " value is larger tha number of " << dref.all().size());
    }
    _data_entry = &dref.all<DataEntry>()[data_reference_index];
}

void SampleEntry::operator >> (OPrintStream &stream) const {
    assert(is_sealed()); // has never been sealed
    stream.out
        << " " << fourcc(type)
        << ", reference: [#"
        << data_reference_index << ": "
        << (_data_entry->location.empty() ? "<same file>" : _data_entry->location) << "]";
}


}} // namespace mpeg::profiler

