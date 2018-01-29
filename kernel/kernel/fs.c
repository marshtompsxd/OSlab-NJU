#include "x86.h"
#include "device.h"
//#include <string.h>

int getAvlInode()
{
	int i;
	for(i = 0;i<NR_INODE;i++)
	{
		if(fs_superpointer->inode_map[i] == available)return i;
	}
	return -1;
}

char* getTopDir(char* path)
{
	//printk("gettopdir %s\n", path);
	int i;	
	int len = strlen(path);
	for(i = 0;i<len;i++)
	{
		top_dir[i] = path[i];
		if(path[i]=='/')
		{
			top_dir[i+1] = '\0';
			path_offset = i + 1;
			break;
		}
	}
	//printk("%s\n", top_dir);
	//printk("top dir is %s\n", top_dir);
	return top_dir;
}


int lastLevel(char* path)
{
	int i;
	int cont = 0;
	int len = strlen(path);
	for (i = 0; i < len; ++i)
	{
		if(path[i] == '/' && i!=len -1 )cont++;
	}
	if(cont == 0) return 1;
	else return 0;
}

char* getLastLevelName(char* abspath)
{
	//printk("get last %s\n", abspath);
	int len = strlen(abspath);
	int i;
	int offset = 0;
	for(i = len-1 ;i >= 0; i--)
	{
		if(i!=len - 1)
		{
			if(abspath[i] == '/')
			{
				offset = i + 1;
				break;
			}
		}
	}
	
	return abspath+offset;
}

int findUpperDirPos(char* abspath)
{
	if(strcmp(abspath, "/")==0)return 0;
	char dir[MAX_FILENAME_LEN];
	strcpy(dir, getTopDir(abspath));
	abspath += path_offset;
	//char *upper = "/";
	int upper_index = 0;
	while(!lastLevel(abspath))
	{
		strcpy(dir, getTopDir(abspath));
		abspath += path_offset;
		int i;
		int index = -1;
		int cont = 0;
		for(i=0;cont<fs_inodepointer[upper_index].filenum;i++)
		{
			if(strcmp(fs_inodepointer[upper_index].dirdata[i].filename, "\0")!=0)
			{
				cont++;
				if(strcmp(fs_inodepointer[upper_index].dirdata[i].filename, dir)==0)
				{
					if(fs_inodepointer[fs_inodepointer[upper_index].dirdata[i].inode_no].type == DIR)
					{
						index = fs_inodepointer[upper_index].dirdata[i].inode_no;
						break;
					}
				}
			}
		}
		//if(index == -1)assert(0);
		upper_index = index;
	}
	return upper_index;
}

int findCurrentDirPos(char* abspath)
{
	if(strcmp(abspath, "/")==0)return 0;
	char dir[MAX_FILENAME_LEN];
	strcpy(dir, getTopDir(abspath));
	abspath += path_offset;
	//char *upper = "/";
	int upper_index = 0;
	
	while(!lastLevel(abspath))
	{
		strcpy(dir, getTopDir(abspath));
		abspath += path_offset;
		int i;
		int index = -1;
		int cont = 0;
		for(i=0;cont<fs_inodepointer[upper_index].filenum;i++)
		{
			if(strcmp(fs_inodepointer[upper_index].dirdata[i].filename, "\0")!=0)
			{
				cont++;
				if(strcmp(fs_inodepointer[upper_index].dirdata[i].filename, dir)==0)
				{
					if(fs_inodepointer[fs_inodepointer[upper_index].dirdata[i].inode_no].type == DIR)
					{
						index = fs_inodepointer[upper_index].dirdata[i].inode_no;
						break;
					}
				}
			}
		}
		if(index == -1)assert(0);
		upper_index = index;
	}

	int i;
	int current_index = -1;
	int cont = 0;
	for(i=0;cont<fs_inodepointer[upper_index].filenum;i++)
	{
		if(strcmp(fs_inodepointer[upper_index].dirdata[i].filename, "\0")!=0)
		{
			cont++;
			if(strcmp(fs_inodepointer[upper_index].dirdata[i].filename, abspath)==0)
			{
				if(fs_inodepointer[fs_inodepointer[upper_index].dirdata[i].inode_no].type == DIR)
				{
					current_index = fs_inodepointer[upper_index].dirdata[i].inode_no;
					break;
				}
			}
		}
	}
	//if(current_index == -1)assert(0);
	
	return current_index;
}


