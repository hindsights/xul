#pragma once

#include <xul/data/numerics.hpp>
#include <tinyxml/tinyxml.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <vector>
#include <stdint.h>


namespace xul {


class tinyxml_element
{
public:
    explicit tinyxml_element(TiXmlElement* elem = NULL) : m_elem(elem)
    {
    }

    bool is_null() const { return NULL == m_elem; }
    operator bool() const { return !is_null(); }

    TiXmlElement* operator->()
    {
        return m_elem;
    }
    const TiXmlElement* operator->() const
    {
        return m_elem;
    }

    std::string get_value() const
    {
        if (is_null())
            return std::string();
        const char* s = m_elem->Value();
        if (NULL == s)
            return std::string();
        return std::string(s);
    }

    std::string get_text() const
    {
        if (is_null())
            return std::string();
        const char* s = m_elem->GetText();
        if (NULL == s)
            return std::string();
        return std::string(s);
    }

    std::string get_attribute(const char* name) const
    {
        if (is_null())
            return std::string();
        const char* s = m_elem->Attribute(name);
        if (NULL == s)
            return std::string();
        return std::string(s);
    }

    void get_elements_by_tag_name(const char* name, std::vector<tinyxml_element>* elems)
    {
        for (tinyxml_element elem = get_first_child(); elem; elem = get_next_sibling())
        {
            if (elem.get_value() == std::string(name))
                elems->push_back(elem);
            std::vector<tinyxml_element> subElems;
            elem.get_elements_by_tag_name(name, &subElems);
            elems->insert(elems->end(), subElems.begin(), subElems.end());
        }
    }
    
    void get_elements_by_tag_name(const char* name, std::vector<tinyxml_element>& elems)
    {
        for (tinyxml_element elem = get_first_child(); elem; elem = get_next_sibling())
        {
            if (elem.get_value() == std::string(name))
                elems.push_back(elem);
            std::vector<tinyxml_element> subElems;
            elem.get_elements_by_tag_name(name, &subElems);
            elems.insert(elems.end(), subElems.begin(), subElems.end());
        }
    }
    
    tinyxml_element get_first_child(const char* name)
    {
        assert(!is_null());
        if (is_null())
            return tinyxml_element();
        return tinyxml_element(m_elem->FirstChildElement(name));
    }

    tinyxml_element get_first_child()
    {
        assert(!is_null());
        if (is_null())
            return tinyxml_element();
        return tinyxml_element(m_elem->FirstChildElement());
    }

    tinyxml_element get_next_sibling(const char* name)
    {
        assert(!is_null());
        if (is_null())
            return tinyxml_element();
        return tinyxml_element(m_elem->NextSiblingElement(name));
    }

    tinyxml_element get_next_sibling()
    {
        assert(!is_null());
        if (is_null())
            return tinyxml_element();
        return tinyxml_element(m_elem->NextSiblingElement());
    }

    std::string get_string(const char* name, const std::string& defaultVal = std::string())
    {
        assert(!is_null());
        if (is_null())
            return defaultVal;
        assert(name);
        TiXmlElement* item = m_elem->FirstChildElement(name);
        if (NULL == item)
            return defaultVal;
        const char* text = item->GetText();
        if (NULL == text)
            return defaultVal;
        return std::string(text);
    }

    template <typename T>
    T get_numeric(const char* name, T defaultVal)
    {
        std::string val = get_string(name);
        if (val.empty())
            return defaultVal;
        return xul::numerics::parse(val, defaultVal);
    }

    int get_int(const char* name, int defaultVal)
    {
        return get_numeric<int>(name, defaultVal);
    }

    int64_t get_int64(const char* name, int64_t defaultVal)
    {
        return get_numeric<int64_t>(name, defaultVal);
    }

    float get_float(const char* name, float defaultVal)
    {
        return get_numeric<float>(name, defaultVal);
    }
    double get_double(const char* name, double defaultVal)
    {
        return get_numeric<double>(name, defaultVal);
    }

    bool get_bool(const char* name, bool defaultVal)
    {
        std::string s = get_string(name);
        if (s.empty())
            return defaultVal;
        boost::algorithm::to_lower(s);
        if ("true" == s)
            return true;
        if ("false" == s)
            return false;
        return defaultVal;
    }

    template <typename T>
    bool get_object(const char* name, T& obj)
    {
        return obj.load(get_first_child(name));
    }

    template <typename T>
    void get_objects(const char* name, std::vector<T>& objs)
    {
        objs.clear();
        get_append_objects(name, objs);
    }
    template <typename T>
    void get_append_objects(const char* name, std::vector<T>& objs)
    {
        append_objects(name, objs);
    }

    template <typename T>
    void append_objects(const char* name, std::vector<T>& objs)
    {
        if (is_null())
        {
            assert(false);
            return;
        }
        tinyxml_element elem = get_first_child(name);
        while (false == elem.is_null())
        {
            T obj;
            if (load(elem, obj))
            {
                objs.push_back(obj);
            }
            elem = elem.get_next_sibling(name);
        }
    }

    TiXmlNode* add_child(TiXmlNode* node)
    {
        if (is_null())
        {
            assert(false);
            return 0;
        }
        return m_elem->LinkEndChild(node);
    }

