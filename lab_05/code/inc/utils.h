#pragma once

#include <string>
#include <vector>

std::string read_text_file(const std::string &path);
void write_text_file(const std::string &path, const std::string &text);

std::vector<std::string> read_lines_file(const std::string &path);

std::string path_dirname(const std::string &path);
std::string path_basename(const std::string &path);
std::string path_stem(const std::string &path);
std::string path_ext(const std::string &path);
std::string path_join(const std::string &a, const std::string &b);

bool file_exists(const std::string &path);
