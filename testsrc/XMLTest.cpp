#include <gtest/gtest.h>
#include "XMLReader.h"
#include "XMLWriter.h"
#include <memory>
#include <string>
#include <sstream>

// Mock classes for testing
class MockDataSink : public CDataSink {
private:
    std::string Data;
public:
    bool Put(char ch) override {
        Data += ch;
        return true;
    }
    
    const std::string& GetData() const { return Data; }
};

class MockDataSource : public CDataSource {
private:
    std::string Data;
    size_t Position = 0;
public:
    MockDataSource(const std::string& data) : Data(data) {}

    bool End() const override {
        return Position >= Data.length();
    }

    bool Get(char& ch) override {
        if (End()) return false;
        ch = Data[Position++];
        return true;
    }
};

class XMLTest : public ::testing::Test {
protected:
    std::shared_ptr<MockDataSink> Sink;
    std::shared_ptr<MockDataSource> Source;
    
    void SetUp() override {
        Sink = std::make_shared<MockDataSink>();
    }

    // Helper function to create an XML entity
    SXMLEntity CreateStartElement(const std::string& name, 
                                const std::vector<std::pair<std::string, std::string>>& attrs = {}) {
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::StartElement;
        entity.DNameData = name;
        entity.DAttributes = attrs;
        return entity;
    }

    SXMLEntity CreateEndElement(const std::string& name) {
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::EndElement;
        entity.DNameData = name;
        return entity;
    }

    SXMLEntity CreateCharData(const std::string& data) {
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::CharData;
        entity.DNameData = data;
        return entity;
    }

    SXMLEntity CreateCompleteElement(const std::string& name,
                                   const std::vector<std::pair<std::string, std::string>>& attrs = {}) {
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::CompleteElement;
        entity.DNameData = name;
        entity.DAttributes = attrs;
        return entity;
    }
};

// Writer Tests
TEST_F(XMLTest, WriteSimpleElement) {
    CXMLWriter writer(Sink);
    
    ASSERT_TRUE(writer.WriteEntity(CreateStartElement("root")));
    ASSERT_TRUE(writer.WriteEntity(CreateEndElement("root")));
    
    EXPECT_EQ(Sink->GetData(), "<root></root>");
}

TEST_F(XMLTest, WriteElementWithAttributes) {
    CXMLWriter writer(Sink);
    
    ASSERT_TRUE(writer.WriteEntity(CreateStartElement("elem", {{"attr1", "value1"}, {"attr2", "value2"}})));
    ASSERT_TRUE(writer.WriteEntity(CreateEndElement("elem")));
    
    EXPECT_EQ(Sink->GetData(), "<elem attr1=\"value1\" attr2=\"value2\"></elem>");
}

TEST_F(XMLTest, WriteCharacterData) {
    CXMLWriter writer(Sink);
    
    ASSERT_TRUE(writer.WriteEntity(CreateStartElement("root")));
    ASSERT_TRUE(writer.WriteEntity(CreateCharData("Hello & Goodbye")));
    ASSERT_TRUE(writer.WriteEntity(CreateEndElement("root")));
    
    EXPECT_EQ(Sink->GetData(), "<root>Hello &amp; Goodbye</root>");
}

TEST_F(XMLTest, WriteCompleteElement) {
    CXMLWriter writer(Sink);
    
    ASSERT_TRUE(writer.WriteEntity(CreateCompleteElement("empty", {{"attr", "value"}})));
    
    EXPECT_EQ(Sink->GetData(), "<empty attr=\"value\"/>");
}

TEST_F(XMLTest, WriteSpecialCharacters) {
    CXMLWriter writer(Sink);
    
    ASSERT_TRUE(writer.WriteEntity(CreateStartElement("root")));
    ASSERT_TRUE(writer.WriteEntity(CreateCharData("<>&'\"")));
    ASSERT_TRUE(writer.WriteEntity(CreateEndElement("root")));
    
    EXPECT_EQ(Sink->GetData(), "<root>&lt;&gt;&amp;&apos;&quot;</root>");
}

// Reader Tests
TEST_F(XMLTest, ReadSimpleElement) {
    Source = std::make_shared<MockDataSource>("<root></root>");
    CXMLReader reader(Source);
    
    SXMLEntity entity;
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "root");
    
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(entity.DNameData, "root");
    
    EXPECT_TRUE(reader.End());
}

