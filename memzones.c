/*********************************************
**  memory_zone_libraries release 1.02	    **
**					    **
**  Copyright (c) John Wellesz 2001 - 2003  **
**  All rights Reserved.		    **
*********************************************/
/*
 * Read/write/create Routines for memory zone. version 1.02
 * You have the right to use these functions ONLY if you let it in these originals file (memzones.c and memzones.h) without
 * modifying them.
 */
#pragma option -r  //register variable can be used
#pragma option -Z  //register re-use variable optimization
#pragma option -v- //De-activate debugging and expand inline functions
#pragma option -y- //No line number in executable !
#pragma option -N- //no stack overflow is made.
#pragma option -O  //delete unrechable code in exe.

#include "memzones.h"
#include "DEF.h"
#include <dos.h>
#include <alloc.h>

/*
   See the file memzones.h to know how to use these functions.
   */

static unsigned int _BALIB_segm_video=0;


// char far *tell_mem_zone_seg {{{
char far *tell_mem_zone_seg( unsigned char id )
{
    char far *MAT_ptr;
    unsigned int bas_add;

    if (id > 19) return NULL;

    if (!_BALIB_segm_video) _BALIB_segm_video=peek(0x4E,0);

    MAT_ptr = (char far *)MK_FP(_BALIB_segm_video + 0x01D2, 0);//pointer on the MAT (Memory Allocation Table)

    bas_add = *((unsigned int far *)&MAT_ptr[5 * id + (id>16)]);

    return (char far *)MK_FP( (((unsigned int)MAT_ptr[5 * id + 2 + (id>16)]) & 0xFF) * 0x100 + (bas_add >> 4), (bas_add & 0xF));
}
// }}}

