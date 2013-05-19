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

#include "iso9660.h"

uint32_t LBA_OFFSET = 45000;

static FILE *iso_file = NULL;

static uint8_t lba_buffer[SECTOR_SIZE];

static uint8_t *path_table = NULL;
static uint32_t path_table_size;

static iso9660_file_info_t *files_list = NULL;
static uint32_t files_list_size = 0;

int iso9660_open(char *filename, uint32_t lba_offset) {
	uint8_t iso_id[] = { 0x01, 0x43, 0x44, 0x30, 0x30, 0x31 };
	uint32_t pvd_path_table_lba;

	LBA_OFFSET = lba_offset;

	if((iso_file = fopen(filename, "rb")) == NULL) {
		printf("Error opening input file\n");
		return 1;
	}

	fseek(iso_file, 16 * SECTOR_SIZE, SEEK_SET);
	fread(lba_buffer, sizeof(lba_buffer), 1, iso_file);

	if(memcmp(iso_id, lba_buffer, 6) != 0) {
		printf("Error: Input file is not an ISO file\n");
		fclose(iso_file);
		return 1;
	}

	path_table_size = *((uint32_t *)(lba_buffer + 132));
	pvd_path_table_lba = *((uint32_t *)(lba_buffer + 140)) - LBA_OFFSET;

	path_table = malloc(path_table_size);

	fseek(iso_file, pvd_path_table_lba * SECTOR_SIZE, SEEK_SET);
	fread(path_table, path_table_size, 1, iso_file);

	// Check if first directory in path table is root.
	// If not, LBA_OFFSET is probably incorrect
	if((path_table[0] != 0x01) || (path_table[8] != 0x00)) {
		printf("Error: First directory in the path table is not root.\n<lba_offset> is probably incorrect!\n");

		fclose(iso_file);
		free(path_table);

		return 1;
	}

	return 0;
}

int iso9660_num_dirs(void) {
	int i, n, c;

	i = 0;
	c = 0;

	while(i < path_table_size) {
		n = path_table[i];

		c++;

		i += 8 + n + ((n % 2) ? 1 : 0);
	}

	return c;
}

int iso9660_get_dir_info(int id, iso9660_dir_info_t* dir_info) {
	int i, n, c;

	if((id > iso9660_num_dirs()) || (id < 1)) {
		return 1;
	}

	i = 0;
	c = 1;

	while(i < path_table_size) {
		n = path_table[i];

		if(c == id) {
			memset(dir_info, 0x00, sizeof(iso9660_dir_info_t));

			dir_info->id = c;
			dir_info->parent_id = *((uint16_t *)(path_table + 6 + i));
			dir_info->lba = *((uint32_t *)(path_table + 2 + i)) - LBA_OFFSET;

			if(path_table[i + 8] == 0x00) {
				strcpy(dir_info->name, DIR_SEP);
			} else {
				strncpy(dir_info->name, (const char*)&path_table[i + 8], n);
			}

			break;
		}

		i += 8 + n + ((n % 2) ? 1 : 0);
		c++;
	}

	return 0;
}

void iso9660_write_file(char *output_file, uint32_t lba, uint32_t size) {
	FILE *outfile;
	uint8_t *databuffer;

	if((databuffer = malloc(size)) == NULL) {
		printf("Error allocating %d bytes for reading file\n", size);
		return;
	}

	if((outfile = fopen(output_file, "wb")) == NULL) {
		printf("Error creating output file: %s\n", output_file);

		free(databuffer);

		return;
	}

	printf("%s - %d\n", output_file, size);

	fseek(iso_file, lba * SECTOR_SIZE, SEEK_SET);
	fread(databuffer, size, 1, iso_file);

	fwrite(databuffer, size, 1, outfile);

	fclose(outfile);
	free(databuffer);
}

int iso9660_extract_files(int directory_id, char *basedir) {
	iso9660_dir_info_t dir;
	int i, n;
	char file_name[256];
	char output_file_name[4096];
	char bdir[256];
	char *p;

	uint8_t *dir_record;
	uint32_t dir_record_size;

	if(iso9660_get_dir_info(directory_id, &dir) != 0) {
		printf("Error getting files for directory: %s\n", dir.name);
		return 1;
	}

	// Sanitize base dir (remove trailing slash)
	strcpy(bdir, basedir);

	if(bdir[strlen(bdir) - 1] == DIR_SEP[0]) {
		bdir[strlen(bdir) - 1] = '\0';
	}

	// Find out and allocate necessary data for the whole directory record
	fseek(iso_file, dir.lba * SECTOR_SIZE, SEEK_SET);
	fread(lba_buffer, SECTOR_SIZE, 1, iso_file);

	dir_record_size = *((uint32_t *)(lba_buffer + 10));

	if((dir_record = malloc(dir_record_size)) == NULL) {
		printf("Error allocating memory for the directory table\n");
		return 1;
	}

	// Read whole directory record
	fseek(iso_file, dir.lba * SECTOR_SIZE, SEEK_SET);
	fread(dir_record, dir_record_size, 1, iso_file);

	i = 0;

	while(i < dir_record_size) {
		n = dir_record[i];

		// Found padding bytes... Skipping remaining data until next LBA
		if(n == 0x00) {
			i++;
			continue;
		}

		// If not a directory entry
		if(!(dir_record[25 + i] & (1 << 1))) {

			// Get file name
			memset(file_name, 0x00, sizeof(file_name));

			strncpy(file_name, (const char*)&dir_record[33 + i], dir_record[32 + i]);

			if((p = strrchr(file_name, ';')) != NULL) {
				*p = '\0';
			}

			sprintf(output_file_name, "%s%s%s", bdir, iso9660_dir_tree(directory_id), file_name);

			// Add file information to the files_list dynamic array
			files_list_size++;

			if(files_list == NULL) {
				files_list = malloc(files_list_size * sizeof(iso9660_file_info_t));
			} else {
				files_list = realloc(files_list, files_list_size * sizeof(iso9660_file_info_t));
			}

			strcpy(files_list[files_list_size-1].full_name, output_file_name);
			files_list[files_list_size-1].lba = *((uint32_t *)(dir_record + 2 + i)) - LBA_OFFSET;
			files_list[files_list_size-1].parent_dir = directory_id;
			files_list[files_list_size-1].size = *((uint32_t *)(dir_record + 10 + i));


			// Write file contents do disk
			iso9660_write_file(output_file_name, *((uint32_t *)(dir_record + 2 + i)) - LBA_OFFSET, *((uint32_t *)(dir_record + 10 + i)));
		}

		i += n;
	}

	free(dir_record);

	return 0;
}