TEST_F(XMLTest, ReadElementWithAttributes) {
    Source = std::make_shared<MockDataSource>("<elem attr1=\"value1\" attr2=\"value2\"></elem>");
    CXMLReader reader(Source);
    
    SXMLEntity entity;
    ASSERT_TRUE(reader.ReadEntity(entity));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "elem");
    EXPECT_EQ(entity.DAttributes.size(), 2);
    EXPECT_EQ(entity.DAttributes[0].first, "attr1");
    EXPECT_EQ(entity.DAttributes[0].second, "value1");
    EXPECT_EQ(entity.DAttributes[1].first, "attr2");
    EXPECT_EQ(entity.DAttributes[1].second, "value2");
}

TEST_F(XMLTest, ReadCharacterData) {
    Source = std::make_shared<MockDataSource>("<root>Hello &amp; Goodbye</root>");
    CXMLReader reader(Source);
    
    SXMLEntity entity;
    ASSERT_TRUE(reader.ReadEntity(entity)); // Start element
    
    ASSERT_TRUE(reader.ReadEntity(entity)); // Character data
    EXPECT_EQ(entity.DType, SXMLEntity::EType::CharData);
    EXPECT_EQ(entity.DNameData, "Hello & Goodbye");
}

TEST_F(XMLTest, SkipCharacterData) {
    Source = std::make_shared<MockDataSource>("<root>  <child/>  </root>");
    CXMLReader reader(Source);
    
    SXMLEntity entity;
    ASSERT_TRUE(reader.ReadEntity(entity, true)); // Start element, skip whitespace
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "root");
    
    ASSERT_TRUE(reader.ReadEntity(entity, true)); // Complete element, skip whitespace
    EXPECT_EQ(entity.DNameData, "child");
}

TEST_F(XMLTest, ReadNestedElements) {
    Source = std::make_shared<MockDataSource>("<root><child1/><child2></child2></root>");
    CXMLReader reader(Source);
    
    SXMLEntity entity;
    std::vector<SXMLEntity::EType> expectedTypes = {
        SXMLEntity::EType::StartElement,  // root
        SXMLEntity::EType::CompleteElement,  // child1
        SXMLEntity::EType::StartElement,  // child2
        SXMLEntity::EType::EndElement,    // child2
        SXMLEntity::EType::EndElement     // root
    };
    
    for (const auto& expectedType : expectedTypes) {
        ASSERT_TRUE(reader.ReadEntity(entity));
        EXPECT_EQ(entity.DType, expectedType);
    }
    
    EXPECT_TRUE(reader.End());
}

// Integration Tests
TEST_F(XMLTest, WriteReadIntegration) {
    // Write complex XML
    CXMLWriter writer(Sink);
    ASSERT_TRUE(writer.WriteEntity(CreateStartElement("root", {{"version", "1.0"}})));
    ASSERT_TRUE(writer.WriteEntity(CreateStartElement("child")));
    ASSERT_TRUE(writer.WriteEntity(CreateCharData("Hello & Goodbye")));
    ASSERT_TRUE(writer.WriteEntity(CreateEndElement("child")));
    ASSERT_TRUE(writer.WriteEntity(CreateCompleteElement("empty", {{"attr", "value"}})));
    ASSERT_TRUE(writer.WriteEntity(CreateEndElement("root")));
    
    // Read it back
    Source = std::make_shared<MockDataSource>(Sink->GetData());
    CXMLReader reader(Source);
    
    SXMLEntity entity;
    std::vector<SXMLEntity::EType> expectedTypes = {
        SXMLEntity::EType::StartElement,  // root
        SXMLEntity::EType::StartElement,  // child
        SXMLEntity::EType::CharData,      // Hello & Goodbye
        SXMLEntity::EType::EndElement,    // child
        SXMLEntity::EType::CompleteElement, // empty
        SXMLEntity::EType::EndElement     // root
    };
    
    for (const auto& expectedType : expectedTypes) {
        ASSERT_TRUE(reader.ReadEntity(entity));
        EXPECT_EQ(entity.DType, expectedType);
    }
    
    EXPECT_TRUE(reader.End());
}
