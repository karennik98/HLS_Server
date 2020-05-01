
#ifndef Playlist_hpp
#define Playlist_hpp

// oatpp
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"
#include "oatpp/core/parser/Caret.hpp"

// opencv
#include <opencv2/opencv.hpp>
//#include <opencv2/videoio.hpp>

// STL
#include <chrono>
#include <string_view>
#include <iostream>

class Playlist {
private:

    struct RecordMarker {
        v_float64 duration;
        oatpp::String uri;
    };

private:
    typedef oatpp::collection::LinkedList<RecordMarker> RecordMarkerList;
private:
    std::shared_ptr<RecordMarkerList> m_records{nullptr};
    std::shared_ptr<cv::VideoCapture> m_capture{nullptr};
    std::shared_ptr<cv::VideoWriter> m_writer{nullptr};

    v_float64 m_totalDuration{0};
    v_int64 m_mediaSequence{0};
    std::string_view m_playlistFilename{};
public:

    Playlist(const std::shared_ptr<RecordMarkerList>& records)
        : m_records(records)
    {
        std::cout << "Playlist Cor Called\n";
        m_records->forEach([this](const RecordMarker& marker){
            m_totalDuration += marker.duration;
        });

        m_capture = std::make_shared<cv::VideoCapture>();
        m_capture->set(cv::CAP_PROP_FRAME_WIDTH, 300);
        m_capture->set(cv::CAP_PROP_FRAME_HEIGHT, 300);
        m_capture->open(EXAMPLE_MEDIA_FOLDER "/playlist1.m3u8");
        if(!m_capture->isOpened())
        {
            OATPP_LOGD("extract_frame", "Unable to open capture playlist\n");
        }

        m_writer = std::make_shared<cv::VideoWriter>();
        m_writer->open(EXAMPLE_MEDIA_FOLDER "/records/output.avi",
                       cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                       29,
                       cv::Size{300, 300});
        if(!m_writer->isOpened())
        {
            OATPP_LOGD("extract_frame", "Unable to open output file output.avi\n");
        }

    }

    static Playlist parse(oatpp::parser::Caret& caret);
    static Playlist parseFromFile(const char* filename);

    void setPlaylistFilename(const std::string_view& filename);

    // Frame extraction from m3u8 playlist(decoded hls data) goes here
    void generateNextFrame();
    void generateVideo();

    // Constructs HLS response.
    // This functons allows us to see the stream in 'live'
    std::shared_ptr<oatpp::data::stream::ChunkedBuffer> generateForTime(v_int64 millis, v_int32 numRecords);
};

#endif /* Playlist_hpp */
