#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <filesystem>

int main(int argc, char *argv[]) {
  std::string currLine;

  if (argc != 2) {
    throw std::invalid_argument("There must only be the file you want to compile as the argument");
    return -1;
  }
  std::filesystem::path filePath = argv[1];

  if (!(filePath.extension() == ".vista")) {
    throw std::invalid_argument("File must end in .vista");
    return -1;
  }
  std::ifstream fp(filePath);

  if (!fp) {
    throw std::invalid_argument("File does not exist");
    return -1;
  }

  while (std::getline(fp, currLine)) {
    std::cout << currLine << std::endl;
  }
  fp.close();

  return 0;
}
