#include <bitstream/opstream.h>
#include <mpeg-isobase/box/data_entry_urn.h>
#include <mpeg-profiler/box/data_entry_urn.h>


namespace mpeg {
namespace profiler {


void DataEntryUrn::event(const bitstream::Parser::Event::Header &event) {
    FullBox::event(event);
    const auto &box = cast_header<isobase::DataEntryUrnBox>(event);

    if (box.flags & isobase::DataEntryUrnBox::SameFileFlag) {
        location = box.location;
        name = box.name;
    }
}

void DataEntryUrn::operator >> (isobase::DataEntryUrnBox &box) const {
}

void DataEntryUrn::operator >> (OPrintStream &stream) const {
}


}} // namespace mpeg::profiler

