#include "XMLReader.h"
#include <expat.h>
#include <queue>
#include <memory>
#include <vector>

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DataSource;
    XML_Parser Parser;
    std::queue<SXMLEntity> EntityQueue;
    bool EndOfData = false;
    std::string CharDataBuffer;

    // Handles both start and end elements
    static void ElementHandler(void *userData, const char *name, const char **atts, bool isStart) {
        auto *impl = static_cast<SImplementation *>(userData);
        impl->FlushCharData();

        SXMLEntity entity;
        entity.DType = isStart ? SXMLEntity::EType::StartElement : SXMLEntity::EType::EndElement;
        entity.DNameData = name;

        if (isStart && atts) {
            for (int i = 0; atts[i] != nullptr; i += 2) {
                if (atts[i + 1] != nullptr) {
                    entity.DAttributes.emplace_back(atts[i], atts[i + 1]);
                }
            }
        }

        impl->EntityQueue.push(entity);
    }

    static void StartElementHandler(void *userData, const char *name, const char **atts) {
        ElementHandler(userData, name, atts, true);
    }

    static void EndElementHandler(void *userData, const char *name) {
        ElementHandler(userData, name, nullptr, false);
    }

    static void CharDataHandler(void *userData, const char *s, int len) {
        if (s && len > 0) {
            static_cast<SImplementation *>(userData)->CharDataBuffer.append(s, len);
        }
    }

    SImplementation(std::shared_ptr<CDataSource> src) : DataSource(std::move(src)) {
        Parser = XML_ParserCreate(nullptr);
        XML_SetUserData(Parser, this);
        XML_SetElementHandler(Parser, StartElementHandler, EndElementHandler);
        XML_SetCharacterDataHandler(Parser, CharDataHandler);
    }

    ~SImplementation() {
        XML_ParserFree(Parser);
    }

    void FlushCharData() {
        if (!CharDataBuffer.empty()) {
            EntityQueue.push({SXMLEntity::EType::CharData, CharDataBuffer, {}});
            CharDataBuffer.clear();
        }
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        while (EntityQueue.empty() && !EndOfData) {
            std::vector<char> buffer(4096);
            size_t length = 0;
            while (length < buffer.size() && !DataSource->End()) {
                char ch;
                if (DataSource->Get(ch)) {
                    buffer[length++] = ch;
                } else {
                    break;
                }
            }

            if (length == 0) {
                EndOfData = true;
                XML_Parse(Parser, nullptr, 0, 1);
                break;
            }

            if (XML_Parse(Parser, buffer.data(), length, 0) == XML_STATUS_ERROR) {
                return false;
            }
        }

        if (!EntityQueue.empty()) {
            entity = EntityQueue.front();
            EntityQueue.pop();
            return !(skipcdata && entity.DType == SXMLEntity::EType::CharData) || ReadEntity(entity, skipcdata);
        }

        return false;
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src)
    : DImplementation(std::make_unique<SImplementation>(std::move(src))) {}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const {
    return DImplementation->EndOfData && DImplementation->EntityQueue.empty();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    return DImplementation->ReadEntity(entity, skipcdata);
}

// SXMLEntity functions
bool SXMLEntity::AttributeExists(const std::string &name) const {
    for (const auto &attr : DAttributes) {
        if (attr.first == name) {
            return true;
        }
    }
    return false;
}

std::string SXMLEntity::AttributeValue(const std::string &name) const {
    for (const auto &attr : DAttributes) {
        if (attr.first == name) {
            return attr.second;
        }
    }
    return "";
}

bool SXMLEntity::SetAttribute(const std::string &name, const std::string &value) {
    for (auto &attr : DAttributes) {
        if (attr.first == name) {
            attr.second = value;
            return true;
        }
    }
    DAttributes.emplace_back(name, value);
    return true;
}
