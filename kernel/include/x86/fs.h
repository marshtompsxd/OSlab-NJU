#define FS_SIZE_MB			2
#define NR_INODE			32	
#define SUPERBLOCK_SIZE_B	sizeof(superblock_t)

#define MAX_FILE_SZ_KB		32
#define MAX_FILENAME_LEN	28
#define MAX_ID_LEN			8

#define NR_DIRENTRY			16

#define FS_NAME				"FS_SXD"
#define ID    				"SunXD"


enum file_type { UNUSED = 0, FL = 1, DIR = 2 };

enum block_type { available = 0, inuse = 1 };

struct superblock_t
{
	char id[MAX_ID_LEN];
	uint32_t fs_sz_mb;
	uint32_t nr_node;
	uint32_t superblock_sz_b;
	uint32_t inodeblcok_sz_b;
	uint32_t inuse_inode_num;
	uint8_t inode_map[NR_INODE];
};
typedef struct superblock_t superblock_t;


struct direntry_t
{
	uint32_t inode_no;
	char filename[MAX_FILENAME_LEN];
};
typedef struct direntry_t direntry_t;

struct inodeblock_t
{
	uint32_t type;
	union
	{
		uint32_t filesz;
		uint32_t filenum;
	};
	char filename[MAX_FILENAME_LEN];
	union
	{
		uint8_t datablock[MAX_FILE_SZ_KB*1024];
		direntry_t dirdata[MAX_FILE_SZ_KB*1024/32];
	};
};
typedef struct inodeblock_t inodeblock_t;

int path_offset;
char top_dir[MAX_FILENAME_LEN];

superblock_t fs_superblock;

uint8_t *fs_addr;
superblock_t *fs_superpointer;
inodeblock_t *fs_inodepointer;

int findUpperDirPos(char* abspath);
int findCurrentDirPos(char* abspath);
int findCurrentFLPos(char* abspath);
int isDIRExist(char* abspath);
int isFLExist(char* abspath);
void makeDir(char* abspath);
void makeFL(char* abspath, uint8_t* data);
void rmFL(char* abspath);
int writeF(int index, char* buf, int len, int offset);
int readF(int index, char* buf, int len, int offset);
void writeBack();
