
#ifndef MediaController_hpp
#define MediaController_hpp

#include "../hls/Playlist.hpp"
#include "../Utils.hpp"
#include "../auth/AuthDataStorage.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/protocol/http/outgoing/BufferBody.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include <unordered_map>
#include <iostream>

/**
 *  EXAMPLE ApiController
 *  Basic examples of howto create ENDPOINTs
 *  More details on oatpp.io
 */
class MediaController : public oatpp::web::server::api::ApiController {
public:
    typedef MediaController __ControllerType;
private:
    OATPP_COMPONENT(std::shared_ptr<StaticFilesManager>, staticFileManager);
    OATPP_COMPONENT(std::shared_ptr<Playlist>, livePlaylist);
    OATPP_COMPONENT(std::shared_ptr<AuthDataStorage>, authDataStorage);
private:
    std::shared_ptr<OutgoingResponse> getStaticFileResponse(const oatpp::String& filename, const oatpp::String& rangeHeader) const;
    std::shared_ptr<OutgoingResponse> getFullFileResponse(const oatpp::String& file) const;
    std::shared_ptr<OutgoingResponse> getRangeResponse(const oatpp::String& rangeStr, const oatpp::String& file) const;
    static bool isAuthorizationOk(const oatpp::String& authHeader, const AuthDataStorage& authDataStorage);
public:
    MediaController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper)
    {}
public:

    /**
   *  Inject @objectMapper component here as default parameter
   */
    static std::shared_ptr<MediaController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)){
        return std::shared_ptr<MediaController>(std::make_shared<MediaController>(objectMapper));
    }

    /**
 *  Begin ENDPOINTs generation ('ApiController' codegen)
 */
#include OATPP_CODEGEN_BEGIN(ApiController)


    /**
   *  index.html endpoint
   */
    ENDPOINT_ASYNC("GET", "/", Root) {

        ENDPOINT_ASYNC_INIT(Root)

                const char* pageTemplate =
                "<html lang='en'>"
                "<head>"
                "<meta charset=utf-8/>"
                "</head>"
                "<body>"
                "<p>HLS stream server!!!</p>"
                "<a href='frames'>Checkout Frames(video/screenshots)</a></br>"
                "<a href='record'>Download Video(video/records)</a></br>"
                "<p>"
                "Note:""</br>"
                "Http Live Streaming supported by Safari browser only.""</br>"
                "You may also use VLC player to play this stream."
                "But the best way to start streaming, is to use script"
                "CurlStartStream.sh in build folder"
                "</p>"
                "</body>"
                "</html>";

        Action act() override {
            return _return(controller->createResponse(Status::CODE_200, pageTemplate));
        }

    };

    /**
   *  Live streaming and frame extraction playlist endpoint
   *
   * The most beautiful part.
   * Here we have two coroutines.
   *
   * The first one, called @Act() is used to generate next frame
   * from the @cv::VideoCapture object which is located in @Playlist
   *
   * The Second one, called @formAndSendResponse is used to
   * to send stream data back to user. It's helps us to make
   * streaming possible.
   *
   * The 'chaining' of coroutines was done using @yieldTo().
   * For more info,
   * see oatpp docs https://oatpp.io/docs/oatpp-coroutines/
   */
    ENDPOINT_ASYNC("GET", "frames", Frames) {

        ENDPOINT_ASYNC_INIT(Frames)

                static v_int64 getTime0(){
            static v_int64 time0 = getMillisTickCount();
            return time0;
        }

        // Extract frame from stream
        Action act() override {
            auto authHeader = request->getHeader("Authorization");
            bool isAuthOk = MediaController::isAuthorizationOk(authHeader, *controller->authDataStorage);
            if(!isAuthOk)
            {
                auto response = controller->createResponse(Status::CODE_401, "Access Denied");
                response->putHeader("WWW-Authenticate", "Basic realm=\"http://127.0.0.1\"");

                return _return(response);
            }

            controller->livePlaylist->generateNextFrame();

            return yieldTo(&Frames::formAndSendResponse);
        }

        // Form response with correct HLS header data
        // and send it to user
        Action formAndSendResponse()
        {
            v_int64 time = getMillisTickCount() - getTime0();
            auto response = controller->createResponse(Status::CODE_200, controller->livePlaylist->generateForTime(time, 5)->toString());
            response->putHeader("Accept-Ranges", "bytes");
            response->putHeader(Header::CONNECTION, Header::Value::CONNECTION_KEEP_ALIVE);
            response->putHeader(Header::CONTENT_TYPE, "application/x-mpegURL");

            return _return(response);
        }

    };

    ENDPOINT_ASYNC("GET", "record", Record) {

        ENDPOINT_ASYNC_INIT(Record)

                static v_int64 getTime0(){
            static v_int64 time0 = getMillisTickCount();
            return time0;
        }

        // Extract frame from stream
        Action act() override {
            controller->livePlaylist->generateVideo();

            return yieldTo(&Record::formAndSendResponse);
        }

        // Form response with correct HLS header data
        // and send it to user
        Action formAndSendResponse()
        {
            v_int64 time = getMillisTickCount() - getTime0();
            auto response = controller->createResponse(Status::CODE_200, controller->livePlaylist->generateForTime(time, 5)->toString());
            response->putHeader("Accept-Ranges", "bytes");
            response->putHeader(Header::CONNECTION, Header::Value::CONNECTION_KEEP_ALIVE);
            response->putHeader(Header::CONTENT_TYPE, "application/x-mpegURL");

            return _return(response);
        }

    };
    /**
 *  Finish ENDPOINTs generation ('ApiController' codegen)
 */
#include OATPP_CODEGEN_END(ApiController)

};

#endif /* MediaController_hpp */
