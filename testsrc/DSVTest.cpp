
#include "DSVReader.h"
#include "DSVWriter.h"
#include <gtest/gtest.h>

// Quick test helpers
class DataString : public CDataSink {
    std::string stuff;
public:
    bool Put(char c) override { stuff += c; return true; }
    auto Get() { return stuff; }
};

class StrSource : public CDataSource {
    std::string data; 
    unsigned p = 0;
public:
    StrSource(std::string s) : data(std::move(s)) {}
    bool End() const override { return p >= data.length(); }
    bool Get(char& c) override { 
        if(End()) return false; 
        c = data[p++]; 
        return true; 
    }
    bool Peek(char& c) override { 
        if(End()) return false; 
        c = data[p]; 
        return true; 
    }
};

// Main tests
TEST(DsvStuff, AllInOne) {
    // Basic read-write
    {
        auto src = std::make_shared<StrSource>("a,b,c\n");
        auto sink = std::make_shared<DataString>();
        
        CDSVReader r(src, ',');
        CDSVWriter w(sink, ',');
        
        std::vector<std::string> row;
        r.ReadRow(row);
        w.WriteRow(row);
        
        EXPECT_EQ(sink->Get(), "a,b,c\n");
    }
    
    // Handle quotes & special chars
    {
        auto src = std::make_shared<StrSource>("\"a,b\",\"x\"\"y\"\n");
        std::vector<std::string> row;
        
        CDSVReader(src, ',').ReadRow(row);
        
        EXPECT_EQ(row[0], "a,b");
        EXPECT_EQ(row[1], "x\"y");  // double quote gets unescaped
    }
    
    // Always quote mode
    {
        auto sink = std::make_shared<DataString>();
        
        CDSVWriter w(sink, ',', true);  // true = always quote
        w.WriteRow({"a", "b"});
        
        EXPECT_EQ(sink->Get(), "\"a\",\"b\"\n");
    }
    
    // Bad args should fail
    {
        auto sink = std::make_shared<DataString>();
        EXPECT_THROW(CDSVWriter(nullptr, ','), std::invalid_argument);
        EXPECT_THROW(CDSVReader(nullptr, ','), std::invalid_argument);
        EXPECT_THROW(CDSVWriter(sink, 0), std::invalid_argument);  // null char delimiter
    }
}
