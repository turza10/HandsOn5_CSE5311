#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Basically stores any type of data, just need to tell it how to compare elements
typedef struct {
    void* data;         
    size_t elem_size;    
    int capacity;        
    int size;           
    int (*compare)(const void*, const void*);
} MinHeap;

// Bit shifts are faster than division/multiplication
#define PARENT(i) ((i - 1) >> 1)
#define LEFT(i) ((i << 1) + 1)
#define RIGHT(i) ((i << 1) + 2)

// Need this because we can't just do a = b with void pointers
void swap(void* a, void* b, size_t size) {
    void* temp = malloc(size);
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
    free(temp);
}

MinHeap* create_min_heap(size_t elem_size, int capacity, int (*compare)(const void*, const void*)) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->data = malloc(elem_size * capacity);
    heap->elem_size = elem_size;
    heap->capacity = capacity;
    heap->size = 0;
    heap->compare = compare;
    return heap;
}

// Recursively fixes heap property starting from idx
void min_heapify(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = LEFT(idx);
    int right = RIGHT(idx);
    void* data = heap->data;

    if (left < heap->size && 
        heap->compare(data + left * heap->elem_size, 
                     data + smallest * heap->elem_size) < 0)
        smallest = left;

    if (right < heap->size && 
        heap->compare(data + right * heap->elem_size, 
                     data + smallest * heap->elem_size) < 0)
        smallest = right;

    if (smallest != idx) {
        swap(data + idx * heap->elem_size, 
             data + smallest * heap->elem_size, 
             heap->elem_size);
        min_heapify(heap, smallest);
    }
}

// O(n) way to build heap, better than inserting n times
void build_min_heap(MinHeap* heap) {
    for (int i = (heap->size / 2) - 1; i >= 0; i--)
        min_heapify(heap, i);
}

void insert(MinHeap* heap, void* element) {
    if (heap->size >= heap->capacity) {
        printf("Heap's full!\n");
        return;
    }

    heap->size++;
    int i = heap->size - 1;
    memcpy(heap->data + i * heap->elem_size, element, heap->elem_size);

    // Bubble up until we find the right spot
    while (i > 0 && heap->compare(heap->data + PARENT(i) * heap->elem_size,
                                 heap->data + i * heap->elem_size) > 0) {
        swap(heap->data + i * heap->elem_size,
             heap->data + PARENT(i) * heap->elem_size,
             heap->elem_size);
        i = PARENT(i);
    }
}

// Returns and removes min element. Caller needs to free the returned pointer
void* pop(MinHeap* heap) {
    if (heap->size <= 0) return NULL;

    void* root = malloc(heap->elem_size);
    memcpy(root, heap->data, heap->elem_size);

    // Move last element to root and shrink heap
    memcpy(heap->data, heap->data + (heap->size - 1) * heap->elem_size, heap->elem_size);
    heap->size--;

    // Fix the mess we just made
    min_heapify(heap, 0);
    return root;
}

void destroy_heap(MinHeap* heap) {
    free(heap->data);
    free(heap);
}

// Comparison functions - pretty self explanatory
int compare_int(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int compare_float(const void* a, const void* b) {
    float diff = (*(float*)a - *(float*)b);
    return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
}

int main() {
    // Let's try with ints first
    printf("Testing with integers:\n");
    MinHeap* int_heap = create_min_heap(sizeof(int), 10, compare_int);
    
    int values[] = {5, 3, 7, 1, 4, 6, 2};
    for (int i = 0; i < 7; i++) {
        insert(int_heap, &values[i]);
        printf("Inserted: %d\n", values[i]);
    }

    printf("\nPopping everything out:\n");
    int* min;
    while ((min = (int*)pop(int_heap)) != NULL) {
        printf("Got: %d\n", *min);
        free(min);
    }

    // Now let's do floats
    printf("\nTesting with floats:\n");
    MinHeap* float_heap = create_min_heap(sizeof(float), 10, compare_float);
    
    float float_values[] = {5.5, 3.3, 7.7, 1.1, 4.4};
    for (int i = 0; i < 5; i++) {
        insert(float_heap, &float_values[i]);
        printf("Inserted: %.1f\n", float_values[i]);
    }

    printf("\nPopping everything out:\n");
    float* float_min;
    while ((float_min = (float*)pop(float_heap)) != NULL) {
        printf("Got: %.1f\n", *float_min);
        free(float_min);
    }

    // Clean up our mess
    destroy_heap(int_heap);
    destroy_heap(float_heap);
    
    return 0;
}