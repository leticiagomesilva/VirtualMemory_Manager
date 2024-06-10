# VirtualMemory_Manager
Implementação do Designing a Virtual Memory Manager projeto da matéria de Infraestrutura de Software, do terceiro período de Ciência da Computação na Cesar School.

### Designing a Virtual Memory Manager
This project consists of writing a program that translates logical to physical addresses for a virtual address space of size 2^16 =  65,536 bytes. Your program will read from a file containing logical  addresses and, using a TLB as well as a page table, will translate each logical address to its corresponding physical address and output the value of the byte stored at the translated physical address. 

Esse git contém o arquivo que vai ser lido com os endereços físicos "addresses.txt", o arquivo binário e a saída esperada para FIFO "correct.txt". 

### Importante:

O programa deve ser implementado em C e ser executável em sistemas Linux, com a compilação feita por Makefile, através simplesmente do comando make via terminal, e retornar o arquivo com nome vm executável.

Para validação da implementação, será utilizado o compilador gcc 13.2.0, com o arquivo de saída seguindo rigorosamente a formatação e texto apresentados.

- Os frames na memória física devem ser preenchido do 0 ao 127, e quando a memória estiver
cheia, aplicasse o algoritmo de substituição a fim de identificar qual frame será atualizado;

- Deve-se implementar dois algoritmos de substituição de página, a saber fifo e lru, enquanto
que na TLB será aplicado apenas o fifo;

- O primeiro argumento por linha de comando será um arquivo de endereços lógicos,
o segundo argumento será o tipo de algoritmo a ser
utilizado para substituição da página (fifo ou lru).

Por exemplo, a chamada:

```
./vm address.txt lru
```

indica que o algoritmo de substituição da página será o lru.
