module;

#include <unordered_map>
#include <fstream>
#include <iosfwd>

export module engine.tensor:region;

import :chunk;
import util;
import zstd;

export namespace engine::tensor {
    /*
    
    HEADER FORMAT
    0 u64 -     REGION DATA VERSION
    8 u64 -     CHUNK DATA VERSION
    16 u64 -    VOXEL DATA VERSION
    24 array(REGION_VOLUME) {
        SegmentPos  CHUNK_POS
    }
    x array(n) {
        u32 -   SIZE OF COMPRESSED CHUNK SEGMENT
        n -     COMPRESSED CHUNK SEGMENT
    }

    */

    // A query-based reader/writer to any region file that allows incremental disk chunk editing.
    // Includes automatic caching to prevent the overuse of file reads.
    template <IsVoxel Voxel>
    class RegionFileManager {
    public:
        // Change every time a major file-based change occurs to the Region struct.
        // This variable will be smashed together with the other data versions of Chunk and Voxel to make a unique tag.
        static constexpr std::size_t DATA_VERSION = 0;

        // The character used to delimit the numbers in their vec3 position.
        static constexpr char FILE_NAME_POS_DELIMITER = '-';

        /// @warning DATA_VERSION should increment if these change:
            static constexpr std::size_t REGION_LENGTH = 32; // in chunks
            static constexpr std::size_t REGION_VOLUME = REGION_LENGTH * REGION_LENGTH * REGION_LENGTH;
            static constexpr zstd::compress_level_t CHUNK_COMPRESS_LEVEL = 5;

            // #bytes after file header where a chunk segment can reside
            using SegmentPos = std::uint64_t;
            using SegmentSize = std::uint32_t;

            // position of the tag in the header that contains a SegmentPos
            using SegmentTagPos = std::size_t;

            // init_new_region_file() hardcodes '0xFF' for every byte, so changing this should change that logic.
            static constexpr SegmentPos INVALID_SEGMENT_POS = std::numeric_limits<SegmentPos>::max();

        // Size of the DATA_VERSION header.
        static constexpr std::size_t DATA_VERSION_HEADER_SIZE = sizeof(DATA_VERSION) + sizeof(Chunk<Voxel>::DATA_VERSION) + sizeof(Voxel::DATA_VERSION);
            static constexpr std::size_t SEGMENT_POSITIONS_HEADER_POS = DATA_VERSION_HEADER_SIZE;

        // Size of the chunk location tracker.
        static constexpr std::size_t SEGMENT_POSITIONS_HEADER_SIZE = REGION_VOLUME * sizeof(SegmentPos);
            static constexpr std::size_t SEGMENT_SPACE_POS = DATA_VERSION_HEADER_SIZE + SEGMENT_POSITIONS_HEADER_SIZE;

        // The total general size of the file header before chunk data is written.
        static constexpr std::size_t FILE_HEADER_SIZE = DATA_VERSION_HEADER_SIZE + SEGMENT_POSITIONS_HEADER_SIZE;
 
        RegionFileManager(std::string region_directory, util::Vec3I region_pos)
            : region_pos(region_pos), region_file_name(region_directory + '/' + region_pos.to_string(FILE_NAME_POS_DELIMITER))
        {
            stream.open(region_file_name, std::ios::in | std::ios::out | std::ios::binary);
            
            if (!stream || !stream.is_open() || stream.bad())
                util::panic("Failed to load region from disk. There is currently no retry mechanism.");

            if (stream.peek() == std::fstream::traits_type::eof())
                init_new_region_file();

            stream.seekg(0, std::ios::end);
            file_end_pos = stream.tellg();
        }

        const util::Vec3I region_pos;
        const std::string region_file_name;
        //
        //
        //


        void compact_file() {
            /// @todo
            // this is optimization based, having no content here will only make the file slowly grow over time but not "break" the system
        }

        //
        //
        //

        // Writes a chunk into an appropriately sized segment.
        void write_chunk(util::Vec3U chunk_pos, Chunk<Voxel>& chunk) {
            zstd::buffer_t buffer = chunk.serialize();
            zstd::buffer_t serialized;

            zstd::inplace::compress(buffer, serialized, CHUNK_COMPRESS_LEVEL);
            
            SegmentTagPos segment_tag_pos = get_segment_tag_pos(chunk_pos);
            write_segment(segment_tag_pos, serialized);
        }

        Chunk<Voxel> load_chunk(util::Vec3U chunk_pos) {
            SegmentTagPos segment_tag_pos = get_segment_tag_pos(chunk_pos);
            
            zstd::buffer_t compressed = read_segment(segment_tag_pos);
            zstd::buffer_t buffer;

            zstd::inplace::decompress(compressed, buffer);

            return Chunk<Voxel>(buffer);
        }

    private:
        struct RegionCache {
            // This map memoizes chunk_index finds so the file only has to be read from once.
            // Key: location in file of pointer to chunk segment location, Value: actual chunk segment location
            std::unordered_map<SegmentTagPos, SegmentPos> segment_tag_destinations;

            std::unordered_map<SegmentPos, SegmentSize> segment_sizes;
        };

        std::streampos file_end_pos;
        std::fstream stream;

        RegionCache cache;

        void test_badbit() {
            util::panic_assert(!stream.bad(), "There was a fatal memory error while managing region file \"" + region_file_name + "\". There is no recovery method set by the developer yet.");   
        }

