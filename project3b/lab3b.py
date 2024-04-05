import sys, math

if len(sys.argv) != 2:
    exit(1)


is_inconsistent = False

# print_set_flag message and set flag
def print_set_flag(message):
    print(message)
    is_inconsistent = True

filename = sys.argv[1]
file = open(filename, "r")

# initialize superblock attributes
total_blocks, total_inodes, block_size, inode_size, blocks_per_group, inodes_per_group, first_nonreserved_inode = [-1]*7

# function to check if a block pointer is reserved
def check_reserved(pointer):
    if pointer == 0:
        # invalid, not reserved
        return False
    blocks_in_inode_table = math.ceil((inodes_per_group*inode_size)/block_size)
    if pointer % blocks_per_group <= 4+blocks_in_inode_table:
        return True
    return False

class Group:
    def __init__(self, group_contents):
        if len(group_contents) != 8:
            print_set_flag("Wrong amount of contents for group!")
            exit(1)
        self.group_number = int(group_contents[0])
        self.blocks_in_group = int(group_contents[1])
        self.inodes_in_group = int(group_contents[2])
        self.free_blocks_in_group = int(group_contents[3])
        self.free_inodes_in_group = int(group_contents[4])
        self.free_block_bitmap_block_num = int(group_contents[5])
        self.free_inode_bitmap_block_num = int(group_contents[6])
        self.first_inode_block_num = int(group_contents[7])

class Inode:
    def __init__(self, inode_contents):
        if len(inode_contents) < 11:
            print_set_flag("Wrong amount of contents for dirent!")
            exit(1)
        self.inode_num = int(inode_contents[0])
        self.type = inode_contents[1]
        self.mode = int(inode_contents[2])
        self.owner = int(inode_contents[3])
        self.group = int(inode_contents[4])
        self.link_count = int(inode_contents[5])
        self.time_last_inode_change = inode_contents[6]
        self.mod_time = inode_contents[7]
        self.time_last_access = inode_contents[8]
        self.file_size = int(inode_contents[9])
        self.num_blocks_taken_up = inode_contents[10]
        self.block_pts = [] 
        if len(inode_contents) > 11:
            # there are pointers
            self.block_pts = inode_contents[11:]
            # print_set_flag(self.block_pts)
            self.block_pts = [int(x) for x in self.block_pts]
            # print_set_flag(self.block_pts)
        
class Dirent:
    def __init__(self, dirent_contents):
        if len(dirent_contents) != 6:
            print_set_flag("Wrong amount of contents for dirent!")
            exit(1)
        [self.parent_inode_number, self.logical_byte_offset, self.inode_of_refd, self.entry_len, self.name_len, self.name] = dirent_contents
        self.parent_inode_number = int(self.parent_inode_number)
        self.logical_byte_offset = int(self.logical_byte_offset)
        self.inode_of_refd = int(self.inode_of_refd)
        self.entry_len = int(self.entry_len)
        self.name_len = int(self.name_len)
        self.name = self.name.strip()

class Indirect:
    def __init__(self, indirect_contents):
        if len(indirect_contents) != 5:
            print_set_flag("Wrong amoutn of contents for indirect!")
            exit(1)
        self.inode_num = int(indirect_contents[0])
        self.level_indir = int(indirect_contents[1])
        self.logical_block_offset = int(indirect_contents[2])
        self.num_of_scanned_block = int(indirect_contents[3])
        self.num_of_refd_block = int(indirect_contents[4])
        
# arrays to store all groups, indirects, inodes, dirents after iterating through the results
groups = []
indirects = []
inodes = []
dirents = []

# uses sets to easily tell if something in on free list or not, or if allocated
bfree = set()
ifree = set()
allocated_inodes = set()