// int create_mem_zone {{{
int create_mem_zone(unsigned char zone_id, unsigned char *zonename, unsigned int size, int flag)
{
    char                 far *MAT_ptr;
    unsigned int              i, found=0;
    int			      ret = 0, id=0;
    unsigned int              original_size = 0;
    struct zone_info     far *all_zones      [20];
    struct zone_info_num far *all_zones_nums [19];
    char                 far *zone_mem_ptr, far *curent_z_ptr, far *write_from_here = NULL;
    unsigned long             transit, last_free_address, max_addres;
    char                 far **zone_add_lookup_table;

    if ( ((flag != DELETE_ZONE) && (size < 14)) || zonename[0]==0 || zone_id > 15)
	return -1;
    else if (!zone_id) return 0;


    if (flag == DELETE_ZONE) size = 0;

    zone_mem_ptr = tell_mem_zone_seg(zone_id);
    write_from_here = (curent_z_ptr   = zone_mem_ptr);

    MAT_ptr = (char far *)MK_FP(_BALIB_segm_video + 0x01D2, 0);
    //Make the MAT info directly editable
    for(i=0; i<17; i++) {
	all_zones      [ i] = (struct zone_info      far *) &MAT_ptr[5 * i    ];
	all_zones_nums [ i] = (struct zone_info_num  far *) &MAT_ptr[5 * i + 3];
    }

    all_zones	       [17] = (struct zone_info      far *) &MAT_ptr[5 * 17 + 1      ];
  //all_zones_nums     [17] = (struct zone_info_num  far *) &MAT_ptr[5 * 17 + 1  + 3 ];
    all_zones	       [18] = (struct zone_info      far *) &MAT_ptr[5 * 18 + 1	     ];
  //all_zones_nums     [18] = (struct zone_info_num  far *) &MAT_ptr[5 * 18 + 1  + 3 ];
    all_zones	       [19] = (struct zone_info      far *) &MAT_ptr[5 * 19 + 1	     ];

    last_free_address = (u_long)(((u_long)all_zones[17]->seg) * 0x1000L) + (u_long)all_zones[17]->offset + 1L;
    max_addres        = (u_long)(((u_long)all_zones[19]->seg) * 0x1000L) + (u_long)all_zones[19]->offset;

    zone_add_lookup_table = (char far **)malloc((all_zones_nums[zone_id]->num + 1) * sizeof(char far *));

    //test if there is enough memory
    if (!zone_add_lookup_table) {
	ret = -2;
	goto quit_function;
    }

    i = 0;
    while (curent_z_ptr[2] == zone_id) {
	zone_add_lookup_table[i++] = curent_z_ptr;
	transit = *((u_int far *)(curent_z_ptr));
	curent_z_ptr = (char far *)((u_long)curent_z_ptr + (transit & 0xF) + ((transit >> 4) << 16));
	//NOTE: the offset of curent_z_ptr will never be reseted if ever it reaches 0xFFFF we will have a problem but
	//it shouldn't happen anyway...
    }
    zone_add_lookup_table[i] = curent_z_ptr;

    // Searches where to add the zone
    // ********** DICOTOMY ********** {{{
    if (i) {
	int lo=0, hi=all_zones_nums[zone_id]->num - 1;
	unsigned int b;
	int          sign;

	for (/*found = 0*/; !found && (lo <= hi);) {
	    id = (lo + hi)>>1;

	    for (b=0; b<8;) {
		sign = (int)(((zonename[b]) - ((u_char)zone_add_lookup_table[id][3 + b])));
		if (sign || !zonename[b])
		    break;
		b++;
	    }

	    if      (!sign)
		found = 1;
	    else if (sign < 0)
		hi = id - 1;
	    else
		lo = id + 1;
	}

	if (found) {
	    if (flag == CREATE_ZONE) {
		ret = -1;
		goto quit_function;
	    }
	    original_size = *((u_int far *)zone_add_lookup_table[id]);
	    if (size == original_size) {
		ret = 0;
		goto quit_function;
	    }
	    id++;
	} else {
	    id += (sign > 0);
	    if (flag == DELETE_ZONE) {
		ret = -1;
		goto quit_function;
	    }
	}
	write_from_here = zone_add_lookup_table[id];
    }
    // }}}

    //********** Is there enough memory for the operation ??
    if (flag != DELETE_ZONE)
	if ((last_free_address + (long)((long)size - (long)original_size)) > max_addres) {
	    ret = -2;
	    goto quit_function;
	}

    //********** MOVE EVERYTHING **********
    {
	unsigned long transit2;
	unsigned long move_n;

	transit = give_absolute_add(write_from_here);
	move_n = last_free_address - transit;
	transit2 = (long)transit + (long)((long)size - (long)original_size);

	huge_movedata(GIVE_SEG_ABSADD(transit ), GIVE_OFF_ABSADD(transit ),
		GIVE_SEG_ABSADD(transit2), GIVE_OFF_ABSADD(transit2), move_n);
    }

    // ********** CREATE THE ZONE *********
    {
	unsigned int far *zone_size;
	unsigned int b;

	//if already exists return on the existing file
	write_from_here = zone_add_lookup_table[id - found];
	//clearing with NULLs the zone
	if (original_size < size) {
	    transit = give_absolute_add(write_from_here) + original_size - (3L * found);
	    init_area(GIVE_SEG_ABSADD(transit), GIVE_OFF_ABSADD(transit), size - original_size, 0, 0);
	} else if (original_size > size) {
	    init_area(GIVE_SEG_ABSADD(last_free_address), GIVE_OFF_ABSADD(last_free_address), original_size - size, 0, 1);
	}

	if (flag != DELETE_ZONE) {
	    zone_size = (unsigned int far *)write_from_here;

	    //writes the zone size at its start
	    *zone_size = size;

	    //writes the zone id
	    write_from_here[2] = zone_id;

	    //if not existing: write the zone name
	    if (!found)
		for (b=0; b<8;b++) {
		    write_from_here[3+b]=zonename[b];
		    if (!zonename[b]) break;
		}

	    transit = give_absolute_add(zone_size) + (u_long)(size - 3);
	    zone_size = (unsigned int far *)give_far_pointer(transit);
	    //writes the NULL of end
	    *zone_size = 0;
	    ((u_long)zone_size)++;
	    //writes the zone size at its end
	    *zone_size = size;
	}
    }

    // *********** UPDATE THE MAT *********
    {
	long transit;
	unsigned int b;
	long new_size=size;
	//increase the zone number
	if (!found)
	    all_zones_nums[zone_id]->num++;
	else new_size = (long)((long)size - (long)original_size);

	if (flag == DELETE_ZONE) all_zones_nums[zone_id]->num--;

	for (b=zone_id + 1; b<18; b++) {
	    transit = (long)((long)all_zones[b]->offset + new_size);
	    if (transit > 0xFFFFL) {
		transit -= 0x10000L;
		all_zones[b]->seg += 0x10;
	    } else if (transit < 0) {
		transit += 0x10000L;
		all_zones[b]->seg -= 0x10;
	    }
	    all_zones[b]->offset = (u_int)(transit);
	}
	//updates the FNAME shortcut
	all_zones[18]->offset = all_zones[3]->offset;
	all_zones[18]->seg    = all_zones[3]->seg;
    }

quit_function:
    free(zone_add_lookup_table);
    return ret;
}
// }}}

