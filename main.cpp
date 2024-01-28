#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string_view>
#include <cstdint>
#include <string>
#include <random>
#include <limits>
#include <memory>
#include <regex>

int random_int(int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max())
{
    static std::random_device rd;
    static std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}

std::string get_obfuscate(int num)
{
    // get the deep and target_level of the tree
    int deep = random_int(1, 10);
    int target_level = random_int(0, deep - 1);

    std::cout << "num: " << num << std::endl;
    std::cout << "deep: " << deep << std::endl;
    std::cout << "target_level: " << target_level << std::endl;

    // input is a int, we use binary search tree to indicate the target string should be placed
    // we put our obfuscate string in the middle of the tree (target_level)
    // other places are padding random bytes (int type)
    constexpr auto obfuscate_operand = "(!(-![]{}))";
    constexpr auto trinary_operator_format = "((%ul & 1) ? (%s) : (%s))";

    std::string obfuscate_string = "(%s)";

    for (int i = 0; i < deep; i++)
    {
        std::string target_obfus = "(%s)";
        std::string padding_string = "";
        if (i == target_level)
        {
            target_obfus = "";
            for (int j = 0; j < num; j++)
            {
                target_obfus += obfuscate_operand;
                target_obfus += "+";
            }
            // remove the last '+'
            target_obfus.pop_back();

            std::cout << "target_obfus: " << target_obfus << std::endl;
        }

        // padding random bytes
        const int random_times = random_int(1, 10);
        for (int j = 0; j < random_times; j++)
        {
            padding_string += obfuscate_operand;
            padding_string += "+";
        }
        // remove the last '+'
        padding_string.pop_back();

        std::cout << "padding_string: " << padding_string << std::endl;
        std::cout << "current position: " << i << std::endl;

        // Generate a random number for the ternary operator's condition.
        int condition = random_int();
        // Allocate enough space for the string.
        auto buffer = std::make_unique<char[]>(padding_string.length() + target_obfus.length() + 128);
        // Format the string using the ternary operator.
        snprintf(buffer.get(), padding_string.length() + target_obfus.length() + 128, trinary_operator_format,
                 condition & 1,
                 condition & 1 ? target_obfus.c_str() : padding_string.c_str(),
                 condition & 1 ? padding_string.c_str() : target_obfus.c_str());

        // use obfuscate_string format and replace the %s with the buffer in c++11
        obfuscate_string = std::regex_replace(obfuscate_string, std::regex("%s"), buffer.get());

        std::cout << "obfuscate_string: " << obfuscate_string << std::endl;
    }

    return obfuscate_string;
}

bool write_hex_to_binary_file(std::string_view hex_data)
{
    // Open out.cpp in binary mode
    std::ofstream out_file("out.cpp", std::ios::out | std::ios::binary);
    if (!out_file.is_open())
    {
        std::cerr << "Failed to open out.cpp for writing.\n";
        return false;
    }

    // Write header to file
    constexpr auto header =
        "#include <iostream>\n"
        "\n"
        "int main()\n"
        "{\n"
        "char out[] = {";
    out_file << header;

    // Convert hex string to binary data and write to file
    for (size_t i = 0; i < hex_data.length(); i += 2)
    {
        // Get two characters from hex string and convert to byte
        char byte_string[3] = {hex_data[i], hex_data[i + 1], '\0'};
        char byte = static_cast<char>(std::stoi(byte_string, nullptr, 16));
        if (byte != 0)
            out_file << get_obfuscate(byte);
        // final output "-(-![]{})" is zero and ',' separator
        out_file << "-(-![]{})"
                 << ",";
    }

    // Write footer to file
    constexpr auto footer =
        "};\n"
        "for (char &c : out)\n"
        "std::cout << c;\n"
        "return 0;\n"
        "}";
    out_file << footer;
    return true;
}

std::string string_to_hex_array(const std::string &input_string)
{
    std::stringstream hex_stream;
    for (char character : input_string)
        hex_stream << std::hex << std::setfill('0') << std::setw(2) << (int)character;
    return hex_stream.str();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <string>\n";
        return 1;
    }

    std::string input_string = argv[1];
    std::string hex_array = string_to_hex_array(input_string);
    if (!write_hex_to_binary_file(hex_array))
    {
        std::cerr << "Failed to write hex data to file.\n";
        return 1;
    }

    return 0;
}
