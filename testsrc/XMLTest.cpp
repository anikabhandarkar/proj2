// define a test case named 'EnhancedReadWriteTest' in the test suite 'XMLTest'
TEST(XMLTest, EnhancedReadWriteTest) {
    // create a shared pointer for the XML input source, initializing it with some nested XML content
    std::shared_ptr<CStringDataSource> xmlInput = std::make_shared<CStringDataSource>("<root><child>example</child></root>");
    // create a shared pointer for the XML output sink.
    std::shared_ptr<CStringDataSink> xmlOutput = std::make_shared<CStringDataSink>();

    // initialize an XML reader with the input source
    CXMLReader xmlReader(xmlInput);
    // initialize an XML writer with the output sink
    CXMLWriter xmlWriter(xmlOutput);

    // create an XML entity object to hold data read from the input
    SXMLEntity xmlEntity;
    // continue reading from the XML source until the end is reached
    while (!xmlReader.End()) {
        // attempt to read an entity from the XML reader
        if (xmlReader.ReadEntity(xmlEntity)) {
            // if an entity is successfully read, write it to the XML writer
            xmlWriter.WriteEntity(xmlEntity);
        }
    }

    // assert that the output string matches the expected XML string
    ASSERT_EQ(xmlOutput->String(), "<root><child>example</child></root>");
    // assert that the XML reader has reached the end of the data stream, ensuring all data was processed
    ASSERT_TRUE(xmlReader.End()); // additional check to ensure reader is at the end of the stream
}
