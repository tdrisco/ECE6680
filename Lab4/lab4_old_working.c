// Austin Repp & Tim Driscoll
// Embedded Computing Lab 4
// Huffman Codec

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char symbols[256]; // All possible characters in "alphabet"
int frequencies[256]; // Each index here holds the number of times the corresponding symbol appears in the input file
int freqDecode[256]; //Unsorted frequencies to be stored at begining of encoded output
int codes[256][500]; // Holds binary coding for each corresponding symbol
int symbolCount = 0;
int current_bit = 0;
unsigned char bit_buffer;

// Node typedef (taken from lecture)
typedef struct node {
    int freq; // cumulative frequency of node and all subtree
    int symbol; // 0-255 for leafs, -1 for non-leafs
    struct node* left_child;
    struct node* right_child;
    struct node* parent;
} node;

// Minheap typedef
typedef struct minheap {
    int capacity;
    int size;
    node **array;
} minheap;

// Helper to allocate a new node
node* create_node(int freq, int symbol) {
    node* newNode = (node*)calloc(1, sizeof(node));
    newNode->freq = freq;
    newNode->symbol = symbol;
    newNode->left_child = NULL;
    newNode->right_child = NULL;
    newNode->parent = NULL;
    return newNode;
}

// Returns 1 if it's a leaf, 0 otherwise
int isLeafNode(node* n) {
    if (!n->left_child && !n->right_child) {
        return 1;
    }
    return 0;
}

int get_symbol_index(unsigned char c) {
  int i, symbolIndex = 0;
  for (i = 0;i < 256;i++) {
      if (symbols[i] == c) {
        symbolIndex = i;
        break;
      }
    }
  return symbolIndex;
}

// Takes input file and populates symbols[] and frequencies[]
void calculate_symbol_frequencies(FILE *fpt) {
    int i;
    unsigned char C;

    while (fread(&C, 1, 1, fpt) != 0) {
        for(i = 0; i < 256; i++){
          if(i == C){
            freqDecode[i] += 1;
            break;
          }
        }
        /*
        for (i = 0;i < symbolCount; i++) {
            if (symbols[i] == C) {
                symbolIndex = i;
                break;
            }
        }
        if (symbolIndex == -1) { // If this character is not already in the symbol array
            symbols[symbolCount] = C;
            frequencies[symbolCount] = 1;
            symbolCount++;
        }
        else { // If this character is in the array already, update the frequency for it
            frequencies[symbolIndex]++;
        }*/
    }
}

// Sorts the symbol array based on ascending frequency
void sort_symbol_frequencies() {
    int i,j, min, tempFreq;
    unsigned char tempSymbol;

    for (i = 0; i < symbolCount; i++) {
        min = i;
        for (j = i + 1; j < symbolCount; j++) {
            if (frequencies[j] < frequencies[min]) {
                min = j;
            }
        }
        tempFreq = frequencies[min];
        tempSymbol = symbols[min];
        frequencies[min] = frequencies[i];
        symbols[min] = symbols[i];
        frequencies[i] = tempFreq;
        symbols[i] = tempSymbol;
    }
}

void print_symbol_frequencies() {
    int i;
    for (i = 0;i < symbolCount;i++) {
        printf("%d(%c): %d\n", symbols[i], symbols[i], frequencies[i]);
    }
}

int height(node* n) {
  if (n == NULL) {
    return 0;
  }
  else {
    int leftHeight = height(n->left_child);
    int rightHeight = height(n->right_child);

    if (leftHeight > rightHeight) {
      return(leftHeight+1);
    }
    else {
      return(rightHeight+1);
    }
  }
}

void printLevel(node* n, int level) {
  if (n == NULL) {
    return;
  }
  if (level == 1) {
    printf("%d(%d) ", n->symbol, n->freq);
  }
  else if(level > 1) {
    printLevel(n->left_child, level-1);
    printLevel(n->right_child, level-1);
  }
}

