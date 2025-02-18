#include "DSVReader.h"
#include <sstream>
#include <iostream>

//Structure for implementation for CDSVReader
//handles the actual data parsing logic
struct CDSVReader::SImplementation {
    std::shared_ptr<CDataSource> Source;
    char Delimiter;
    
    //Constructor, intialize data source and delimiter
    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
        : Source(std::move(src)), Delimiter(delimiter) {}
    
    // Reads a row of data from the DSV file and stores it in a vector
    // Returns true if a row is read false if end of file is reached
    bool ReadRow(std::vector<std::string> &row) {
        // Reset the row before reading new data
        row.clear();
        std::string right;
        char ch;
        bool quotes = false;
        bool data = false;
        
        while (!Source->End()) {
            // If unable to read a character, return false
            if (!Source->Get(ch)) return false;
            data = true;
            
            //for double quotes
            if (ch == '"') {
                char next;
                // Check the next character
                if (!Source->End() && Source->Peek(next)) {
                    if (next == '"') {
                        Source->Get(next);
                        // Add a single quote to the field
                        right += '"';
                    } else {
                        quotes = !quotes;
                    }
                } else {
                    quotes = !quotes;
                }
            } else if (ch == Delimiter && !quotes) {
                //store string and clear it 
                row.push_back(std::move(right));
                right.clear();
            // End of row detected
            } else if ((ch == '\n' || ch == '\r') && !quotes) {
                if (!right.empty() || !row.empty()) {
                    // Store last string
                    row.push_back(std::move(right));
                }
                
                if (ch == '\r' && !Source->End()) {
                    char next;
                    if (Source->Peek(next) && next == '\n') {
                        Source->Get(next);
                    }
                }
                //read a row
                return true;
            } else {
                //append char to current string
                right += ch;
            }
        }
        //store last string if data was read
        if (!right.empty() || !row.empty()) {
            row.push_back(std::move(right));
        }
        // Return whether any data was read
        return data;
    }
};
// Constructor for DSV reader, src specifies the data source and delimiter
// specifies the delimiting character
CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter)
    : DImplementation(std::make_unique<SImplementation>(src, delimiter)) {}
// Destructor for DSV reader
CDSVReader::~CDSVReader() = default;
// Returns true if all rows have been read from the DSV
bool CDSVReader::End() const {
    return DImplementation->Source->End();
}
// Returns true if the row is successfully read, one string will be put in
// the row per column
bool CDSVReader::ReadRow(std::vector<std::string> &row) {
    return DImplementation->ReadRow(row);
}
