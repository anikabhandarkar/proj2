#include "DSVReader.h"
#include "DSVWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include <gtest/gtest.h>

TEST(DSVTest, BasicReadWrite) {
    std::shared_ptr<CStringDataSource> src = std::make_shared<CStringDataSource>("hello,world,hi\na,b,c\n");
    std::shared_ptr<CStringDataSink> sink = std::make_shared<CStringDataSink>();

    CDSVReader reader(src, ',');
    CDSVWriter writer(sink, ',');

    std::vector<std::string> row;
    while (reader.ReadRow(row)) { 
        writer.WriteRow(row);
        
    }
EXPECT_EQ(sink->String(), "hello,world,hi\na,b,c\n"); 
}