for this_line in file.readlines():
    line_contents = this_line.split(",")
    
    match line_contents[0]:
        case "SUPERBLOCK":
            line_contents[1:] = [int(x) for x in line_contents[1:]]
            [total_blocks, total_inodes, block_size, inode_size, blocks_per_group, inodes_per_group, first_nonreserved_inode] = line_contents[1:]
        case "GROUP":   
            groups.append(line_contents[1:])
        case "BFREE":
            bfree.add(int(line_contents[1]))
        case "IFREE":
            ifree.add(int(line_contents[1]))
        case "DIRENT":
            dirents.append(Dirent(line_contents[1:]))
        case "INODE":
            inodes.append(Inode(line_contents[1:]))
            allocated_inodes.add(int(line_contents[1]))
        case "INDIRECT":
            indirects.append(Indirect(line_contents[1:]))
        case _:
            print_set_flag("Unrecognizable input")
            exit(1)

# block audits
# every block pointer in an I-node or indirect block should be valid (a legal data block, within the file system) or zero.
def check_block_ptr(pointer):
    if pointer < 0 or pointer > total_blocks:
        return "INVALID" 
    # reserved blocks span the first four blocks of each block group
    if check_reserved(pointer):
        return "RESERVED"
    return "VALID"

# check every inode
for inode in inodes:
    if len(inode.block_pts) == 0:
        continue

    for it, pointer in enumerate(inode.block_pts):
        # check each block pointer in each inode 
        pointer_check = check_block_ptr(pointer)
        if (pointer_check) != "VALID":
            # handle invalid case
            print_set_flag_str = " BLOCK " + str(pointer) + " IN INODE " + str(inode.inode_num) + " AT OFFSET "
            if (it < 12):
                # first 12 direct pointers
                print_set_flag_str = pointer_check + print_set_flag_str + str(it)
            else:
                # indirect pointers
                if (it == 12):
                    print_set_flag_str = pointer_check + " INDIRECT" + print_set_flag_str + "12"
                elif (it == 13):
                    print_set_flag_str = pointer_check + " DOUBLE INDIRECT" + print_set_flag_str + "268"
                elif (it == 14):
                    print_set_flag_str = pointer_check + " TRIPLE INDIRECT" + print_set_flag_str + "65804"
                
            print_set_flag(print_set_flag_str)

# check every indirect block
for indirect in indirects:
    pointer_check = check_block_ptr(indirect.num_of_refd_block)
    
    if pointer_check != "VALID":
        # handle invalid case
        print_set_flag_str = pointer_check
        if indirect.level_indir == 2:
            print_set_flag_str += " DOUBLE "
        elif indirect.level_indir == 3:
            print_set_flag_str += " TRIPLE "
        print_set_flag_str += "INDIRECT BLOCK " + str(indirect.num_of_refd_block) + " IN INODE " + str(indirect.inode_num) + " AT OFFSET " + str(indirect.logical_block_offset)
        print_set_flag(print_set_flag_str)

# Determine all referenced blocks

reference_map = {} #keys are block numbers, value is array of references

# takes a reference and adds it to the reference map
def add_ref(pointer, the_object):
    if pointer in reference_map:
        reference_map[pointer].append(the_object)
    else:
        reference_map[pointer] = [the_object]

# build the reference map
for inode in inodes:
    for it, pointer in enumerate(inode.block_pts):
        if pointer != 0:
            print_set_flag_str = "BLOCK " + str(pointer) + " IN INODE " + str(inode.inode_num) + " AT OFFSET "
            if (it < 12):
                print_set_flag_str = print_set_flag_str + str(it)
            else:
                if (it == 12):
                    print_set_flag_str = "INDIRECT " + print_set_flag_str + "12"
                elif (it == 13):
                    print_set_flag_str = "DOUBLE INDIRECT " + print_set_flag_str + "268"
                elif (it == 14):
                    print_set_flag_str = "TRIPLE INDIRECT " + print_set_flag_str + "65804"
            add_ref(pointer, print_set_flag_str)

for indirect in indirects:
    print_set_flag_str = ""
    if indirect.level_indir == 2:
        print_set_flag_str += "DOUBLE "
    elif indirect.level_indir == 3:
        print_set_flag_str += "TRIPLE "
    print_set_flag_str += "INDIRECT BLOCK " + str(indirect.num_of_refd_block) + " IN INODE " + str(indirect.inode_num) + " AT OFFSET " + str(indirect.logical_block_offset)
    add_ref(indirect.num_of_refd_block, print_set_flag_str)


