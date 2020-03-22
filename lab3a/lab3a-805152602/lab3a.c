// NAME: Jenny Zhan
// EMAIL: yqzhan@g.ucla.edu
// ID: 005139695

// NAME: Wenxuan Liu
// EMAIL: allenliux01@163.com
// ID: 805152602

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ext2_fs.h"

int disk_fd;
struct ext2_super_block superblock; //for the only one superblock
struct ext2_group_desc group;   //for current group
unsigned block_size;
unsigned current_group_offset;


void superblock_summary()
{
    if (pread(disk_fd, &superblock, sizeof(superblock), 1024)<0)
    {
        fprintf(stderr, "ERROR: can't read superblock.\n");
        exit(1);
    }
    if (superblock.s_magic != EXT2_SUPER_MAGIC)
    {
        fprintf(stderr, "ERROR: wrong magic number for superblock.\n");
        exit(1);
    }
    block_size=EXT2_MIN_BLOCK_SIZE << superblock.s_log_block_size;
    fprintf(stdout, "SUPERBLOCK,%u,%u,%u,%u,%u,%u,%u\n", superblock.s_blocks_count, superblock.s_inodes_count, block_size, superblock.s_inode_size, superblock.s_blocks_per_group, superblock.s_inodes_per_group, superblock.s_first_ino);
}

unsigned long block_offset(unsigned int block) {
    return 1024 + (block - 1) * block_size;
}

void directory_entries(unsigned int inode, unsigned int block_num)
{
    unsigned total_rec_len=0;
    while (1)
    {
        if (total_rec_len>= block_size)
        {
            break;
        }
        struct ext2_dir_entry dir;
        if (pread(disk_fd, &dir, sizeof(dir), block_size*block_num+total_rec_len)<0)
        {
            fprintf(stderr, "ERROR: can't pread in directory_entries.\n");
            exit(1);
        }
        if (dir.inode==0)
        {
            break;
        }
        fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,'%s'\n",inode, total_rec_len,dir.inode,dir.rec_len,dir.name_len,dir.name);
        total_rec_len+=dir.rec_len;
    }
}


void indirect(unsigned int inode_num, unsigned int offset, unsigned int exp, char filetype){
    __u32 *indir_ptrs = malloc(block_size);
    __u32 ptr_num = block_size / sizeof(__u32);

    unsigned long indir3_offset = block_offset(offset);
    pread(disk_fd, indir_ptrs, block_size, indir3_offset);

    unsigned int i, j, k;
    for (i = 0; i < ptr_num; i++) {
        if (indir_ptrs[i] != 0) {
            if (filetype == 'd' && exp==1) {
                directory_entries(inode_num, indir_ptrs[i]);
            }
            unsigned int blockoff = 12;
            if(exp==2)
                blockoff += 256;
            else if(exp==3)
                blockoff += 256*256+256;
            fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inode_num, exp, blockoff+i, offset, indir_ptrs[i]);
            //double indirect:
            if(exp>1){
                __u32 *indir2_ptrs = malloc(block_size);
                unsigned long indir2_offset = block_offset(indir_ptrs[i]);
                pread(disk_fd, indir2_ptrs, block_size, indir2_offset);

                for (j = 0; j < ptr_num; j++) {
                    if (indir2_ptrs[j] != 0) {
                        if (filetype == 'd' && exp==2) {
                            directory_entries(inode_num, indir2_ptrs[i]);
                        }
                        fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inode_num, exp-1, blockoff+j, indir_ptrs[i], indir2_ptrs[j]);  
                        //triple indirect
                        if(exp>2){
                            __u32 *indir3_ptrs = malloc(block_size);
                            unsigned long indir_offset = block_offset(indir2_ptrs[j]);
                            pread(disk_fd, indir3_ptrs, block_size, indir_offset);
                            for (k = 0; k < ptr_num; k++) {
                                if (indir3_ptrs[k] != 0) {
                                    if (filetype == 'd') {
                                        directory_entries(inode_num, indir3_ptrs[k]);
                                    }
                                    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inode_num, exp-2, blockoff+k, indir2_ptrs[j], indir3_ptrs[k]);
                                }
                            }
                            free(indir3_ptrs);
                        }
                    }
                }
                free(indir2_ptrs);
            }
        }
    }
    free(indir_ptrs);
}

