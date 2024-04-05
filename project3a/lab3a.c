// NAME: Tom Lam, Serena Mao
// EMAIL: tlam@hmc.edu, smao@hmc.edu
// ID: 40210352, 40210158

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#include "ext2_fs.h"

#define SB_OFF 1024
#define BLOCKSIZE 1024
#define MAX_NAME_LEN 4096
void handleIndirectBlock(int image_fd, int inode_num, int level, int num_being_scanned, int offset);
void handleDirectBlock(int image_fd, int block_num, int inode_num, unsigned int * dir_offset);
void handleIndirectBlockGetDirEntries(int image_fd, int inode_num, int level, int num_being_scanned, unsigned int * dir_offset);

// defining main with arguments
void Pread(int fd, void* buf, size_t count, off_t offset);
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect usage, lab3a takes requires ONE argument which is a file image!\n");
        exit(1);
    }

    char* image_filename = strdup(argv[1]);

    int image_fd = open(image_filename, O_RDONLY | O_EXCL);
    if (image_fd == -1) {
        fprintf(stderr, "Opening image file failed due to error: %s\n", strerror(errno));
        exit(1);
    }

    //     SUPERBLOCK

    // read in superblock
    struct ext2_super_block super_block;
    Pread(image_fd, &super_block, sizeof(super_block), SB_OFF);

    printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n",
        super_block.s_blocks_count,
        super_block.s_inodes_count,
        EXT2_MIN_BLOCK_SIZE << super_block.s_log_block_size, 
        super_block.s_inode_size,
        super_block.s_blocks_per_group, 
        super_block.s_inodes_per_group,
        super_block.s_first_ino);

    // GROUP
    // group number (decimal, starting from zero)
    // total number of blocks in this group (decimal)
    // total number of i-nodes in this group (decimal)
    // number of free blocks (decimal)
    // number of free i-nodes (decimal)
    // block number of free block bitmap for this group (decimal)
    // block number of free i-node bitmap for this group (decimal)
    // block number of first block of i-nodes in this group (decimal)
    int num_groups = ((EXT2_MIN_BLOCK_SIZE << super_block.s_log_block_size) + super_block.s_blocks_per_group - 1) / super_block.s_blocks_per_group;

    // Iterate through each group in the file system

    void* group_buf = malloc(BLOCKSIZE);

    Pread(image_fd, group_buf, BLOCKSIZE, SB_OFF+BLOCKSIZE);
    for (int group_id = 0; group_id < num_groups; group_id++) {
        // reading each group into group_desc
        struct ext2_group_desc* group_desc = (struct ext2_group_desc*) group_buf + group_id*sizeof(struct ext2_group_desc); // buffer to hold each group descriptor as it is read
        // calculate number of blocks in group
        int blocks_in_group = super_block.s_blocks_per_group; // default is blocks per group
        
        if (group_id == num_groups-1) {
            // edge case of last block group
            blocks_in_group = super_block.s_blocks_count % super_block.s_blocks_per_group;
            if (blocks_in_group == 0) {
                // perfectly divides, thus, last block is not special
                blocks_in_group = super_block.s_blocks_per_group;
            }
        }
        
        // calculate number of inodes in group
        int inodes_in_group = super_block.s_inodes_per_group;
        
        if (group_id == num_groups-1) {
            // edge case of last block group
            inodes_in_group = super_block.s_inodes_count % super_block.s_inodes_per_group;
            if (inodes_in_group == 0) {
                // perfectly divides, thus, last block is not special
                inodes_in_group = super_block.s_inodes_per_group;
            }
        }
        
        
        printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
            group_id,
            blocks_in_group,
            inodes_in_group,
            group_desc->bg_free_blocks_count,
            group_desc->bg_free_inodes_count,
            group_desc->bg_block_bitmap,
            group_desc->bg_inode_bitmap,
            (super_block.s_blocks_per_group*(group_id)) + 5);

        // read through the block bitmap 
        
        uint32_t* bitmap_buf = malloc(BLOCKSIZE);
        Pread(image_fd, bitmap_buf, BLOCKSIZE, BLOCKSIZE*(group_desc->bg_block_bitmap));

        uint16_t num_free_blocks = 0;

        // the number of 32 bit ints needed to iterate through the entire bitmap
        // divide by 32 and round up
        int ints_needed = ((blocks_in_group + 32 - 1)/32);
        // int 1 = 10101010101...
        // int 2 = 00000000000...0001

        int block_count = 0;
        for (int j = 0; j < ints_needed; j++) {
            for (int k = 0; k < 32; k++) {
                // Read each int from right to left
                block_count++;
                if (block_count > blocks_in_group) {
                    break;
                }
                
                int mask = 1 << k;
                // iterate through each possible block in group
                if((bitmap_buf[j] & mask) == 0) {
                    printf("BFREE,%d\n", k + 32 * j + 1 + group_id*super_block.s_blocks_per_group);
                    num_free_blocks++;
                }
            }
        }

        // read through the inode bitmaps
        uint32_t* inode_bitmap_buf = malloc(BLOCKSIZE);
        Pread(image_fd, inode_bitmap_buf, BLOCKSIZE, BLOCKSIZE*(group_desc->bg_inode_bitmap));

        ints_needed = ((inodes_in_group + 32 - 1)/32);

        int inode_count = 0;
        for (int j = 0; j < ints_needed; j++) {
            for (int k = 0; k < 32; k++) {
                inode_count++;
                if (inode_count > inodes_in_group) {
                    break;
                }
                int mask = 1 << k;
                // iterate through each possible block in group
                if((inode_bitmap_buf[j] & mask) == 0) {
                    printf("IFREE,%d\n", k + (32 * j) + 1 + group_id*super_block.s_inodes_per_group);
                }
            }
        }

        /*
            0.INODE
            1.inode number (decimal)
            2.file type ('f' for file, 'd' for directory, 's' for symbolic link, '?" for anything else)
            3.mode (low order 12-bits, octal ... suggested format "%o")
            4.owner (decimal)
            5.group (decimal)
            6.link count (decimal)
            7.time of last I-node change (mm/dd/yy hh:mm:ss, GMT)
            8.modification time (mm/dd/yy hh:mm:ss, GMT)
            9.time of last access (mm/dd/yy hh:mm:ss, GMT)
            10.file size (decimal)
            11.number of (512 byte) blocks of disk space (decimal) taken up by this file
        */
        void * inode_buf = malloc(super_block.s_inode_size * super_block.s_inodes_per_group);
        Pread(image_fd, inode_buf, super_block.s_inode_size*super_block.s_inodes_per_group, group_id*(blocks_in_group*BLOCKSIZE) + SB_OFF + (4 * BLOCKSIZE));
        struct tm* tm_info;
        

        for (int j = 0; j < (int) super_block.s_inodes_per_group; j++) {
            
            struct ext2_inode * curr_inode = inode_buf+(super_block.s_inode_size*j);

            if (curr_inode->i_mode != 0 && curr_inode->i_links_count != 0) {
                // determine file type
                char filetype = '?';
                if ((curr_inode->i_mode & 0xF000) == 0xA000) {
                    // symbolic link
                    filetype = 's';
                } else if ((curr_inode->i_mode & 0xF000) == 0x8000) {
                    // normal file
                    filetype = 'f';
                } else if ((curr_inode->i_mode & 0xF000) == 0x4000) {
                    // directory
                    filetype = 'd';
                }

                // calculate inode number
                int inode_num = (group_id*super_block.s_inodes_per_group) + j + 1;

                
                printf("INODE,%d,%c,%o,%d,%d,%d", 
                    inode_num,
                    filetype,
                    curr_inode->i_mode & 0xFFF,
                    curr_inode->i_uid,
                    curr_inode->i_gid,
                    curr_inode->i_links_count);
                // mm/dd/yy hh:mm:ss, GMT

                time_t time = curr_inode->i_ctime;
                tm_info = gmtime(&time);
                printf(",%02d/%02d/%02d %02d:%02d:%02d",
                    (tm_info->tm_mon)+1,
                    tm_info->tm_mday,
                    ((tm_info->tm_year)+1900)%100,
                    tm_info->tm_hour,
                    tm_info->tm_min,
                    tm_info->tm_sec
                );
                time = curr_inode->i_mtime;
                tm_info = gmtime(&time);
                printf(",%02d/%02d/%02d %02d:%02d:%02d",
                    (tm_info->tm_mon)+1,
                    tm_info->tm_mday,
                    ((tm_info->tm_year)+1900)%100,
                    tm_info->tm_hour,
                    tm_info->tm_min,
                    tm_info->tm_sec
                );
                time = curr_inode->i_atime;
                tm_info = gmtime(&time);
                printf(",%02d/%02d/%02d %02d:%02d:%02d",
                    (tm_info->tm_mon)+1,
                    tm_info->tm_mday,
                    ((tm_info->tm_year)+1900)%100,
                    tm_info->tm_hour,
                    tm_info->tm_min,
                    tm_info->tm_sec
                );
                    
                
                uint64_t top_32_bits = curr_inode->i_dir_acl;
                top_32_bits = top_32_bits << 32;
                uint64_t file_size = curr_inode->i_size | (top_32_bits);

                printf(",%ld,%d",
                    file_size,
                    curr_inode->i_blocks
                );

               
                if ((filetype == 'd' || filetype == 'f') || ((file_size > 60) && filetype == 's')){
                    // if it's a directory or a file, and file length greater than size of pointers
                    // iterate through the blocks, printing out valid pointers
                    for (int k = 0; k < EXT2_N_BLOCKS; k++){
                        // ONLY DIRECT BLOCKS
                    
                        printf(",%d", curr_inode->i_block[k]);
                    }
                }
                printf("\n");

                //if it's a directory, print the directory entries
                if (filetype == 'd') {
                    // read in i_block

                    // counter for directory entry offset
                    unsigned int dir_offset = 0;
                    for (unsigned int k = 0; k < 12; k++) {
                        // for every data block
                        int block_num = curr_inode->i_block[k];

                        while (dir_offset < (k+1)*BLOCKSIZE) {
                            
                            // continue going through this block until you get all the directory entries
                            struct ext2_dir_entry dir_entry;
                            Pread(image_fd, &dir_entry, sizeof(struct ext2_dir_entry), BLOCKSIZE*block_num + (dir_offset%BLOCKSIZE));
                            // printf("offset %d, limit %d, inode %d\n", dir_offset, (k+1)*BLOCKSIZE, dir_entry.inode);
                            
                            // check if inode number is not 0 
                            if ((dir_entry.inode == 0) && (dir_entry.rec_len != 0)){
                                // this is just padding for a deleted file
                                dir_offset += dir_entry.rec_len;
                                continue;
                            } else if (dir_entry.inode == 0) {
                                // this is the actual end
                                break;
                            }

                            char dir_entry_name[MAX_NAME_LEN];
                            memcpy(dir_entry_name, &dir_entry.name, dir_entry.name_len);
                            dir_entry_name[dir_entry.name_len] = '\0';
                            printf("DIRENT,%d,%d,%d,%d,%d,'%s'\n",inode_num, dir_offset, dir_entry.inode, dir_entry.rec_len, dir_entry.name_len, dir_entry_name);
                            // increment offset by entry size
                            dir_offset += dir_entry.rec_len;
                        }
                        
                    }
                    handleIndirectBlockGetDirEntries(image_fd, inode_num, 1, curr_inode->i_block[12],&dir_offset);
                    handleIndirectBlockGetDirEntries(image_fd, inode_num, 2, curr_inode->i_block[13],&dir_offset);
                    handleIndirectBlockGetDirEntries(image_fd, inode_num, 3, curr_inode->i_block[14],&dir_offset);
                    // dealing with indirect blocks
                }

                // call recursive function three times: once on single indirect, once on double indirect, once on triple indirect
                if ((filetype == 'f') || (filetype == 'd')) {
                    handleIndirectBlock(image_fd, inode_num, 1, curr_inode->i_block[12],12);
                    handleIndirectBlock(image_fd, inode_num, 2, curr_inode->i_block[13],268);
                    handleIndirectBlock(image_fd, inode_num, 3, curr_inode->i_block[14],65804);
                }

            }
        }
    }

    close(image_fd);
    exit(0);
}