    tinyxml_element add_element(const char* name)
    {
        if (NULL == m_elem)
        {
            assert(false);
            return tinyxml_element();
        }
        TiXmlElement* elem = new TiXmlElement(name);
        if (NULL == m_elem->LinkEndChild(elem))
        {
            assert(false);
            return tinyxml_element();
        }
        return tinyxml_element(elem);
    }
    template <typename T>
    tinyxml_element add_element(const char* name, const T& obj)
    {
        tinyxml_element elem = add_element(name);
        if (elem.is_null())
        {
            assert(false);
            return elem;
        }
        save(elem, obj);
        return elem;
    }

    tinyxml_element add_text_element(const char* name, const char* value)
    {
        tinyxml_element elem = add_element(name);
        if (is_null())
            return tinyxml_element();
        TiXmlText * Datetext = new TiXmlText(value);
        if (NULL == elem.add_child(Datetext))
        {
            assert(false);
            return tinyxml_element();
        }
        return elem;
    }
    tinyxml_element add_text_element(const char* name, const std::string& value)
    {
        return add_text_element(name, value.c_str());
    }

    template <typename T>
    tinyxml_element add_numeric_element(const char* name, T value)
    {
        std::string s = xul::numerics::format<T>(value);
        return add_text_element(name, s.c_str());
    }

    template <typename T>
    void add_elements(const char* name, const std::vector<T>& objs)
    {
        if (objs.size() > 0)
        {
            add_elements(name, &objs[0], objs.size());
        }
    }

    template <typename T>
    void add_elements(const char* name, const T* objs, size_t count)
    {
        tinyxml_element elem;
        for (size_t index = 0; index < count; ++index)
        {
            elem = add_element(name, objs[index]);
        }
    }

    template <typename T>
    void add_elements_tree(const char* name, const char* itemName, const T* objs, size_t count)
    {
        xul::tinyxml_element ownerElem = add_element(name);
        ownerElem.set_attribute("type", "array");
        ownerElem.add_elements(itemName, objs, count);
    }

    template <typename T>
    void add_elements_tree(const char* name, const char* itemName, const std::vector<T>& objs)
    {
        add_elements_tree(name, itemName, objs.empty() ? NULL : &objs[0], objs.size());
    }

    void set_attribute(const char* name, const char* value)
    {
        if (m_elem)
        {
            m_elem->SetAttribute(name, value);
        }
    }
    void set_attribute(const std::string& name, const std::string& value)
    {
        if (m_elem)
        {
            m_elem->SetAttribute(name.c_str(), value.c_str());
        }
    }

private:
    TiXmlElement* m_elem;
};


class tinyxml_document : private boost::noncopyable
{
public:
    tinyxml_document()
    {

    }

    TiXmlDeclaration* add_declaration()
    {
        TiXmlDeclaration * xmlDec = new TiXmlDeclaration("1.0", "UTF-8", "yes");
        m_document.LinkEndChild(xmlDec);
        return xmlDec;
    }

    tinyxml_element add_element(const char* name)
    {
        TiXmlElement* elem = new TiXmlElement(name);
        m_document.LinkEndChild(elem);
        return tinyxml_element(elem);
    }

    bool save_to_string(std::string& s)
    {
        TiXmlPrinter printer;
        if (false == m_document.Accept(&printer))
        {
            assert(false);
            return false;
        }
        s.assign(printer.CStr(), printer.Size());
        return true;
    }

    bool load(const char* filename)
    {
        return m_document.LoadFile(filename);
    }

    bool parse(const char* xmldata)
    {
        m_document.Parse(xmldata);
        return !m_document.Error();
    }

    tinyxml_element get_root()
    {
        return tinyxml_element(m_document.RootElement());
    }

private:
    TiXmlDocument m_document;
};



template <typename T>
inline bool load(tinyxml_element& elem, T& obj)
{
    return obj.load(elem);
}

inline bool load(tinyxml_element& elem, std::string& obj)
{
    if (elem.is_null())
        return false;
    obj = elem.get_text();
    return true;
}

inline bool load(tinyxml_element& elem, int& obj)
{
    if (elem.is_null())
        return false;
    obj = xul::numerics::parse<int>(elem.get_text(), obj);
    return true;
}

inline bool load(tinyxml_element& elem, int64_t& obj)
{
    if (elem.is_null())
        return false;
    obj = xul::numerics::parse<int64_t>(elem.get_text(), obj);
    return true;
}

inline bool load(tinyxml_element& elem, float& obj)
{
    if (elem.is_null())
        return false;
    obj = xul::numerics::parse<float>(elem.get_text(), obj);
    return true;
}

inline bool load(tinyxml_element& elem, double& obj)
{
    if (elem.is_null())
        return false;
    obj = xul::numerics::parse<double>(elem.get_text(), obj);
    return true;
}



template <typename T>
inline bool save(tinyxml_element& elem, const T& obj)
{
    return obj.save(elem);
}


}


#define XUL_XML_GET_STRING(x, name, val) \
    do { (val) = (x).get_string((name), (val)); } while (false)

#define XUL_XML_GET_INT(x, name, val) \
    do { (val) = (x).get_int((name), (val)); } while (false)

#define XUL_XML_GET_OBJECT(x, name, val) \
    do { (x).get_object((name), (val)); } while (false)

#define XUL_XML_GET_VECTOR(x, name, val) \
    do { (val).load((x).get_first_child(name)); } while (false)

