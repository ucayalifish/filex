//
// Created by s-skorokhodov on 08.02.2022.
//

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>

#include "fx_api.h"

#define SECTOR_SIZE 512U

#define SECTORS_PER_BLOCK 256U

#define TOTAL_BLOCKS 7U

#define TOTAL_SECTORS (SECTORS_PER_BLOCK * TOTAL_BLOCKS)

#define FLASH_SPACE (SECTOR_SIZE * SECTORS_PER_BLOCK * TOTAL_BLOCKS)

static uint8_t ram_disk_memory_[FLASH_SPACE];

static uint8_t media_memory_[SECTOR_SIZE];

extern VOID _fx_ram_driver(FX_MEDIA * media_ptr);

static void pseudo_thread_entry_(ULONG input);

static UINT copy_to_filex_(CHAR * file_name);

static UINT print_device_file_(CHAR * file_name);

static void walk_src_tree_(void);

static FX_MEDIA media_;

static FX_FILE file_;

static CHAR * tree_[] = {"css", "css/images", "img", "lib"};

static CHAR * css_imgs_[] = {
  "css/images/ui-icons_444444_256x240.png",
  "css/images/ui-icons_555555_256x240.png",
  "css/images/ui-icons_777620_256x240.png",
  "css/images/ui-icons_777777_256x240.png",
  "css/images/ui-icons_cc0000_256x240.png",
  "css/images/ui-icons_ffffff_256x240.png",
};

static void make_tree_(void);

static void check_media_(void);

static void dump_ram_disk_(void);

int main(void)
{
  // walk_src_tree_();
  memset(ram_disk_memory_, -1, FLASH_SPACE);
#ifdef FX_STANDALONE_ENABLE
  fx_system_initialize();

  pseudo_thread_entry_(0);
#endif // FX_STANDALONE_ENABLE

  return 0;
}