void free_block_entries()
{
    char block_bitmap[block_size];

    int count=0;
    if (pread(disk_fd,&block_bitmap,sizeof(block_bitmap),current_group_offset+(block_size*group.bg_block_bitmap))<0)
    {
        fprintf(stderr, "ERROR: can't pread in free block entries.\n");
        exit(1);
    }
    int i;
    int j;
    for (i=0;i<(int)block_size;i++)
    {
        char tmp=block_bitmap[i];
        for (j=0;j<8;j++)

        {
            count+=1;
            if ((tmp&0x01)==0)
            {
                fprintf(stdout, "BFREE,%d\n",count);
            }
            tmp=tmp>>1;
        }
    }
}

void get_time(time_t raw_time, char* buf) {
    struct tm ts = *gmtime(&raw_time);
    strftime(buf, 80, "%m/%d/%y %H:%M:%S", &ts);
}

void read_inode(unsigned int inode_num){
    struct ext2_inode inode;
    unsigned long offset = block_offset(group.bg_inode_table) + (inode_num-1) * sizeof(inode);
    pread(disk_fd, &inode, sizeof(inode), offset);
    if (inode.i_mode == 0 && inode.i_links_count == 0) {
        return;
    }
    char filetype = '0';
    uint16_t file_val = (inode.i_mode >> 12) << 12;
    if (file_val == 0x4000) { //directory
        filetype = 'd';
    }
    else if (file_val == 0x8000) { //regular file
        filetype = 'f';
    } 
    else if (file_val == 0xa000) { //symbolic link
        filetype = 's';
    }
    else
    {
        filetype='?';
    }

    unsigned int block_num = 2 * (inode.i_blocks / (2 << superblock.s_log_block_size));
    fprintf(stdout, "INODE,%d,%c,%o,%d,%d,%d,",inode_num, filetype, inode.i_mode & 0xFFF,inode.i_uid,inode.i_gid,inode.i_links_count);

    char ctime[40], mtime[40], atime[40];
    get_time(inode.i_ctime, ctime); //creation time
    get_time(inode.i_mtime, mtime); //modification time
    get_time(inode.i_atime, atime); //access time
    fprintf(stdout, "%s,%s,%s,", ctime, mtime, atime);     
    fprintf(stdout, "%d,%d", inode.i_size, block_num);
    unsigned int i;
    for (i = 0; i < 15; i++) { //block addresses
        fprintf(stdout, ",%d", inode.i_block[i]);
    }
    fprintf(stdout, "\n");

    //if the filetype is a directory, need to create a directory entry
    for (i = 0; i < 12; i++) { //direct entries
        if (inode.i_block[i] != 0 && filetype == 'd') {
            directory_entries(inode_num, inode.i_block[i]);
        }
    }

    if(inode.i_block[12] != 0){ //12
        indirect(inode_num, inode.i_block[12], 1, filetype);
    }

    if(inode.i_block[13] != 0){   //13
        indirect(inode_num, inode.i_block[13], 2, filetype);
    }

    if(inode.i_block[14] != 0){   //14
        indirect(inode_num, inode.i_block[14], 3, filetype);
    }


}


void free_inode_entries()
{
    char inode_bitmap[block_size];
    int count=1;
    if (pread(disk_fd,&inode_bitmap,sizeof(inode_bitmap),current_group_offset+(block_size*group.bg_inode_bitmap))<0)
    {
        fprintf(stderr, "ERROR: can't pread in free block entries.\n");
        exit(1);
    }
    int i=0;
    int j;
    for (;i<(int)(superblock.s_inodes_per_group/8);i++)
    {
        char tmp=inode_bitmap[i];
        for (j=0;j<8;j++)
        {
            if ((tmp&0x01)==0)
            {
                fprintf(stdout, "IFREE,%d\n",count);
            }
            else
            {
                read_inode(count);
            }
            
            tmp=tmp>>1;
            count+=1;
        }
    }
}



void group_summary()
{
    int i=0;
    if (pread(disk_fd, &group, sizeof(group), 2*block_size)<0)
    {
        fprintf(stderr, "ERROR: can't pread in group summary.\n");
        exit(1);
    }
    fprintf(stdout,"GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",i,superblock.s_blocks_count,superblock.s_inodes_count,group.bg_free_blocks_count,group.bg_free_inodes_count,group.bg_block_bitmap,group.bg_inode_bitmap,group.bg_inode_table);
    free_block_entries();
    free_inode_entries();
}

int main(int argc, const char * argv[]) 
{
    if (argc != 2)
    {
        fprintf(stderr, "ERROR: can't recognize the argument.\n");
        exit(1);
    }
    if ((disk_fd = open(argv[1], O_RDONLY)) < 0)
    {
        fprintf(stderr, "ERROR: can't open.\n");
        exit(1);
    }

    superblock_summary();
    group_summary();
    return 0;
}
