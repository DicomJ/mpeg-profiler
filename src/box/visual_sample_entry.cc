#include <bitstream/opstream.h>
#include <mpeg-isobase/box/visual_sample_entry.h>
#include <mpeg-profiler/box/visual_sample_entry.h>


namespace mpeg {
namespace profiler {


void VisualSampleEntry::operator >> (isobase::VisualSampleEntry &box) const {
    SampleEntry::operator >> (box);

    box.compressorname = compressorname;
    box.width = width;
    box.height = height;
    box.vertresolution = vertresolution;
    box.horizresolution = horizresolution;
    box.frame_count = frame_count;
}

void VisualSampleEntry::event(const bitstream::Parser::Event::Header &event) {
    SampleEntry::event(event);
    const auto &box = cast_header<isobase::VisualSampleEntry>(event);

    compressorname = box.compressorname;
    width = box.width;
    height = box.height;
    vertresolution = box.vertresolution;
    horizresolution = box.horizresolution;
    frame_count = box.frame_count;
}

void VisualSampleEntry::operator >> (OPrintStream &stream) const {
    SampleEntry::operator >> (stream);
    stream.out
        << ", compressor: " << (compressorname.empty() ? "<undetermined>" : compressorname)
        << ", " << width << " x " << height
        << ", resolution: " << double(horizresolution) << " x " << double(vertresolution)
        << ", frames per sample: " << frame_count;
}


}} // namespace mpeg::profiler