void print2D(node* root, int space) {
  if (root == NULL)
    return;

  space += 10;
  print2D(root->right_child, space);
  printf("\n");
  for (int i = 10; i < space; i++) {
    printf(" ");
  }
  printf("%d(%d) ", root->symbol, root->freq);
  print2D(root->left_child, space);
}

void print_symbol_frequencies_heap(minheap* heap) {
    print2D(heap->array[0], 0);
    printf("\nTop = Right\nBottom = Left\n");
}

//Helper function to resort the heap
void sort_heap(minheap* minHeap, int nodeIndex){
  int top, left, right;
  node *tempNode;
  //tempNode = create_node(0,0);

  top = nodeIndex;
  left = 2 * nodeIndex + 1;
  right = 2 * nodeIndex + 2;

  //printf("size: %d top: %d left: %d right %d\n",minHeap->size, top, left, right);
  //printf("top: %d left: %d right: %d\n",minHeap->array[top]->freq,minHeap->array[left]->freq,minHeap->array[right]->freq);

  if(left < minHeap->size && minHeap->array[left]->freq < minHeap->array[top]->freq)
    top = left;
  if(right < minHeap->size && minHeap->array[right]->freq < minHeap->array[top]->freq)
    top = right;
  if(top != nodeIndex)//if it changed make swap
    {
      tempNode = minHeap->array[top];
      minHeap->array[top] = minHeap->array[nodeIndex];
      minHeap->array[nodeIndex] = tempNode;
      sort_heap(minHeap, top);//after swap recursively check altered tree
    }

}

// Creates a minheap and assigns the beginning array from the symbol array
minheap* create_minheap() {
    int i;

    minheap* minHeap = (minheap*)calloc(1, sizeof(minheap));
    minHeap->capacity = symbolCount;

    minHeap->array = (node**)calloc(symbolCount, sizeof(node*));

    for (i = 0; i < symbolCount; i++) {
        minHeap->array[i] = create_node(frequencies[i], symbols[i]);
    }

    //printf("\n\n");
    //for (i = 0; i < symbolCount; i++) {
        //printf("%d: %d\n", minHeap->array[i]->symbol, minHeap->array[i]->freq);
    //}

    minHeap->size = symbolCount;
    int ii = symbolCount - 1;
    //heap data is intialized now it needs to be built
    for(int i = (ii - 1)/2; i >= 0; i--)
      sort_heap(minHeap,i);

    return minHeap;
}

// Takes intialized minheap and sorts it to produce tree
void create_SortedTree(minheap* minHeap) {

  int arrayPosition = 0;
  int combinedFrequency;
  node *minNode1, *minNode2, *parentNode;
  //minNode1 = create_node(0,0);
  //minNode2 = create_node(0,0);

  //combine nodes until there is one parent (heap size = 1)
  while(minHeap->size != 1)
    {
      //grab smallest item in array
      minNode1 = minHeap->array[0];
      minHeap->array[0] = minHeap->array[minHeap->size -1];//shift last item to removed first
      minHeap->size -= 1; //update the size
      sort_heap(minHeap, 0); //sort the heap after removal

      //printf("\n\n");
      //for (int i = 0; i < minHeap->size; i++) {
          //printf("%d: %d\n", minHeap->array[i]->symbol, minHeap->array[i]->freq);
    //  }
      //break;
      //Repeat to extraxt second min frequency
      minNode2 = minHeap->array[0];
      minHeap->array[0] = minHeap->array[minHeap->size -1];
      minHeap->size -= 1;
      sort_heap(minHeap, 0);

      //Combine two smallest frequencies and create new node (parent)
    //  printf("Minval 1 = %d Minval2 = %d ",minNode1->freq, minNode2->freq);
      combinedFrequency = minNode1->freq + minNode2->freq;
      parentNode = create_node(combinedFrequency,33);
      parentNode->left_child = minNode1;
      parentNode->right_child = minNode2;

      //add the new parent node to the minHeap
      arrayPosition = minHeap->size;
      minHeap->size += 1;
      while(arrayPosition > 0 && parentNode->freq < minHeap->array[(arrayPosition-1)/2]->freq){
        minHeap->array[arrayPosition] = minHeap->array[(arrayPosition-1)/2];
        arrayPosition = (arrayPosition-1)/2;
      }
      minHeap->array[arrayPosition] = parentNode;
      //printf("inserting %d(%d) at %d\n", parentNode->symbol, parentNode->freq, arrayPosition);
    }
}

