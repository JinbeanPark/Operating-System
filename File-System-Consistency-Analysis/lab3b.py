#NAME: Jinbean Park
#EMAIL: keiserssose@gmail.com
#ID: 805330751

import sys, csv

freeBlockEntries = []
freeInodeEntries = []
inodeContents = []
direcContents = []
indirecContents = []
referBlockDict = dict()
reservedBlocks = set([1, 2, 3, 4, 5, 6, 7, 64])

class superblock:
    def __init__(self, line):
        self.s_blocks_count = int(line[1])
        self.s_inodes_count = int(line[2])
        self.bsize = int(line[3])
        self.s_inode_size = int(line[4])
        self.s_blokcs_per_group = int(line[5])
        self.s_inodes_per_group = int(line[6])
        self.s_first_ino = int(line[7])

class Group:
    def __init__(self, line):
        self.inodeNumGp = int(line[3])
        self.bg_inode_table = int(line[8])

class inode:
    def __init__(self, line):
        self.nInode = int(line[1])
        self.fileType = line[2]
        self.i_links_count = int(line[6])
        self.i_size = int(line[10])
        self.direcBlock = [int(bN) for bN in line[12:24]]
        self.inDirecBlock = [int(bN) for bN in line[24:27]]
        
class dirent:
    def __init__(self, line):
        self.nInode = int(line[1])
        self.inode = int(line[3])
        self.name = line[6]
    
class indirect:
    def __init__(self, line):
        self.nInode = int(line[1])
        self.level = int(line[2])
        self.blockOffset = int(line[3])
        self.blockPtr = int(line[5])

