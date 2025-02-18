#include "DSVWriter.h"
#include "DataSink.h"

// Implementation structure for CDSVWriter, which handles writing to a data sink
struct CDSVWriter::SImplementation {
    std::shared_ptr<CDataSink> Sink; // Data sink for writing
    char Delimiter; // Character used as delimiter
    bool QuoteAll; // Determines if all fields should be quoted

    // Constructor for SImplementation, initializes the data sink, delimiter, and quote
    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
        : Sink(sink), Delimiter(delimiter), QuoteAll(quoteall) {}

    // Writes a row of data to the sink
    bool WriteRow(const std::vector<std::string>& row) {
        //  sink is valid
        if (!Sink) return false; 
        // Write only a newline for an empty row
        if (row.empty()) {
            return Sink->Put('\n');
        }
        // Iterate over each field in the row
        for (size_t i = 0; i < row.size(); ++i) {
            bool quote = QuoteAll || row[i].find(Delimiter) != std::string::npos ||
                              row[i].find('"') != std::string::npos || row[i].find('\n') != std::string::npos;
            
            if (quote) {
                // Start quoted field
                Sink->Put('"'); 
                // Iterate over each character in the field
                for (char ch : row[i]) {
                    if (ch == '"') {
                        // Escape double quotes by doubling them (e.g., "example" -> ""example"")
                        Sink->Put('"');
                        Sink->Put('"');
                    } else {
                        // Write the character as is
                        Sink->Put(ch);
                    }
                }
                // End quoted field
                Sink->Put('"'); 
            // If no quoting is needed, write the field character by character
            } else {
                for (char ch : row[i]) {
                    Sink->Put(ch);
                }
            }
            // Add delimiter between fields, but not after the last field
            if (i < row.size() - 1) {
                Sink->Put(Delimiter); 
            }
        }
        // End the row with a newline character
        return Sink->Put('\n'); 
    }
};
// Constructor for DSV writer, sink specifies the data destination, delimiter
// specifies the delimiting character, and quoteall specifies if all values
// should be quoted or only those that contain the delimiter, a double quote,
// or a newline
CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
    : DImplementation(std::make_unique<SImplementation>(sink, delimiter, quoteall)) {}

// Destructor for CDSVWriter
CDSVWriter::~CDSVWriter() = default;

// Returns true if the row is successfully written, one string per column
// should be put in the row vector
bool CDSVWriter::WriteRow(const std::vector<std::string>& row) {
    return DImplementation->WriteRow(row);
}