void create_codes(node* root, int top, int* arr) {
    if (root->left_child) {
        arr[top] = 0;
        create_codes(root->left_child, top + 1, arr);
    }
    if (root->right_child) {
        arr[top] = 1;
        create_codes(root->right_child, top + 1, arr);
    }
    if (isLeafNode(root)) {
        printf("%c(%d): ", root->symbol, root->symbol);
        int i;
        //for (i = 0; i < top; ++i) {
            //printf("%d", arr[i]);
        //}
        //printf("\n");

        // Save code
        int symbolIndex = get_symbol_index((unsigned char)root->symbol);
        printf("% d ", top);
        codes[symbolIndex][0] = top; // Store the length of the code in the first index, followed by the code itself
        for (i = 0; i < top; ++i) {
            codes[symbolIndex][i+1] = arr[i];
            printf("%d",arr[i]);
        }
        printf("\n");

        // Write code to file
        // for (i = top - 1; i >= 0; i--) {
        //   buffer |= arr[i] << nbuffer;
        //   nbuffer++;
        //   if (nbuffer == 8) {
        //     fputc(buffer, fpt);
        //     buffer = 0;
        //     nbuffer = 0;
        //   }
        // }
    }
}

void write_bit_to_file(int bit, FILE* fpt) {
  if (bit) {
    bit_buffer |= 128 >> current_bit;//I think we need to make bit buffer an int because codes could become longer then 8 bits
  }
  current_bit++;
  if (current_bit == 8) {
    fwrite(&bit_buffer, 1, 1, fpt);
    current_bit = 0;
    bit_buffer = 0;
  }
}

// Reads input file, fetches code for each character, and writes codes to output file
void encode_input(FILE *in, FILE *out) {
  unsigned char C;//, buffer = 0, nbuffer = 0;
  unsigned char arr[500];
  int i;

  while (fread(&C, 1, 1, in) != 0) {
    int symbolIndex = get_symbol_index(C);
    int codeSize = codes[symbolIndex][0];

    //printf("\nsymbol: %c, index: %d, code: ", C, symbolIndex);
    for (i = 0; i < codeSize; ++i) {
        //printf("%d", codes[symbolIndex][i+1]);
        arr[i] = codes[symbolIndex][i+1];
    }
    //printf("  ");

    // Write code to file
    for (i = 0; i < codeSize; i++) {
      write_bit_to_file(arr[i], out);
    }
  }
  // Write any "leftovers" to file
  while (current_bit)
    write_bit_to_file(0, out);
}

//write frequencies to the output file so tree can be reconstructed
void write_symbol_frequencies(FILE *fpt_out){
  int i;
  for(i = 0; i < 256; i++)
    fwrite(&freqDecode[i],sizeof(int),1,fpt_out);
}

void read_symbol_frequencies(FILE *fpt){
  int i;
  for(i = 0; i < 256; i++)
    fread(&freqDecode[i],sizeof(int),1,fpt);
}

// Sorts the symbol array based on ascending frequency
void eliminate_frequencies_decode() {
    int i;

    symbolCount = 0;
    for(i = 0; i < 256; i++){
      if(freqDecode[i] != 0){
        frequencies[symbolCount] = freqDecode[i];
        //printf("Here: %d ",i);
        symbols[symbolCount] = i;
        symbolCount++;
      }
    }
}

