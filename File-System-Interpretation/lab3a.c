/*
NAME: Jinbean Park
EMAIL: keiserssose@gmail.com
ID: 805330751
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include "ext2_fs.h" // for superblock

#define EXT2_SUPER_MAGIC 0xEF53
int fdi;
unsigned int bsize;

void setTimeFormat(char *timeFormat, time_t t) {
    struct tm timeInfo = *gmtime(&t);
    strftime(timeFormat, 100, "%m/%d/%y %H:%M:%S", &timeInfo);
}

void direcSummary(int numBlock, long nInode) {

    struct ext2_dir_entry direcEntry;
    long offsetIblock = 1024 + (numBlock - 1) * bsize;
    long byteOffset = 0;
    while (bsize > byteOffset) {
        pread(fdi, &direcEntry, sizeof(direcEntry), offsetIblock + byteOffset);
        // non-zero I-node number
        if (direcEntry.inode != 0) {
            fprintf(stdout, "DIRENT,%ld,%ld,%d,%d,%d,'%s'\n",
            nInode,
            byteOffset,
            direcEntry.inode,
            direcEntry.rec_len,
            direcEntry.name_len,
            direcEntry.name);
        }
    byteOffset += direcEntry.rec_len;
    }
}
 
void singleIndirecSummary(struct ext2_inode inode, long nInode, char fileType, char sdt, unsigned int parentBp) {

    int inodeIndx;
    int indirecBp;

    if (sdt == 's') { // Case 1. Single indirect block
        inodeIndx = 12;
        indirecBp = inode.i_block[12];
    }
    else if (sdt == 'd') { // Case 2. Double indirect block
        inodeIndx = 268; // 256 + 12
        indirecBp = parentBp;
    }
    else { // Case 3. Triple indirect block
        inodeIndx = 65804; // 65536 + 256 + 12
        indirecBp = parentBp;
    }

    unsigned int *blockPtrs = (unsigned int *)malloc(bsize);
    long offsetBlockPtr = 1024 + (indirecBp - 1) * bsize;
    pread(fdi, blockPtrs, bsize, offsetBlockPtr);
    
    // Check how many block pointers are there in a block.
    int numBptrs = bsize / sizeof(unsigned int);
    int i;
    for (i = 0; i < numBptrs; i++) {
        if (blockPtrs[i] != 0) {
            if (fileType == 'd')
                direcSummary(blockPtrs[i], nInode);
            else {
                printf("INDIRECT,%ld,%d,%d,%d,%d\n",
                nInode,
                1,
                inodeIndx + i,
                indirecBp,
                blockPtrs[i]);
            }
        }
    }
    free(blockPtrs);
}

void doubleIndirecSummary(struct ext2_inode inode, long nInode, char fileType, char sdt, unsigned parentBp) {

    int inodeIndx;
    int indirecBp;
    char whichSDT;
    
    if (sdt == 'd') { // Case 1. Double indirect block
        inodeIndx = 268; // 256 + 12
        indirecBp = inode.i_block[13];
        whichSDT = 'd';
    } 
    else { // Case 2. Triple indirect block
        inodeIndx = 65804; // 65536 + 256 + 12
        indirecBp = parentBp;
        whichSDT = 't';
    }

    unsigned int *doubleBlockPtr = (unsigned int *)malloc(bsize);
    long offsetDbBlockPtr = 1024 + (indirecBp - 1) * bsize;
    pread(fdi, doubleBlockPtr, bsize, offsetDbBlockPtr);

    // Check how many block pointers are there in a block.
    int numBptrs = bsize / sizeof(unsigned int);
    int i;
    for (i = 0; i < numBptrs; i++) {
        if (doubleBlockPtr[i] != 0) {
            printf("INDIRECT,%ld,%d,%d,%d,%d\n",
            nInode,
            2,
            inodeIndx + i,
            indirecBp,
            doubleBlockPtr[i]);
        }
        singleIndirecSummary(inode, nInode, fileType, whichSDT, doubleBlockPtr[i]);
    }
    free(doubleBlockPtr);
}

void tripleIndirecSummary(struct ext2_inode inode, long nInode, char fileType) {

    unsigned int *tripleBlockPtr = (unsigned int *)malloc(bsize);
    long offsetTriBlockPtr = 1024 + (inode.i_block[14] - 1) * bsize;
    pread(fdi, tripleBlockPtr, bsize, offsetTriBlockPtr);

    // Check how many block pointers are there in a block.
    int numBptrs = bsize / sizeof(unsigned int);
    int i;
    for (i = 0; i < numBptrs; i++) {
        if (tripleBlockPtr[i] != 0) {
            printf("INDIRECT,%ld,%d,%d,%d,%d\n",
            nInode,
            3,
            65804 + i,
            inode.i_block[14],
            tripleBlockPtr[i]);
        }
        doubleIndirecSummary(inode, nInode, fileType, 't', tripleBlockPtr[i]);
    }
    free(tripleBlockPtr);
}

static struct option long_options[] = { {0, 0, 0, 0} };

int main(int argc, char **argv) {

    int opt;
    
    opt = getopt_long(argc, argv, "", long_options, NULL);
    if (opt != -1) {
        fprintf(stderr, "Entered wrong argument.\n");
        exit(1);
    }
    if (argc > 2) {
        fprintf(stderr, "The number of arguments is wrong\n");
        exit(1);
    }
    if ((fdi = open(argv[1], O_RDONLY)) == -1) {
        fprintf(stderr, "Failed to open the input file '%s'\n", argv[1]);
        exit(1);
    }

    // 1. Superblock summary
    struct ext2_super_block superBlock;

    // The superblock is stored at an offset of 1024 bytes from the start.
    pread(fdi, &superBlock, sizeof(superBlock), 1024);

    // Calculate block size
    bsize = EXT2_MIN_BLOCK_SIZE << superBlock.s_log_block_size;

    // Summarizing the key file system parameters:
    printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", 
            superBlock.s_blocks_count,
            superBlock.s_inodes_count,
            bsize,
            superBlock.s_inode_size,
            superBlock.s_blocks_per_group,
            superBlock.s_inodes_per_group,
            superBlock.s_first_ino);


    // 2. Group summary

    // Calculate number of groups
    int numGroup = superBlock.s_blocks_count / superBlock.s_blocks_per_group;
    if ((superBlock.s_blocks_count % superBlock.s_blocks_per_group) != 0)
        numGroup++;

    int indexGp;
    for (indexGp = 0; indexGp < numGroup; indexGp++) {
        struct ext2_group_desc blockGroup;
        int blckNumGp, inodeNumGp;
        
        // Check if the block size is bigger than 1KB or not
        int startBlock;
        if (bsize == 1024)
            startBlock = 2;
        else
            startBlock = 1;
        
        // Calculate offset of each block group
        long offsetGp = (bsize * startBlock) + (32 * indexGp);

        pread(fdi, &blockGroup, sizeof(blockGroup), offsetGp);

        // The number of blocks & inodes in last group can be different with before groups
        if (indexGp == numGroup - 1) {
            blckNumGp = superBlock.s_blocks_count - (superBlock.s_blocks_per_group * (numGroup - 1));
            inodeNumGp = superBlock.s_inodes_count - (superBlock.s_inodes_per_group * (numGroup - 1));
        }
        else {
            blckNumGp = superBlock.s_blocks_per_group;
            inodeNumGp = superBlock.s_inodes_per_group;
        }

        printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
                indexGp,
                blckNumGp,
                inodeNumGp,
                blockGroup.bg_free_blocks_count,
                blockGroup.bg_free_inodes_count,
                blockGroup.bg_block_bitmap,
                blockGroup.bg_inode_bitmap,
                blockGroup.bg_inode_table);

        
        // 3. Free block entries
        long offsetBitmap = 1024 + (bsize * (blockGroup.bg_block_bitmap - 1));
        char *bgBlockBitmap = (char *)malloc(bsize);
        pread(fdi, bgBlockBitmap, bsize, offsetBitmap);
        long nBlock = superBlock.s_first_data_block + (superBlock.s_blocks_per_group * indexGp);
        unsigned int posByte, posBit;
        for (posByte = 0; posByte < bsize; posByte++) {
            char indexByte = bgBlockBitmap[posByte];
            unsigned char filter = 0x01;
            for (posBit = 0; posBit < 8; posBit++) {
                if (!(indexByte & filter))
                    printf("BFREE,%ld\n", nBlock);
                filter = filter << 1;
                nBlock++;
            }
        }
        free(bgBlockBitmap);


        // 4. Free i-node entries
        unsigned int numByte = superBlock.s_inodes_per_group / 8;
        char *bgInodeBitmap = (char *)malloc(numByte);

        long offsetInode = 1024 + (bsize * (blockGroup.bg_inode_bitmap - 1));
        pread(fdi, bgInodeBitmap, numByte, offsetInode);

        // inode number
        long nInode = (superBlock.s_inodes_per_group * indexGp) + 1; // Inodes are numbered starting from 1.
        
        //long nInode = superBlock.s_first_ino + (superBlock.s_inodes_per_group * indexGp);
        for (posByte = 0; posByte < numByte; posByte++) {
            char indexByte = bgInodeBitmap[posByte];
            unsigned char filter = 0x01;
            for (posBit = 0; posBit < 8; posBit++) {

                if (!(indexByte & filter)) // Case 1. Inode is free.
                    printf("IFREE,%ld\n", nInode);
                
                // 5. I-node summary
                else { // Case 2. Inode was allocated.
                    struct ext2_inode inode;
                    long offsetInodeTable = 1024 + (bsize * (blockGroup.bg_inode_table - 1));
                    long offsetInode = offsetInodeTable + ((posByte * 8) + posBit) * sizeof(inode);
                    pread(fdi, &inode, sizeof(inode), offsetInode);
                    
                    // Non-zero mode and non-zero link count
                    if (inode.i_mode == 0 || inode.i_links_count == 0) {
                        filter = filter << 1;
                        nInode++;
                        continue;
                    }

                    // File type
                    char fileType;
                    if (S_ISREG(inode.i_mode))
                        fileType = 'f';
                    else if (S_ISDIR(inode.i_mode))
                        fileType = 'd';
                    else if (S_ISLNK(inode.i_mode))
                        fileType = 's';
                    else
                        fileType = '?';
                    
                    // Mode
                    int mode = inode.i_mode & 0x0FFF;

                    // Time
                    char cTime[100], mTime[100], aTime[100];
                    setTimeFormat(cTime, inode.i_ctime);
                    setTimeFormat(mTime, inode.i_mtime);
                    setTimeFormat(aTime, inode.i_atime);

                    printf("INODE,%ld,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d",
                            nInode,
                            fileType,
                            mode,
                            inode.i_uid,
                            inode.i_gid,
                            inode.i_links_count,
                            cTime,
                            mTime,
                            aTime,
                            inode.i_size,
                            inode.i_blocks);

                    // The next fifteen fields are block addresses
                    int i;
                    if (fileType != 's' || inode.i_size > 60) {
                        for (i = 0; i < EXT2_N_BLOCKS; i++)
                            printf(",%d", inode.i_block[i]);
                    }
                    printf("\n");


                    // 6. Directory entries
                    if (fileType == 'd') {
                        for (i = 0; i < EXT2_NDIR_BLOCKS; i++) {
                            if (inode.i_block[i] != 0) {
                                direcSummary(inode.i_block[i], nInode);
                            }
                        }
                    }

                    // 7. Indirect block references
                    // 7 - 1 Single indirect
                    if (inode.i_block[12] != 0)
                        singleIndirecSummary(inode, nInode, fileType, 's', 0);

                    // 7 - 2 Double indirect
                    if (inode.i_block[13] != 0)
                        doubleIndirecSummary(inode, nInode, fileType, 'd', 0);

                    // 7 - 3 Triple indirect
                    if (inode.i_block[14] != 0)
                        tripleIndirecSummary(inode, nInode, fileType);

                }
                filter = filter << 1;
                nInode++;
            }
        }
        free(bgInodeBitmap);
    }
    close(fdi);
    exit(0);
}