def main():
    
    # Execution errors should be reported to standard error.
    if len(sys.argv) > 2:
        sys.stderr.write("Invalid arguments\n")
        exit(1)
    try:
        csvFile = open(sys.argv[1], 'r')
    except:
        sys.stderr.write("Unable to open required file\n")
        exit(1)
    
    # Python has a csv library
    readLines = csv.reader(csvFile)
    
    for line in readLines:
        if line[0] == "SUPERBLOCK":
            superBlock = superblock(line)
        elif line[0] == "GROUP":
            group = Group(line)
        elif line[0] == "BFREE":
            freeBlockEntries.append(int(line[1]))
        elif line[0] == "IFREE":
            freeInodeEntries.append(int(line[1]))
        elif line[0] == "INODE":
            inodeContents.append(inode(line))
        elif line[0] == "DIRENT":
            direcContents.append(dirent(line))
        elif line[0] == "INDIRECT":
            indirecContents.append(indirect(line))
        else:
            sys.stderr.write("There is invalid line in CSV file\n")
            exit(1)
    
    MaxBlockNum = superBlock.s_blocks_count
    dupBlockList = []

    # 1. Block Consistency Audits
    # Every block pointer in an I-node or indirect block should be valid or zero.
    for inodeElmt in inodeContents:

        # For symbolic links with a short length the i-node block pointer.
        if inodeElmt.fileType == 's' and inodeElmt.i_size <= 60:
            continue

        # 1) Direct block pointer
        blockOffset = 0
        for blockPtr in inodeElmt.direcBlock:
            if blockPtr == 0:
                continue
            # (1) Invalid
            if blockPtr < 0 or blockPtr > MaxBlockNum:
                print('INVALID BLOCK ' + str(blockPtr) + ' IN INODE ' + str(inodeElmt.nInode) + ' AT OFFSET 0')
            # (2) Reserved
            elif blockPtr in reservedBlocks:
                print('RESERVED BLOCK ' + str(blockPtr) + ' IN INODE ' + str(inodeElmt.nInode) + ' AT OFFSET 0')
            # (3) Check unreferenced / allocated / duplicate
            elif blockPtr not in referBlockDict:
                referBlockDict[blockPtr] = [[inodeElmt.nInode, blockOffset, 0]] # inode number, offset, level
            else:
                dupBlockList.append(blockPtr)
                referBlockDict[blockPtr].append([inodeElmt.nInode, blockOffset, 0])
            blockOffset += 1
        

        # 2) Indirect single block pointer
        blockPtr = inodeElmt.inDirecBlock[0]
        if blockPtr != 0:
            # (1) Invalid
            if blockPtr < 0 or blockPtr > MaxBlockNum:
                print('INVALID INDIRECT BLOCK ' + str(blockPtr) + ' IN INODE ' + str(inodeElmt.nInode) + ' AT OFFSET 12')
            # (2) Reserved
            elif blockPtr in reservedBlocks:
                print('RESERVED INDIRECT BLOCK ' + str(blockPtr) + ' IN INODE ' + str(inodeElmt.nInode) + ' AT OFFSET 12')
            # (3) Check unreferenced / allocated / duplicate
            elif blockPtr not in referBlockDict:
                referBlockDict[blockPtr] = [[inodeElmt.nInode, 12, 1]] # inode number, offset, level
            else:
                dupBlockList.append(blockPtr)
                referBlockDict[blockPtr].append([inodeElmt.nInode, 12, 1])  
        

        # 3) Indirect double block pointer
        blockPtr = inodeElmt.inDirecBlock[1]
        if blockPtr != 0:
            # (1) Invalid
            if blockPtr < 0 or blockPtr > MaxBlockNum:
                print('INVALID DOUBLE INDIRECT BLOCK ' + str(blockPtr) + ' IN INODE ' + str(inodeElmt.nInode) + ' AT OFFSET 268')
            # (2) Reserved
            elif blockPtr in reservedBlocks:
                print('RESERVED DOUBLE INDIRECT BLOCK ' + str(blockPtr) + ' IN INODE ' + str(inodeElmt.nInode) + ' AT OFFSET 268')
            # (3) Check unreferenced / allocated / duplicate
            elif blockPtr not in referBlockDict:
                referBlockDict[blockPtr] = [[inodeElmt.nInode, 268, 2]] # inode number, offset, level
            else:
                dupBlockList.append(blockPtr)
                referBlockDict[blockPtr].append([inodeElmt.nInode, 268, 2])  
        

        # 4) Indirect triple block pointer
        blockPtr = inodeElmt.inDirecBlock[2]
        if blockPtr != 0:
            # (1) Invalid
            if blockPtr < 0 or blockPtr > MaxBlockNum:
                print('INVALID TRIPLE INDIRECT BLOCK ' + str(blockPtr) + ' IN INODE ' + str(inodeElmt.nInode) + ' AT OFFSET 65804')
            # (2) Reserved
            elif blockPtr in reservedBlocks:
                print('RESERVED TRIPLE INDIRECT BLOCK ' + str(blockPtr) + ' IN INODE ' + str(inodeElmt.nInode) + ' AT OFFSET 65804')
            # (3) Check unreferenced / allocated / duplicate
            elif blockPtr not in referBlockDict:
                referBlockDict[blockPtr] = [[inodeElmt.nInode, 65804, 3]] # inode number, offset, level
            else:
                dupBlockList.append(blockPtr)
                referBlockDict[blockPtr].append([inodeElmt.nInode, 65804, 3])  
        

    # 5) Block pointers in indirect block
    for indirecElmt in indirecContents:
        indirecLvl = ""
        if indirecElmt.level == 2:
            indirecLvl = "DOUBLE"
        elif indirecElmt.level == 3:
            indirecLvl = "TRIPLE"
        # (1) Invalid
        if indirecElmt.blockPtr < 0 or indirecElmt.blockPtr > MaxBlockNum:
            print('INVALID ' + indirecLvl + ' INDIRECT BLOCK ' + str(indirecElmt.blockPtr) + ' IN INODE ' + str(indirecElmt.nInode) + ' AT OFFSET ' + str(indirecElmt.offset))
        # (2) Reserved
        elif indirecElmt.blockPtr in reservedBlocks:
            print('RESERVED ' + indirecLvl + ' INDIRECT BLOCK ' + str(indirecElmt.blockPtr) + ' IN INODE ' + str(indirecElmt.nInode) + ' AT OFFSET ' + str(indirecElmt.offset))
        # (3) Check unreferenced / allocated / duplicate
        elif indirecElmt.blockPtr not in referBlockDict:
            referBlockDict[indirecElmt.blockPtr] = [[indirecElmt.nInode, indirecElmt.blockOffset, indirecElmt.level]]
        else:
            dupBlockList.append(indirecElmt.blockPtr)
            referBlockDict[indirecElmt.blockPtr].append([indirecElmt.nInode, indirecElmt.offset, indirecElmt.level])
    

    # 6) Print out unreferenced / allocated
    # Inodes per block = block size / size of inode
    # Size in blocks of the inode table
    # First legal block number = first inode block + size of the inode table.
    startBlock = group.bg_inode_table + superBlock.s_inode_size * group.inodeNumGp / superBlock.bsize
    for block in range(startBlock, MaxBlockNum):
        # (1) Unreferenced
        if block not in referBlockDict and block not in freeBlockEntries:
            print('UNREFERENCED BLOCK ' + str(block))
        # (2) Allocated
        elif block in referBlockDict and block in freeBlockEntries:
            print('ALLOCATED BLOCK ' + str(block) + ' ON FREELIST')


    # 7) Print out duplicate
    # Delete overlap element in list.
    dupBlockList = list(set(dupBlockList))
    for block in dupBlockList:
        for blockPtr in referBlockDict[block]:
            blockLvl = ""
            if blockPtr[2] == 1:
                blockLvl = "INDIRECT "
            elif blockPtr[2] == 2:
                blockLvl = "DOUBLE INDIRECT "
            elif blockPtr[2] == 3:
                blockLvl = "TRIPLE INDIRECT "
            print('DUPLICATE ' + blockLvl + 'BLOCK ' + str(block) + ' IN INODE ' + str(blockPtr[0]) + ' AT OFFSET ' + str(blockPtr[1]))


    # 2. I-node Allocation Audits
    # 1) Inode have some valid type while inode is on a free I-node list.
    inodeNumList = []
    for inodeElmt in inodeContents:
        if inodeElmt.nInode == 0:
            continue
        inodeNumList.append(inodeElmt.nInode)
        if inodeElmt.nInode in freeInodeEntries:
            print('ALLOCATED INODE ' + str(inodeElmt.nInode) + ' ON FREELIST')
    
    # 2) Unallocated I-node is on a free I-node list.
    for inodeElmt in inodeContents:
        if inodeElmt.fileType == '0' and inodeElmt.nInode not in freeInodeEntries:
            print('UNALLOCATED INODE ' + str(inodeElmt.nInode) + ' NOT ON FREELIST')
    
    for inodeNum in range(superBlock.s_first_ino, superBlock.s_inodes_count):
        if inodeNum not in inodeNumList and inodeNum not in freeInodeEntries:
            print('UNALLOCATED INODE ' + str(inodeNum) + ' NOT ON FREELIST')
    
    # 3) Directory entries refer to invalid and unallocated I-nodes.    
    # 4) Scan all of the directories to enumerate all links.
    discoveredLinks = dict()
    for direcEntry in direcContents:
        if direcEntry.inode < 1 or direcEntry.inode > superBlock.s_inodes_count:
            print('DIRECTORY INODE ' + str(direcEntry.nInode) + ' NAME ' + direcEntry.name + ' INVALID INODE ' + str(direcEntry.inode))
        elif direcEntry.inode not in inodeNumList:
            print('DIRECTORY INODE ' + str(direcEntry.nInode) + ' NAME ' + direcEntry.name + ' UNALLOCATED INODE ' + str(direcEntry.inode))
        else:
            if direcEntry.inode not in discoveredLinks:
                discoveredLinks[direcEntry.inode] = 1
            else:
                discoveredLinks[direcEntry.inode] += 1
            
    # 5) Allocated I-node whose reference count does not match the number of discovered links.
    for inodeElmt in inodeContents:
        if inodeElmt.nInode in discoveredLinks:
            if inodeElmt.i_links_count != discoveredLinks[inodeElmt.nInode]:
                print('INODE ' + str(inodeElmt.nInode) + ' HAS ' + str(discoveredLinks[inodeElmt.nInode]) + ' LINKS BUT LINKCOUNT IS ' + str(inodeElmt.i_links_count))
        else:
            if inodeElmt.i_links_count != 0:
                print('INODE ' + str(inodeElmt.nInode) + ' HAS 0 LINKS BUT LINKCOUNT IS ' + str(inodeElmt.i_links_count))

    
    # 6) Check for the correctness of itself (.) and one to its parent(..) on every directory.
    parentDict = dict()
    for direcEntry in direcContents:
        if direcEntry.name != "'.'" and direcEntry.name != "'..'":
            parentDict[direcEntry.inode] = direcEntry.nInode
    for direcEntry in direcContents:
        if direcEntry.name == "'.'" and direcEntry.nInode != direcEntry.inode:
            print('DIRECTORY INODE ' + str(direcEntry.nInode) + ' NAME ' + "'.'" + ' LINK TO INODE ' + str(direcEntry.inode) + ' SHOULD BE ' + str(direcEntry.nInode))
        elif direcEntry.nInode in parentDict:
            if direcEntry.name == "'..'" and parentDict[direcEntry.nInode] != direcEntry.inode:
                print('DIRECTORY INODE ' + str(direcEntry.nInode) + ' NAME ' + "'..'" + ' LINK TO INODE ' + str(direcEntry.inode) + ' SHOULD BE ' + str(parentDict[direcEntry.nInode]))
        elif direcEntry.nInode not in parentDict:
            if direcEntry.name == "'..'" and direcEntry.inode != direcEntry.nInode:
                print('DIRECTORY INODE ' + str(direcEntry.nInode) + ' NAME ' + "'..'" + ' LINK TO INODE ' + str(direcEntry.inode) + ' SHOULD BE ' + str(direcEntry.nInode))
    
    
if __name__ == '__main__':
    main()