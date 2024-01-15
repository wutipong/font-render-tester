#pragma once

#include <filesystem>
#include <fstream>
#include <streambuf>

template <class container_type>
container_type LoadFile(const std::filesystem::path &path,
                        std::ios_base::openmode mode = std::ios_base::in) {

  typedef typename container_type::value_type value_type;
  container_type data;

  std::basic_ifstream<value_type> file(path, mode);

  std::error_code ec{};
  if (auto filesize = std::filesystem::file_size(path, ec);
      filesize != -1 && !ec) {
    data.reserve(filesize);
  }

  data.insert(data.end(), std::istreambuf_iterator<value_type>(file),
              std::istreambuf_iterator<value_type>());

  file.close();

  return data;
}