// recursive function to print information about indirect blocks
void handleIndirectBlockGetDirEntries(int image_fd, int inode_num, int level, int num_being_scanned, unsigned int * dir_offset){
    // level 1 = single indirect
    // level 2 = double 
    // level 3 = triple indirect

    // store list of block pointers in this block
    uint32_t block_pointers[BLOCKSIZE/sizeof(uint32_t)];
    Pread(image_fd, block_pointers, BLOCKSIZE, num_being_scanned*BLOCKSIZE); // read in one block 
    
    for (long unsigned int k = 0; k < BLOCKSIZE/sizeof(uint32_t); k++){
        // for each entry in the indirect block
        // check if it's a valid pointer
        if (block_pointers[k] != 0) {
            if (level != 1) {
                // recursive case (not base case)
                handleIndirectBlockGetDirEntries(image_fd, inode_num, level-1, block_pointers[k], dir_offset);
            } else {
                // is base case, must handle this block 
                int block_num = block_pointers[k];
                handleDirectBlock(image_fd, block_num, inode_num, dir_offset);
            }
        }
    }
}

void handleDirectBlock(int image_fd, int block_num, int inode_num, unsigned int * dir_offset) {
    // this handles a single BLOCK of 1024, give the current offset's address
    do {
        // continue going through this block until you get all the directory entries
        struct ext2_dir_entry dir_entry;
        Pread(image_fd, &dir_entry, sizeof(struct ext2_dir_entry), BLOCKSIZE*block_num + (*dir_offset%BLOCKSIZE));

            // check if inode number is not 0 
        if ((dir_entry.inode == 0) && (dir_entry.rec_len != 0)){
            // this is just padding for a deleted file
            dir_offset += dir_entry.rec_len;
            continue;
        } else if (dir_entry.inode == 0) {
            // this is the actual end
            break;
        }

        char dir_entry_name[MAX_NAME_LEN];
        memcpy(dir_entry_name, &dir_entry.name, dir_entry.name_len);
        dir_entry_name[dir_entry.name_len] = '\0';
        printf("DIRENT,%d,%d,%d,%d,%d,'%s'\n",inode_num, *dir_offset, dir_entry.inode, dir_entry.rec_len, dir_entry.name_len, dir_entry_name);
        // increment offset by entry size
        *dir_offset += dir_entry.rec_len;

    } while (*dir_offset % BLOCKSIZE != 0);
}


// recursive function to print information about indirect blocks
void handleIndirectBlock(int image_fd, int inode_num, int level, int num_being_scanned, int offset){
    // level 1 = single indirect
    // level 2 = double 
    // level 3 = triple indirect

    // store list of block pointers in this block
    uint32_t block_pointers[BLOCKSIZE/sizeof(uint32_t)];
    Pread(image_fd, block_pointers, BLOCKSIZE, num_being_scanned*BLOCKSIZE); // read in one block 
    
    for (long unsigned int k = 0; k < BLOCKSIZE/sizeof(uint32_t); k++){
        // for each entry in the indirect block
        // check if it's a valid pointer
        if (block_pointers[k] != 0) {
            printf("INDIRECT,%d,%d,%ld,%d,%d\n", inode_num, level, offset+k, num_being_scanned,block_pointers[k]);
            if (level != 1) {
                // recursive case (not base case)
                handleIndirectBlock(image_fd, inode_num, level-1, block_pointers[k], offset + k);
            }
        }
    }   
    
}

// error checked pread
void Pread(int fd, void* buf, size_t count, off_t offset) {
    if (pread(fd, buf, count, offset) == -1) {
        fprintf(stderr, "Pread failed\n");
        exit(2);
    }
}