static void pseudo_thread_entry_(ULONG input)
{
  (void) input;

  UINT status = fx_media_format(&media_,
                                _fx_ram_driver,              // Driver entry
                                ram_disk_memory_,      // RAM disk memory pointer
                                media_memory_,           // Media buffer pointer
                                sizeof(media_memory_),   // Media buffer size
                                "RAM_DISK",        // Volume Name
                                1,                    // Number of FATs
                                32,                  // Directory Entries
                                0,                    // Hidden sectors
                                TOTAL_SECTORS,          // Total sectors
                                SECTOR_SIZE,         // Sector size
                                8,                 // Sectors per cluster
                                1,                           // Heads
                                1);                  // Sectors per track

  if (status != FX_SUCCESS)
    {
      printf("\t!!! error of fx_media_format: %d\n", status);
      return;
    }
  printf("fx_media_format -> %d\n", status);

  status = fx_media_open(&media_, "RAM DISK", _fx_ram_driver, ram_disk_memory_, media_memory_, sizeof(media_memory_));
  if (status != FX_SUCCESS)
    {
      printf("\t!!! error of fx_media_open: %d\n", status);
      return;
    }
  printf("fx_media_open -> %d\n", status);

  ULONG empty_size;
  status = fx_media_space_available(&media_, &empty_size);
  if (status != FX_SUCCESS)
    {
      printf("!!! error calculating free FS space: %d\n", status);
      return;
    }
  printf("Empty FS bytes available: %lu in %d bytes in memory\n", empty_size, FLASH_SPACE);

/*
  status = fx_directory_create(&media_, "web-root");
  if (status != FX_SUCCESS)
    {
      printf("\t!!! error of fx_directory_create[web-root]: %d\n", status);
      return;
    }
  printf("fx_directory_create[web-root] -> %d\n", status);

  status = fx_directory_default_set(&media_, "web-root");
  if (status != FX_SUCCESS)
    {
      printf("\t!!! error of fx_directory_default_set[[web-root]]: %d\n", status);
      return;
    }
  printf("fx_directory_default_set[[web-root]] -> %d\n", status);
*/

  make_tree_();

  CHAR * fname = "index.html";
  status = copy_to_filex_(fname);
  if (status != FX_SUCCESS)
    {
      printf("\t!!! copy_to_filex_ - error copying %s: %d\n", fname, status);
      return;
    }
  printf("copy_to_filex_: %s -> %d\n", fname, status);

  fname  = "app.js";
  status = copy_to_filex_(fname);
  if (status != FX_SUCCESS)
    {
      printf("\t!!! copy_to_filex_ - error copying %s: %d\n", fname, status);
      return;
    }
  printf("copy_to_filex_: %s -> %d\n", fname, status);

  fname  = "css/style.css";
  status = copy_to_filex_(fname);
  if (status != FX_SUCCESS)
    {
      printf("\t!!! copy_to_filex_ - error copying %s: %d\n", fname, status);
      return;
    }
  printf("copy_to_filex_: %s -> %d\n", fname, status);

  fname  = "css/jquery-ui.min.css";
  status = copy_to_filex_(fname);
  if (status != FX_SUCCESS)
    {
      printf("\t!!! copy_to_filex_ - error copying %s: %d\n", fname, status);
      return;
    }
  printf("copy_to_filex_: %s -> %d\n", fname, status);

  for (int i = 0; i < sizeof css_imgs_ / sizeof css_imgs_[0]; ++i)
    {
      fname  = css_imgs_[i];
      status = copy_to_filex_(fname);
      if (status != FX_SUCCESS)
        {
          printf("\t!!! copy_to_filex_ - error copying %s: %d\n", fname, status);
          return;
        }
      printf("copy_to_filex_: %s -> %d\n", fname, status);
    }

  fname  = "img/Smartec_logo.gif";
  status = copy_to_filex_(fname);
  if (status != FX_SUCCESS)
    {
      printf("\t!!! copy_to_filex_ - error copying %s: %d\n", fname, status);
      return;
    }
  printf("copy_to_filex_: %s -> %d\n", fname, status);

  fname  = "lib/jquery-3.6.0.min.js";
  status = copy_to_filex_(fname);
  if (status != FX_SUCCESS)
    {
      printf("\t!!! copy_to_filex_ - error copying %s: %d\n", fname, status);
      return;
    }
  printf("copy_to_filex_: %s -> %d\n", fname, status);

  fname  = "lib/jquery-ui.min.js";
  status = copy_to_filex_(fname);
  if (status != FX_SUCCESS)
    {
      printf("\t!!! copy_to_filex_ - error copying %s: %d\n", fname, status);
      return;
    }
  printf("copy_to_filex_: %s -> %d\n", fname, status);

  fname  = "lib/cbor.js";
  status = copy_to_filex_(fname);
  if (status != FX_SUCCESS)
    {
      printf("\t!!! copy_to_filex_ - error copying %s: %d\n", fname, status);
      return;
    }
  printf("copy_to_filex_: %s -> %d\n", fname, status);

  printf("Consistency check...\n");
  check_media_();

  ULONG available;
  status = fx_media_space_available(&media_, &available);
  if (status != FX_SUCCESS)
    {
      printf("!!! error calculating free FS space: %d\n", status);
      return;
    }
  printf("Still available bytes in FS: %lu, used: %lu\n", available, empty_size - available);

  status = fx_media_flush(&media_);
  if (status != FX_SUCCESS)
    {
      printf("!!! error flashing FS: %d\n", status);
      return;
    }
  status = fx_media_close(&media_);
  if (status != FX_SUCCESS)
    {
      printf("!!! error closing FS: %d\n", status);
      return;
    }

  dump_ram_disk_();
#if 1
  status = fx_media_open(&media_, "RAM DISK", _fx_ram_driver, ram_disk_memory_, media_memory_, sizeof(media_memory_));
  if (status != FX_SUCCESS)
    {
      printf("\t!!! error of fx_media_open: %d\n", status);
      return;
    }
  printf("fx_media_open -> %d\n", status);

/*
  status = fx_directory_default_set(&media_, "web-root");
  if (status != FX_SUCCESS)
    {
      printf("\t!!! error of fx_directory_default_set[[web-root]]: %d\n", status);
      return;
    }
  printf("fx_directory_default_set[[web-root]] -> %d\n", status);
*/

  CHAR * dir_name;
  status = fx_directory_default_get(&media_, (CHAR **) &dir_name);
  if (status != FX_SUCCESS)
    {
      printf("\t!!! error of fx_directory_default_get: %d\n", status);
      return;
    }
  if (dir_name)
    {
      printf("CWD: %s\n", dir_name);
    }
  else
    {
      printf("CWD: \\\n");
    }

  status = print_device_file_("/css/jquery-ui.min.css");
  if (status != FX_SUCCESS)
    {
      printf("\t!!! error of print_device_file_: %d\n", status);
    }

  status = print_device_file_("/css/style.css");
  if (status != FX_SUCCESS)
    {
      printf("\t!!! error of print_device_file_: %d\n", status);
    }

#endif
}

