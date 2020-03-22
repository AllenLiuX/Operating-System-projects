#! /usr/local/cs/bin/python3

import sys
from collections import defaultdict


class Parser(object):
    def __init__(self):
        self.superblock = []
        self.group = []
        self.bfree = []
        self.ifree = []
        self.inode = []
        self.dirent = []
        self.indirect = []
        self.block_size = 0
        self.inode_size = 0
        self.num_blocks = 0
        self.num_inodes = 0
        self.first_valid_block = 0
        self.first_inode=0
        self.total_inodes=0
        self.block_list = defaultdict(list)
        self.allocated_inode_map=[]
        self.free_inode_map=[]
        self.link_count=[]


    def list_parser(self, str):
        out = []
        temp = ''
        for c in str:
            if c==',' and temp != '':
                if temp.isdigit():
                    out.append(int(temp))
                    temp = ''
                else:
                    out.append(temp)
                    temp = ''
            elif c != '\n':
                temp = temp+c
        if temp.isdigit():
            out.append(int(temp))
        else:
            out.append(temp)
        return out

    def getdata(self, f):
        for line in f:
            info = self.list_parser(line)
            if info[0] == 'SUPERBLOCK':
                self.superblock.append(info)
                self.total_inodes=info[2]
                self.block_size = info[3]
                self.inode_size = info[4]
                self.first_inode=info[7]
            if info[0] == 'GROUP':
                self.group.append(info)
                self.num_blocks = info[2]
                self.num_inodes = info[3]
                self.first_valid_block = int(info[8]+self.inode_size * self.num_inodes / self.block_size)
            if info[0] == 'BFREE':
                self.bfree.append(info)
                self.block_list[info[1]].append(['BFREE'])
            if info[0] == 'IFREE':
                self.ifree.append(info[1])
            if info[0] == 'INODE':
                self.inode.append(info)
                for i in range(12, 27):
                    block = info[i]
                    offset = i-12
                    type = ''
                    if i<24:
                        type = ''
                    elif i==24:
                        type = 'INDIRECT'
                    elif i==25:
                        type = 'DOUBLE INDIRECT'
                        offset = 12+256
                    elif i==26:
                        type = 'TRIPLE INDIRECT'
                        offset = 12+256+256*256
                    if block != 0:
                        self.block_list[block].append([type, info[1], offset])

            if info[0] == 'DIRENT':
                self.dirent.append(info)
            if info[0] == 'INDIRECT':
                self.indirect.append(info)
                inode = info[1]
                block = info[5]
                offset = info[3]
                type = ''
                if info[2] == 1:
                    type = 'INDIRECT'
                if info[2] == 2:
                    type = 'DOUBLE INDIRECT'
                if info[2] == 3:
                    type = 'TRIPLE INDIRECT'
                self.block_list[block].append([type, inode, offset])


