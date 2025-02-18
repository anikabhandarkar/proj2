#include "XMLWriter.h"
#include <stack>
#include <string>


struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DDataSink;
    std::stack<std::string> DElementStack;

    explicit SImplementation(std::shared_ptr<CDataSink> sink) 
        : DDataSink(std::move(sink)) {}

    bool WriteText(const std::string &str, bool escape) {
        for (char ch : str) {
            if (escape) {
                switch (ch) {
                    case '<':  if (!WriteText("&lt;", false)) return false; break;
                    case '>':  if (!WriteText("&gt;", false)) return false; break;
                    case '&':  if (!WriteText("&amp;", false)) return false; break;
                    case '\'': if (!WriteText("&apos;", false)) return false; break;
                    case '"':  if (!WriteText("&quot;", false)) return false; break;
                    default:   if (!DDataSink->Put(ch)) return false;
                }
            } else {
                if (!DDataSink->Put(ch)) return false;
            }
        }
        return true;
    }

    
    bool Flush() {
        while (!DElementStack.empty()) {
            if (!WriteText("</" + DElementStack.top() + ">", false)) {
                return false;
            }
            DElementStack.pop();
        }
        return true;
    }

   
    bool WriteEntity(const SXMLEntity &entity) {
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                if (!WriteText("<" + entity.DNameData, false)) return false;

                for (const auto &attr : entity.DAttributes) {
                    if (!WriteText(" " + attr.first + "=\"", false)) return false;
                    if (!WriteText(attr.second, true)) return false;  
                    if (!WriteText("\"", false)) return false;
                }

                if (!WriteText(">", false)) return false;
                DElementStack.push(entity.DNameData);
                break;

            case SXMLEntity::EType::EndElement:
                if (!WriteText("</" + entity.DNameData + ">", false)) return false;
                if (!DElementStack.empty()) {
                    DElementStack.pop();
                }
                break;

            case SXMLEntity::EType::CharData:
                if (!WriteText(entity.DNameData, true)) return false;  
                break;

            case SXMLEntity::EType::CompleteElement:
                if (!WriteText("<" + entity.DNameData, false)) return false;

                for (const auto &attr : entity.DAttributes) {
                    if (!WriteText(" " + attr.first + "=\"", false)) return false;
                    if (!WriteText(attr.second, true)) return false;  \
                    if (!WriteText("\"", false)) return false;
                }

                if (!WriteText("/>", false)) return false;
                break;
        }
        return true;
    }
};


CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink)
    : DImplementation(std::make_unique<SImplementation>(std::move(sink))) {}

CXMLWriter::~CXMLWriter() = default;

bool CXMLWriter::Flush() {
    return DImplementation->Flush();
}

bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    return DImplementation->WriteEntity(entity);
}
