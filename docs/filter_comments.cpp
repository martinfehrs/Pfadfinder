/**
 * @file
 * @brief Simple C++ comment filter for Doxygen
 *
 * This program filters C++ comments from all .cppm files in a directory.
 * Usage: filter_comments <input_directory> <output_directory>
 */

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

std::string filter_line(const std::string& line, bool& in_block_comment)
{
    if (in_block_comment)
    {
        const size_t end_pos = line.find("*/");

        if (end_pos != std::string::npos)
        {
            in_block_comment = false;
            std::string result = line.substr(end_pos + 2);
            return filter_line(result, in_block_comment);
        }
        return "";
    }
    
    const size_t block_start = line.find("/*");

    if (block_start != std::string::npos)
    {
        std::string result;
        if (block_start > 0)
            result = line.substr(0, block_start);
        
        const size_t block_end = line.find("*/", block_start + 2);

        if (block_end != std::string::npos)
        {
            std::string after_comment = line.substr(block_end + 2);
            after_comment = filter_line(after_comment, in_block_comment);
            if (!after_comment.empty())
            {
                if (!result.empty())
                    result += after_comment;
                else
                    result = after_comment;
            }
        }
        else
        {
            in_block_comment = true;
        }
        
        return result;
    }
    
    const size_t single_line_pos = line.find("//");
    if (single_line_pos != std::string::npos)
        return line.substr(0, single_line_pos);
    
    return line;
}

void filter_file(const fs::path& input_path, const fs::path& output_path)
{
    std::ifstream input(input_path);
    std::ofstream output(output_path);
    
    if (!input || !output)
    {
        std::cerr << "Error: Cannot open file " << input_path << " or " << output_path << std::endl;
        return;
    }
    
    std::string line;
    bool in_block_comment = false;
    
    while (std::getline(input, line))
    {
        std::string filtered = filter_line(line, in_block_comment);
        output << filtered << "\n";
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: filter_comments <input_directory> <output_directory>" << std::endl;
        return 1;
    }
    
    fs::path input_dir = argv[1];
    fs::path output_dir = argv[2];
    
    if (!fs::exists(input_dir) || !fs::is_directory(input_dir))
    {
        std::cerr << "Error: Input directory does not exist: " << input_dir << std::endl;
        return 1;
    }
    
    if (!fs::exists(output_dir))
        fs::create_directories(output_dir);
    
    for (const auto& entry : fs::directory_iterator(input_dir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".cppm")
        {
            fs::path output_path = output_dir / entry.path().filename();
            filter_file(entry.path(), output_path);
        }
    }
    
    return 0;
}