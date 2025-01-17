
#ifndef Utils_hpp
#define Utils_hpp

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>
#include <string>
#include <vector>

class StaticFilesManager {
private:
  oatpp::String m_basePath;
  oatpp::concurrency::SpinLock m_lock;
  std::unordered_map<oatpp::String, oatpp::String> m_cache;
private:
  oatpp::String getExtension(const oatpp::String& filename);
public:
  
  StaticFilesManager(const oatpp::String& basePath)
    : m_basePath(basePath)
  {}
  
  oatpp::String getFile(const oatpp::String& path);
  
  oatpp::String guessMimeType(const oatpp::String& filename);
  
};

oatpp::String loadFromFile(const char* fileName);
oatpp::String formatText(const char* text, ...);
v_int64 getMillisTickCount();
void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);

#endif /* Utils_hpp */
