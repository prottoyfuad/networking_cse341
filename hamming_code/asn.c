
/**
 *  author  : prottoyfuad
 *  created : January 27, 2021
**/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main() {
  int ops = 1;
  while(ops) {
    puts("Press 1 to send message...\n2 to check received message...\nAnd 0 to exit...");
    scanf("%d", &ops);
    while(ops != 0 && ops != 1 && ops != 2) {
      puts("Invalid value, input again(1 to send, 2 to check message, 0 to exit) :");
      scanf("%d", &ops);
    }
    if(ops == 1) {
      int t;
      puts("Press 0 to input bit sequence or press 1 to input text data :");
      scanf("%d", &t);
      while(t != 0 && t != 1) {
        puts("Invalid value, input again(0 for bit sequence, 1 for text) :");
        scanf("%d", &t);
      }
      char* message = '\0';
      _Bool* rawdata = '\0';
      int bits = 0;
      while(bits == 0) {
        if(t) puts("Enter 'one line' containing the message you want to send :");
        else puts("Enter 'one line' containing the bit sequence you want to send :");
        char ch = '\n';
        while(ch == '\n') ch = getchar();
        while(ch != '\n') {
          if(t || ch == '0' || ch == '1') {
            if(t) {
              rawdata = realloc(rawdata, (size_t)(bits += 8));
              message = realloc(message, (size_t)((bits >> 3) + 1) * sizeof(char));
              message[(bits >> 3) - 1] = ch;
              for(int i = bits - 1, j = bits - 8; i >= j; i--) {
                rawdata[i] = (ch & 1);
                ch >>= 1;
              }
            } else {
              rawdata = realloc(rawdata, (size_t)++bits);
              rawdata[bits - 1] = (ch - '0');
            }
          }
          ch = getchar();
        }
      }
      puts("The entered message is :");
      if(t) {
        message[bits >> 3] = '\0';
        puts(message);
        free(message);
        puts("\nAnd the binary representation of the message is :");
      }
      for(int i = 0; i < bits; i++) {
        printf("%d", rawdata[i]);
      }
      puts("");

      int block_size, block_size_limit = 1e9;
      if(bits < block_size_limit) block_size_limit = bits;
      printf("Specify the block size(minimum 1, maximum %d) :\n", block_size_limit);
      scanf("%d", &block_size);
      while(block_size < 1 || block_size > block_size_limit) {
        printf("Invalid size, input again(minimum 1, maximum %d) :\n", block_size_limit);
        scanf("%d", &block_size);
      }
      int total_blocks = (bits + block_size - 1) / block_size;
      int last_block = bits % block_size;
      if(last_block == 0) last_block = block_size;
      _Bool** dataword = (_Bool**) malloc((size_t)total_blocks * sizeof(_Bool*));
      printf("\nGenerated datawords for this message with %d block size :\n", block_size);

      for(int i = 0, x = 0; i < total_blocks; i++) {
        int cur_size = block_size;
        if(i + 1 == total_blocks) cur_size = last_block;

        dataword[i] = (_Bool*) malloc((size_t)cur_size * sizeof(_Bool));
        for(int j = 0; j < cur_size; j++) {
          dataword[i][j] = rawdata[x++];
        }
        for(int j = 0; j < cur_size; j++) {
          printf("%d", dataword[i][j]);
        } puts("");
      }
      free(rawdata);

      _Bool** codeword = (_Bool**) malloc((size_t)total_blocks * sizeof(_Bool*));
      puts("\nGenerated Codewords with parity bits :");

      for(int i = 0; i < total_blocks; i++) {
        int cur_size = block_size;
        if(i + 1 == total_blocks) cur_size = last_block;
        int p = 0;
        while((1 << p) <= (cur_size + p)) p++;
        int code_size = cur_size + p;

        codeword[i] = (_Bool*) malloc((size_t)code_size * sizeof(_Bool));
        _Bool** parity_bits = (_Bool**) malloc((size_t)p * sizeof(_Bool*));
        for(int j = 0; j < p; j++) {
          parity_bits[j] = &codeword[i][(1 << j) - 1];
          *parity_bits[j] = 0;
        }

        for(int j = 0, k = 0; j < code_size; j++) {
          if(j & (j + 1)) {
            codeword[i][j] = dataword[i][k++];
            for(int x = 0; x < p; x++) {
              int y = 1 << x;
              if(y & (j + 1)) {
                *parity_bits[x] ^= codeword[i][j];
              }
            }
          }
        }
        free(parity_bits);
        for(int j = 0; j < code_size; j++) {
          printf("%d", codeword[i][j]);
        } puts("");
      }
      puts("\nMESSAGE SENT!\n");

      for(int i = 0; i < total_blocks; i++) {
        free(dataword[i]), free(codeword[i]);
      }
      free(dataword), free(codeword);
    }
    else if(ops == 2) {
      int total_blocks, block_size, last_block;
      puts("Enter the number of Codeword blocks :");
      scanf("%d", &total_blocks);
      while(total_blocks < 1) {
        puts("Invalid total number of blocks(at least 1), input again :");
        scanf("%d", &total_blocks);
      }
      puts("Enter the number of bits in every blocks :");
      scanf("%d", &block_size);
      while(block_size < 1) {
        puts("Invalid block size(at least 1), input again :");
        scanf("%d", &block_size);
      }
      puts("Enter the number of bits in last block :");
      scanf("%d", &last_block);
      while(last_block < 1 || last_block > block_size) {
        printf("Invalid block size(minimum 1, maximum %d), input again :\n", block_size);
        scanf("%d", &last_block);
      }
      printf("Enter %d lines containing one codeword blocks per line :\n", total_blocks);

      _Bool** codeword = (_Bool**) malloc((size_t)total_blocks * sizeof(_Bool*));
      _Bool** dataword = (_Bool**) malloc((size_t)total_blocks * sizeof(_Bool*));
      int* error = (int*) malloc((size_t)total_blocks * sizeof(int));
      int data_block = 0, last_data_block = 0;

      for(int i = 0; i < total_blocks; i++) {
        int cur_size = block_size;
        if(i + 1 == total_blocks) cur_size = last_block;
        codeword[i] = (_Bool*) malloc((size_t)cur_size * sizeof(_Bool));

        int bits = 0;
        char ch = '\n';
        while(ch == '\n') ch = getchar();
        while(ch != '\n') {
          if(ch == '0' || ch == '1') {
            if(++bits <= cur_size)
              codeword[i][bits - 1] = (ch - '0');
          }
          ch = getchar();
        }
        if(bits != cur_size) {
          printf("Invalid block, expected %d bits, found %d!\n", cur_size, bits);
          puts("Enter the last block again.");
          free(codeword[i--]);
          continue;
        }
        int p = 0;
        while((1 << p) <= cur_size) p++;
        int data_size = cur_size - p;
        if(i == 0) data_block = data_size;
        else if(i + 1 == total_blocks) last_data_block = data_size;
        else assert(data_block == data_size);
        dataword[i] = (_Bool*) malloc((size_t)data_size * sizeof(_Bool));

        _Bool* parity_bits = (_Bool*) malloc((size_t)p * sizeof(_Bool));
        for(int j = 0; j < p; j++) {
          parity_bits[j] = 0;
        }
        for(int j = 0; j < cur_size; j++) {
          for(int x = 0; x < p; x++) {
            int y = 1 << x;
            if(y & (j + 1)) {
              parity_bits[x] ^= codeword[i][j];
            }
          }
        }
        error[i] = 0;
        for(int j = 0; j < p; j++) {
          if(parity_bits[j]) error[i] += (1 << j);
        }
        free(parity_bits);
      }
      puts("The entered codeword blocks are :");
      for(int i = 0; i < total_blocks; i++) {
        int cur_size = block_size;
        if(i + 1 == total_blocks) cur_size = last_block;
        for(int j = 0; j < cur_size; j++)
          printf("%d", codeword[i][j]);
        puts("");
      }
      puts("\nError correction details :");
      for(int i = 0; i < total_blocks; i++) {
        int cur_size = block_size;
        if(i + 1 == total_blocks) cur_size = last_block;
        for(int j = 0; j < cur_size; j++) {
          printf("%d", codeword[i][j]);
        }
        if(!error[i]) puts("\tCorrect! No error!");
        else {
          codeword[i][error[i] - 1] ^= 1;
          printf("\tError detected at %2dth bit! Corrected code : ", error[i]);
          for(int j = 0; j < cur_size; j++)
            printf("%d", codeword[i][j]);
          puts("");
        }
      }
      puts("\nWithout the redundant bit, the original corrected datawords are :");
      for(int i = 0; i < total_blocks; i++) {
        int cur_size = block_size;
        if(i + 1 == total_blocks) cur_size = last_block;
        for(int j = 0, k = 0; j < cur_size; j++) {
          if(j & (j + 1)) {
            dataword[i][k] = codeword[i][j];
            printf("%d", dataword[i][k++]);
          }
        }
        puts("");
      }
      puts("");

      for(int i = 0; i < total_blocks; i++) {
        free(dataword[i]), free(codeword[i]);
      }
      free(error), free(dataword), free(codeword);
    }
  }
  return 0;
}

