#include "rprintf.h"
#include "fat.h"
#include "ide.h"
#include <string.h>
extern void putc(char c);

#define SECTOR_SIZE 512

struct boot_sector *bs;
int root_dir_region_start = 0;
char boot_sector[SECTOR_SIZE];
char root_directory_region[16384];

struct root_directory_entry *rde = (struct root_directory_entry *) root_directory_region;

// fatinit
void fatInit() {
   //Read boot sector and RDE region off the disk.
   ata_lba_read(2048,(unsigned char*)boot_sector, 1);
   bs = (struct boot_sector *) boot_sector;

  esp_printf((int (*)(int))putc,"Number of bytes per sector = %d\n",bs->bytes_per_sector);
  esp_printf((int (*)(int))putc,"Number of sectors per cluster = %d\n",bs->num_sectors_per_cluster);
  esp_printf((int (*)(int))putc,"Number of reserved sectors = %d\n",bs->num_reserved_sectors);
  esp_printf((int (*)(int))putc,"Number of FAT tables = %d\n",bs->num_fat_tables);
  esp_printf((int (*)(int))putc,"Number of RDEs = %d\n", bs->num_root_dir_entries);
  esp_printf((int (*)(int))putc,"Boot signature = %d\n", bs->boot_signature);

   root_dir_region_start = 2048
                         + bs->num_reserved_sectors
			 + (bs->num_fat_tables * bs->num_sectors_per_fat);
esp_printf((int (*)(int))putc,"root directory region start (sectors) = %d\n", root_dir_region_start);

 ata_lba_read(root_dir_region_start,(unsigned char*)root_directory_region,32);

}


void extract_filename(struct root_directory_entry *rde, char *fname);

// fatOpen()
// Find the RDE for a file given a path
struct root_directory_entry * fatOpen(char *path) {
 // Iterate throught the RDE region searching for a file's RDE. Return a pointer to that RDE.
   for(int k = 0; k < 512; k++){
      struct root_directory_entry *rde_ptr = &rde[k];

      if ((rde_ptr->file_name)[0] == 0x00)
          continue;
      char fname[13];
      extract_filename(rde_ptr, fname);


      if (strcmp(fname,path) == 0) {
        esp_printf((int (*)(int))putc,"Match!\n");
	 return rde_ptr;
      }

   // esp_printf("File name: \"%s.%s\"\n", rde[k].file_name, rde[k].file_extension);
   // esp_printf("Data cluster: %d\n", rde[k].cluster);
   // esp_printf("File size: %d\n", rde[k].file_size);
  }
  esp_printf((int (*)(int))putc,"File not found.\n");
  return NULL;
}  


void extract_filename(struct root_directory_entry *rde, char *fname) {
    int k = 0;
    while((rde->file_name[k] != ' ') && (k < 8)){
       fname[k] = (rde->file_name)[k];
       k++;
    }
    fname[k] = '\0';
    if((rde->file_extension)[0] == ' ') {
      return;
    }
    fname[k++] = '.';
    fname[k] = '\0';
    int n = 0;
   
    while((rde->file_extension[n] != ' ') && (n < 3)){
        fname[k] = (rde->file_extension)[n];
	k++;
	n++;
    }
    fname[k] = '\0';
  }


// strcpy()
void strcpy_nick(char *dest, char *src) {
 // copy from arr1 to arr2
  int k = 0;
  while(src[k] != 0) {
     dest[k] = src[k];
     k++;
  }
}

// strcmp()
int strcmp_nick(char *s1, char *s2) {
  int i = 0;
  while(s1[i] != '\0' && s2[i] != '\0') {
     if (s1[i] != s2[i]) {
         return 1;
       }	 
	 i++;
    }
  if (s1[i] == '\0' && s2[i] == '\0') {
     return 1;
   } else {
   return 0;
  }
}  



//int FatRead()
int fatRead(struct root_directory_entry *rde, char * buf, int n) {
  // read file data inside buf from file described for rde. 
  if (rde == NULL) {
    esp_printf((int (*)(int))putc,"fatRead: invalid RDE pointer\n");
    return -1;
  }

  int start_cluster = rde->cluster;
  int file_size = rde->file_size;

  int data_region_start = root_dir_region_start +
                         ((bs->num_root_dir_entries * 32) / bs->bytes_per_sector);

  int first_sector_of_cluster = data_region_start +
                               ((start_cluster - 2) * bs->num_sectors_per_cluster);
			       
  int bytes_to_read = (n < file_size) ? n : file_size;

  int sectors_to_read = (bytes_to_read + bs->bytes_per_sector - 1) / bs->bytes_per_sector;

  esp_printf((int (*)(int))putc,"Reading file at cluster %d, %d bytes (%d sectors)\n",
             start_cluster, bytes_to_read, sectors_to_read);

  ata_lba_read(first_sector_of_cluster,(unsigned char*)buf, sectors_to_read);

  return bytes_to_read;

}



int main() {
//  struct root_directory_entry *rde = (struct root_directory_entry *) root_directory_region;
  //  char dataBuf[100];

 // fatInit();
 // struct rde *rde = fatOpen("file.txt");
 // fatRead(rde,dataBuf,sizeof(dataBuf));
 // struct root_directory_entry *myfile = fatOpen("TEST.TXT");
 //  if (myfile != NULL) {
 //  int bytes = fatRead(myfile, dataBuf, sizeof(dataBuf));
 //  esp_printf("Read %d bytes from file.\n", bytes);

 //  for (int i = 0; i < bytes; i++) {
  //   esp_printf("%c", dataBuf[i]);
 //  }
 //  esp_printf("\n");

// } else {
 // printf("data read from file = %s\n", dataBuf);
 // esp_printf("File not found.\n");
 // }
 // return 0;
 }
