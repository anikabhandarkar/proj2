#include <gtest/gtest.h>
#include "DSVReader.h"
#include "DSVWriter.h"
#include <memory>
#include <string>
#include <vector>

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

    bool Peek(char& ch) override {
        if (End()) return false;
        ch = Data[Position];
        return true;
    }
};

class DSVTest : public ::testing::Test {
protected:
    std::shared_ptr<MockDataSink> Sink;
    std::shared_ptr<MockDataSource> Source;
    
    void SetUp() override {
        Sink = std::make_shared<MockDataSink>();
    }
};

// Writer Tests
TEST_F(DSVTest, WriterBasicRow) {
    CDSVWriter writer(Sink, ',');
    std::vector<std::string> row = {"a", "b", "c"};
    ASSERT_TRUE(writer.WriteRow(row));
    EXPECT_EQ(Sink->GetData(), "a,b,c\n");
}

TEST_F(DSVTest, WriterEmptyRow) {
    CDSVWriter writer(Sink, ',');
    std::vector<std::string> row;
    ASSERT_TRUE(writer.WriteRow(row));
    EXPECT_EQ(Sink->GetData(), "\n");
}

TEST_F(DSVTest, WriterQuotedFields) {
    CDSVWriter writer(Sink, ',');
    std::vector<std::string> row = {"a,b", "c\"d", "e\nf"};
    ASSERT_TRUE(writer.WriteRow(row));
    EXPECT_EQ(Sink->GetData(), "\"a,b\",\"c\"\"d\",\"e\nf\"\n");
}

TEST_F(DSVTest, WriterQuoteAll) {
    CDSVWriter writer(Sink, ',', true);
    std::vector<std::string> row = {"a", "b", "c"};
    ASSERT_TRUE(writer.WriteRow(row));
    EXPECT_EQ(Sink->GetData(), "\"a\",\"b\",\"c\"\n");
}

// Reader Tests
TEST_F(DSVTest, ReaderBasicRow) {
    Source = std::make_shared<MockDataSource>("a,b,c\n");
    CDSVReader reader(Source, ',');
    std::vector<std::string> row;
    ASSERT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "a");
    EXPECT_EQ(row[1], "b");
    EXPECT_EQ(row[2], "c");
    EXPECT_TRUE(reader.End());
}

TEST_F(DSVTest, ReaderEmptyRow) {
    Source = std::make_shared<MockDataSource>("\n");
    CDSVReader reader(Source, ',');
    std::vector<std::string> row;
    ASSERT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row.size(), 0);
    EXPECT_TRUE(reader.End());
}

TEST_F(DSVTest, ReaderQuotedFields) {
    Source = std::make_shared<MockDataSource>("\"a,b\",\"c\"\"d\",\"e\nf\"\n");
    CDSVReader reader(Source, ',');
    std::vector<std::string> row;
    ASSERT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "a,b");
    EXPECT_EQ(row[1], "c\"d");
    EXPECT_EQ(row[2], "e\nf");
    EXPECT_TRUE(reader.End());
}

TEST_F(DSVTest, ReaderCRLF) {
    Source = std::make_shared<MockDataSource>("a,b,c\r\nd,e,f\r\n");
    CDSVReader reader(Source, ',');
    std::vector<std::string> row;
    
    ASSERT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "a");
    EXPECT_EQ(row[1], "b");
    EXPECT_EQ(row[2], "c");
    
    ASSERT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "d");
    EXPECT_EQ(row[1], "e");
    EXPECT_EQ(row[2], "f");
    
    EXPECT_TRUE(reader.End());
}

TEST_F(DSVTest, ReaderNoFinalNewline) {
    Source = std::make_shared<MockDataSource>("a,b,c");
    CDSVReader reader(Source, ',');
    std::vector<std::string> row;
    ASSERT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "a");
    EXPECT_EQ(row[1], "b");
    EXPECT_EQ(row[2], "c");
    EXPECT_TRUE(reader.End());
}

TEST_F(DSVTest, WriterReaderIntegration) {
    // Write some data
    CDSVWriter writer(Sink, ',');
    std::vector<std::string> writeRow = {"hello,world", "test\"quote", "new\nline"};
    ASSERT_TRUE(writer.WriteRow(writeRow));
    
    // Read it back
    Source = std::make_shared<MockDataSource>(Sink->GetData());
    CDSVReader reader(Source, ',');
    std::vector<std::string> readRow;
    ASSERT_TRUE(reader.ReadRow(readRow));
    
    EXPECT_EQ(writeRow.size(), readRow.size());
    for (size_t i = 0; i < writeRow.size(); ++i) {
        EXPECT_EQ(writeRow[i], readRow[i]);
    }
}

// Error Cases
TEST_F(DSVTest, WriterNullSink) {
    EXPECT_THROW(CDSVWriter(nullptr, ','), std::invalid_argument);
}

TEST_F(DSVTest, ReaderNullSource) {
    EXPECT_THROW(CDSVReader(nullptr, ','), std::invalid_argument);
}

TEST_F(DSVTest, WriterNullDelimiter) {
    EXPECT_THROW(CDSVWriter(Sink, '\0'), std::invalid_argument);
}

TEST_F(DSVTest, ReaderNullDelimiter) {
    EXPECT_THROW(CDSVReader(Source, '\0'), std::invalid_argument);
}
