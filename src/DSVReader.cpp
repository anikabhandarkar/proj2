#include "DSVReader.h"
#include "StringUtils.h"
#include <sstream>

struct CDSVReader::SImplementation{
    std::shared_ptr<CDataSource> Source;
    char Delimiter;

    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
        : Source(src), Delimiter(delimiter){}
};

// Constructor for DSV reader, src specifies the data source and delimiter
// specifies the delimiting character
//CDSVReader::CDSVReader(std::shared_ptr< CDataSource > src, char delimiter)
    //:DImplementation(std::make_unique<SImplementation>(src, delimiter)){ }


// Destructor for DSV reader
// CDSVReader::~CDSVReader() = default;


// Returns true if all rows have been read from the DSV
// bool CDSVReader::End() const{
    // return DImplementation->Source->End();
//}
// Returns true if the row is successfully read, one string will be put in
// the row per column
bool CDSVReader::ReadRow(std::vector< std::string > &row){
    row.clear();
    std::string right;
    char ch;
    bool quotes = false;

    while(!DImplementation->Source->End()){
        if(DImplementation->Source->Get(ch)){
            break;
        }

        if(ch == '"'){
            quotes = !quotes;
        }
        else if(ch == DImplementation->Delimiter && !quotes){
            row.push_back(right);
            right.clear();
        }
        else if(ch == '\n' && !quotes){
            row.push_back(right);
            return true;
        }
        else{
            right += ch;
        }
    }

    if(!right.empty() || !row.empty()){
        row.push_back(right);
        return true;
    }
    return false;
}

CDSVReader::CDSVReader(std::shared_ptr< CDataSource > src, char delimiter)
    :DImplementation(std::make_unique<SImplementation>(src, delimiter)){ }


// Destructor for DSV reader
CDSVReader::~CDSVReader() = default;


// Returns true if all rows have been read from the DSV
bool CDSVReader::End() const{
    return DImplementation->Source->End();
}
