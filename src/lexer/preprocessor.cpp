#include <fstream>
#include <iostream>
#include <vector>
#include <regex>
#include <cstring>

#include "../../include/lexer/preprocessor.hpp"

namespace {

class FileInfo
{
public:
    FileInfo(std::string_view absolute_path)
        : m_path(absolute_path.substr(0, absolute_path.find_last_of('/')    ))
        , m_name(absolute_path.substr(   absolute_path.find_last_of('/') + 1))
    { }

    const std::string& name() const noexcept
    {
        return m_name;
    }

    const std::string& path() const noexcept
    {
        return m_path;
    }

private:
    std::string m_path;
    std::string m_name;
};

bool ends_with(std::string_view src, std::string ending)
{
    if (src.length() >= ending.length())
    {
        return (0 == src.compare(src.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}

std::vector<std::string_view> list_of_filenames(std::string_view contents)
{
    std::vector<std::string_view> filenames;

    std::size_t pos = 0;

    while (pos != std::string::npos)
    {
        pos = contents.find_first_of("load \"");
        if (pos != std::string_view::npos)
        {
            contents = contents.substr(pos);
            pos = contents.find_first_of('\"') + 1;
            contents = contents.substr(pos);
            pos = contents.find_first_of('\"');

            std::string filename(contents.substr(0, pos));

            if (ends_with(filename, "wl"))
            {
                filenames.emplace_back(contents.substr(0, pos));
            }
        }
    }

    return filenames;
}
} // anonymous namespace

std::string preprocess_file(std::string_view filename)
{
    FileInfo file_info(filename);

    std::ifstream file(filename.data());

    if (file.fail())
        throw std::runtime_error("Cannot open file: " + std::string(filename));

    std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    for (auto load_file : list_of_filenames(contents))
        contents.insert(0, preprocess_file(file_info.path() + "/" + std::string(load_file)));

    const std::regex remove_load_re("load\\ \\\".*?\\\"\\;");
    std::ostringstream processed_file;
    std::regex_replace(std::ostream_iterator<char>(processed_file), contents.begin(), contents.end(), remove_load_re, "");

    return processed_file.str();
}