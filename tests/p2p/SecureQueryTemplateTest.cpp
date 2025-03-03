#include <gtest/gtest.h>
#include "../../src/p2p/coordinator/SecureDatabaseSync.hpp"

using namespace rathena::p2p;

class SecureQueryTemplateTest : public ::testing::Test {
protected:
    void SetUp() override {
        simple_template = SecureQueryTemplate::create(
            "SELECT * FROM characters WHERE char_id = ? /* int */");
        
        complex_template = SecureQueryTemplate::create(
            "INSERT INTO inventory (char_id, nameid, amount, identify) "
            "VALUES (?, ?, ?, ?) /* int */ /* int */ /* int */ /* int */");
        
        text_template = SecureQueryTemplate::create(
            "INSERT INTO mail (sender_id, receiver_id, title, message) "
            "VALUES (?, ?, ?, ?) /* int */ /* int */ /* string */ /* string */");
    }

    SecureQueryTemplate simple_template;
    SecureQueryTemplate complex_template;
    SecureQueryTemplate text_template;
};

TEST_F(SecureQueryTemplateTest, ValidateIntegerParameters) {
    EXPECT_TRUE(simple_template.validate_parameters({"123"}));
    EXPECT_TRUE(simple_template.validate_parameters({"-456"}));
    EXPECT_FALSE(simple_template.validate_parameters({"12.34"}));
    EXPECT_FALSE(simple_template.validate_parameters({"abc"}));
    EXPECT_FALSE(simple_template.validate_parameters({"123abc"}));
}

TEST_F(SecureQueryTemplateTest, ValidateMultipleParameters) {
    EXPECT_TRUE(complex_template.validate_parameters({"1", "2", "3", "4"}));
    EXPECT_FALSE(complex_template.validate_parameters({"1", "2", "3"}));  // Too few
    EXPECT_FALSE(complex_template.validate_parameters({"1", "2", "3", "4", "5"}));  // Too many
    EXPECT_FALSE(complex_template.validate_parameters({"1", "abc", "3", "4"}));  // Invalid type
}

TEST_F(SecureQueryTemplateTest, ValidateStringParameters) {
    EXPECT_TRUE(text_template.validate_parameters({
        "1",
        "2",
        "Test Title",
        "Hello, this is a test message!"
    }));
    
    // Test string with special characters
    EXPECT_TRUE(text_template.validate_parameters({
        "1",
        "2",
        "Title with 'quotes'",
        "Message with \"quotes\" and \\ backslashes"
    }));
    
    // Test invalid parameters
    EXPECT_FALSE(text_template.validate_parameters({
        "abc",  // Invalid int
        "2",
        "Title",
        "Message"
    }));
}

TEST_F(SecureQueryTemplateTest, BuildSimpleQuery) {
    auto query = simple_template.build_query({"123"});
    EXPECT_EQ(query, "SELECT * FROM characters WHERE char_id = 123");
}

TEST_F(SecureQueryTemplateTest, BuildComplexQuery) {
    auto query = complex_template.build_query({"1", "2", "3", "4"});
    EXPECT_EQ(query,
        "INSERT INTO inventory (char_id, nameid, amount, identify) "
        "VALUES (1, 2, 3, 4)");
}

TEST_F(SecureQueryTemplateTest, BuildQueryWithStrings) {
    auto query = text_template.build_query({
        "1",
        "2",
        "Test Title",
        "Hello, World!"
    });
    EXPECT_EQ(query,
        "INSERT INTO mail (sender_id, receiver_id, title, message) "
        "VALUES (1, 2, 'Test Title', 'Hello, World!')");
}

TEST_F(SecureQueryTemplateTest, SqlInjectionPrevention) {
    auto query = text_template.build_query({
        "1",
        "2",
        "'; DROP TABLE users; --",
        "' OR '1'='1"
    });
    
    // Check if special characters are properly escaped
    EXPECT_TRUE(query.find("''") != std::string::npos);
    EXPECT_FALSE(query.find(";") != std::string::npos);
    EXPECT_FALSE(query.find("DROP") != std::string::npos);
}

TEST_F(SecureQueryTemplateTest, ExceptionOnInvalidParameters) {
    EXPECT_THROW(
        simple_template.build_query({"invalid"}),
        std::runtime_error
    );
    
    EXPECT_THROW(
        complex_template.build_query({"1", "2", "3"}),  // Missing parameter
        std::runtime_error
    );
    
    EXPECT_THROW(
        text_template.build_query({"1", "2", std::string(70000, 'a'), "msg"}),  // Too long
        std::runtime_error
    );
}

TEST_F(SecureQueryTemplateTest, TemplateHashing) {
    // Create two identical templates
    auto template1 = SecureQueryTemplate::create(
        "SELECT * FROM table WHERE id = ? /* int */");
    auto template2 = SecureQueryTemplate::create(
        "SELECT * FROM table WHERE id = ? /* int */");
    
    // Create a different template
    auto template3 = SecureQueryTemplate::create(
        "SELECT * FROM table WHERE name = ? /* string */");
    
    // Identical templates should have the same hash
    EXPECT_EQ(template1.get_hash(), template2.get_hash());
    
    // Different templates should have different hashes
    EXPECT_NE(template1.get_hash(), template3.get_hash());
}

TEST_F(SecureQueryTemplateTest, SpecialCharacterHandling) {
    auto template_with_special = SecureQueryTemplate::create(
        "INSERT INTO messages (text) VALUES (?) /* string */");
    
    std::vector<std::pair<std::string, std::string>> test_cases = {
        {"Test\nNewline", "'Test\\nNewline'"},
        {"Tab\tCharacter", "'Tab\\tCharacter'"},
        {"Back\\slash", "'Back\\\\slash'"},
        {"'Single'Quote'", "'''Single''Quote'''"},
        {"\"Double\"Quotes\"", "'\"Double\"Quotes\"'"},
        {"Null\0Byte", "'Null\\0Byte'"},
        {"Mixed'\"\\Characters", "'Mixed''\"\\\\Characters'"}
    };
    
    for (const auto& test_case : test_cases) {
        auto query = template_with_special.build_query({test_case.first});
        EXPECT_TRUE(query.find(test_case.second) != std::string::npos)
            << "Failed to properly escape: " << test_case.first;
    }
}

TEST_F(SecureQueryTemplateTest, LargeParameterHandling) {
    auto large_template = SecureQueryTemplate::create(
        "INSERT INTO large_data (data) VALUES (?) /* string */");
    
    // Test with a large but valid string
    std::string large_string(65000, 'x');
    EXPECT_NO_THROW(large_template.build_query({large_string}));
    
    // Test with a string that's too large
    std::string too_large_string(66000, 'x');
    EXPECT_THROW(
        large_template.build_query({too_large_string}),
        std::runtime_error
    );
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}