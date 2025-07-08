#include <vector>
#include <iostream>
#include <random>

#define INITIAL_SIZE 16 // Escolhi o valor como uma potência de 2 não muito grande, não sei bem se existe alguma regra para o tamanho inicial da tabela
#define P 4397 // Número primo aleatório para utilizar na função de hash

using namespace std;

class Node {
    public:
        int key;
        unsigned short deleted : 1;

        Node() {
            this->key = -1;
            this->deleted = false;
        }

        int setValue(int k);
        void deleteNode();
};

int Node::setValue(int k) {
    this->key = k;
    return k;
}

void Node::deleteNode() {
    // Vamos usar lazy-deletion, apenas sinalizando que o nó foi deletado
    this->deleted = true;
}

class HashTable {
    public:
        int size;
        std::vector<Node> table;
        int filled; // Vamos armazenar quantos elementos do vetor estão preenchidos, a fim de mantermos a tabela sempre com tamanho m >= 2n, onde n é a qtde de chaves

        float a1, a2, a3, a4, a5; // Coeficientes aleatórios do polinômio de grau 4 para a função de hash 5-independente

        HashTable() {
            // Inicializando a table de dispersão com tamanho inicial igual a INITIAL_SIZE
            this->size = INITIAL_SIZE;
            std::vector<Node> vec(INITIAL_SIZE);
            this->table = vec;
            this->filled = 0;

            // Gerando os coeficientes aleatórios
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(2, P);
            this->a1 = distrib(gen);
            this->a2 = distrib(gen);
            this->a3 = distrib(gen);
            this->a4 = distrib(gen);
            this->a5 = distrib(gen);

            cout << a1 << ", " << a2 << ", " << a3 << ", " << a4 << ", " << a5 << endl;
        }

        void tableDoubling();
        void tableHalving();
        int insertKey(int k);
        int findKey(int k);
        int deleteKey(int k);
        int hash(int k);
        void printTable();
};

void HashTable::tableDoubling() {
    std::vector<Node> aux(this->size*2);
    std::vector<Node> cp(this->table);

    // duplicando o vetor
    this->size = this->size*2;
    this->table = aux;
    this->filled = 0;

    // reinserindo as chaves na nova posição    
    for (int i = 0; i < cp.size(); i++) {
        if (cp[i].key != -1 && cp[i].deleted != true) {
            this->insertKey(cp[i].key);
        }
    }
}

void HashTable::tableHalving() {
    std::vector<Node> aux((int) this->size/2);
    std::vector<Node> cp(this->table);

    // "metadeando" o vetor
    this->size = (int) this->size/2;
    this->table = aux;
    this->filled = 0;

    // reinserindo as chaves na nova posição    
    for (int i = 0; i < cp.size(); i++) {
        if (cp[i].key != -1 && cp[i].deleted != true) {
            this->insertKey(cp[i].key);
        }
    }
}

int HashTable::hash(int k) {
    // Vamos utilzar uma função de hash 5-independente, a fim de obtermos tempo esperado de O(1)/op
    double sum = this->a5*std::pow(k,4) + this->a4*std::pow(k,3) + this->a3*std::pow(k,2) + this->a2*k + this->a1;
    return ((long int) sum % P) % this->size;
}

int HashTable::insertKey(int k) {

    Node node = Node();
    node.setValue(k);

    int idx = this->hash(k);
    cout << "Inserindo na posição: " << idx << endl;


    while (this->table[idx].key != -1 && this->table[idx].deleted != true && this->table[idx].key != k) {
        idx++;
        if (idx >= this->size) {
            idx = 0;
        }
    }

    if (this->table[idx].key == k && this->table[idx].deleted == false) {
        return -1; // nó já inserido
    } else {
        this->table[idx] = node;
        this->filled++;
    }

    if (this->filled > this->size/2) {
        this->tableDoubling();
    }

    return idx;
}

int HashTable::findKey(int k) {
    int idx = this->hash(k);
    int idx_ = idx;

    while (this->table[idx_].key != -1 && idx_ != idx-1 && this->table[idx_].key != k) {
        idx_++;
        if (idx_ >= this->size) {
            idx_ = 0;
        }
    }

    if (this->table[idx_].key == -1 || idx_ == idx-1) {
        // não encontramos o elemento ou demos a volta no vetor
        return -1;
    }

    if (this->table[idx_].key == k && this->table[idx_].deleted == true) {
        // elemento existe mas foi deletado
        return -1;
    }

    return idx;
}

int HashTable::deleteKey(int k) {
    int idx = this->findKey(k);

    if (idx == -1) {
        return -1; // Nó não existe ou já foi deletado
    }

    this->table[idx].deleteNode();
    this->filled--;

    if (this->filled < this->size/2) {
        this->tableHalving();
    }

    return idx;
}

void HashTable::printTable() {
    std::cout<< this->filled << " POSITIONS FILLED" << endl << "|";
    for (int i = 0; i < this->size; i++) {
        if (this->table[i].key != -1 && this->table[i].deleted != true) {
            std::cout << " " << this->table[i].key << " |";
        } else {
            std::cout << " X |";
        }
    }
    std::cout << endl << endl;
}

int main() {

    HashTable table = HashTable();

    int k=0;

    while (k!=-1) {
        std::cout << "Digite um valor para inserir na tabela: ";
        std::cin >> k;
        std::cout << endl;

        int idx = table.insertKey(k);
        table.printTable();
    }

    return 0;
}