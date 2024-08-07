//
// Created by user on 06.08.24.
//

#pragma once

#ifndef CASA_XML_H
#define CASA_XML_H

#include <memory>
#include "cpprest/details/basic_types.h"

namespace web
{
namespace xml
{

class value
{
public:
    enum value_type
    {
        /// Number value
        Number,
        /// Boolean value
        Boolean,
        /// String value
        String,
        /// Object value
        Object,
        /// Array value
        Array,
        /// Null value
        Null
    };
    _ASYNCRTIMP value();
    _ASYNCRTIMP explicit value(utility::string_t value);
    _ASYNCRTIMP value(const value&);
    _ASYNCRTIMP value(value&&) CPPREST_NOEXCEPT;
    _ASYNCRTIMP value& operator=(const value&);
    _ASYNCRTIMP value& operator=(value&&) CPPREST_NOEXCEPT;

    _ASYNCRTIMP static value __cdecl parse(const utility::string_t& value);

private:
    utility::string_t m_value;
};


namespace details
{

template<typename CharType>
class XML_Parser;


} // namespace details
} // namespace xml
} // namespace web

#endif // CASA_XML_H
