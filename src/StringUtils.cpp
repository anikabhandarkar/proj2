#include "StringUtils.h"
#include <algorithm>
#include <cctype>
#include <vector>

namespace StringUtils{

std::string Slice(const std::string &str, ssize_t start, ssize_t end) noexcept{
    ssize_t strLength = str.size(); // have the whole length of the string
    
    // Adjust the start index for negative values
    if (start < 0) { 
        start += strLength; // calculate the start from the end of the string
    }
    start = std::max(ssize_t(0), std::min(start, strLength)); // limit the start index so that it is within range

    // Adjust the end index and make it less than and equal to zero for negative values
    if (end <= 0) {
        end += strLength; // calculate end from the end or set to the last character
    }
    end = std::max(ssize_t(0), std::min(end, strLength)); // end to a valid range 

    // Compute the length of slice to ensure it is a non-negative value
    ssize_t sliceLength = std::max(ssize_t(0), end - start);

    // Return specified substring
    return str.substr(start, sliceLength);
}

std::string Capitalize(const std::string &str) noexcept{
    if (str.empty()) {
        return str; // returns string if empty
    }
    std::string result = str;
    result[0] = std::toupper(result[0]); // capitalize the first character
    for (size_t i = 1; i < result.size(); i++) {
        result[i] = std::tolower(result[i]); // lowercase the rest of the characters
    }
    return result; // return all the characters
}

std::string Upper(const std::string &str) noexcept{
    std::string result = str;
    // Using the transform function allows you to change the string to make it
    // all uppercase.
    std::transform(result.begin(), result.end(), result.begin(),::toupper);
    return result;
}

std::string Lower(const std::string &str) noexcept{
    std::string result = str;
    // Using the transform function allows you to change the string to make it
    // all lowercase.
    std::transform(result.begin(), result.end(), result.begin(),::tolower);
    return result;
}

std::string LStrip(const std::string &str) noexcept{
    int start = 0;
    // iterate through string until non-whitespace char is found
    while (start < str.size() && isspace(str[start])) {
        start++; // increment to next char
    }
    // return substring from first non-whitespace char
    return str.substr(start);
}

std::string RStrip(const std::string &str) noexcept{
    int end = str.size() - 1; // initialize end index to the last char
    // iterate backward through string until non-whitespace char is found
    while (end >= 0 && isspace(str[end])) { 
        end--; // move to previous char
    }
    // iterate from beginning to last non-whitespace char
    return str.substr(0, end + 1);
}

std::string Strip(const std::string &str) noexcept{
    // removes whitespace from beginning and end of string
    return RStrip(LStrip(str));
}

std::string Center(const std::string &str, int width, char fill) noexcept{
    if (str.size() >= width) {
        return str; // return string if it is already at or exceeds the width
    }
    int paddingTotal = width - str.size(); // calculates total padding
    int paddingLeft = paddingTotal / 2; // calculates the padding on the left side
    int paddingRight = paddingTotal - paddingLeft; // calculates the padding on the right side
    return std::string(paddingLeft, fill) + str + std::string(paddingRight, fill); // returns the string with padding
}

std::string LJust(const std::string &str, int width, char fill) noexcept{
    if (str.size() >= width) {
        return str; // return string if it is already at or exceeds the width
    }
    return str + std::string(width - str.size(), fill); // returns string with padding on the left side
}

std::string RJust(const std::string &str, int width, char fill) noexcept{
    if (str.size() >= width) {
        return str; // return string if it is already at or exceeds the width
    }
    return std::string(width - str.size(), fill) + str; // returns string with padding on the right side
}

std::string Replace(const std::string &str, const std::string &old, const std::string &rep) noexcept{
    if (old.empty()) {
        return str; // return string if the old string is empty
    }
    std::string result = str;
    size_t position = 0; // initialize position to 0
    while ((position = result.find(old, position)) != std::string::npos) {
        result.replace(position, old.size(), rep); // replaces old string with replacement string
        position += rep.length(); // increment position by the size of the replacement string
    }
    return result; // return the string with the replacement string;
}

std::vector< std::string > Split(const std::string &str, const std::string &splt) noexcept{
    std::vector< std::string > result; // initializes vector to store the split strings
    size_t start = 0; // initializes start index to 0
    size_t end = 0; // initializes end index to 0

    if (str.empty()) { 
        return result; // return empty vector if the split string is empty
    }
    
    if (splt.empty()) { // split by whitespace
        std::string word; // initializes string to store the split string
        for (size_t i = 0; i < str.size(); i++) {
            if (std::isspace(str[i])) {
                if (!word.empty()) {
                    result.push_back(word); // add the word to the vector
                    word.clear(); // clear the word
                }
            } else {
                word += str[i]; // add the char to the word
            }
        }
        if (!word.empty()) {
            result.push_back(word); // add the last word to the vector
        }
    } else {
        // split by the specified string
        while ((end = str.find(splt, start)) != std::string::npos) {
            result.push_back(str.substr(start, end - start)); // add the substring to the vector
            start = end + splt.length(); // update the start index
        }
        result.push_back(str.substr(start)); // add the last substring to the vector
    }
    return result;
}

std::string Join(const std::string &str, const std::vector< std::string > &vect) noexcept{
    if (vect.empty()) {
        return " "; // return empty string if the vector is empty
    }
    std::string result = vect[0]; // this initializes the result to the first element of the vector
    // iterate through the vector and join the strings
    for (size_t i = 1; i < vect.size(); i++) {
        result += str + vect[i]; // add string and vector element to the result
    }
    return result; // returns joined string
}

std::string ExpandTabs(const std::string &str, int tabsize) noexcept{
    std::string result; // initializes the result string
    int currentColumn = 0; // initializes the current column
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '\t') {
            size_t spaces = 0;
            if (tabsize != 0) {
                spaces = tabsize - (currentColumn % tabsize);
            }
            for (int j = 0; j < spaces; j++) {
                result += ' '; // add spaces to the result
            }

            currentColumn += spaces; // increment the current column
        } else {
            result += str[i]; // add the character to the result
            currentColumn++; // increment the current column
        }
    }
    return result; // return the expanded string
}

