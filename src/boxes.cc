#include <mpeg-profiler/box_map.h>

#include <mpeg-profiler/box/container_data.h>
#include <mpeg-profiler/box/container_type.h>
#include <mpeg-profiler/box/movie.h>
#include <mpeg-profiler/box/movie_header.h>
#include <mpeg-profiler/box/media.h>
#include <mpeg-profiler/box/media_header.h>
#include <mpeg-profiler/box/media_handler.h>
#include <mpeg-profiler/box/edit_list.h>
#include <mpeg-profiler/box/track.h>
#include <mpeg-profiler/box/track_header.h>
#include <mpeg-profiler/box/media_samples.h>
#include <mpeg-profiler/box/data_entry_url.h>
#include <mpeg-profiler/box/data_entry_urn.h>
#include <mpeg-profiler/box/media_samples_description.h>
#include <mpeg-profiler/box/media_samples_size.h>
#include <mpeg-profiler/box/media_samples_decoding_time.h>
#include <mpeg-profiler/box/media_samples_composition_offset.h>
#include <mpeg-profiler/box/media_samples_chunk_offset.h>
#include <mpeg-profiler/box/media_samples_tochunk.h>
#include <mpeg-profiler/box/media_samples_sync.h>
#include <mpeg-profiler/box/media_samples_dependency_type.h>

#include <mpeg-isobase/box/movie_header.h>
#include <mpeg-isobase/box/track_header.h>
#include <mpeg-isobase/box/media_header.h>
#include <mpeg-isobase/box/handler.h>
#include <mpeg-isobase/box/data_entry_url.h>
#include <mpeg-isobase/box/data_entry_urn.h>
#include <mpeg-isobase/box/time_to_sample.h>
#include <mpeg-isobase/box/composition_offset.h>
#include <mpeg-isobase/box/sample_description.h>
#include <mpeg-isobase/box/sample_size.h>
#include <mpeg-isobase/box/compact_sample_size.h>
#include <mpeg-isobase/box/sample_to_chunk.h>
#include <mpeg-isobase/box/chunk_offset.h>
#include <mpeg-isobase/box/chunk_large_offset.h>
#include <mpeg-isobase/box/sync_sample.h>
#include <mpeg-isobase/box/edit_list.h>
#include <mpeg-isobase/box/sample_dependency_type.h>



namespace mpeg {
namespace profiler {


struct RegisterBoxes: Box::Map {
    RegisterBoxes() {
        add<Container::Type>('ftyp');
        add<Container::Type>('styp');
        add<Container::Data>('mdat');
        add<Movie>('moov');
        add<isobase::MovieHeaderBox, Movie::Header>();
        add<Track>('trak');
        add<isobase::TrackHeaderBox, Track::Header>();
        add<Media>('mdia');
        add<isobase::MediaHeaderBox, Media::Header>();
        add<isobase::HandlerBox, Media::Handler>();
        add<Media::Samples>('stbl');
        add<isobase::SampleDescriptionBox, Media::Samples::Description>();
        add<isobase::SampleSizeBox, Media::Samples::RegularSize>();
        add<isobase::TimeToSampleBox, Media::Samples::DecodingTime>();
        add<isobase::CompositionOffsetBox, Media::Samples::CompositionOffset>();
        add<isobase::ChunkOffsetBox, Media::Samples::ChunkRegularOffset>();
        add<isobase::ChunkLargeOffsetBox, Media::Samples::ChunkLargeOffset>();
        add<isobase::SampleToChunkBox, Media::Samples::ToChunk>();
        add<isobase::SyncSampleBox, Media::Samples::Sync>();
        add<isobase::SampleDependencyTypeBox, Media::Samples::DependencyType>();
        add<isobase::DataEntryUrlBox, DataEntryUrl>();
        add<isobase::DataEntryUrnBox, DataEntryUrn>();
        add<isobase::EditListBox, EditList>();
    }
};


const Box::Map &Box::all_boxes = RegisterBoxes();


}} // namespace mpeg::profiler