        // If the file has completely empty, set it up to conform with the current standard.
        void init_new_region_file() {
            std::byte segment[FILE_HEADER_SIZE];

            std::byte* ptr = segment;

            // DATA_VERSION HEADER
            std::memcpy(ptr, &DATA_VERSION, sizeof(DATA_VERSION)); ptr += sizeof(DATA_VERSION);
            std::memcpy(ptr, &Chunk<Voxel>::DATA_VERSION, sizeof(Chunk<Voxel>::DATA_VERSION)); ptr += sizeof(Chunk<Voxel>::DATA_VERSION);
            std::memcpy(ptr, &Voxel::DATA_VERSION, sizeof(Voxel::DATA_VERSION)); ptr += sizeof(Voxel::DATA_VERSION);

            // SEGMENT POSITION HEADER

            // uninitialized SegmentPos should just be 11111111
            std::memset(ptr, 0xFF, REGION_VOLUME * sizeof(SegmentPos));

            /// @warning ENDIAN
            stream.write(segment, sizeof(segment));
            test_badbit();
        }

        //
        //
        //

        void extend_file_end_pos(std::streampos amount) {
            file_end_pos += amount;
            stream.seekp(file_end_pos - static_cast<std::streamsize>(1));
            char empty_byte = 0x00;
            stream.write(&empty_byte, 1);
        }

        //
        //
        //

        [[nodiscard]]
        SegmentTagPos get_segment_tag_pos(util::Vec3U chunk_pos) {
            return util::cube_pos_to_index<REGION_LENGTH>(chunk_pos) * sizeof(SegmentPos) + SEGMENT_POSITIONS_HEADER_POS;
        }

        [[nodiscard]]
        SegmentPos read_segment_tag(SegmentTagPos segment_tag_pos) {
            // First attempt to access a memoized copy, then try to read from the file.
            auto potentially_cached_itr = cache.segment_tag_destinations.find(segment_tag_pos);

            if (potentially_cached_itr != cache.segment_tag_destinations.end())
                return potentially_cached_itr->second;

            // Default to finding the location from directly in the file.
            stream.seekg( segment_tag_pos);
            
            SegmentPos segment_pos = 0;

            /// @warning ENDIAN
            stream.read(reinterpret_cast<char*>(&segment_pos), sizeof(segment_pos));
            test_badbit();

            cache.segment_tag_destinations[segment_tag_pos] = segment_pos;
            
            return segment_pos;
        };

        void set_segment_tag(SegmentTagPos segment_tag_pos, SegmentPos new_segment_pos) {
            stream.seekp(segment_tag_pos);
            /// @warning ENDIAN
            stream.write(reinterpret_cast<const char*>(&new_segment_pos), sizeof(new_segment_pos));
            test_badbit();

            cache.segment_tag_destinations[segment_tag_pos] = new_segment_pos;
        }

        [[nodiscard]]
        SegmentSize get_segment_size_metadata(SegmentPos segment_pos) {
            auto potentially_cached_itr = cache.segment_sizes.find(segment_pos);

            if (potentially_cached_itr != cache.segment_sizes.end())
                return potentially_cached_itr->second;

            stream.seekg(segment_pos);

            SegmentSize segment_size = 0;

            /// @warning ENDIAN
            stream.read(reinterpret_cast<char*>(&segment_size), sizeof(segment_size));
            test_badbit();

            cache.segment_sizes[segment_pos] = segment_size;

            return segment_size;
        }

        void set_segment_size_metadata(SegmentPos segment_pos, SegmentSize new_segment_size) {
            stream.seekp(segment_pos);
            /// @warning ENDIAN
            stream.write(reinterpret_cast<const char*>(&new_segment_size), sizeof(new_segment_size));
            test_badbit();

            cache.segment_sizes[segment_pos] = new_segment_size;
        }

        [[nodiscard]]
        SegmentPos new_segment(SegmentTagPos segment_tag_pos, zstd::buffer_t& segment_content) {
            SegmentPos new_segment_pos = file_end_pos;
            SegmentSize segment_size = segment_content.size();
            
            extend_file_end_pos(sizeof(SegmentSize) + segment_size); // account for segment content and the bytes that determine size
            set_segment_size_metadata(new_segment_pos, segment_size);
            set_segment_tag(segment_tag_pos, new_segment_pos);
            
            stream.seekp(new_segment_pos + sizeof(SegmentSize));
            stream.write(reinterpret_cast<const char*>(segment_content.data()), segment_content.size());
            test_badbit();

            return new_segment_pos;
        }

        zstd::buffer_t read_segment(SegmentTagPos segment_tag_pos) {
            SegmentPos segment_pos = read_segment_tag(segment_tag_pos);
            SegmentSize segment_size = get_segment_size_metadata(segment_pos);

            zstd::buffer_t buffer;
            buffer.resize(segment_size);

            stream.seekg(segment_pos + sizeof(SegmentSize));
            stream.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
            test_badbit();
        }

        // Updates a segment to contain new content. If the size of the content requires expanding, a new segment is allocated and the current is deemed garbage to be compacted.
        [[nodiscard]]
        SegmentPos write_segment(SegmentTagPos segment_tag_pos, zstd::buffer_t& segment_content) {
            SegmentPos segment_pos = read_segment_tag(segment_tag_pos);
            SegmentSize segment_size = segment_content.size();
            
            // If there is no existing segment entry for the given tag position:
            if (segment_pos == INVALID_SEGMENT_POS)
                return new_segment(segment_tag_pos, segment_content);

            SegmentSize current_segment_size = get_segment_size_metadata(segment_pos);
            
            // If the new segment contents is too large, we need to allocate a new segment.
            if (segment_size > current_segment_size)
                // Note: new_segment() will automatically override the previous segment space, marking it garbage for compacting.
                return new_segment(segment_tag_pos, segment_content);

            if (segment_size < current_segment_size)
                set_segment_size_metadata(segment_pos, segment_size);

            stream.seekp(segment_pos + sizeof(SegmentSize));
            stream.write(reinterpret_cast<const char*>(segment_content.data()), segment_content.size());
            test_badbit();

            return segment_pos;
        }
    };
}