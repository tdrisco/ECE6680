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
    return newNode;
}

// Returns 1 if it's a leaf, 0 otherwise
int isLeafNode(node* n) {
    if (!n->left_child && !n->right_child) {
        return 1;
    }
    return 0;
}

// Returns integer containing the index (positon) of desired symbol
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

// Takes input file and determines the frequency of every possible character (0-255)
void calculate_symbol_frequencies(FILE *fpt) {
    int i;
    unsigned char C;

    //Loop through all possible characters and count occurences
    while (fread(&C, 1, 1, fpt) != 0) {
        for(i = 0; i < 256; i++){
          if(i == C){
            freqDecode[i] += 1;
            break;
          }
        }
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

// Print out array of symbols with corresponding frequency
void print_symbol_frequencies() {
    int i;
    for (i = 0;i < symbolCount;i++) {
        printf("%d(%c): %d\n", symbols[i], symbols[i], frequencies[i]);
    }
}

//Move down tree to determine hieght of tree (integer)
int height(node* n) {
  //Bottom of tree
  if (n == NULL) {
    return 0;
  }
  //Move down tree recursively looking at left and right child
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

// Helper function to print the tree with tree like visual
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

// Helper function to call print2D and add reading information
void print_symbol_frequencies_heap(minheap* heap) {
    print2D(heap->array[0], 0);
    printf("\nTop = Right\nBottom = Left\n");
}

//Helper function to resort the heap
void sort_heap(minheap* minHeap, int nodeIndex){

  int top, left, right;
  node *tempNode;

  //Starting position
  top = nodeIndex;
  //left child index
  left = 2 * nodeIndex + 1;
  //right child index
  right = 2 * nodeIndex + 2;

  //Check if left node frequency is greater then top
  if(left < minHeap->size && minHeap->array[left]->freq < minHeap->array[top]->freq)
    top = left; //left node gets shifted to top
  //Check if right node frequency is greater then top
  if(right < minHeap->size && minHeap->array[right]->freq < minHeap->array[top]->freq)
    top = right; //right node gets shifted to top
  if(top != nodeIndex)//if either if statement was true make swap
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

    //Intialize a minheap structure
    minheap* minHeap = (minheap*)calloc(1, sizeof(minheap));
    minHeap->capacity = symbolCount; //number of different symbols to occur in input

    minHeap->array = (node**)calloc(symbolCount, sizeof(node*)); //allocate array of nodes

    //Initalize all the nodes with symbols and corresponding frequency
    for (i = 0; i < symbolCount; i++) {
        minHeap->array[i] = create_node(frequencies[i], symbols[i]);
    }

    minHeap->size = symbolCount;
    int ii = symbolCount - 1;

    //heap data is intialized now it needs to be built
    for(int i = (ii - 1)/2; i >= 0; i--)
      sort_heap(minHeap,i);// Sort heap to create the intial tree structure

    return minHeap;
}

// Takes intialized minheap and sorts it to produce huffman tree
minheap* create_HuffmanTree() {

  int arrayPosition = 0;
  int combinedFrequency;
  node *minNode1, *minNode2, *parentNode;

  minheap* minHeap = create_minheap();//Intialize heap

  //combine nodes until there is one parent (heap size = 1)
  while(minHeap->size != 1)
    {
      //grab smallest frequency in array
      minNode1 = minHeap->array[0];
      minHeap->array[0] = minHeap->array[minHeap->size -1];//shift last item to removed first
      minHeap->size -= 1; //update the size
      sort_heap(minHeap, 0); //sort the heap after removal

      //Repeat to grab second smallest frequency
      minNode2 = minHeap->array[0];
      minHeap->array[0] = minHeap->array[minHeap->size -1];
      minHeap->size -= 1;
      sort_heap(minHeap, 0);

      //Combine two smallest frequencies and create new node (parent)
      combinedFrequency = minNode1->freq + minNode2->freq;
      parentNode = create_node(combinedFrequency,33); //33 is for random internal symbol that doesn't matter (!)
      parentNode->left_child = minNode1;
      parentNode->right_child = minNode2;

      //add the new parent node to the minHeap
      arrayPosition = minHeap->size;
      minHeap->size += 1; //increase size
      while(arrayPosition > 0 && parentNode->freq < minHeap->array[(arrayPosition-1)/2]->freq){ // while frequency of parentNode is less then parent
        minHeap->array[arrayPosition] = minHeap->array[(arrayPosition-1)/2]; //move parent (larger frequency up tree)
        arrayPosition = (arrayPosition-1)/2; //move down tree
      }
      minHeap->array[arrayPosition] = parentNode; // insert parentNode at proper position
    }
    return minHeap;
}

//Determine the varying bit patterns for all the symbols
void create_codes(node* root, int top, int* arr) {
    if (root->left_child) { //Moving left means code val is 0
        arr[top] = 0;
        create_codes(root->left_child, top + 1, arr); //recursively call to determine next value in code
    }
    if (root->right_child) { //Moving right means code val is 1
        arr[top] = 1;
        create_codes(root->right_child, top + 1, arr);//recursively call to determine next value in code
    }
    if (isLeafNode(root)) { // Leaf means bottom of the tree and code is done for given symbol
        int i;
        // Save code
        int symbolIndex = get_symbol_index((unsigned char)root->symbol);// Find which symbol code is for
        codes[symbolIndex][0] = top; // Store the length of the code in the first index, followed by the code itself
        for (i = 0; i < top; ++i) {
            codes[symbolIndex][i+1] = arr[i];
        }
    }
}

void write_bit_to_file(int bit, FILE* fpt) {
  if (bit) {
    bit_buffer |= 128 >> current_bit;
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

    for (i = 0; i < codeSize; ++i) {
        arr[i] = codes[symbolIndex][i+1];
    }

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

//Read all thre frequencies in from input file in decode process
void read_symbol_frequencies(FILE *fpt){
  int i;
  for(i = 0; i < 256; i++)
    fread(&freqDecode[i],sizeof(int),1,fpt);
}

// Eliminate symbols that didn't appear in the input and track corresponding symbols
void eliminate_frequencies_decode() {
    int i;

    symbolCount = 0;
    for(i = 0; i < 256; i++){
      if(freqDecode[i] != 0){ //If character occurs in file
        frequencies[symbolCount] = freqDecode[i]; // Assign frequency count into new array
        symbols[symbolCount] = i; //Determine corresponding symbol
        symbolCount++; //Keep track of number of symbols in file
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

      //iterate 8 times (full byte) to view all the bits in character read in
      for(i = 0; i < 8; i++){
        //look at first character determine which way to move
        if((C&128) == 128){ //(mask all but leading bit) -> navigate right (1)
          //check for leaf, if leaf write symbol and resent currNode to root
          //shift out leading bit
          currNode = currNode->right_child; // 1 means move to right child
          C = C << 1; //Shift out leading bit (already checked)
          if(isLeafNode(currNode)){ // Check for leaf (bottom of tree)
            write = currNode->symbol; //If bottom determine and write symbol
            fwrite(&write, 1, 1, out);
            currNode = root; //Recent currNode to root of tree
          }
        }
        else{//navigate left (0)
          currNode = currNode->left_child; //Same process as above but moving left because value was 0
          C = C << 1;
          if(isLeafNode(currNode)){
            write = currNode->symbol;
            fwrite(&write, 1, 1, out);
            currNode = root;
          }
        }
      }
      decode_input(in, out, currNode, root);//Recursively call while bytes still in file currNode also handles cases where codes are split between bytes
}

//Wrapper function to handle compression frequency determination and sort process
void Compress_Frequency_Process(FILE *in, FILE *out) {
    calculate_symbol_frequencies(in); //determine frequency for every symbot
    write_symbol_frequencies(out); //write data to output for decompression
    fseek(in, 0, SEEK_SET); //rewind file position in input
    eliminate_frequencies_decode(); //Remove symbols that didn't occur in file
    sort_symbol_frequencies(); //Sort frequencies/symbols in asceding order
}

//Wrapper function to handle decompression frequency determination and sort process
void Decompress_Frequency_Process(FILE *in) {
    read_symbol_frequencies(in); //read through and extract the first 256*4 bytes to get frequency values (ints)
    eliminate_frequencies_decode(); //Remove symbols that didn't occur in file
    sort_symbol_frequencies(); //Sort frequencies/symbols in asceding order
}

int main(int argc, char *argv[]) {
    FILE *fpt, *fpt_out;
    int compress = 0;

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
        Compress_Frequency_Process(fpt, fpt_out);

        minheap* heap = create_HuffmanTree();

        int h = height(heap->array[0]) + 1;
        int* codeArr = (int*)calloc(h, sizeof(int));

        create_codes(heap->array[0], 0, codeArr);
        encode_input(fpt, fpt_out);
    }
    //Run through decompression sequence
    else{
      Decompress_Frequency_Process(fpt);

      minheap* heap = create_HuffmanTree();

      decode_input(fpt, fpt_out, heap->array[0], heap->array[0]);
    }
}