static UINT copy_to_filex_(CHAR * file_name)
{
  UINT error            = FX_SUCCESS;
  bool device_file_open = false;

  FILE * host_file = fopen(file_name, "rb");
  if (!host_file)
    {
      error = -120;
      goto exit_point_;
    }

  ULONG total = 0;
  error = fx_file_create(&media_, file_name);
  if (error != FX_SUCCESS)
    {
      goto cleanup_point_;
    }

  error            = fx_file_open(&media_, &file_, file_name, FX_OPEN_FOR_WRITE);
  if (error != FX_SUCCESS)
    {
      device_file_open = false;
      goto cleanup_point_;
    }
  device_file_open = true;

  size_t bytes_read;
  char   buf[256];
  memset(buf, 0, sizeof buf);
  while ((bytes_read = fread(buf, 1, sizeof buf, host_file)))
    {
      error = fx_file_write(&file_, buf, bytes_read);
      if (error != FX_SUCCESS)
        {
          break;
        }
      total += bytes_read;
      memset(buf, 0, sizeof buf);
    }

cleanup_point_:
  printf("Total bytes written: %lu\n", total);

  if (device_file_open)
    {
      error = fx_file_close(&file_);
      error = fx_media_flush(&media_);
    }

  (void) fclose(host_file);

exit_point_:
  return error;
}

static UINT print_device_file_(CHAR * file_name)
{
  UINT status = fx_file_open(&media_, &file_, file_name, FX_OPEN_FOR_READ);
  if (status != FX_SUCCESS)
    {
      goto exit_point_;
    }

  CHAR buf[256];
  memset(buf, 0, sizeof buf);
  ULONG bytes_read;
  ULONG total = 0;
  status = fx_file_read(&file_, buf, 256, &bytes_read);
  if (status != FX_SUCCESS)
    {
      goto cleanup_point_;
    }
  printf("%.*s", 256, buf);

  while (bytes_read > 0)
    {
      total += bytes_read;
      memset(buf, 0, sizeof buf);
      status = fx_file_read(&file_, buf, 256, &bytes_read);
      if (status == FX_END_OF_FILE)
        {
          break;
        }
      if (status != FX_SUCCESS)
        {
          break;
        }
      printf("%.*s", 256, buf);
    }

cleanup_point_:
  printf("\nTotal bytes read: %lu\n", total);
  status = fx_file_close(&file_);

exit_point_:
  return status;
}

void walk_src_tree_(void)
{
  DIR * dir = opendir(".");
  if (dir == NULL)
    {
      printf("!!! error open root dir");
    }
  struct dirent * entry = readdir(dir);
  while (entry != NULL)
    {
      char buf[1024];
      snprintf(buf, sizeof buf, "entry '%s', type: %d\n", entry->d_name, entry->d_type);
      printf("found entry: %s\n", buf);
      entry = readdir(dir);
    }
  closedir(dir);
}

void make_tree_(void)
{
  for (int i = 0; i < sizeof tree_ / sizeof tree_[0]; ++i)
    {
      CHAR * dir_name = tree_[i];
      UINT status = fx_directory_create(&media_, dir_name);
      if (status != FX_SUCCESS)
        {
          printf("\t!!! error of fx_directory_create [/%s]: %d\n", dir_name, status);
          return;
        }
      printf("fx_directory_create: [/%s] -> %d\n", dir_name, status);
    }
}

void check_media_(void)
{
  UCHAR scratch[2048];
  ULONG detected_errors;
  UINT  status = fx_media_check(&media_,
                                scratch,
                                sizeof scratch,
                                FX_FAT_CHAIN_ERROR | FX_DIRECTORY_ERROR | FX_LOST_CLUSTER_ERROR,
                                &detected_errors);
  if (status != FX_SUCCESS)
    {
      printf("!!! error checking media: %d\n", status);
      return;
    }

  printf("Flags of found FS errors: 0x%lx\n", detected_errors);
}

void dump_ram_disk_(void)
{
  FILE * fl               = fopen("web-root.bin", "wb");

  if (!fl)
    {
      goto exit_point_;
    }

#define CHUNK_ 1024
  _Static_assert(FLASH_SPACE % CHUNK_ == 0, "");

  uint8_t   buf[CHUNK_];
  int const num_of_writes = sizeof ram_disk_memory_ / CHUNK_;
  size_t    total         = 0;
  for (int  i             = 0; i < num_of_writes; ++i)
    {
      uint8_t const * dest = ram_disk_memory_ + CHUNK_ * i;
      memcpy(buf, dest, CHUNK_);
      size_t written = fwrite(buf, CHUNK_, 1, fl);
      if (written != 1)
        {
          printf("!!! error dumping ram disk: chank %d, expected %d, written: %zu\n", i, CHUNK_, written);
          exit(-1);
        }
      total += CHUNK_;
    }

  (void) fclose(fl);
  printf("%zu bytes dumped to web-root.bin\n", total);

exit_point_:
  return;
}