for block_num in range(1, total_blocks):
    # iterate through all blocks except block 0, the superblock. 0 indezed
    
    if not check_reserved(block_num):
        # don't consider block if they are reserved
        if block_num not in reference_map:
            # it is not referenced, implying it should be on the free list
            if block_num not in bfree:
                print_set_flag("UNREFERENCED BLOCK " + str(block_num))
        else:
            #block num is being referenced, should not be on freelist
            if block_num in bfree:
                print_set_flag("ALLOCATED BLOCK " + str(block_num) + " ON FREELIST")
            if len(reference_map[block_num]) > 1:
                # there are multiple references
                for reference in reference_map[block_num]:
                    print_set_flag("DUPLICATE " + reference)


# I-node Allocation Audits
for curr_inode_num in range(1, total_inodes+1):
    if curr_inode_num in ifree and curr_inode_num in allocated_inodes: # always an error is allocated inodes on freelist
        print_set_flag("ALLOCATED INODE " + str(curr_inode_num) + " ON FREELIST")
    elif curr_inode_num not in ifree and curr_inode_num not in allocated_inodes: 
        if curr_inode_num >= first_nonreserved_inode: # its okay for unallocated inodes to not be on freelist IFF reserved
            print_set_flag("UNALLOCATED INODE " + str(curr_inode_num) + " NOT ON FREELIST")

# Directory Consistency Audits
real_link_counts = {}
# enumerate actual amount of links for each inode
for dirent in dirents:
    if dirent.inode_of_refd in real_link_counts:
        real_link_counts[dirent.inode_of_refd] += 1
    else:
        real_link_counts[dirent.inode_of_refd] = 1

for inode in inodes:
    real_links = 0
    if inode.inode_num in real_link_counts:
        real_links = real_link_counts[inode.inode_num]
    if real_links != inode.link_count: # ensure that the recorded number of links is same as real
        print_set_flag("INODE " + str(inode.inode_num) + " HAS " + str(real_links) + " LINKS BUT LINKCOUNT IS " + str(inode.link_count))

for dirent in dirents:
    if dirent.inode_of_refd < 1 or dirent.inode_of_refd > total_inodes: # out of inode bounds checking
        print_set_flag("DIRECTORY INODE " + str(dirent.parent_inode_number) + " NAME " + dirent.name + " INVALID INODE " + str(dirent.inode_of_refd))
    elif dirent.inode_of_refd not in allocated_inodes: # check if referred inode is in allocated list
        print_set_flag("DIRECTORY INODE " + str(dirent.parent_inode_number) + " NAME " + dirent.name + " UNALLOCATED INODE " + str(dirent.inode_of_refd))
    elif dirent.name == "\'.\'" and dirent.inode_of_refd != dirent.parent_inode_number: # check that . refers to same directory inode
        print_set_flag("DIRECTORY INODE " + str(dirent.parent_inode_number) + " NAME " + dirent.name + " LINK TO INODE " + str(dirent.inode_of_refd) + " SHOULD BE " + str(dirent.parent_inode_number))
    elif dirent.name == "\'..\'":
        if dirent.parent_inode_number == 2: # special case for root directory
            if dirent.inode_of_refd != dirent.parent_inode_number:
                print_set_flag("DIRECTORY INODE " + str(dirent.parent_inode_number) + " NAME " + dirent.name + " LINK TO INODE " + str(dirent.inode_of_refd) + " SHOULD BE " + str(dirent.parent_inode_number))
        else:
            for check_dirent in dirents:
                if check_dirent.name == "\'.\'": # ignore match for inode referring to itself
                    continue
                if check_dirent.inode_of_refd == dirent.parent_inode_number: # an inode match should have a back and forth link with its child inode
                    if check_dirent.parent_inode_number != dirent.inode_of_refd:
                        print_set_flag("DIRECTORY INODE " + str(dirent.parent_inode_number) + " NAME " + dirent.name + " LINK TO INODE " + str(dirent.inode_of_refd) + " SHOULD BE " + str(dirent.parent_inode_number))
                        break

if (is_inconsistent):
    exit(2)
else:
    exit(0)