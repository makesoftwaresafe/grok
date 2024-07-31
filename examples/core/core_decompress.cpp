/*
 *    Copyright (C) 2016-2024 Grok Image Compression Inc.
 *
 *    This source code is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This source code is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cstdio>
#include <string>
#include <cstring>
#include <filesystem>
#include <cassert>

#include "grok.h"
#include "grk_examples_config.h"

uint8_t img_buf[] = {
	0xff, 0x4f, 0xff, 0x51, 0x00, 0x2c, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0c,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0c,
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x07, 0x04, 0x01, 0x07, 0x01, 0x01,
	0xff, 0x52, 0x00, 0x0e, 0x07, 0x02, 0x00, 0x01, 0x00, 0x01, 0x04, 0x04, 0x00, 0x01, 0x00, 0x11,
	0xff, 0x53, 0x00, 0x0b, 0x01, 0x01, 0x01, 0x04, 0x04, 0x00, 0x01, 0x11, 0x22, 0xff, 0x5c, 0x00,
	0x07, 0x40, 0x40, 0x48, 0x48, 0x50, 0xff, 0x64, 0x00, 0x2d, 0x00, 0x01, 0x43, 0x72, 0x65, 0x61,
	0x74, 0x6f, 0x72, 0x3a, 0x20, 0x41, 0x56, 0x2d, 0x4a, 0x32, 0x4b, 0x20, 0x28, 0x63, 0x29, 0x20,
	0x32, 0x30, 0x30, 0x30, 0x2c, 0x32, 0x30, 0x30, 0x31, 0x20, 0x41, 0x6c, 0x67, 0x6f, 0x20, 0x56,
	0x69, 0x73, 0x69, 0x6f, 0x6e, 0xff, 0x90, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x01, 0xb2, 0x00,
	0x01, 0xff, 0x93, 0xff, 0x91, 0x00, 0x04, 0x00, 0x00, 0xcf, 0xb4, 0x14, 0xff, 0x92, 0x0d, 0xe6,
	0x72, 0x28, 0x08, 0xff, 0x91, 0x00, 0x04, 0x00, 0x01, 0xcf, 0xb4, 0x04, 0xff, 0x92, 0x07, 0xff,
	0x91, 0x00, 0x04, 0x00, 0x02, 0xdf, 0x80, 0x28, 0xff, 0x92, 0x07, 0x99, 0x26, 0x2e, 0xe7, 0xff,
	0x91, 0x00, 0x04, 0x00, 0x03, 0xcf, 0xb4, 0x04, 0xff, 0x92, 0x07, 0xff, 0x91, 0x00, 0x04, 0x00,
	0x04, 0xdf, 0x80, 0x28, 0xff, 0x92, 0x0e, 0x07, 0xe6, 0x46, 0xd9, 0xff, 0x91, 0x00, 0x04, 0x00,
	0x05, 0xcf, 0xb4, 0x04, 0xff, 0x92, 0x07, 0xff, 0x91, 0x00, 0x04, 0x00, 0x06, 0xcf, 0xb4, 0x14,
	0xff, 0x92, 0x0c, 0xf6, 0x74, 0xf6, 0xcb, 0xff, 0x91, 0x00, 0x04, 0x00, 0x07, 0xcf, 0xb4, 0x04,
	0xff, 0x92, 0x07, 0xff, 0x91, 0x00, 0x04, 0x00, 0x08, 0xdf, 0x80, 0x18, 0xff, 0x92, 0x0e, 0x07,
	0x78, 0xff, 0x91, 0x00, 0x04, 0x00, 0x09, 0xcf, 0xb4, 0x04, 0xff, 0x92, 0x07, 0xff, 0x91, 0x00,
	0x04, 0x00, 0x0a, 0xcf, 0xb4, 0x0c, 0xff, 0x92, 0x0c, 0xfa, 0x1b, 0xff, 0x91, 0x00, 0x04, 0x00,
	0x0b, 0xcf, 0xb4, 0x04, 0xff, 0x92, 0x07, 0xff, 0x91, 0x00, 0x04, 0x00, 0x0c, 0xcf, 0xc0, 0x04,
	0xff, 0x92, 0x04, 0xff, 0x91, 0x00, 0x04, 0x00, 0x0d, 0xc7, 0xda, 0x09, 0x0f, 0xa8, 0x12, 0x1f,
	0x68, 0x18, 0xff, 0x92, 0x02, 0x48, 0x0a, 0x04, 0x0b, 0x81, 0x06, 0x3b, 0x0b, 0x66, 0x81, 0xff,
	0x91, 0x00, 0x04, 0x00, 0x0e, 0x80, 0xff, 0x92, 0xff, 0x91, 0x00, 0x04, 0x00, 0x0f, 0xc7, 0xda,
	0x09, 0x1f, 0x68, 0x24, 0x3e, 0xd0, 0x40, 0xff, 0x92, 0x02, 0xe8, 0x7b, 0xe6, 0x07, 0xcd, 0xd0,
	0x8e, 0x0b, 0x72, 0x34, 0xd4, 0xff, 0x91, 0x00, 0x04, 0x00, 0x10, 0xcf, 0xc0, 0x04, 0xff, 0x92,
	0x04, 0xff, 0x91, 0x00, 0x04, 0x00, 0x11, 0x80, 0xff, 0x92, 0xff, 0x91, 0x00, 0x04, 0x00, 0x12,
	0xcf, 0xc0, 0x04, 0xff, 0x92, 0x04, 0xff, 0x91, 0x00, 0x04, 0x00, 0x13, 0xc7, 0xda, 0x07, 0x0f,
	0xa8, 0x0a, 0x1f, 0x68, 0x10, 0xff, 0x92, 0x06, 0x40, 0x23, 0x07, 0xb1, 0x08, 0x0c, 0xff, 0x91,
	0x00, 0x04, 0x00, 0x14, 0x80, 0xff, 0x92, 0xff, 0x91, 0x00, 0x04, 0x00, 0x15, 0xc7, 0xda, 0x0a,
	0x00, 0xff, 0x92, 0x01, 0x66, 0x0a, 0xa0, 0x2c, 0xff, 0x91, 0x00, 0x04, 0x00, 0x16, 0xcf, 0xc0,
	0x04, 0xff, 0x92, 0x04, 0xff, 0x91, 0x00, 0x04, 0x00, 0x17, 0x80, 0xff, 0x92, 0xff, 0x91, 0x00,
	0x04, 0x00, 0x18, 0xcf, 0xc0, 0x04, 0xff, 0x92, 0x04, 0xff, 0x91, 0x00, 0x04, 0x00, 0x19, 0xc7,
	0xda, 0x06, 0x00, 0xff, 0x92, 0x01, 0x6d, 0x0f, 0xff, 0x91, 0x00, 0x04, 0x00, 0x1a, 0x80, 0xff,
	0x92, 0xff, 0x91, 0x00, 0x04, 0x00, 0x1b, 0xc7, 0xda, 0x0a, 0x00, 0xff, 0x92, 0x01, 0x66, 0x1a,
	0xa1, 0x0d, 0xff, 0x91, 0x00, 0x04, 0x00, 0x1c, 0xcf, 0xc0, 0x04, 0xff, 0x92, 0x04, 0xff, 0x91,
	0x00, 0x04, 0x00, 0x1d, 0x80, 0xff, 0x92, 0xff, 0xd9};

const std::string dataRoot = GRK_DATA_ROOT;

void errorCallback(const char* msg, [[maybe_unused]] void* client_data)
{
   auto t = std::string(msg) + "\n";
   fprintf(stderr, "%s", t.c_str());
}
void warningCallback(const char* msg, [[maybe_unused]] void* client_data)
{
   auto t = std::string(msg) + "\n";
   fprintf(stdout, "%s", t.c_str());
}
void infoCallback(const char* msg, [[maybe_unused]] void* client_data)
{
   auto t = std::string(msg) + "\n";
   fprintf(stdout, "%s", t.c_str());
}

struct ReadStreamInfo
{
   ReadStreamInfo(grk_stream_params* streamParams)
	   : streamParams_(streamParams), data_(nullptr), dataLen_(0), offset_(0), fp_(nullptr)
   {}
   grk_stream_params* streamParams_;
   uint8_t* data_;
   size_t dataLen_;
   size_t offset_;
   FILE* fp_;
};

size_t stream_read_fn(uint8_t* buffer, size_t numBytes, void* user_data)
{
   auto sinfo = (ReadStreamInfo*)user_data;
   size_t readBytes = numBytes;
   if(sinfo->data_)
   {
	  size_t bytesAvailable = sinfo->dataLen_ - sinfo->offset_;
	  readBytes = std::min(numBytes, bytesAvailable);
   }
   if(readBytes)
   {
	  if(sinfo->data_)
		 memcpy(buffer, sinfo->data_ + sinfo->offset_, readBytes);
	  else if(sinfo->fp_)
	  {
		 readBytes = fread(buffer, 1, readBytes, sinfo->fp_);
	  }
   }

   return readBytes;
}
bool stream_seek_fn(uint64_t offset, void* user_data)
{
   auto sinfo = (ReadStreamInfo*)user_data;
   if(offset <= sinfo->dataLen_)
	  sinfo->offset_ = offset;
   else
	  sinfo->offset_ = sinfo->dataLen_;
   if(sinfo->fp_)
   {
	  return fseek(sinfo->fp_, (long int)offset, SEEK_SET) == 0;
   }
   else
   {
	  return true;
   }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
   int rc = EXIT_FAILURE;

   uint16_t numTiles = 0;

   // a file can be passed in as a command line argument
   // example:
   // $ core_decompress foo.jp2
   // otherwise a file from the Grok test suite, specified below, will be used.

   std::string inputFilePath = dataRoot + std::filesystem::path::preferred_separator + "input" +
							   std::filesystem::path::preferred_separator + "nonregression" +
							   std::filesystem::path::preferred_separator + "boats_cprl.j2k";
   if(argc > 1)
	  inputFilePath = argv[1];

   // initialize decompress parameters
   grk_decompress_parameters decompressParams;
   grk_decompress_set_default_params(&decompressParams);
   decompressParams.compression_level = GRK_DECOMPRESS_COMPRESSION_LEVEL_DEFAULT;
   decompressParams.verbose_ = true;

   grk_image* image = nullptr;
   const char* inputFileStr = nullptr;
   bool fromBuffer = (argc == 1);
   bool useCallbacks = true;

   // if true, decompress a particular tile, otherwise decompress
   // all tiles
   bool decompressTile = false;

   // index of tile to decompress.
   uint16_t tile_index = 0;

   // if true, decompress window of dimension specified below,
   // otherwise decompress entire image
   bool decompressWindow = false;

   grk_codec* codec = nullptr;

   // initialize library
   grk_initialize(nullptr, 0, false);

   // create j2k file stream
   inputFileStr = inputFilePath.c_str();
   if(fromBuffer)
   {
	  printf("Decompressing buffer\n");
   }
   else
   {
	  printf("Decompressing file %s\n", inputFilePath.c_str());
   }

   // set message handlers for info,warning and error
   grk_set_msg_handlers(infoCallback, nullptr, warningCallback, nullptr, errorCallback, nullptr);

   // initialize decompressor
   grk_stream_params streamParams;
   grk_set_default_stream_params(&streamParams);
   ReadStreamInfo sinfo(&streamParams);
   if(!fromBuffer)
   {
	  sinfo.fp_ = fopen(inputFilePath.c_str(), "rb");
	  if(!sinfo.fp_)
	  {
		 fprintf(stderr, "Failed to open file %s for reading\n", inputFilePath.c_str());
		 goto beach;
	  }
   }

   if(useCallbacks)
   {
	  streamParams.seek_fn = stream_seek_fn;
	  streamParams.read_fn = stream_read_fn;
	  streamParams.user_data = &sinfo;
	  if(fromBuffer)
	  {
		 streamParams.stream_len = sizeof(img_buf);
		 sinfo.data_ = img_buf;
		 sinfo.dataLen_ = sizeof(img_buf);
	  }
	  else
	  {
		 // Move the file pointer to the end and get the file size
		 fseek(sinfo.fp_, 0, SEEK_END);
		 auto len = ftell(sinfo.fp_);
		 if(len == -1)
			goto beach;
		 streamParams.stream_len = (size_t)len;
		 rewind(sinfo.fp_);
	  }
   }
   else if(fromBuffer)
   {
	  streamParams.buf = img_buf;
	  streamParams.buf_len = sizeof(img_buf);
   }
   else
   {
	  streamParams.file = inputFileStr;
   }
   codec = grk_decompress_init(&streamParams, &decompressParams);
   if(!codec)
   {
	  fprintf(stderr, "Failed to set up decompressor\n");
	  goto beach;
   }

   // read j2k header
   grk_header_info headerInfo;
   memset(&headerInfo, 0, sizeof(headerInfo));
   if(!grk_decompress_read_header(codec, &headerInfo))
   {
	  fprintf(stderr, "Failed to read the header\n");
	  goto beach;
   }

   // set decompress window
   if(decompressWindow)
   {
	  // decompress window of dimensions {0,0,1000,1000}
	  if(!grk_decompress_set_window(codec, 0, 0, 1000, 1000))
	  {
		 fprintf(stderr, "Failed to set decompress region\n");
		 goto beach;
	  }
   }

   // retrieve image that will store uncompressed image data
   image = grk_decompress_get_composited_image(codec);
   if(!image)
   {
	  fprintf(stderr, "Failed to retrieve image \n");
	  goto beach;
   }

   numTiles = (uint16_t)(headerInfo.t_grid_width * headerInfo.t_grid_height);
   printf("\nImage Info\n");
   printf("Width: %d\n", image->x1 - image->x0);
   printf("Height: %d\n", image->y1 - image->y0);
   printf("Number of components: %d\n", image->numcomps);
   for(uint16_t compno = 0; compno < image->numcomps; ++compno)
	  printf("Precision of component %d : %d\n", compno, image->comps[compno].prec);
   printf("Number of tiles: %d\n", numTiles);
   if(numTiles > 1)
   {
	  printf("Nominal tile dimensions: (%d,%d)\n", headerInfo.t_width, headerInfo.t_height);
   }

   if(decompressTile)
   {
	  // decompress a particular tile
	  if(!grk_decompress_tile(codec, tile_index))
		 goto beach;
   }
   else
   {
	  // decompress all tiles
	  if(!grk_decompress(codec, nullptr))
		 goto beach;
   }

   // see grok.h header for full details of image structure
   for(uint16_t compno = 0; compno < image->numcomps; ++compno)
   {
	  auto comp = image->comps + compno;
	  auto compWidth = comp->w;
	  auto compHeight = comp->h;
	  auto compData = comp->data;
	  if(!compData)
	  {
		 fprintf(stderr, "Image has null data for component %d\n", compno);
		 goto beach;
	  }
	  printf("Component %d : dimensions (%d,%d) at precision %d\n", compno, compWidth, compHeight,
			 comp->prec);

	  // copy data, taking component stride into account
	  auto copiedData = new int32_t[compWidth * compHeight];
	  auto copyPtr = copiedData;
	  for(uint32_t j = 0; j < compHeight; ++j)
	  {
		 memcpy(copyPtr, compData, compWidth * sizeof(int32_t));
		 copyPtr += compWidth;
		 compData += comp->stride;
	  }
	  delete[] copiedData;
   }

   rc = EXIT_SUCCESS;
beach:
   // cleanup
   grk_object_unref(codec);
   grk_deinitialize();

   return rc;
}
