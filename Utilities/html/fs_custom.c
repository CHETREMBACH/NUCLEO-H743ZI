
#include "ff.h"
extern FATFS filesystem;

#include <stdio.h>
#include <string.h>

#include "printf_dbg.h"

#include "lwip/apps/httpd_opts.h"
#include "lwip/def.h"
#include "lwip/apps/fs.h"
#include "fsdata.h"
#include <string.h>

struct fs_custom_data {
	FIL *f;
};
DIR d;

void
fs_ex_init(const char *httpd_root_dir)
{
	printf("fs_ex_init \n");
	//fs_ex_root_dir = strdup(httpd_root_dir);
}

int fs_open_custom(struct fs_file *file_fs, const char *name) {
	
	FIL *file = malloc(sizeof(FIL));
	memset(file, 0, sizeof(FIL));
	  
	const char *str;
	str = name + 1; 
	//printf("try open file name = %s \n\r", str);
	/* Could not open filesystem */
	if(f_mount(&filesystem, (TCHAR const*)"", 0) == FR_OK)
	{
		/* Could not open the selected directory */
		if (f_opendir(&d, "/") == FR_OK)
		{
			/* If Could not open the file which will be transmitted  */
			if (f_open(file, (const TCHAR*)str, FA_OPEN_EXISTING | FA_READ) == FR_OK)
			{
				//printf ("try read stats file name from info = %s \n\r",info.name);
				file_fs->len = f_size(file);
				file_fs->index = 0;
				file_fs->pextension = file;
				file_fs->is_custom_file = 1;
				//printf("file opened size = %d\n\r", file_fs->len);
				return 1;
			}	
		}	
	}	
	
	free(file);
	return 0; 
}

int fs_close_custom(struct fs_file *file_fs) {
	
	if (file_fs && file_fs->pextension) {
		FIL *file = (FIL *)file_fs->pextension;
		if (file != NULL) {
			printf("fs_close_custom \n");			
			f_close(file);
			file = NULL;
		}
	}	
    return 1;
}

int fs_read_custom(struct fs_file *file_fs, char *buffer, int count) {
	FIL *file = (FIL *)file_fs->pextension;
	int len, left;
	int read = 0;
	//uint8_t lfs_read_buf[512] = {0};

  //printf("fs_read_custom try open read file name = %s count = %d \n\r", file_fs->data, count);
	if(file_fs->index == file_fs->len) {
		return FS_READ_EOF;
	}
	else {
		read = file_fs->len - file_fs->index;
		if (read > count) {
			read = count;
		}
		//if (read>512) read = 512;
		
		//printf("fs_read_custom read = %d \n\r", read);
#if LWIP_HTTPD_EXAMPLE_CUSTOMFILES_LIMIT_READ
	read = LWIP_MIN(read, LWIP_HTTPD_EXAMPLE_CUSTOMFILES_LIMIT_READ);
#endif		
		
		//read = lfs_file_read(&lfs, file, buffer, read);
		f_read(file, buffer, read, (void *)&len);	
		left = file_fs->len - file_fs->index;	
		file_fs->index += len;
		file_fs->is_custom_file = 1;
		
		if (left > 0)
		{
			tcpip_try_callback();
		}
		
		//MEMCPY(buffer,lfs_read_buf,read);
		//printf("try read = %d data from file = %s \n\r", read, lfs_read_buf);
	}
	return len;
}

