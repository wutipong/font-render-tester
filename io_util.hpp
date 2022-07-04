#pragma once

#include <fstream>
#include <streambuf>
#include <filesystem>

template <class container_type>
void LoadFile(const std::filesystem::path &path, container_type &data,
              std::ios_base::openmode mode = std::ios_base::in) {

  typedef typename container_type::value_type value_type;
  std::basic_ifstream<value_type> file(path, mode);

  data = {std::istreambuf_iterator<value_type>(file),
          std::istreambuf_iterator<value_type>()};

  file.close();
}
