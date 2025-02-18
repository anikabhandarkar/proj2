
#include "XMLWriter.h"
#include <stack>
#include <string>

// Internal implementation of CXMLWriter
struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> Sink;
    std::stack<std::string> Stack;

    explicit SImplementation(std::shared_ptr<CDataSink> sink) 
        : Sink(std::move(sink)) {}

    // Writes a string to the output, escaping special XML characters if needed
    bool WriteText(const std::string &str, bool escape) {
        for (char ch : str) {
            if (escape) {
                switch (ch) {
                    case '<':  if (!WriteText("&lt;", false)) return false; break;
                    case '>':  if (!WriteText("&gt;", false)) return false; break;
                    case '&':  if (!WriteText("&amp;", false)) return false; break;
                    case '\'': if (!WriteText("&apos;", false)) return false; break;
                    case '"':  if (!WriteText("&quot;", false)) return false; break;
                    default:   if (!Sink->Put(ch)) return false;
                }
            } else {
                if (!Sink->Put(ch)) return false;
            }
        }
        return true;
    }

    // Closes all open XML elements
    bool Flush() {
        while (!Stack.empty()) {
            if (!WriteText("</" + Stack.top() + ">", false)) {
                return false;
            }
            Stack.pop();
        }
        return true;
    }

   // Writes an XML entity (tag, data, or self-closing element)
    bool WriteEntity(const SXMLEntity &entity) {
        switch (entity.DType) {
            // Opening tag
            case SXMLEntity::EType::StartElement:
                if (!WriteText("<" + entity.DNameData, false)) return false;

                for (const auto &attr : entity.DAttributes) {
                    if (!WriteText(" " + attr.first + "=\"", false)) return false;
                    if (!WriteText(attr.second, true)) return false;  
                    if (!WriteText("\"", false)) return false;
                }

                if (!WriteText(">", false)) return false;
                // Store open tag for future closure
                Stack.push(entity.DNameData);
                break;
            // Closing tag
            case SXMLEntity::EType::EndElement:
                if (!WriteText("</" + entity.DNameData + ">", false)) return false;
                // Remove the corresponding start tag from stack
                if (!Stack.empty()) {
                    Stack.pop();
                }
                break;

            // Character data 
            case SXMLEntity::EType::CharData:
                if (!WriteText(entity.DNameData, true)) return false;  
                break;

            // Self-closing tag
            case SXMLEntity::EType::CompleteElement:
                if (!WriteText("<" + entity.DNameData, false)) return false;

                for (const auto &attr : entity.DAttributes) {
                    if (!WriteText(" " + attr.first + "=\"", false)) return false;
                    if (!WriteText(attr.second, true)) return false;  \
                    if (!WriteText("\"", false)) return false;
                }

                // Self-closing tag
                if (!WriteText("/>", false)) return false;
                break;
        }
        return true;
    }
};

// Constructor for XML writer, sink specifies the data destination
CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink)
    : DImplementation(std::make_unique<SImplementation>(std::move(sink))) {}

// Destructor for XML writer
CXMLWriter::~CXMLWriter() = default;

// Outputs all end elements for those that have been started
bool CXMLWriter::Flush() {
    return DImplementation->Flush();
}

// Writes out the entity to the output stream
bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    return DImplementation->WriteEntity(entity);
}