void iso9660_dir_tree_helper(int directory_id, char *dtree) {
	iso9660_dir_info_t dir;

	iso9660_get_dir_info(directory_id, &dir);

	if(dir.id == 1) {
		//printf("%s\n", dir.name);
		strcat(dtree, dir.name);

		return;
	}

	iso9660_dir_tree_helper(dir.parent_id, dtree);

	//printf("%s\n", dir.name);
	strcat(dtree, dir.name);
	strcat(dtree, DIR_SEP);
}

char *iso9660_dir_tree(int directory_id) {
	static char dir_tree[2048];

	memset(dir_tree, 0x00, sizeof(dir_tree));

	iso9660_dir_tree_helper(directory_id, dir_tree);

	if(dir_tree[strlen(dir_tree) - 1] != DIR_SEP[0]) {
		strcat(dir_tree, DIR_SEP);
	}

	return dir_tree;
}

void iso9660_mkdir_tree_helper(int directory_id, char *dtree) {
	iso9660_dir_info_t dir;

	iso9660_get_dir_info(directory_id, &dir);

	if(dir.id == 1) {
		//printf("%s\n", dir.name);
		strcat(dtree, dir.name);
		//printf("mkdir %s\n", dtree);

#ifdef WINDOWS_PLATFORM
		_mkdir(dtree);
#else
		mkdir(dtree, S_IRWXU | S_IRWXG | S_IRWXO);
#endif

		return;
	}

	iso9660_mkdir_tree_helper(dir.parent_id, dtree);

	//printf("%s\n", dir.name);
	strcat(dtree, dir.name);
	strcat(dtree, DIR_SEP);

	//printf("mkdir %s\n", dtree);
#ifdef WINDOWS_PLATFORM
	_mkdir(dtree);
#else
	mkdir(dtree, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
}

void iso9660_mkdir_tree(int directory_id, char *basedir) {
	static char dir_tree[2048];

	strcpy(dir_tree, basedir);

	if(dir_tree[strlen(dir_tree)-1] == DIR_SEP[0]) {
		dir_tree[strlen(dir_tree)-1] = '\0';
	}

	iso9660_mkdir_tree_helper(directory_id, dir_tree);

	if(dir_tree[strlen(dir_tree) - 1] != DIR_SEP[0]) {
		strcat(dir_tree, DIR_SEP);
	}
}

int iso9660_dump_bootsector(char *outfilename) {
	FILE *outfile;
	int i;

	if((outfile = fopen(outfilename, "wb")) == NULL) {
		printf("Error creating bootsector file: %s\n", outfilename);

		return 1;
	}

	for(i = 0; i < 16; i++) {
		fseek(iso_file, i * SECTOR_SIZE, SEEK_SET);
		fread(lba_buffer, SECTOR_SIZE, 1, iso_file);
		fwrite(lba_buffer, SECTOR_SIZE, 1, outfile);
	}

	fclose(outfile);

	printf("First 16 sectors dumped as: %s\n", outfilename);

	return 0;
}

int iso9660_write_sortfile_helper (const void *a, const void *b) {
	return (((iso9660_file_info_t *)b)->lba - ((iso9660_file_info_t *)a)->lba);
}

int iso9660_write_sortfile(char *outfilename) {
	FILE *outfile;
	int i;

	if((outfile = fopen(outfilename, "w")) == NULL) {
		printf("Error creating sortfile: %s\n", outfilename);

		return 1;
	}

	// Sort files_list array by LBA (DESC)
	qsort(files_list, files_list_size, sizeof(iso9660_file_info_t), iso9660_write_sortfile_helper);

	for(i = 0; i < files_list_size; i++) {
		fprintf(outfile, "%s %d\n",files_list[i].full_name, i+1);
	}

	fclose(outfile);

	printf("Sortfile saved as: %s\n", outfilename);

	return 0;
}

void iso9660_close(void) {
	if(iso_file != NULL)
		fclose(iso_file);

	if(path_table != NULL)
		free(path_table);

	if(files_list != NULL) {
		free(files_list);
	}
}