int findCurrentFLPos(char* abspath)
{
	char dir[MAX_FILENAME_LEN];
	strcpy(dir, getTopDir(abspath));
	abspath += path_offset;
	//char *upper = "/";
	int upper_index = 0;
	
	while(!lastLevel(abspath))
	{
		strcpy(dir, getTopDir(abspath));
		abspath += path_offset;
		int i;
		int index = -1;
		int cont = 0;
		for(i=0;cont<fs_inodepointer[upper_index].filenum;i++)
		{
			if(strcmp(fs_inodepointer[upper_index].dirdata[i].filename, "\0")!=0)
			{
				cont++;
				if(strcmp(fs_inodepointer[upper_index].dirdata[i].filename, dir)==0)
				{
					if(fs_inodepointer[fs_inodepointer[upper_index].dirdata[i].inode_no].type == DIR)
					{
						index = fs_inodepointer[upper_index].dirdata[i].inode_no;
						break;
					}
				}
			}
		}
		if(index == -1)assert(0);
		upper_index = index;
	}

	int i;
	int current_index = -1;
	int cont = 0;
	for(i=0;cont<fs_inodepointer[upper_index].filenum;i++)
	{
		if(strcmp(fs_inodepointer[upper_index].dirdata[i].filename, "\0")!=0)
		{
			cont++;
			if(strcmp(fs_inodepointer[upper_index].dirdata[i].filename, abspath)==0)
			{
				if(fs_inodepointer[fs_inodepointer[upper_index].dirdata[i].inode_no].type == FL)
				{
					current_index = fs_inodepointer[upper_index].dirdata[i].inode_no;
					break;
				}
			}
		}
	}
	//if(current_index == -1)assert(0);
	
	return current_index;
}


int isDIRExist(char* abspath)
{
	if(findCurrentDirPos(abspath) == -1)return 0;
	else return 1;
}

int isFLExist(char* abspath)
{
	if(findCurrentFLPos(abspath) == -1)return 0;
	else return 1;
}

int getAvlDirdata(int p_index)
{
	int i;
	for(i = 0;i<MAX_FILE_SZ_KB*1024/32;i++)
	{
		if(strcmp(fs_inodepointer[p_index].dirdata[i].filename, "\0") == 0)return i;
	}
	return -1;
}

void makeDir(char* abspath)
{
	

	int index, p_index;
	if( (index = getAvlInode()) != -1 && (p_index = findUpperDirPos(abspath)) != -1)
	{

		fs_superpointer->inuse_inode_num++;

		char lastLevelName[MAX_FILENAME_LEN];
		strcpy(lastLevelName, getLastLevelName(abspath)) ;
		int len = strlen(lastLevelName);
		if(lastLevelName[len - 1]!='/')
		{
			lastLevelName[len] = '/';
			lastLevelName[len + 1] = '\0';
		}

		int dirdata_index = getAvlDirdata(p_index);
		if(dirdata_index == -1)assert(0);
		fs_inodepointer[p_index].dirdata[dirdata_index].inode_no = index;
		strcpy(fs_inodepointer[p_index].dirdata[dirdata_index].filename, lastLevelName); 
		fs_inodepointer[p_index].filenum++;

		fs_inodepointer[index].type = DIR;
		strcpy(fs_inodepointer[index].filename, lastLevelName);
		fs_inodepointer[index].dirdata[0].inode_no = index;
		strcpy(fs_inodepointer[index].dirdata[0].filename, ".");
		fs_inodepointer[index].dirdata[1].inode_no = p_index;
		strcpy(fs_inodepointer[index].dirdata[1].filename, "..");
		fs_inodepointer[index].filenum = 2;

		fs_superpointer->inuse_inode_num++;
		fs_superpointer->inode_map[index] = inuse;

		//printk("%s is in inode %d\n", abspath, index);
	} 
	else printk("abspath error\n");
}

