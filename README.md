# Árvore Van Emde Boas - Estruturas de Dados Avançadas (2025.1)

Uma implementação da **Árvore Van Emde Boas** com espaço linear usando tabela de dispersão com *table doubling/halving*.

Segue abaixo a especificação das funções e estruturas. Iremos utilizar uma **Tabela Hash linear** com uma **função de hash 5-independente** a fim de manter tempo esperado de O(1)/operação.

```c++
class Node {
    public:
        int key;
        unsigned short deleted : 1;

        Node();
        int setValue(int k);
        void deleteNode(); // Vamos utilizar um lazy-delete, apenas setando o valor `deleted` para true
};
```

```c++
class HashTable {
    public:
        int size;
        std::vector<Node> table;
        int filled; // Vamos armazenar quantos elementos do vetor estão preenchidos, a fim de mantermos a tabela sempre com tamanho m >= 2n, onde n é a qtde de chaves

        float a1, a2, a3, a4, a5; // Coeficientes aleatórios do polinômio de grau 4 para a função de hash 5-independente

        HashTable();

        void tableDoubling();
        void tableHalving();
        int insertKey(int k);
        int findKey(int k);
        int deleteKey(int k);
        int hash(int k);
        void printTable();
};
```