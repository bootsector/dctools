/*
* DCTools - DreamCast CD-R authoring tools.
* Copyright (c) 2013 Bruno Freitas - bootsector@ig.com.br
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ISO9660_H_
#define ISO9660_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#define WINDOWS_PLATFORM
#endif
#ifdef _WIN64
#define WINDOWS_PLATFORM
#endif

#define DIR_SEP "/"

#ifdef WINDOWS_PLATFORM
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#define SECTOR_SIZE	2048

struct iso9660_dir_info_s {
	char name[255];
	uint16_t id;
	uint16_t parent_id;
	uint32_t lba;
} ;

typedef struct iso9660_dir_info_s iso9660_dir_info_t;

struct iso9660_file_info_s {
	char full_name[4096];
	uint16_t parent_dir;
	uint32_t lba;
	uint32_t size;
} ;

typedef struct iso9660_file_info_s iso9660_file_info_t;

int iso9660_num_dirs(void);
int iso9660_open(char *filename, uint32_t lba_offset);
void iso9660_close(void);
int iso9660_get_dir_info(int id, iso9660_dir_info_t* dir_info) ;
int iso9660_extract_files(int directory_id, char *basedir);
char *iso9660_dir_tree(int directory_id);
void iso9660_mkdir_tree(int directory_id, char *basedir);
int iso9660_dump_bootsector(char *outfilename);
int iso9660_write_sortfile(char *outfilename);

#endif /* ISO9660_H_ */
