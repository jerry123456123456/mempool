#include<stdio.h>
#include<stdlib.h>

#define MEM_SIZE 4096

//跟页做到大小一致
typedef struct mempool_s{
    int block_size;      //每一个块的大小
    int free_count;      //还剩下多少块

    char *free_ptr;      //指向下一个可分配的地址
    char *mem;           //指向内存池头
}mempool_t;

int mp_init(mempool_t *m,int size){
    if(!m)return -1;
    if(size<16)size=16;

    m->block_size=size;

    m->mem=(char *)malloc(MEM_SIZE);   //在空间中分配一块空间，返回的指针强转成char*
    if(!m->mem)return -1;
    m->free_ptr=m->mem;
    m->free_count=MEM_SIZE/size;

    char *ptr=m->free_ptr;
    for(int i=0;i<m->free_count;i++){
        *(char **)ptr=ptr+size;    //把ptr解释成指向char*的指针，并解引用（指向的值）指向下一块的起始
        ptr+=size;
    }
    *(char **)ptr=NULL;   //最后一块置空
    return 0;
}

void mp_dest(mempool_t *m){
    if(!m || !m->mem)return;
    free(m->mem);
}

void *mp_alloc(mempool_t *m){
    if(!m || m->free_count==0)return NULL;

    void *ptr=m->free_ptr;     //ptr是要返回的分配空间的指针

    m->free_ptr=*(char **)ptr;   //每个内存块最开始都存着下一个内存块的地址，所以将m->free_ptr指向ptr内存块中前指向char*指针大小的地址中的内容解引用出来，也就是下一个地址
    m->free_count--;

    return ptr;
}

void mp_free(mempool_t *m,void *ptr){
    *(char **)ptr=m->free_ptr;
    m->free_ptr=(char *)ptr;
    m->free_count++;
}

int main() {

	mempool_t m;

	mp_init(&m, 32);

	void *p1 = mp_alloc(&m);
	printf("1: mp_alloc: %p\n", p1);

	void *p2 = mp_alloc(&m);
	printf("2: mp_alloc: %p\n", p2);

	void *p3 = mp_alloc(&m);
	printf("3: mp_alloc: %p\n", p3);

	void *p4 = mp_alloc(&m);
	printf("4: mp_alloc: %p\n", p4);

	mp_free(&m, p2);

	void *p5 = mp_alloc(&m);
	printf("5: mp_alloc: %p\n", p5);
	

	return 0;
}
