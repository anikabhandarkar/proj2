#include "DSVReader.h"


// Constructor for DSV reader, src specifies the data source and delimiter
// specifies the delimiting character
CDSVReader::CDSVReader(std::shared_ptr< CDataSource > src, char delimiter)
    :DImplementation(std::make_unique<SImplementation>(src, delimiter)){

    }
// Destructor for DSV reader
CDSVReader::~CDSVReader() = default;
// Returns true if all rows have been read from the DSV
bool CDSVReader::End() const{
    //return;
}
// Returns true if the row is successfully read, one string will be put in
// the row per column
bool CDSVReader::ReadRow(std::vector< std::string > &row){

}




// Constructor for DSV writer, sink specifies the data destination, delimiter
// specifies the delimiting character, and quoteall specifies if all values
// should be quoted or only those that contain the delimiter, a double quote,
// or a newline
//CDSVWriter(std::shared_ptr< CDataSink > sink, char delimiter,
//    bool quoteall = false);
// Destructor for DSV writer
//~CDSVWriter();
// Returns true if the row is successfully written, one string per column
// should be put in the row vector
//bool WriteRow(const std::vector< std::string > &row);
