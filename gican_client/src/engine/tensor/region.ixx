module;

#include "zlc/zlibcomplete.hpp"

#include <unordered_map>
#include <fstream>

export module engine.tensor:region;

import :chunk;
import util;

export namespace engine::tensor {
    /*
    
    HEADER FORMAT
    0 u64 -     REGION DATA VERSION
    8 u64 -     CHUNK DATA VERSION
    16 u64 -    VOXEL DATA VERSION
    24 array(REGION_VOLUME) {
        SegmentPos  CHUNK_POS
    }
    x y -       FREE SPACE FOR CHUNK ALLOCATION

    */
    // A RAM representation of an existing region file for fast incremental disk editing
    template <IsVoxel Voxel>
    class Region {
    public:
        // Change every time a major file-based change occurs to the Region struct.
        // This variable will be smashed together with the other data versions of Chunk and Voxel to make a unique tag.
        static constexpr std::size_t DATA_VERSION = 0;

        // The character used to delimit the numbers in their vec3 position.
        static constexpr char FILE_NAME_POS_DELIMITER = '-';

        // The size of each segment chunk the file system should use when reading and writing.
        static constexpr std::size_t FSTREAM_BUFFER_SIZE = 5e6; // 5 megs. magic number, change however

        /// @warning DATA_VERSION should increment if these change:
            // #bytes after file header where a chunk segment can reside
            using SegmentPos = std::uint32_t;
            using SegmentSize = std::uint32_t;

            static constexpr std::size_t REGION_LENGTH = 32; // in chunks
            static constexpr std::size_t REGION_VOLUME = REGION_LENGTH * REGION_LENGTH * REGION_LENGTH;

            // init_new_region_file() hardcodes '0xFF' for every byte, so changing this should change that logic.
            static constexpr SegmentPos INVALID_SEGMENT_POS = std::numeric_limits<SegmentPos>::max();

        // Size of the DATA_VERSION header.
        static constexpr std::size_t DATA_VERSION_HEADER_SIZE = sizeof(DATA_VERSION) + sizeof(Chunk<Voxel>::DATA_VERSION) + sizeof(Voxel::DATA_VERSION);
        
        // Size of the chunk location tracker.
        static constexpr std::size_t REGION_HEADER_SIZE = REGION_VOLUME * sizeof(SegmentPos);

        // The total general size of the file header before chunk data is written.
        static constexpr std::size_t FILE_HEADER_SIZE = DATA_VERSION_HEADER_SIZE + REGION_HEADER_SIZE; 
 
        Region(std::string region_directory, util::Vec3I region_pos)
            : region_pos(region_pos) {
                stream.open(region_directory + '/' + region_pos.to_string(FILE_NAME_POS_DELIMITER), std::ios::in | std::ios::out | std::ios::binary);

                if (stream.peek() == std::fstream::traits_type::eof())
                    init_new_region_file();
            }

        const util::Vec3I region_pos;

        // This map memoizes chunk_index finds so the file only has to be read from once.
        // Key: location in file of pointer to chunk segment location, Value: actual chunk segment location
        std::unordered_map<std::size_t, SegmentPos> memoized_chunk_locations;

        // If the file has completely empty, set it up to conform with the current standard.
        void init_new_region_file() {
            std::byte segment[FILE_HEADER_SIZE];

            std::byte* ptr = segment;

            std::memcpy(ptr, &DATA_VERSION, sizeof(DATA_VERSION)); ptr += sizeof(DATA_VERSION);
            std::memcpy(ptr, &Chunk<Voxel>::DATA_VERSION, sizeof(Chunk<Voxel>::DATA_VERSION)); ptr += sizeof(Chunk<Voxel>::DATA_VERSION);
            std::memcpy(ptr, &Voxel::DATA_VERSION, sizeof(Voxel::DATA_VERSION)); ptr += sizeof(Voxel::DATA_VERSION);

            // uninitialized SegmentPos should be the highest value possible. 
            std::memset(ptr, 0xFF, REGION_VOLUME * sizeof(SegmentPos));

            /// @warning ENDIAN
            stream.write(segment, sizeof(segment));
        }

        // Does not determine if the given index is garbage.
        SegmentPos find_segment_pos(util::Vec3U chunk_pos) {
            // Location in the file of the tag that points to the chunk segment's location.
            SegmentPos segment_pos = util::cube_pos_to_index<std::uint64_t, SegmentPos, REGION_LENGTH>(chunk_pos);
        
            // First attempt to access a memoized copy, then try to read from the file.
            auto memoized_location_itr = memoized_chunk_locations.find(segment_pos);

            if (memoized_location_itr != memoized_chunk_locations.end())
                return memoized_location_itr->second;

            // Default to finding the location from directly in the file.
            stream.seekg(segment_pos);
                SegmentPos pos = 0;
                /// @warning ENDIAN
                stream.write(reinterpret_cast<char*>(pos), sizeof(pos));
                return pos;
        };

        SegmentPos alloc_segment(std::size_t chunk_index, SegmentSize segment_size) {
            // get existing chunk segment location if any
            // 
        }

        /* u32 - #bytes of chunk for quick access - n - bytes chunk takes up  */
        // Writes a chunk into an appropriately sized segment.
        void write_chunk(util::Vec3U chunk_pos, Chunk<Voxel>& chunk) {
            std::string serialized = chunk.serialize();
            SegmentSize new_size = serialized.length();
            std::string length_indicator;

            /// @todo: to_append needs to include 4 bytes in the beginning stating how long the chunk of memory is

            std::string to_append = compressor.compress(serialized);

            SegmentPos index = find_segment_pos(chunk_pos);

            if (index == INVALID_SEGMENT_POS)
                index = alloc_segment(chunk_pos, new_size);

            stream.seekp(index);
            stream.write(to_append.data(), to_append.length());

            /// @todo move pointer to appropriate header position to update where the chunk is located
        }

        void load_chunk(util::Vec3U chunk_pos, Chunk<Voxel>& chunk) {
            SegmentPos index = find_segment_pos(chunk_pos);
            stream.seekg(index);

            // read first n bits to get segment length.
            /// @warning ENDIAN
            SegmentSize segment_length;
            stream.read(reinterpret_cast<char*>(&segment_length), sizeof(segment_length));

            std::string compressed;
            stream.read(compressed.data(), segment_length);

            std::string decompressed = decompressor.decompress(compressed);
            chunk.deserialize(decompressed);
        }

    private:
        std::fstream stream;

        zlibcomplete::ZLibCompressor compressor;
        zlibcomplete::ZLibDecompressor decompressor;
    };
}