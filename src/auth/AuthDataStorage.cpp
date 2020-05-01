#include "AuthDataStorage.hpp"
#include "../Utils.hpp"

#include <iostream>
#include <string>

std::optional<std::string> AuthDataStorage::getAuthTypeFromHeader(const std::string &header) const
{
    std::vector<std::string> headerValues;
    tokenize(header, headerValues, " ");

    if(headerValues.size() != 2)
    {
        return std::nullopt;
    }

    return headerValues[0];
}

std::optional<std::string> AuthDataStorage::getAuthValueFromHeader(const std::string &header) const
{
    std::vector<std::string> headerValues;
    tokenize(header, headerValues, " ");

    if(headerValues.size() != 2)
    {
        return std::nullopt;
    }

    return headerValues[1];
}

bool AuthDataStorage::isAuthValueExists(const std::string &authValue) const
{
    auto authValueOptional = getAuthValueFromHeader(authValue);

    if(!authValueOptional.has_value())
    {
        return false;
    }

    auto authValueStr = authValueOptional.value();
    auto itEncodedLoginPass = (std::find(std::begin(m_encodedAuthData),
                                         std::end(m_encodedAuthData),
                                         authValueStr) != std::end(m_encodedAuthData));

    return itEncodedLoginPass;
}