// void my_movedata  {{{
void my_movedata (u_int src_seg, u_int src_off, u_int des_seg, u_int des_off, size_t size_to_copy, int direction)
{
    if (size_to_copy < 2) {
	if (size_to_copy) *((unsigned char far *)MK_FP(des_seg,des_off))=*((unsigned char far *)MK_FP(src_seg,src_off));
	return;
    }

    if (direction) {
	asm{
	    push si; push es; push di; push cx; push bx;//saves registers
	    pushf;//save flags

	    mov bx, ds;//saves ds
	    mov cx, size_to_copy;
	    //set segment and offset source and destination.
	    mov di, des_off;  mov es, des_seg;
	    mov si, src_off;  mov ds, src_seg;// MUST BE THE LAST SET FROM VARIABLE !!!!

	    //set offsets to end
	    dec cx; dec cx;//makes si and di point 1 word before the string end.
	    add si, cx; add di, cx;

	    //set number of word to copy
	    inc cx; inc cx;
	    shr cx, 1;  //cx/=2

	    std;//backward copy
	    rep movsw;

	    mov ds, bx;//restor ds
	    //copy the first byte (if impair...)
	    mov es, src_seg;  mov di, src_off;
	    mov cl, es:[di];
	    mov es, des_seg;  mov di, des_off;
	    mov es:[di], cl;

	    popf;//restore flags
	    pop bx; pop cx; pop di; pop es; pop si;
	}
	return;
    }
    asm{
	push si; push es; push di; push cx; push bx;//saves registers
	pushf;//save flags

	mov bx, ds;//saves ds

	mov cx, size_to_copy;
	//set number of word to copy
	shr cx, 1;  //cx/=2

	//set segment and offset source and destination.
	mov di, des_off;  mov es, des_seg;
	mov si, src_off;  mov ds, src_seg;// MUST BE THE LAST SET FROM VARIABLE !!!!

	cld;//backward copy
	rep movsw;

	mov ds, bx;//restor ds
	//copy the last byte (if impair...)
	//set offsets to end
	mov cx, size_to_copy;  dec cx;
	mov es, src_seg;   mov di, src_off;
	add di, cx;
	mov bl, es:[di];
	mov es, des_seg;   mov di, des_off;
	add di, cx;
	mov es:[di], bl;

	popf;//restore flags
	pop bx; pop cx; pop di; pop es; pop si;
    }
    return;
}
// }}}

// void huge_movedata {{{
void huge_movedata(unsigned int src_seg, unsigned int src_off, unsigned int des_seg, unsigned int des_off, unsigned long num)
{
    unsigned int size_to_copy = 0xFFF0;
    unsigned long src, des;
    long direction;
    int dir_flag;

    src = (u_long)( ((u_long)src_seg) * 0x10L + (u_long)src_off);
    des = (u_long)( ((u_long)des_seg) * 0x10L + (u_long)des_off);
    direction = (long)(src - des);
    dir_flag = (direction<0);
    if (dir_flag) {
	src += num;
	des += num;
    }

    for (;;) {
	if (num < (u_long)size_to_copy)
	    size_to_copy = (u_int)num;

	if (dir_flag) {
	    src -= (u_long)size_to_copy;
	    des -= (u_long)size_to_copy;
	}

	src_seg = (u_int)(src >> 4 );
	src_off = (u_int)(src & 0xF);

	des_seg = (u_int)(des >> 4 );
	des_off = (u_int)(des & 0xF);

	my_movedata(src_seg, src_off, des_seg, des_off, size_to_copy, dir_flag);

	num -= size_to_copy;
	if (!num)
	    break;

	if (!dir_flag){
	    src += (u_long)size_to_copy;
	    des += (u_long)size_to_copy;
	}
    }
}
// }}}