void makeFL(char* abspath, uint8_t* data)
{
	int index, p_index;
	if( (index = getAvlInode()) != -1 && (p_index = findUpperDirPos(abspath)) != -1)
	{
		fs_superpointer->inuse_inode_num++;

		char lastLevelName[MAX_FILENAME_LEN];
		strcpy(lastLevelName, getLastLevelName(abspath));


		int dirdata_index = getAvlDirdata(p_index);
		if(dirdata_index == -1)assert(0);
		fs_inodepointer[p_index].dirdata[dirdata_index].inode_no = index;
		strcpy(fs_inodepointer[p_index].dirdata[dirdata_index].filename, lastLevelName); 
		fs_inodepointer[p_index].filenum++;

		fs_inodepointer[index].type = FL;
		strcpy(fs_inodepointer[index].filename, lastLevelName);
		if(data != NULL)
		{
			fs_inodepointer[index].filesz = strlen((char*)data);
			strcpy((char*)fs_inodepointer[index].datablock, (char*)data);
		}
		
		

		fs_superpointer->inuse_inode_num++;
		fs_superpointer->inode_map[index] = inuse;
	} 
	else assert(0);
}

void rmFL(char* abspath)
{
	int index, p_index;
	index = findCurrentFLPos(abspath);
	p_index = findUpperDirPos(abspath);
	char* filename = fs_inodepointer[index].filename;

	int i;
	int cont = 0;
	for(i = 0;cont<fs_inodepointer[p_index].filenum;i++)
	{
		if(strcmp(fs_inodepointer[p_index].dirdata[i].filename, "\0")!=0)
		{
			cont++;
			if(strcmp(filename, fs_inodepointer[p_index].dirdata[i].filename) == 0)
			{
				fs_inodepointer[p_index].dirdata[i].inode_no = 0;
				strcpy(fs_inodepointer[p_index].dirdata[i].filename, "\0");
				break;
			}
		}
	}

	fs_superpointer->inode_map[index] = 0;
}

void makeRootDir()
{
	fs_inodepointer[0].type = DIR;
	strcpy(fs_inodepointer[0].filename, "/");

	fs_inodepointer[0].filenum = 2;
	fs_inodepointer[0].dirdata[0].inode_no = 0;
	strcpy(fs_inodepointer[0].dirdata[0].filename, ".");
	fs_inodepointer[0].dirdata[1].inode_no = 0;
	strcpy(fs_inodepointer[0].dirdata[1].filename, "..");

	fs_superpointer->inuse_inode_num++;
	fs_superpointer->inode_map[0] = inuse;
}

int writeF(int index, char* buf, int len, int offset)
{	
	int i;
	assert(len + offset < MAX_FILE_SZ_KB*1024);
	for(i = 0;i<len;i++)
	{
		fs_inodepointer[index].datablock[i + offset] = buf[i];
	}
	fs_inodepointer[index].filesz =  (fs_inodepointer[index].filesz >  offset + len) ? fs_inodepointer[index].filesz : offset + len;
	return i;


}

void writeBack()
{
	writeBytes(fs_addr, FS_OFFSET_IN_DISK, FS_SIZE_MB*1024*1024);
}

int readF(int index, char* buf, int len, int offset)
{
	int i;
	for(i = 0;i<len && i+offset < fs_inodepointer[index].filesz;i++)
	{
		buf[i] = fs_inodepointer[index].datablock[i + offset];
	}
	return i;
}

void ls(char* abspath)
{
	int index = findCurrentDirPos(abspath);
	//printk("%d", index);
	int i;
	for(i = 0;i<fs_inodepointer[index].filenum;i++)
	{
		printk("%s\n", fs_inodepointer[index].dirdata[i].filename);
	}
}

void testfs()
{
	int i;
	for(i = 0;i<4;i++)
	{
		printk("%s\n", fs_inodepointer[i].filename );
	}
	//ls("/");
	//ls("/boot/");
	//ls("/dev/");
	//ls("/usr/");
}

void initFS()
{
	fs_superblock.fs_sz_mb = FS_SIZE_MB;
	fs_superblock.nr_node = NR_INODE;
	fs_superblock.superblock_sz_b = SUPERBLOCK_SIZE_B;
	fs_superblock.inodeblcok_sz_b = sizeof(inodeblock_t);
	fs_superblock.inuse_inode_num = 0;


	fs_superpointer = (superblock_t*)fs_addr;
	fs_inodepointer = (inodeblock_t*)(fs_addr + SUPERBLOCK_SIZE_B);


	printk("FS ID is %s\n", fs_superpointer->id);

	//testfs();
}