int EditDistance(const std::string &left, const std::string &right, bool ignorecase) noexcept{
    std::string strLeft= left; // initializes the left string
    std::string strRight = right; // initializes the right string

    // this would convert strings to lowercase if case is ignored
    if (ignorecase) {
        strLeft= Lower(left); // converts the left string to lowercase
        strRight = Lower(right); // converts the right string to lowercase
    }

    int stringLengthL = strLeft.size(); // initializes the size of the left string
    int stringLengthR = strRight.size(); // initializes the size of the right string

    if (stringLengthL < stringLengthR) {
        // remember learning this from ECS32C
        // we implemented the swap function as it swaps the values of two variables
        // this is used here as it swaps the values of the two strings.
        std::swap(strLeft, strRight); // swaps the strings if the left string is smaller
        std::swap(stringLengthL, stringLengthR); // swaps the sizes of the strings
    }

    // creates a 2D vector to store the distance between the strings
    std::vector<std::vector<int>> distance(stringLengthL + 1, std::vector<int>(stringLengthR + 1));

    // initializes the distance between the strings
    for (int i = 0; i <= stringLengthL; i++) {
        distance[i][0] = i;
    }
    for (int j = 1; j <= stringLengthR; j++) {
        distance[0][j] = j; 
    }

    // calculates the distance between the strings
    // i learned this from ECS122A where we used dynamic programming to problems involving cost and distance
    // i used this here as the Levenshtein distance is calculated using dynamic programming
    for (int i = 1; i <= stringLengthL; i++) {
        for (int j = 1; j <= stringLengthR; j++) {
            int deletion = distance[i - 1][j] + 1; // calculates the deletion distance
            int insertion = distance[i][j - 1] + 1; // calculates the insertion distance
            int substitution = distance[i - 1][j - 1] + (strLeft[i - 1] != strRight[j - 1]); // calculates the substitution distance

            // calculates the minimum distance
            distance[i][j] = std::min({deletion, insertion, substitution});
        }
    }
    return distance[strLeft.length()][strRight.length()]; // returns the distance between the strings
}

};