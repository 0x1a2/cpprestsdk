//
// Created by user on 07.08.24.
//

#include "cpprest/xml.h"

web::xml::value::value() : m_value(std::move(nullptr)) {

}

web::xml::value::value(utility::string_t value) : m_value(std::move(value)) {

}