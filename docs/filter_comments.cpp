/**
 * @file
 * @brief Simple C++ comment filter for Doxygen INPUT_FILTER
 *
 * This program reads C++ source code from stdin and writes it to stdout
 * with all C++ comments removed. This is used as INPUT_FILTER for Doxygen
 * to generate English documentation from source files that contain German comments.
 * The English documentation comes from the corresponding .dox files.
 */

#include <iostream>
#include <string>

int main()
{
    std::string line;
    bool in_block_comment = false;
    
    while (std::getline(std::cin, line))
    {
        if (in_block_comment)
        {
            // Look for end of block comment
            const size_t end_pos = line.find("*/");

            if (end_pos != std::string::npos)
            {
                // Remove everything up to and including */
                line = line.substr(end_pos + 2);
                in_block_comment = false;
                
                // If there's content after */, process it
                if (!line.empty())
                {
                    // Check if there's a single-line comment
                    const size_t single_line_pos = line.find("//");

                    if (single_line_pos != std::string::npos)
                        line = line.substr(0, single_line_pos);

                    // Check for new block comment
                    const size_t new_block_pos = line.find("/*");

                    if (new_block_pos != std::string::npos)
                    {
                        line = line.substr(0, new_block_pos);
                        in_block_comment = true;
                    }

                    if (!line.empty())
                        std::cout << line << std::endl;
                }
            }

            continue;
        }
        
        // Check for start of block comment
        const size_t block_start = line.find("/*");

        if (block_start != std::string::npos)
        {
            // Output everything before the comment
            if (block_start > 0)
                std::cout << line.substr(0, block_start);
            
            // Check if the block comment ends on the same line
            const size_t block_end = line.find("*/", block_start + 2);

            if (block_end != std::string::npos)
            {
                // Single-line block comment, output everything after */
                std::string after_comment = line.substr(block_end + 2);
                if (!after_comment.empty())
                {
                    // Check for single-line comment in the remaining part
                    const size_t single_line_pos = after_comment.find("//");

                    if (single_line_pos != std::string::npos)
                        after_comment = after_comment.substr(0, single_line_pos);

                    std::cout << after_comment << std::endl;
                }
                else
                {
                    std::cout << std::endl;
                }
            }
            else
            {
                // Multi-line block comment starts
                in_block_comment = true;
                std::cout << std::endl;
            }

            continue;
        }
        
        // Check for single-line comment
        const size_t single_line_pos = line.find("//");

        if (single_line_pos != std::string::npos)
            line = line.substr(0, single_line_pos);
        
        // Output the line (preserve empty lines)
        std::cout << line << std::endl;
    }
    
    return 0;
}