// Reads input file, navigates tree according to code, writes symbol
void decode_input(FILE *in, FILE *out, node* currNode, node* root) {
  unsigned char C;//, buffer = 0, nbuffer = 0;
  unsigned char write;
  int i;

  if(fread(&C, 1, 1, in) == 0)
    return;

    //while (fread(&C, 1, 1, in) != 0) {
      //iterate 8 times (full byte) -> need to determine how to maintain position if code doesnt fit within current byte
      for(i = 0; i < 8; i++){
        //printf("here\n");
        //look at first character determine which way to move
        if((C&128) == 128){ //(mask all but leading bit) -> navigate right (1)
          //check for leaf, if leaf write symbol and resent trackerNode to root
          //shift out leading bit
          currNode = currNode->right_child;
          C = C << 1;
          if(isLeafNode(currNode)){
            write = currNode->symbol;
            //printf("Right: %c\n", write);
            fwrite(&write, 1, 1, out);
            currNode = root;
          }
        }
        else{//navigate left (0)
          currNode = currNode->left_child;
          C = C << 1;
          if(isLeafNode(currNode)){
            write = currNode->symbol;
            //printf("Left: %c\n", write);
            fwrite(&write, 1, 1, out);
            currNode = root;
          }
        }
      }
      decode_input(in, out, currNode, root);
}

int main(int argc, char *argv[]) {
    FILE *fpt, *fpt_out;
    int compress = 0; // 1 = compress, 0 = decompress

    // Validate command arguments
    if (argc != 4)
    {
        printf("Usage:  ./lab4 [compress/decompres] [input file] [output file]\n");
        exit(0);
    }
    if (strcmp("compress", argv[1]) == 0)
    {
        compress = 1;
    }
    else if (strcmp("decompress", argv[1]) == 0)
    {
        compress = 0;
    }
    else
    {
        printf("Use compress or decompress command in first argument\n");
        exit(0);
    }

    //Open files for reading and writing
    fpt = fopen(argv[2], "rb");
    if (fpt == NULL)
    {
        printf("Unable to open %s for reading\n", argv[2]);
        exit(0);
    }
    fpt_out = fopen(argv[3], "wb");
    if (fpt_out == NULL)
    {
        printf("Unable to open %s for writing\n", argv[3]);
        exit(0);
    }
    //If compression build tree, determine codes and write coded values to file
    if (compress) {

        calculate_symbol_frequencies(fpt);

        //for(int i = 0; i < 256; i++)
          //printf("%d: %d\n",i,freqDecode[i]);
        //write frequencies to the output file
        write_symbol_frequencies(fpt_out);

        fseek(fpt, 0, SEEK_SET);
        eliminate_frequencies_decode();
        sort_symbol_frequencies();


        //printf("\nIntialized Huff Tree:\n");
        //print_symbol_frequencies();

        minheap* heap = create_minheap();
        //print_symbol_frequencies_heap(heap);

        create_SortedTree(heap);
        //printf("\nsorted HUFF Tree:\n");
        //print_symbol_frequencies_heap(heap);

        int h = height(heap->array[0]) + 1;
        int* codeArr = (int*)calloc(h, sizeof(int));

        create_codes(heap->array[0], 0, codeArr);
        encode_input(fpt, fpt_out);


    }
    //Run through decompression sequence
    else{

      //read through and extract the first 256*4 bytes to get frequency values (ints)
      read_symbol_frequencies(fpt);
      eliminate_frequencies_decode();
      sort_symbol_frequencies();
      //print_symbol_frequencies();

      minheap* heap = create_minheap();
      //print_symbol_frequencies_heap(heap);

      create_SortedTree(heap);
      //printf("\nsorted HUFF Tree:\n");
      //print_symbol_frequencies_heap(heap);

      //int h = height(heap->array[0]) + 1;
      //int* codeArr = (int*)calloc(h, sizeof(int));
      //create_codes(heap->array[0], 0, codeArr);

      decode_input(fpt, fpt_out, heap->array[0], heap->array[0]);

      //for(int i = 0; i < symbolCount; i++)
        //printf("%c: %d\n",symbols[i],frequencies[i]);


    }
}