// unsigned long search_mem_zone {{{
long search_mem_zone(int zone_id, unsigned char *zone_name, struct memory_zone *bf)
{
    unsigned long abs_address;
    unsigned int found=0;
    unsigned int b=0, i;
    unsigned char far *curent_b_ptr;
    unsigned long transit;

    if (!zone_id) return -1;
    curent_b_ptr = (unsigned char far *)tell_mem_zone_seg(zone_id);

    while (curent_b_ptr[2] == zone_id) {
	while (zone_name[b]==curent_b_ptr[3 + b] && zone_name[b]!='\0' && b<8)
	    b++;
	if (b == 8 || zone_name[b] == curent_b_ptr[3 + b]) {
	    found = 1;
	    break;
	}
	transit = *((u_int far *)(curent_b_ptr));
	curent_b_ptr = (unsigned char far *)((u_long)curent_b_ptr + (transit & 0xF) + ((transit >> 4) << 16));
    }

    if (!found) {
	return -1;
    }

    abs_address = give_absolute_add(curent_b_ptr);// + 3L;

    bf->b_offset  = (unsigned int)(abs_address &0xf);
    bf->b_segment = (unsigned int)(abs_address >> 4);

    bf->b_size	    = *((int far *)(curent_b_ptr));
    bf->b_real_size = bf->b_size - (24 - (zone_id!=1)*10);//real usable size of a basic file.

    bf->b_inner_offset = bf->b_offset + 16  - (zone_id!=1)*10 + 2 + 3;//on the content of the prog  (8 + 8 + 2)

    for (i=0; i<8; i++) {
	bf->b_password[i]   =	curent_b_ptr[3 + 8 + i] * (zone_id==1);
	bf->b_name[i]	    =	curent_b_ptr[3 +     i];
    }
    bf->b_password[8]=0;
    bf->b_name[8]=0;
    bf->b_ztype=zone_id;

    return bf->b_size;
}
// }}}

// int change_password {{{
void change_password(struct memory_zone *bf, unsigned char *password)
{
    int i;
    unsigned char ctow=1;

    if (bf->b_ztype != 1)
	return;

    for (i=0; i<8; i++) {
	if (ctow)
	    ctow = password[i];
	pokeb(bf->b_segment, bf->b_offset + 8 + 3 + i, ctow);
    }
}
// }}}

// unsigned read_mem_zone {{{
unsigned read_mem_zone(struct memory_zone *bf, void *dest, unsigned offset, unsigned length)
{
    if (length > (bf->b_real_size - offset))
	length = bf->b_real_size - offset;

    my_movedata(bf->b_segment,  bf->b_inner_offset + offset, _DS, (unsigned)dest, length, 0);

    return length;
}
// }}}

// unsigned int write_mem_zone {{{
unsigned int write_mem_zone(struct memory_zone *bf, const void *src, unsigned offset, unsigned length)
{
    if (length > (bf->b_real_size - offset))
	length = bf->b_real_size - offset;

    my_movedata(_DS, (unsigned)src, bf->b_segment, bf->b_inner_offset + offset, length, 0);

    return length;
}
// }}}

// void clear_mem_zone {{{
void clear_mem_zone(const struct memory_zone *bf)
{
    init_area(bf->b_segment, bf->b_inner_offset, bf->b_real_size, 0, 0);
}
// }}}

// void init_area {{{
void init_area(u_int b_segment, u_int b_offset, u_int size, u_int value, int direction)
{
    u_long transit;
    if (!size) return;

    transit = (((u_long)b_segment)<<4) + (u_long)b_offset;
    if (direction) {
	transit-=size;
    }

    if (size>0xFFF0) size = 0xFFF0;

    b_segment = (u_int)(transit >>   4);
    b_offset  = (u_int)(transit &  0xF);


    if (size%2) {
	asm{
	    push ax; push es; push di;
	    mov es,b_segment;
	    mov di,b_offset;
	    add di,size;
	    dec di;
	    mov ax, value;
	    mov es:[di],al;
	    pop di; pop es; pop ax;
	}
    }


    asm {
	pushf; push es; push di; push cx; push ax;
	mov es, b_segment;
	mov di, b_offset;
	mov cx, size;
	shr cx,1;
	mov ax, value;
	cld;
	rep stosw;
	pop ax; pop cx; pop di; pop es;	popf;
    }
    return;
}
// }}}

unsigned long give_absolute_add(void far *pointer)
{
    return ((((u_long)pointer) >> 12) & 0xFFFF0L) + (((u_long)pointer) & 0xFFFFL);
}

void far *give_far_pointer(unsigned long abs_add)
{
    return (void far *)MK_FP((u_int)((abs_add>>4) &0xFFFF), (u_int)(abs_add&0xf));
}

unsigned long afxleft (void)
{
    return (give_absolute_add(tell_mem_zone_seg(19)) - give_absolute_add(tell_mem_zone_seg(17)) - 1L);
}