class Checker(Parser):
    def __init__(self):
        super(Checker,self).__init__()  #initialize Parser's init
        self.total_link = 0
        self.reserved = [0,1,2,3,4,5,6,7,64]
        self.valid = True

    def block_consistency(self):
        # UNREFERENCED test
        for block in range(64):
            if block not in self.reserved and block not in self.block_list.keys():
                sys.stdout.write('UNREFERENCED BLOCK %d\n' % block)
                self.valid = False

        for block, value in self.block_list.items():
            refer_times = 0
            free = False
            allocated = False
            for item in value:
                type = item[0]
                if type != 'BFREE':
                    inode = item[1]
                    offset = item[2]
                    allocated = True
                    refer_times += 1
                else:
                    free = True
                if type != '':
                    type += ' '
                # VALID test
                if block<0 or block>self.num_blocks:
                    sys.stdout.write('INVALID %sBLOCK %d IN INODE %d AT OFFSET %d\n' % (type, block, inode, offset))
                    self.valid = False
                # RESERVED test
                elif block in self.reserved:
                    sys.stdout.write('RESERVED %sBLOCK %d IN INODE %d AT OFFSET %d\n' % (type, block, inode, offset))
                    self.valid = False
            # ALLOCATED test
            if allocated and free:
                sys.stdout.write('ALLOCATED BLOCK %d ON FREELIST\n' % (block))
                self.valid = False
            # DUPLICATE test
            if refer_times>1:
                for item in value:
                    if item[0] != 'BFREE':
                        type = item[0]
                        if type != '':
                            type+=' '
                        inode = item[1]
                        offset = item[2]
                        sys.stdout.write('DUPLICATE %sBLOCK %d IN INODE %d AT OFFSET %d\n' % (type, block, inode, offset))
                        self.valid = False


    def inode_allocation(self):
        
        self.allocated_inode_map=[0 for i in range(self.total_inodes)]
        self.free_inode_map=[0 for i in range(self.total_inodes)]
        self.link_count=[None]+[0 for i in range(self.total_inodes)]
        for i in range(self.first_inode):
            self.allocated_inode_map[i]=1
            self.free_inode_map[i]=0
        for current_inode in self.inode:
            self.allocated_inode_map[current_inode[1]-1]=1
        for current_ifree in self.ifree:
            self.free_inode_map[current_ifree-1]=1
        self.free_inode_map=[None]+self.free_inode_map  #so pos corresponds to inode num
        self.allocated_inode_map=[None]+self.allocated_inode_map
        for i in range(len(self.allocated_inode_map)):
            if self.allocated_inode_map[i]==1 and self.free_inode_map[i]==1:
                print("ALLOCATED INODE "+ str(i) + " ON FREELIST")
                self.valid = False
            elif self.allocated_inode_map[i]==0 and self.free_inode_map[i]==0:
                print("UNALLOCATED INODE "+ str(i) + " NOT ON FREELIST")
                self.valid = False

    def dir_consistency(self):
        dirents_with_name_two_dots=[]
        for current_dirent in self.dirent:
            if current_dirent[3]>1 and current_dirent[3]<self.total_inodes:
                self.link_count[current_dirent[3]]+=1
        for current_inode in self.inode:
            if self.link_count[current_inode[1]]!=current_inode[6]:
                print("INODE " + str(current_inode[1]) + " HAS " + str(self.link_count[current_inode[1]]) + " LINKS BUT LINKCOUNT IS " + str(current_inode[6]))
                self.valid = False

        for current_dirent in self.dirent:
            if current_dirent[3] < 2 or current_dirent[3]>self.total_inodes:
                print("DIRECTORY INODE "+ str(current_dirent[1]) +" NAME "+str(current_dirent[6])+" INVALID INODE "+str(current_dirent[3]))
                self.valid = False
            elif self.allocated_inode_map[current_dirent[3]]==0:
                print("DIRECTORY INODE "+str(current_dirent[1])+" NAME "+str(current_dirent[6])+" UNALLOCATED INODE "+str(current_dirent[3]))
                self.valid = False

            if current_dirent[6]=="'.'":
                if current_dirent[1]!=current_dirent[3]:
                    print("DIRECTORY INODE "+str(current_dirent[1])+" NAME "+str(current_dirent[6])+" LINK TO INODE "+str(current_dirent[3])+" SHOULD BE "+str(current_dirent[1]))
                    self.valid = False

            elif current_dirent[6]=="'..'":
                if current_dirent[1]==2:
                    if current_dirent[3]!=2:
                        print("DIRECTORY INODE 2 NAME '..' LINK TO INODE "+str(current_dirent[3])+" SHOULD BE 2")
                        self.valid = False
                else:
                    dirents_with_name_two_dots.append([current_dirent[1],current_dirent[3]])
        
        dirent_child_to_parent={}
    

        for current_dirent in self.dirent:
            if current_dirent[6]=="'..'" or current_dirent[6]=="'.'":
                continue
            else:
                dirent_child_to_parent[current_dirent[3]]=current_dirent[1]
        for child, parent in dirents_with_name_two_dots:
            #TODO: verify this
            if child not in dirent_child_to_parent.keys():
                continue
            if parent==dirent_child_to_parent[child]:
                continue
            else:
                print("DIRECTORY INODE "+str(child)+" NAME '..' LINK TO INODE "+str(parent)+" SHOULD BE "+str(dirent_child_to_parent[child]))
                self.valid = False



            


def main():
    try:
        f = open(sys.argv[1], 'r')
    except:
        sys.stderr.write('Error: failed to open the file %s\n' % sys.argv[1])
        exit(1)
    data = Checker()
    data.getdata(f)
    data.block_consistency()
    data.inode_allocation()
    data.dir_consistency()

    if data.valid:
        exit(0)
    else:
        exit(2)


    #test
    # print(data.inode)
    # print(data.first_valid_block)
    # print(data.block_list.keys())
    # for i in data.block_list.items():
    #     print(i)



if __name__ == '__main__':
    main()
