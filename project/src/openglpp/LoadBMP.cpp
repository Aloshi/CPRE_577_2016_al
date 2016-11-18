#include <openglpp/LoadBMP.h>

#include <stdio.h>
#include <cstring>  // for std::memcmp
#include <openglpp/GLException.h>

// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
std::vector<unsigned char> loadBMP(const std::string& path, unsigned int* width_out, unsigned int* height_out)
{
  //**********************************************************************************************
  // Loads the file content

  int channels = 3;
  
  unsigned char header[54]; // Each BMP file begins by a 54-bytes header
  unsigned int dataPos;     // Position in the file where the actual data begins
  unsigned int width, height;
  unsigned int imageSize;

  // This opens a file
  FILE* file;
  file = fopen(path.c_str(), "rb");

  if (file == NULL)
    throw GLException() << "Could not open file '" << path << "'";

  // This reads the header of the file and checks the length.
  if (fread(header, 1, 54, file) != 54)
    throw GLException() << "File too short (missing BMP header)";

  if (std::memcmp(&header[0], "BM", 2) != 0)
    throw GLException() << "Invalid BMP file! (missing header identifier)";

  // Read the start position of the data, the size, the width, and height.
  dataPos = *(int*)&(header[0x0A]);
  imageSize = *(int*)&(header[0x22]);
  width = *(int*)&(header[0x12]);
  height = *(int*)&(header[0x16]);
  channels = imageSize / (width * height);

  // Create memory for this texture
  std::vector<unsigned char> image_data;
  image_data.resize(width * height * channels);

  // Read the data from a file.
  if (fread(image_data.data(), width * height * channels, 1, file) != 1)
    throw GLException() << "Could not read all BMP data (file truncated?)";

  // Release the file.
  fclose(file);

  if (width_out)
    *width_out = width;
  if (height_out)
    *height_out = height;

  // Return the texture.
  return image_data;
}
