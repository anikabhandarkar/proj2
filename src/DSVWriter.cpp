#include "DSVWriter.h"
#include "DataSink.h"

struct CDSVWriter::SImplementation
{
    std::shared_ptr<CDataSink> Sink;
    char Delimiter;
    bool QuoteAll;

    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
        : Sink(sink), Delimiter(delimiter), QuoteAll(quoteall) {}
};
// Constructor for DSV writer, sink specifies the data destination, delimiter
// specifies the delimiting character, and quoteall specifies if all values
// should be quoted or only those that contain the delimiter, a double quote,
// or a newline
CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall = false)
    : DImplementation(std::make_unique<SImplementation>(sink, delimiter, quoteall)) {}

// Destructor for DSV writer
CDSVWriter::~CDSVWriter() = default;

// Returns true if the row is successfully written, one string per column
// should be put in the row vector
bool CDSVWriter::WriteRow(const std::vector<std::string> &row)
{
    if (!DImplementation || !DImplementation->Sink)
    {
        return false;
    }

    if (row.empty())
    {
        return DImplementation->Sink->Put('\n');
    }

    auto &sink = DImplementation->Sink;
    char delimiter = DImplementation->Delimiter;
    bool quoteAll = DImplementation->QuoteAll;

    for (size_t i = 0; i < row.size(); ++i)
    {
        const auto &field = row[i];

        bool needsQuote = quoteAll ||
                          field.find(delimiter) != std::string::npos ||
                          field.find('"') != std::string::npos ||
                          field.find('\n') != std::string::npos;

        if (needsQuote)
        {
            if (!sink->Put('"'))
                return false;

            for (char ch : field)
            {
                if (ch == '"')
                {
                    if (!sink->Put('"'))
                        return false;
                    if (!sink->Put('"'))
                        return false;
                }
                else
                {
                    if (!sink->Put(ch))
                        return false;
                }
            }

            if (!sink->Put('"'))
                return false;
        }
        else
        {
            for (char ch : field)
            {
                if (!sink->Put(ch))
                    return false;
            }
        }

        if (i < row.size() - 1)
        {
            if (!sink->Put(delimiter))
                return false;
        }
    }

    return sink->Put('\n');
}
