/*********************************************
 **  memory_zone_libraries release 1.02	    **
 **					    **
 **  Copyright (c) John Wellesz 2001 - 2003 **
 **  All rights Reserved.		    **
 *********************************************/

#include <stddef.h>

/*
 * You must include this file in your source code and add mem_zones.c to your makefile or project file.
 * #include "mem_zones.h"
 */


/*
 * This structure is used by all the functions
 * to have information about the memory zone.
 * you have to define one and init it with the
 * function search_mem_zone.
 */


struct memory_zone {
    int          b_ztype;       // indicates the zone id
    unsigned int b_offset;	// --|
    unsigned int b_segment;	// --| point on the size of the zone (3 bytes before the name).
    unsigned int b_inner_offset;//point on the offset where it cans start to write/read data.
    unsigned int b_size;	//total zone size.
    unsigned int b_real_size;	//the real size available for data (used to prevent overflow).
    unsigned char b_name[10];   //name of the zone.
    unsigned char b_password[10];//password (for basic files only).
};

// MACRO FOR COMPATIBILITY WITH OLDER VERSONS OF THIS LIB
#define search_basic_file(a,b)	search_mem_zone(1,a,b)
#define BASICfile		memory_zone
#define read_toBfile		read_mem_zone
#define write_toBfile		write_mem_zone
#define clear_basfile		clear_mem_zone

// Flags for the function create_mem_zone
#define CREATE_ZONE		0// Create a memory zone, if zone already exists it returns -1
#define RESIZE_ZONE		2// create or change the size of an existing zone.
#define DELETE_ZONE		3// can delete a memory zone.

// ######## internal use only #########
struct zone_info {
    unsigned int  offset;
    unsigned char seg;
};

struct zone_info_num {
    unsigned int num;
};
// ####################################
#ifdef __cplusplus
extern "C" {
#endif

/* -----------      create_mem_zone      -----------
 * Creates a memory zone specified which type is given by "id" with the name "zonename", and a size of "size" bytes.
 * (size >= 14 and >=24 for a basic file)
 * it returns -1 in case of failure (already exist...) and -2 if not enough memory to create the file
 * or not enough memory for the function itself.
 * flag can be one of these values: CREATE_ZONE RESIZE_ZONE DELETE_ZONE
 */
int create_mem_zone(unsigned char zone_id, unsigned char *zonename, unsigned int size, int flag);

/* -----------      search_mem_zone      -----------
 * This function must be used one time before functions read_mem_zone, write_mem_zone or change_password
 * to init the memory_zone structure
 * then you will have to send the ====> ADDRESS <==== of this structure to the other functions.
 * (you will have problems if you don't correctly understand the meaning od "address")
 *
 *  zonename is the name of the memory zone
 *  zone_id is the id type of the memory zone you want to find
 *  bf is the name of a memory_zone structure
 *
 *  this function returns the size of the memory zone if found and -1 if not.
 */
long search_mem_zone(int zone_id, unsigned char *zone_name, struct memory_zone *bf);

/* -----------      write_mem_zone      -----------
 *
 * This function works like the write function for real files,
 * src can be everything, an int, a string, a structure...
 * offset is the location in the memory zone where to write.
 * length is the length in byte of src to write, if length > b_size then only b_size -24 are written.
 * So your calculator memory is protected it CAN only write within the memory zone pointed by bf.
 */

unsigned int write_mem_zone (struct memory_zone *bf, const void *src, unsigned offset, unsigned length);


/* -----------      read_mem_zone      -----------
 * works like the write one, dest can be everything an int, a string...
 */
unsigned read_mem_zone (struct memory_zone *bf, void *dest, unsigned offset, unsigned length);

/* -----------      clear_mem_zone      -----------
 * This fill a memory zone with NULLs.
 */
void		clear_mem_zone
(const struct memory_zone *bf);

/* -----------      change_password      -----------
 * Well this change the password of a basic prog, password is the new one.
 * */
void		change_password
(struct memory_zone *bf, unsigned char *password);

/* -----------      afxleft               -----------
 * This return the available memory for user's data in the afx memory.
 * */
unsigned long afxleft (void);

/*############### The functions below are used by other function of this lib ###############*/

/* -----------      tell_mem_zone_seg      -----------
 * This function returns a pointer to the first zone which type is id (0<=id<0xF).
 */
char far *tell_mem_zone_seg( unsigned char id );

/* --------      huge_movedata      --------
 * Can move till 0xFFFFFFFF bytes of memory!
 */

void
huge_movedata(unsigned int src_seg, unsigned int src_off, unsigned int des_seg, unsigned int des_off, unsigned long num);

void
my_movedata(unsigned int src_seg, unsigned int src_off, unsigned int des_seg, unsigned int des_off, size_t size_to_copy, int direction);

/* --------      init_area      --------
 *  clear an area of "size" (<65536) bytes of memory since("direction"==0) or till(direction==1) b_segment:b_offset with the
 *  two bytes contained in "value"
 */

void init_area(unsigned int b_segment, unsigned int b_offset, unsigned int size, unsigned int value, int direction);

unsigned long give_absolute_add(void far *pointer);

void far *give_far_pointer(unsigned long abs_add);

#ifdef __cplusplus
}
#endif

#define GIVE_SEG_ABSADD(a) ((u_int)((a)>> 4))
#define GIVE_OFF_ABSADD(a) ((u_int)((a) & 0xF))

