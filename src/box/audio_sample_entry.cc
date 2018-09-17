#include <bitstream/opstream.h>
#include <mpeg-isobase/box/audio_sample_entry.h>
#include <mpeg-profiler/box/audio_sample_entry.h>


namespace mpeg {
namespace profiler {


void AudioSampleEntry::operator >> (isobase::AudioSampleEntry &box) const {
    SampleEntry::operator >> (box);


    box.channelcount = channelcount;
    box.samplesize = samplesize;
    box.samplerate = samplerate;
}

void AudioSampleEntry::event(const bitstream::Parser::Event::Header &event) {
    SampleEntry::event(event);
    const auto &box = cast_header<isobase::AudioSampleEntry>(event);

    channelcount = box.channelcount;
    samplesize = box.samplesize;
    samplerate = box.samplerate;
}

void AudioSampleEntry::operator >> (OPrintStream &stream) const {
    SampleEntry::operator >> (stream);
    stream.out
        << ", channels: " << channelcount
        << ", sample size: " << samplesize
        << ", sample rate: " << double(samplerate);
}


}} // namespace mpeg::profiler

