# Árvore Van Emde Boas - Estruturas de Dados Avançadas (2025.1)

Uma implementação da **Árvore Van Emde Boas** com espaço linear usando tabela de dispersão com *table doubling/halving*.

Segue abaixo a especificação das funções e estruturas. Iremos utilizar uma **Tabela Hash linear** com uma **função de hash 5-independente** a fim de manter tempo esperado de O(1)/operação.

```c++
class Node {
    public:
        int32_t key;
        unsigned short deleted : 1;
        vEBTree* pointer;

        Node();

        int32_t setValue(int32_t k);
        void initializePointer(vEBTree* tree);
        void deleteNode();
};
```

```c++
class HashTable {
    public:
        int32_t size;
        std::vector<Node> table;
        int32_t filled; // Vamos armazenar quantos elementos do vetor estão preenchidos, a fim de mantermos a tabela sempre com tamanho m >= 2n, onde n é a qtde de chaves

        float a1, a2, a3, a4, a5; // Coeficientes aleatórios do polinômio de grau 4 para a função de hash 5-independente

        HashTable();

        void tableDoubling();
        void tableHalving();
        int32_t insertKey(int32_t k, vEBTree* tree);
        std::pair<int, Node*> findKey(int32_t k);
        int32_t deleteKey(int32_t k);
        int hash(int32_t k);
        void printTable();
};
```

A árvore van Emde Boas é inicializada com um universo de chaves de tamanho 2^32 = 4294967296. 

```c++
class vEBTree {
    public:
        long long int min;
        long long int max;

        HashTable clusters;
        vEBTree* resumo;

        vEBTree();

        int32_t insert(int32_t x);
        long long int findNext(int32_t x);
        long long int findPrevious(int32_t x);
        int32_t remove(int32_t x);
        void print(std::ofstream& Output);
        void initializeResumo(vEBTree* res);
};
```

Se um elemento não possui predecessor (respec. sucessor), o valor retornado como predecessor dele será INT32_MIN (respec. INT32_MAX). Exemplo no arquivo `saida.txt`:

```
PRE 1
-2147483648
SUC 6553678
2147483647
```

### Instruções

Para executar, compile o arquivo main.cpp:

```
g++ main.cpp
```

Se desejar, pode adicionar a tag `-o` seguida de um nome para o arquivo compilado. Por exemplo:

```
g++ main.cpp -o vebtree.out
```

Caso um nome para o arquivo compilado não seja especificado, será criado um arquivo chamado `a.out` (ou `a.exe` no caso do Windows). Execute o arquivo passando como parâmetro o caminho para o arquivo com as operações:

```
./a.out ./entrada2.txt
```

Um arquivo chamado `saida.txt` será gerado com as impressões e resultados das operações de sucessor, predecessor e imprimir especificadas no arquivo de entrada.