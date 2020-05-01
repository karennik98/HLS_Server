#ifndef AUTHDATASTORAGE_H
#define AUTHDATASTORAGE_H

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <optional>

class AuthDataStorage
{
public:
    AuthDataStorage()
    {
        // https://www.base64encode.org/
        // This encode login:password are generated from online base64 encode/decode site
        // login : login 
        // password : password
        m_encodedAuthData.push_back("bG9naW46cGFzc3dvcmQ=");
    }

    std::optional<std::string> getAuthTypeFromHeader(const std::string& header) const;
    std::optional<std::string> getAuthValueFromHeader(const std::string& header) const;

    bool isAuthValueExists(const std::string& authValue) const;

private:
    // Contains base64 encoded login:password strings
    std::vector<std::string> m_encodedAuthData;
};

#endif // AUTHDATASTORAGE_H
