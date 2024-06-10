#include <stdio.h>
#include <string.h>

int turnOffset(int endereco);
int turnPageNumber(int endereco);
void readBinFile(FILE *arquivo, int page_number);
int BinaryToDecimal(char *lista_binarios);
int findLRU(int time[], int n);
void atualizarLRU(int frame, int time[], int cont_address);
void decimalToBinary(int decimal, char *listaBin);
int binaryToDecimal(const char *listaBin);

int main(int argc, char *argv[]) {

    int page_table[128];  
    int TLB[16][2];       
    int main[128][256];
    signed char buffer[256];
    int time[128];
    char listaBin[17];

    FILE *arqv_address, *correct, *sec_memory;

    arqv_address = fopen(argv[1], "r");
    correct = fopen("correct.txt", "w");
    sec_memory = fopen("BACKING_STORE.bin", "rb");

    long long page_number, offset;
    int address, logical_address, frame, physical_address = 0;
    int cont_address = 0, page_fault = 0, frame_init = 0, value = 0, valor = 0;
    int negativo = 0, tlb_hit = 0, indice = 0;

    for(int i = 0; i < 128; i++){
        page_table[i] = -1;
        time[i] = 0;
    }

    for(int i = 0; i < 16; i++){
        TLB[i][0] = -1;
    }

    for(int i = 0; i < 128; i++){
        main[i][0] = -1;
    }

    while(fscanf(arqv_address, "%d", &address) != EOF){
        
        logical_address = address;
        decimalToBinary(address, listaBin);

        char offsetBin[9];
        char pageNumberBin[9];

        for (int i = 0; i < 8; i++) {
            offsetBin[i] = listaBin[8 + i];
            pageNumberBin[i] = listaBin[i];
        }
        offsetBin[8] = '\0'; 
        pageNumberBin[8] = '\0';

        offset = binaryToDecimal(offsetBin);
        page_number = binaryToDecimal(pageNumberBin);

        cont_address++;

        int negativo = 0, tlb_found = 0, pageTable_found = 0;
        int tlb_index = -1;

        for(int i = 0; i < 16; i++){
            if(TLB[i][0] == page_number){
                tlb_hit++;
                tlb_found = 1;
                frame = TLB[i][1];
                tlb_index = i;
                break;
            }
        }

        if(tlb_found == 0){ //não encontramos o page_number na TLB vamos procurar na Page Table

            for(int i = 0; i < 128; i++){
                if(page_table[i] == page_number){
                    frame = i;
                    pageTable_found = 1;
                    break;
                }
            }
            // se não encontrarmos na page table é necessário ir pro bin - atualizar cm fifo e lru // pageTable_found == 0
            if(pageTable_found == 0){
                page_fault++;

                if(strcmp(argv[2], "fifo") == 0){
                    frame = frame_init;
                    frame_init++;
                    if(frame_init == 128){
                        frame_init = 0;
                    }
                }
                else if(strcmp(argv[2], "lru") == 0){
                    frame = findLRU(time, 128);
                }

                fseek(sec_memory, page_number * 256, SEEK_SET);
                fread(buffer, sizeof(signed char), 256, sec_memory);
                
                for(int i = 0; i < 256; i++){
                    main[frame][i] = buffer[i];
                }

            }
            
            //atuzalizar TLB
            TLB[indice][0] = page_number;
            TLB[indice][1] = frame;
            tlb_index = indice;
            indice = (indice + 1) % 16;
        }

        if(strcmp(argv[2], "lru") == 0){
            atualizarLRU(frame, time, cont_address);
        }

        page_table[frame] = page_number;
        physical_address = (frame * 256) + offset;
        value = main[frame][offset];
        fprintf(correct, "Virtual address: %d TLB: %d Physical address: %d Value: %d\n", logical_address, tlb_index, physical_address, value);

        char binario[9];
        for(int i = 0; i < 256; i++){
            //tranformando de byte para binário 8 bits
            for(int j = 7; j >= 0; j--){
                binario[j] = (buffer[i] & 1) ? '1' : '0';
                buffer[i] >>= 1;
            }
            binario[8] = '\0';
            if(binario[0] == '1'){
                negativo = 1;
            }
            else {
                negativo = 0;
            }

            //tranformando o número binário em complemento a dois
            for (int k = 0; k < 8; k++) {
                binario[k] = (binario[k] == '1') ? '0' : '1';
            }
            for (int w = 7; w >= 0; w--) {
                if (binario[w] == '0') {
                    binario[w] = '1';
                    break;
                } else {
                    binario[w] = '0';
                }
            }

            //tranformar de binário para decimal
            if(negativo == 1){
                valor = BinaryToDecimal(binario) * (-1);
            }else{
                valor = BinaryToDecimal(binario);
            }
        }
    }

    float pageFaultRate = (float)page_fault / (float)cont_address;
    float tlbHitRate = (float)tlb_hit / (float)cont_address;
    fprintf(correct, "Number of Translated Addresses = %d\n", cont_address);
    fprintf(correct, "Page Faults = %d\n", page_fault);        
    fprintf(correct, "Page Fault Rate = %.3f\n", pageFaultRate);
    fprintf(correct, "TLB Hits = %d\n", tlb_hit);
    fprintf(correct, "TLB Hit Rate = %.3f\n", tlbHitRate);

    fclose(arqv_address);
    fclose(correct);
    fclose(sec_memory);

    return 0;
}

int turnOffset(int endereco){
    return endereco & 255;
}

int turnPageNumber(int endereco){
    endereco = endereco >> 8;
    endereco = endereco & 255;
    return endereco;
}

int findLRU(int time[], int n){
    int minimum = time[0], posicao = 0;
    for(int i = 1; i < n; i++){
        if(time[i] < minimum){
            minimum = time[i];
            posicao = i;
        }
    }
    return posicao;
}

void atualizarLRU(int frame, int time[], int cont_address){
    time[frame] = cont_address;
}

int BinaryToDecimal(char *lista_binarios){
    int decimal = 0;
    int fator = 1;

    for (int i = 7; i >= 0; i--) {
        if (lista_binarios[i] == '1') {
            decimal += fator;
        }
        fator *= 2;
    }

    return decimal;
}

void decimalToBinary(int decimal, char *listaBin) {
    for (int i = 15; i >= 0; i--) {
        listaBin[i] = (decimal & 1) ? '1' : '0';
        decimal >>= 1;
    }
    listaBin[16] = '\0'; 
}

int binaryToDecimal(const char *listaBin) {
    int decimal = 0;
    int fator = 1;
    
    for (int i = 7; i >= 0; i--) {
        if (listaBin[i] == '1') {
            decimal += fator;
        }
        fator *= 2;
    }
    return decimal;
}