
#include "Playlist.hpp"
#include "../Utils.hpp"

#include <cmath>
#include <string_view>

Playlist Playlist::parse(oatpp::parser::Caret& caret) {

    auto result = RecordMarkerList::createShared();

    while (caret.canContinue()) {
        caret.findChar('#');
        if(caret.isAtText("#EXTINF:", true)) {
            v_float64 secs = caret.parseFloat64();
            caret.findROrN();
            caret.skipAllRsAndNs();
            auto uriLabel = caret.putLabel();
            caret.findChar('\n');
            result->pushBack({secs, oatpp::String(uriLabel.toString())});
        }
        caret.findROrN();
        caret.skipAllRsAndNs();
    }

    return Playlist(result);
}

Playlist Playlist::parseFromFile(const char* filename) {
    std::cout << "***** filename = " << filename << " ******\n";
    auto text = loadFromFile(filename);
    if(!text){
        throw std::runtime_error("Can't find playlist file. Make sure you specified full file path's for video and playlist in AppComponent.hpp");
    }

    // Caret should help us to parse HLS header
    oatpp::parser::Caret caret(text);
    return parse(caret);
}

void Playlist::setPlaylistFilename(const std::string_view &filename)
{
    m_playlistFilename = filename;
}

void Playlist::generateNextFrame()
{
    if(m_capture->isOpened())
    {
        // Where frame captured from HLS stream will be stored
        cv::Mat frame;

        // Extract single frame
        *m_capture >> frame;

        // If it's not empty,
        // then save as a .png image
        if(!frame.empty())
        {
            const std::string imgName = "image_" + std::to_string(m_mediaSequence) + ".png";
            OATPP_LOGD("extract_frame", std::string{"generate png image " + imgName}.c_str());

            const std::string pathToSave = EXAMPLE_MEDIA_FOLDER "/screenshots/" + imgName;
            std::cout << "pathToSave: " << pathToSave << std::endl;
            cv::imwrite( pathToSave.c_str(), frame);
        }
        else
        {
            OATPP_LOGD("extract_frame", std::string{"frame with index " + std::to_string(m_mediaSequence) + " is empty\n"}.c_str());
        }
    }
}

void Playlist::generateVideo()
{
    if(m_capture->isOpened() && m_writer->isOpened())
    {
        cv::Mat frame;
//        for(;;)
//        {
            *m_capture >> frame;

            if(frame.empty())
            {
                return;
            }

            OATPP_LOGD("record", "generating mp4 output");

            m_writer->write(frame);
//        }
    }
    else
    {
        m_writer->release();
    }
}

std::shared_ptr<oatpp::data::stream::ChunkedBuffer> Playlist::generateForTime(v_int64 millis, v_int32 numRecords) {
    if(m_records->count() == 0) {
        throw std::runtime_error("Empty playlist");
    }
    auto stream = oatpp::data::stream::ChunkedBuffer::createShared();

    v_float64 secs = (v_float64) millis / 1000.0;

    v_int64 rounds = std::floor(secs / m_totalDuration);
    v_float64 loopTime = secs - rounds * m_totalDuration;
    v_float64 currTime = 0;

    v_int64 mediaSequence = 1 + rounds * m_records->count();

    auto currRecordNode = m_records->getFirstNode();
    while (currRecordNode->getNext() != nullptr) {
        if(currRecordNode->getData().duration + currTime >= loopTime) {
            break;
        }
        currTime += currRecordNode->getData().duration;
        currRecordNode = currRecordNode->getNext();
        mediaSequence ++;
    }

    v_float64 targetDuration = 0;
    RecordMarkerList list;
    for(v_int32 i = 0; i < numRecords; i++) {
        auto& marker = currRecordNode->getData();
        list.pushBack(marker);

        if(marker.duration > targetDuration) {
            targetDuration = marker.duration;
        }

        currRecordNode = currRecordNode->getNext();
        if(currRecordNode == nullptr) {
            currRecordNode = m_records->getFirstNode();
        }
    }

    m_mediaSequence = mediaSequence;
    OATPP_LOGD("playlist", "generating sequence %d", mediaSequence);

    *stream
            << "#EXTM3U\n"
            << "#EXT-X-TARGETDURATION:" << (v_int32)(targetDuration + 1) << "\n"
            << "#EXT-X-VERSION:3\n"
            << "#EXT-X-MEDIA-SEQUENCE:" << mediaSequence << "\n";

    list.forEach([stream](const RecordMarker& marker){

        *stream
                << "#EXTINF:" << marker.duration << ",\n"
                << marker.uri << "\n";

    });

    return stream;
}
