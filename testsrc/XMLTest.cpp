#include "XMLReader.h"
#include "XMLWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include <gtest/gtest.h> 

// define a test case for basic XML read and write operations
TEST(XMLTest, BasicReadWrite) {
    // create a data source from a string containing XML data
    std::shared_ptr<CStringDataSource> src = std::make_shared<CStringDataSource>("<tag>data</tag>");

    // create a data sink to capture output from the XML writer
    std::shared_ptr<CStringDataSink> sink = std::make_shared<CStringDataSink>();

    // instantiate the XML reader with the string data source
    CXMLReader reader(src);

    // instantiate the XML writer with the string data sink
    CXMLWriter writer(sink);

    // define an XML entity to store data read from the XML reader
    SXMLEntity entity;

    // continuously read from the XML reader until the end of the data is reached
    while (!reader.End()) {
        // attempt to read an entity from the XML stream
        if (reader.ReadEntity(entity)) {
            // if an entity is successfully read, write it to the XML writer
            writer.WriteEntity(entity);
        }
    }

    // After all entities are processed, check if the output matches the original input
    EXPECT_EQ(sink->String(), "<tag>data</tag>");
}
