# **MPEG-PROFILER** - mpeg-isobmf/mp4 profiling library

## Key features

2. Like [MPEG-ISOBASE](https://github.com/DicomJ/mpeg-isobase) library it's totally use-case agnostic. Provides pure MPEG-ISOBMF/MP4 profiling abilities without superfluous functionality;
1. Simplified data model represented as `Container` of `Tracks` with `Media::Samples`;
1. Simplified memory managment what results into a single user faced `std::unique_ptr`. No shared objects (and so responsibilities);
1. Fine-grained access to the `Box` tree;
1. Ultrasmart `Media::Samples::Iterator` facilitates access to the timing and data indexing information of the `Media::Samples` in the `Track` and hides all the complexity of iteration over `stbl` related `Box`es;
1. Seeking abilities with almost O(1) complexity by time in any scale (`mvhd` or `mdhd`) or by time in seconds;
11. Apart and like [MPEG-ISOBASE](https://github.com/DicomJ/mpeg-isobase) it relies only on pure C++ and C++ Standard Library.

## What's is not


It's not a full featured media stream processing solution which does repackaging. Nevertheless, it's very easy to build such solutions based on MPEG-PROFILER and [MPEG-ISOBASE](https://github.com/DicomJ/mpeg-isobase) libraries. For example:

https://github.com/DicomJ/mpeg-packager

## Known limitations:

It implies the same limitations as [MPEG-ISOBASE](https://github.com/DicomJ/mpeg-isobase) library as it's based on it.

## Building

If you would like to check out and build MPEG-PROFILER library, the procedure is as follows (requires modern C++17 compiler):

1. Check out MPEG-PROFILER

    ```bash
    git clone git@github.com:DicomJ/mpeg-profiler.git mpeg-profiler
    ```

1. Build MPEG-PROFILER

    ```bash
    mkdir mpeg-profiler/build
    cd mpeg-profiler/build
    cmake -DCMAKE_BUILD_TYPE=Release -DCXX_STD=c++17 -DCMAKE_INSTALL_PREFIX=$(pwd)/install $(pwd)/../../mpeg-profiler
    make install
    ```

## Usage

### Profiling

All parsing hard work is done by `mpeg::isobase::Parser` of [MPEG-ISOBASE](https://github.com/DicomJ/mpeg-isobase) library. `Profiler` is just an implementation of `Parser::Observer` which handles corresponding parsing events.


```bash
cat ../../mpeg-profiler/demo/dump.cc
```

```C++
bitstream::input::file::Stream stream("sample.mp4");
mpeg::profiler::Profiler profiler;
mpeg::isobase::Parser parser(stream, profiler);

parser.parse();
```

### Container

As the result of profiling `Container` object is built and could be detached as `std::unique_ptr` off the `Profiler` object:

```C++
auto container_ptr = profiler.detach_container();
const auto &container = *container_ptr;
```

`Container` object is a root node of `Box` tree starting from which all other `Box`es could be obtained by the following methods (note that all returned types are weak references/pointers to the objects life time of which is determined by `std::unique_ptr` of detached `Container` object):

* traverse to the required `Box` by the subscript `operator []`:

    ```C++
    const auto &mvhd = container['moov']['mvhd'];
    ```

* obtain `Box` object casted to its type by counterpart `as` method:

    ```C++
    const auto &mvhd = container['moov'].as<Movie::Header>('mvhd');
    ```

* obtain a pointer to the optional `Box` by `get` method. `nullptr` is returned if object is not found:

    ```C++
    assert(container.get('foo ') == nullptr);
    ```

* obtain optional object but casted to its type:

    ```C++
    const Foo *foo = container.get<Foo>('bar ');
    ```

* if there're more than one `Box` `all` method should be used to iterate over them:

    ```C++
    for (const auto &track: container['moov'].all<Track>('trak')) {
        track['mdia'].as<Media::Header>('mdhd');
        // ...
    }
    ```

### Tracks

`Container` object itself has a built in methods to iterate over tracks, even though technically tracks are stored under `moov` `Box`:

```C++
container.tracks(); // all tracks
container.video_tracks();
container.audio_tracks();
```

In its turn `Track` object has handy methods to figure out the type of track:

```C++
track.is_audio();
track.is_video();
```

### Samples

Conceptually `Track` comprises `Media::Samples`, mostly defined by `stbl` `Box`. So `Track` object has corresponding method to obtain the `Media::Samples` object:

```C++
const auto &samples = track.samples();
```

`Media::Samples` object encapsulates all the details of traversing over `stbl` `Box` by exposing very simple `Sample::Iterator` interface.

```C++
assert(samples.count() == (samples.end() - samples.begin()));
for (auto it = samples.begin(); it != samples.end(); ++it) {
    // ...
}
```

`Samples::Iterator` can be obtained at any `Media::Sample` by its index in the following range \[0, `samples.count()`) :

```C++
const auto &it = samples.at(i);
```

Then a `Media::Sample` itself can be obtained by dereferencing `Sample::Iterator`:

```C++
const auto &sample = *it;
```

The `Media::Sample` object provides all the time and data indexing infomation defined by the following interface:

```C++
struct Media::Sample {
    // Timing information
    Time decoding_time() const;
    Time composition_time() const;

    // Offset in bytes within media defined by the DataReferenceBox
    uint64_t offset() const;

    // Size of sample in bytes
    uint32_t size() const;

    // Sample's metadata
    const SampleEntry &sample_entry() const;
    const DependencyType::Entry *dependency() const;    // optional

    // The rest technical indexes and metadata there's no real reason to care about
    ...
}
```

### Seeking

`Media::Samples` object exposes methods which allow to seek by time in `mdhd` or `mvhd` timescales or by time in seconds. As the result index of the `Media::Sample` is returned. The following example demonstrates how to iterate over all `Media::samples` in all `Track`s from 5.7s up to 6.3s:

```C++
for (const auto &track: container.tracks()) {
    const auto begin = samples.seek(5.7), end = samples.seek(6.3);
    auto i = 0;
    for (auto it = samples.at(begin); it != end; ++it) {
        const auto &sample = *it;
        std::cout
            << "#" << std::setw(3) << std::left <<  i++ << ": ["
            << "time: " << sample.decoding_time().seconds() << "s, "
            << "size: " << sample.size() << " bytes, "
            << "offset: " << sample.offset() << " bytes]" << std::endl;
    }
}
```

Note that by default Sync Sample Table is used to locate real random access point (RAP) prior sample determined by given time. What means that unlike in this example and first of all seeking should happen on the video `Track` to determine the time boundaries of the segment aligned by random access points of the video `Track`, and only then do seeking on the rest required audio and other `Track`s which has more fine-grained alignment of RAPs. Checkout the output of this example:

```
soun: 26 samples:
#0  : [time: 5.69s, size: 213 bytes, offset: 392953 bytes]
#1  : [time: 5.71s, size: 206 bytes, offset: 393166 bytes]
#2  : [time: 5.74s, size: 194 bytes, offset: 393372 bytes]
#3  : [time: 5.76s, size: 209 bytes, offset: 393566 bytes]
#4  : [time: 5.78s, size: 201 bytes, offset: 393775 bytes]
...
#21 : [time: 6.18s, size: 206 bytes, offset: 443427 bytes]
#22 : [time: 6.20s, size: 231 bytes, offset: 443633 bytes]
#23 : [time: 6.22s, size: 193 bytes, offset: 443864 bytes]
#24 : [time: 6.25s, size: 202 bytes, offset: 444057 bytes]
#25 : [time: 6.27s, size: 220 bytes, offset: 444259 bytes]
vide: 24 samples:
#0  : [time: 4.99s, size: 22215 bytes, offset: 327198 bytes]
#1  : [time: 5.03s, size: 3488 bytes, offset: 349413 bytes]
#2  : [time: 5.08s, size: 1373 bytes, offset: 352901 bytes]
#3  : [time: 5.12s, size: 1604 bytes, offset: 354274 bytes]
#4  : [time: 5.16s, size: 1707 bytes, offset: 355878 bytes]
...
#18 : [time: 5.74s, size: 2913 bytes, offset: 399832 bytes]
#19 : [time: 5.78s, size: 1939 bytes, offset: 402745 bytes]
#20 : [time: 5.82s, size: 2232 bytes, offset: 404684 bytes]
#21 : [time: 5.87s, size: 2316 bytes, offset: 406916 bytes]
#22 : [time: 5.91s, size: 2383 bytes, offset: 409232 bytes]
#23 : [time: 5.95s, size: 2374 bytes, offset: 411615 bytes]
```

`no_sync=true` could be specified while seeking to avoid alignment on RAPs:

```C++
const auto begin = samples.seek(5.7, no_sync=true), end = samples.seek(6.3, no_sync=true);
```

The result is still the same 26 audio samples, and only 14 video samples, started from and ended with not aligned RAPs in the video `Track`:

```
soun: 26 samples:
#0  : [time: 5.69s, size: 213 bytes, offset: 392953 bytes]
#1  : [time: 5.71s, size: 206 bytes, offset: 393166 bytes]
#2  : [time: 5.74s, size: 194 bytes, offset: 393372 bytes]
#3  : [time: 5.76s, size: 209 bytes, offset: 393566 bytes]
#4  : [time: 5.78s, size: 201 bytes, offset: 393775 bytes]
...
#21 : [time: 6.18s, size: 206 bytes, offset: 443427 bytes]
#22 : [time: 6.20s, size: 231 bytes, offset: 443633 bytes]
#23 : [time: 6.22s, size: 193 bytes, offset: 443864 bytes]
#24 : [time: 6.25s, size: 202 bytes, offset: 444057 bytes]
#25 : [time: 6.27s, size: 220 bytes, offset: 444259 bytes]
vide: 14 samples:
#0  : [time: 5.70s, size: 2889 bytes, offset: 396943 bytes]
#1  : [time: 5.74s, size: 2913 bytes, offset: 399832 bytes]
#2  : [time: 5.78s, size: 1939 bytes, offset: 402745 bytes]
#3  : [time: 5.82s, size: 2232 bytes, offset: 404684 bytes]
#4  : [time: 5.87s, size: 2316 bytes, offset: 406916 bytes]
...
#9  : [time: 6.07s, size: 1094 bytes, offset: 439393 bytes]
#10 : [time: 6.12s, size: 1255 bytes, offset: 440487 bytes]
#11 : [time: 6.16s, size: 1264 bytes, offset: 441742 bytes]
#12 : [time: 6.20s, size: 1308 bytes, offset: 447492 bytes]
#13 : [time: 6.24s, size: 1361 bytes, offset: 448800 bytes]
```

## What else?

1. Entire container can be dumped by the following few lines of code:

    ```bash
    cat ../../mpeg-profiler/demo/dump.cc
    ```

    ```C++
    auto container_ptr = profiler.detach_container();
    const auto &container = *container_ptr;
    bitstream::output::print::to_stdout << container;
    ```

2. Build and run it:

    ```bash
    g++ \
        -o dump \
        -std=c++17 -Wno-multichar \
        -I install/include \
        -Wl,-rpath,$(pwd)/install/lib64 install/lib64/*.so \
        ../../mpeg-profiler/demo/dump.cc
    time ./dump sample.mp4
    ```

3. Check out the output

    ```
    |  Container: [major_brand: mp42, minor_version: 1, compatible_brands: [mp42, avc1]]
    |  Data: [size: 5229792 bytes in 2 'mdat' boxes]
    +- Movie [duration: 36048/600=60.08s, created: Wed Mar 16 10:41:51 2011, modified: Wed Mar 16 10:42:48 2011]
    |  +- Track(soun) [duration: 36048/600=60.08s, created: Wed Mar 16 10:41:53 2011, modified: Wed Mar 16 10:42:48 2011]
    |  |  +- Edit List: [duration: 36048/600=60.08s, time: 0/44100=0.00s, rate: 1.00]
    |  |  +- Media [duration: 2652160/44100=60.14s, language: English, created: Wed Mar 16 10:41:53 2011, modified: Wed Mar 16 10:42:48 2011]
    |  |  |  +- Samples [
    |  |  |  |      Decoding times: [duration: 2652160/44100=60.14s, 2590 samples (in 1 entry)],
    |  |  |  |      Descriptions: [1 entry,  mp4a, reference: [#0: <same file>], channels: 2, sample size: 16, sample rate: 44100.00],
    |  |  |  |      Sizes: [2590 samples, total size: 470957 bytes (9.01%)],
    |  |  |  |      Samples to chunks: [2590 samples (in 236 runs)],
    |  |  |  |      Chunks (offsets): [236 chunks],
    |  |  |  |  ]
    |  |  |  |  #0:
    |  |  |  |      size: 4 bytes
    |  |  |  |      decoding: [#0: time: 0/44100=0.00s]
    |  |  |  |      composition: [offset: 0/44100=0.00s, time: 0/44100=0.00s]
    |  |  |  |      chunk: [#0: first_index: 0, index: 0, offset: 44558 bytes]
    |  |  |  |      offset: [44558 bytes, #0 samples in #0 chunck of 14 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: mp4a, reference: [#0: <same file>], channels: 2, sample size: 16, sample rate: 44100.00]
    |  |  |  |  #1:
    |  |  |  |      size: 56 bytes
    |  |  |  |      decoding: [#0: time: 1024/44100=0.02s]
    |  |  |  |      composition: [offset: 0/44100=0.00s, time: 1024/44100=0.02s]
    |  |  |  |      chunk: [#0: first_index: 0, index: 0, offset: 44558 bytes]
    |  |  |  |      offset: [44562 bytes, #1 samples in #0 chunck of 14 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: mp4a, reference: [#0: <same file>], channels: 2, sample size: 16, sample rate: 44100.00]
    |  |  |  |  #2:
    |  |  |  |      size: 200 bytes
    |  |  |  |      decoding: [#0: time: 2048/44100=0.05s]
    |  |  |  |      composition: [offset: 0/44100=0.00s, time: 2048/44100=0.05s]
    |  |  |  |      chunk: [#0: first_index: 0, index: 0, offset: 44558 bytes]
    |  |  |  |      offset: [44618 bytes, #2 samples in #0 chunck of 14 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: mp4a, reference: [#0: <same file>], channels: 2, sample size: 16, sample rate: 44100.00]
    |  |  |  |  #3:
    |  |  |  |      size: 264 bytes
    |  |  |  |      decoding: [#0: time: 3072/44100=0.07s]
    |  |  |  |      composition: [offset: 0/44100=0.00s, time: 3072/44100=0.07s]
    |  |  |  |      chunk: [#0: first_index: 0, index: 0, offset: 44558 bytes]
    |  |  |  |      offset: [44818 bytes, #3 samples in #0 chunck of 14 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: mp4a, reference: [#0: <same file>], channels: 2, sample size: 16, sample rate: 44100.00]
    |  |  |  |  #4:
    |  |  |  |      size: 221 bytes
    |  |  |  |      decoding: [#0: time: 4096/44100=0.09s]
    |  |  |  |      composition: [offset: 0/44100=0.00s, time: 4096/44100=0.09s]
    |  |  |  |      chunk: [#0: first_index: 0, index: 0, offset: 44558 bytes]
    |  |  |  |      offset: [45082 bytes, #4 samples in #0 chunck of 14 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: mp4a, reference: [#0: <same file>], channels: 2, sample size: 16, sample rate: 44100.00]
    |  |  |  |
    |  |  |  |  ...
    |  |  |  |
    |  |  |  |  #2585:
    |  |  |  |      size: 143 bytes
    |  |  |  |      decoding: [#0: time: 2647040/44100=60.02s]
    |  |  |  |      composition: [offset: 0/44100=0.00s, time: 2647040/44100=60.02s]
    |  |  |  |      chunk: [#234: first_index: 234, index: 234, offset: 5170386 bytes]
    |  |  |  |      offset: [5172369 bytes, #11 samples in #0 chunck of 13 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: mp4a, reference: [#0: <same file>], channels: 2, sample size: 16, sample rate: 44100.00]
    |  |  |  |  #2586:
    |  |  |  |      size: 61 bytes
    |  |  |  |      decoding: [#0: time: 2648064/44100=60.05s]
    |  |  |  |      composition: [offset: 0/44100=0.00s, time: 2648064/44100=60.05s]
    |  |  |  |      chunk: [#234: first_index: 234, index: 234, offset: 5170386 bytes]
    |  |  |  |      offset: [5172512 bytes, #12 samples in #0 chunck of 13 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: mp4a, reference: [#0: <same file>], channels: 2, sample size: 16, sample rate: 44100.00]
    |  |  |  |  #2587:
    |  |  |  |      size: 4 bytes
    |  |  |  |      decoding: [#0: time: 2649088/44100=60.07s]
    |  |  |  |      composition: [offset: 0/44100=0.00s, time: 2649088/44100=60.07s]
    |  |  |  |      chunk: [#235: first_index: 235, index: 235, offset: 5172573 bytes]
    |  |  |  |      offset: [5172573 bytes, #0 samples in #0 chunck of 3 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: mp4a, reference: [#0: <same file>], channels: 2, sample size: 16, sample rate: 44100.00]
    |  |  |  |  #2588:
    |  |  |  |      size: 4 bytes
    |  |  |  |      decoding: [#0: time: 2650112/44100=60.09s]
    |  |  |  |      composition: [offset: 0/44100=0.00s, time: 2650112/44100=60.09s]
    |  |  |  |      chunk: [#235: first_index: 235, index: 235, offset: 5172573 bytes]
    |  |  |  |      offset: [5172577 bytes, #1 samples in #0 chunck of 3 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: mp4a, reference: [#0: <same file>], channels: 2, sample size: 16, sample rate: 44100.00]
    |  |  |  |  #2589:
    |  |  |  |      size: 4 bytes
    |  |  |  |      decoding: [#0: time: 2651136/44100=60.12s]
    |  |  |  |      composition: [offset: 0/44100=0.00s, time: 2651136/44100=60.12s]
    |  |  |  |      chunk: [#235: first_index: 235, index: 235, offset: 5172573 bytes]
    |  |  |  |      offset: [5172581 bytes, #2 samples in #0 chunck of 3 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: mp4a, reference: [#0: <same file>], channels: 2, sample size: 16, sample rate: 44100.00]
    |  +- Track(vide) [duration: 36048/600=60.08s, resolution: 640.00x360.00, created: Wed Mar 16 10:41:52 2011, modified: Wed Mar 16 10:42:48 2011]
    |  |  +- Edit List: [duration: 36048/600=60.08s, time: 0/2500=0.00s, rate: 1.00]
    |  |  +- Media [duration: 150280/2500=60.11s, language: English, created: Wed Mar 16 10:41:52 2011, modified: Wed Mar 16 10:42:48 2011]
    |  |  |  +- Samples [
    |  |  |  |      Decoding times: [duration: 150280/2500=60.11s, 1445 samples (in 1 entry)],
    |  |  |  |      Descriptions: [1 entry,  avc1, reference: [#0: <same file>], compressor: <undetermined>, 640 x 360, resolution: 72.00 x 72.00, frames per sample: 1],
    |  |  |  |      Sizes: [1445 samples, total size: 4758819 bytes (90.99%)],
    |  |  |  |      Samples to chunks: [1445 samples (in 11 runs)],
    |  |  |  |      Chunks (offsets): [119 chunks],
    |  |  |  |      Sync samples: [61 entries],
    |  |  |  |  ]
    |  |  |  |  #0:
    |  |  |  |      size: 28627 bytes
    |  |  |  |      decoding: [#0: time: 0/2500=0.00s]
    |  |  |  |      composition: [offset: 0/2500=0.00s, time: 0/2500=0.00s]
    |  |  |  |      chunk: [#0: first_index: 0, index: 0, offset: 40 bytes]
    |  |  |  |      offset: [40 bytes, #0 samples in #0 chunck of 5 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: avc1, reference: [#0: <same file>], compressor: <undetermined>, 640 x 360, resolution: 72.00 x 72.00, frames per sample: 1]
    |  |  |  |  #1:
    |  |  |  |      size: 12533 bytes
    |  |  |  |      decoding: [#0: time: 104/2500=0.04s]
    |  |  |  |      composition: [offset: 0/2500=0.00s, time: 104/2500=0.04s]
    |  |  |  |      chunk: [#0: first_index: 0, index: 0, offset: 40 bytes]
    |  |  |  |      offset: [28667 bytes, #1 samples in #0 chunck of 5 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: avc1, reference: [#0: <same file>], compressor: <undetermined>, 640 x 360, resolution: 72.00 x 72.00, frames per sample: 1]
    |  |  |  |  #2:
    |  |  |  |      size: 1239 bytes
    |  |  |  |      decoding: [#0: time: 208/2500=0.08s]
    |  |  |  |      composition: [offset: 0/2500=0.00s, time: 208/2500=0.08s]
    |  |  |  |      chunk: [#0: first_index: 0, index: 0, offset: 40 bytes]
    |  |  |  |      offset: [41200 bytes, #2 samples in #0 chunck of 5 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: avc1, reference: [#0: <same file>], compressor: <undetermined>, 640 x 360, resolution: 72.00 x 72.00, frames per sample: 1]
    |  |  |  |  #3:
    |  |  |  |      size: 1024 bytes
    |  |  |  |      decoding: [#0: time: 312/2500=0.12s]
    |  |  |  |      composition: [offset: 0/2500=0.00s, time: 312/2500=0.12s]
    |  |  |  |      chunk: [#0: first_index: 0, index: 0, offset: 40 bytes]
    |  |  |  |      offset: [42439 bytes, #3 samples in #0 chunck of 5 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: avc1, reference: [#0: <same file>], compressor: <undetermined>, 640 x 360, resolution: 72.00 x 72.00, frames per sample: 1]
    |  |  |  |  #4:
    |  |  |  |      size: 1095 bytes
    |  |  |  |      decoding: [#0: time: 416/2500=0.17s]
    |  |  |  |      composition: [offset: 0/2500=0.00s, time: 416/2500=0.17s]
    |  |  |  |      chunk: [#0: first_index: 0, index: 0, offset: 40 bytes]
    |  |  |  |      offset: [43463 bytes, #4 samples in #0 chunck of 5 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: avc1, reference: [#0: <same file>], compressor: <undetermined>, 640 x 360, resolution: 72.00 x 72.00, frames per sample: 1]
    |  |  |  |
    |  |  |  |  ...
    |  |  |  |
    |  |  |  |  #1440:
    |  |  |  |      size: 34173 bytes
    |  |  |  |      decoding: [#0: time: 149760/2500=59.90s]
    |  |  |  |      composition: [offset: 0/2500=0.00s, time: 149760/2500=59.90s]
    |  |  |  |      chunk: [#10: first_index: 118, index: 118, offset: 5172585 bytes]
    |  |  |  |      offset: [5188518 bytes, #5 samples in #0 chunck of 10 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: avc1, reference: [#0: <same file>], compressor: <undetermined>, 640 x 360, resolution: 72.00 x 72.00, frames per sample: 1]
    |  |  |  |  #1441:
    |  |  |  |      size: 1573 bytes
    |  |  |  |      decoding: [#0: time: 149864/2500=59.95s]
    |  |  |  |      composition: [offset: 0/2500=0.00s, time: 149864/2500=59.95s]
    |  |  |  |      chunk: [#10: first_index: 118, index: 118, offset: 5172585 bytes]
    |  |  |  |      offset: [5222691 bytes, #6 samples in #0 chunck of 10 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: avc1, reference: [#0: <same file>], compressor: <undetermined>, 640 x 360, resolution: 72.00 x 72.00, frames per sample: 1]
    |  |  |  |  #1442:
    |  |  |  |      size: 3000 bytes
    |  |  |  |      decoding: [#0: time: 149968/2500=59.99s]
    |  |  |  |      composition: [offset: 0/2500=0.00s, time: 149968/2500=59.99s]
    |  |  |  |      chunk: [#10: first_index: 118, index: 118, offset: 5172585 bytes]
    |  |  |  |      offset: [5224264 bytes, #7 samples in #0 chunck of 10 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: avc1, reference: [#0: <same file>], compressor: <undetermined>, 640 x 360, resolution: 72.00 x 72.00, frames per sample: 1]
    |  |  |  |  #1443:
    |  |  |  |      size: 1588 bytes
    |  |  |  |      decoding: [#0: time: 150072/2500=60.03s]
    |  |  |  |      composition: [offset: 0/2500=0.00s, time: 150072/2500=60.03s]
    |  |  |  |      chunk: [#10: first_index: 118, index: 118, offset: 5172585 bytes]
    |  |  |  |      offset: [5227264 bytes, #8 samples in #0 chunck of 10 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: avc1, reference: [#0: <same file>], compressor: <undetermined>, 640 x 360, resolution: 72.00 x 72.00, frames per sample: 1]
    |  |  |  |  #1444:
    |  |  |  |      size: 964 bytes
    |  |  |  |      decoding: [#0: time: 150176/2500=60.07s]
    |  |  |  |      composition: [offset: 0/2500=0.00s, time: 150176/2500=60.07s]
    |  |  |  |      chunk: [#10: first_index: 118, index: 118, offset: 5172585 bytes]
    |  |  |  |      offset: [5228852 bytes, #9 samples in #0 chunck of 10 sampleses in run of 1 chunk]
    |  |  |  |      description: [#0: avc1, reference: [#0: <same file>], compressor: <undetermined>, 640 x 360, resolution: 72.00 x 72.00, frames per sample: 1]
    ```

