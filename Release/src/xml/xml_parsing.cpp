//
// Created by user on 06.08.24.
//

#include <cstring>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidxml/rapidxml.hpp>
#include <unordered_map>
#include <vector>
#include "cpprest/xml.h"

namespace web
{
namespace xml
{
namespace details
{

template<typename CharType>
class XML_Parser
{
public:
    XML_Parser() {}
    virtual web::xml::value parse() = 0;

    std::basic_string<CharType> convertJsonToString(rapidjson::Value &json){
        rapidjson::StringBuffer jsonBuffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(jsonBuffer);
        json.Accept(writer);
        return jsonBuffer.GetString();
    }

}; // class XML_Parser

template<typename CharType>
class XML_StringParser : public XML_Parser<CharType>
{
public:
    XML_StringParser(const std::basic_string<CharType>& string) : m_startpos(&string[0]), m_BufferSize(string.size() + 1)
    {
        this->m_endpos = this->m_startpos + string.size();
    }
    virtual web::xml::value parse() override;

private:
    const CharType* m_startpos;
    const CharType* m_endpos;
    const size_t m_BufferSize;

    void convertXmlToJson(rapidxml::xml_node<> *node, rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator);

    static std::unordered_map<std::string, std::vector<rapidxml::xml_node<>*>> preprocess(rapidxml::xml_node<> *node);

}; // class XML_StringParser

} // namespace details
} // namespace xml
} // namespace web

template<typename CharType>
web::xml::value web::xml::details::XML_StringParser<CharType>::parse()
{
    CharType* buffer = new char[this->m_BufferSize];
    std::memcpy(buffer, this->m_startpos, this->m_BufferSize);

    rapidjson::Document json;
    json.SetObject();

    rapidxml::xml_document<> doc;
    doc.parse<0>(buffer);

    convertXmlToJson(&doc, json, json.GetAllocator());
    std::basic_string<CharType> jsonString = this->convertJsonToString(json);
    std::cout << jsonString << std::endl;

    delete[] buffer;

    return web::xml::value(jsonString);
}

template<typename CharType>
static web::xml::value _parse_string(const std::basic_string<CharType>& str)
{
    web::xml::details::XML_StringParser<CharType>parser(str);
    web::xml::value value = parser.parse();



    return value;
}

web::xml::value web::xml::value::parse(const utility::string_t& str) { return _parse_string(str); }

template<typename CharType>
void web::xml::details::XML_StringParser<CharType>::convertXmlToJson(rapidxml::xml_node<> *node, rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) {
    std::unordered_map<std::string, std::vector<rapidxml::xml_node<>*>> nodes = web::xml::details::XML_StringParser<CharType>::preprocess(node);
    for (rapidxml::xml_node<> *child = node->first_node(); child; child = child->next_sibling()) {
        rapidjson::Value json_child(rapidjson::kObjectType);
        json_child.SetObject();
        if (child->first_attribute()) {
            for (rapidxml::xml_attribute<> *attr = child->first_attribute(); attr; attr = attr->next_attribute()) {
                json_child.AddMember(rapidjson::Value(attr->name(), allocator).Move(),
                                     rapidjson::Value(attr->value(), allocator).Move(),
                                     allocator);
            }
        }

        if (child->first_node()) {
            convertXmlToJson(child, json_child, allocator);
        } else {
            //json_child.SetString(child->name(), allocator);
            //json.AddMember(rapidjson::Value(child->name(), allocator).Move(), json_child, allocator);
        }

        rapidjson::Value name;
        name.SetString(child->name(), allocator);

        if (json.HasMember(child->name())) {
            // If the key already exists, convert it to an array or append to the existing array
            rapidjson::Value& existingValue = json[child->name()];
            if (!existingValue.IsArray()) {
                rapidjson::Value array(rapidjson::kArrayType);
                array.PushBack(existingValue, allocator);
                array.PushBack(json_child, allocator);
                json.RemoveMember(child->name());
                json.AddMember(name, array, allocator);
            } else {
                existingValue.PushBack(json_child, allocator);
            }
        } else {
            json.AddMember(name, json_child, allocator);
        }

        if(json_child.IsObject())
            json.AddMember(rapidjson::Value(child->name(), allocator).Move(), json_child, allocator);
    }
}

template<typename CharType>
std::unordered_map<std::string, std::vector<rapidxml::xml_node<>*>> web::xml::details::XML_StringParser<CharType>::preprocess(rapidxml::xml_node<> *node) {
    std::unordered_map<std::string, std::vector<rapidxml::xml_node<>*>> children;

    for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling()) {
        children[child->name()].push_back(child);
    }
    return children;
}