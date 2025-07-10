#include <vector>
#include <iostream>
#include <random>

#define INITIAL_SIZE 16 // Escolhi o valor como uma potência de 2 não muito grande, não sei bem se existe alguma regra para o tamanho inicial da tabela
#define P 4397 // Número primo aleatório para utilizar na função de hash
#define M 4294967296 // Universo de chaves = 2^32
#define MSQRT 65536 // Raiz quadrada de M

using namespace std;

class Node;
class HashTable;
class vEBTree;

class Node {
    public:
        int32_t key;
        unsigned short deleted : 1;
        vEBTree* pointer;

        Node() {
            this->key = -1;
            this->deleted = false;
        }

        int32_t setValue(int32_t k);
        void initializePointer(vEBTree* tree);
        void deleteNode();
};

int32_t Node::setValue(int32_t k) {
    this->key = k;
    return k;
}

void Node::deleteNode() {
    // Vamos usar lazy-deletion, apenas sinalizando que o nó foi deletado
    this->deleted = true;
    return;
}

void Node::initializePointer(vEBTree* tree) {
    //std::cout << "Inicializando ponteiro para posicao: " << tree << endl;
    this->pointer = tree;
    return;
}

class HashTable {
    public:
        int32_t size;
        std::vector<Node> table;
        int32_t filled; // Vamos armazenar quantos elementos do vetor estão preenchidos, a fim de mantermos a tabela sempre com tamanho m >= 2n, onde n é a qtde de chaves

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

            //std::cout << a1 << ", " << a2 << ", " << a3 << ", " << a4 << ", " << a5 << endl;
        }

        void tableDoubling();
        void tableHalving();
        int32_t insertKey(int32_t k);
        std::pair<int, Node*> findKey(int32_t k);
        int32_t deleteKey(int32_t k);
        int hash(int32_t k);
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
    for (int32_t i = 0; i < cp.size(); i++) {
        if (cp[i].key != -1 && cp[i].deleted != true) {
            this->insertKey(cp[i].key);
        }
    }
}

void HashTable::tableHalving() {
    std::vector<Node> aux((int32_t) this->size/2);
    std::vector<Node> cp(this->table);

    // "metadeando" o vetor
    this->size = (int32_t) this->size/2;
    this->table = aux;
    this->filled = 0;

    // reinserindo as chaves na nova posição    
    for (int32_t i = 0; i < cp.size(); i++) {
        if (cp[i].key != -1 && cp[i].deleted != true) {
            this->insertKey(cp[i].key);
        }
    }
}

int HashTable::hash(int32_t k) {
    // Vamos utilzar uma função de hash 5-independente, a fim de obtermos tempo esperado de O(1)/op
    unsigned long long int sum = (unsigned long long int) this->a5*std::pow(k,4) 
    + (unsigned long long int) this->a4*std::pow(k,3) + 
    (unsigned long long int) this->a3*std::pow(k,2) + 
    (unsigned long long int) this->a2*k + this->a1;
    return (sum % P) % this->size;
}

int32_t HashTable::insertKey(int32_t k) {

    Node node = Node();
    node.setValue(k);

    int32_t idx = this->hash(k);
    //cout << "Inserindo na posição: " << idx << endl;


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

std::pair<int, Node*> HashTable::findKey(int32_t k) {
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
        return std::pair<int, Node*> {-1, &this->table[idx_]};
    }

    if (this->table[idx_].key == k && this->table[idx_].deleted == true) {
        // elemento existe mas foi deletado
        return std::pair<int, Node*> {-1, &this->table[idx_]};
    }

    return std::pair<int, Node*> {idx_, &this->table[idx_]};
}

int32_t HashTable::deleteKey(int32_t k) {
    int32_t idx = this->findKey(k).first;

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
    for (int32_t i = 0; i < this->size; i++) {
        if (this->table[i].key != -1 && this->table[i].deleted != true) {
            std::cout << " " << this->table[i].key << " |";
        } else {
            std::cout << " X |";
        }
    }
    std::cout << endl << endl;
}

class vEBTree {
    public:
        int32_t min;
        int32_t max;

        HashTable clusters;
        vEBTree* resumo;

        vEBTree() {
            this->min = INT32_MAX;
            this->max = INT32_MIN;
            this->clusters = HashTable();
        }

        int32_t insert(int32_t x);
        long int findNext(int32_t x);
        long int findPrevious(int32_t x);
        void print();
};

int32_t vEBTree::insert(int32_t x) {
    if (this->min == x || this->max == x) {
        // Elemento já foi inserido
        return x;
    }

    if (this->min > this->max) {
        // Árvore está vazia
        this->min = x;
        this->max = x;
        return x;
    }

    if (x < this->min) {
        // swap(x, T.min)
        int32_t aux = x;
        x = this->min;
        this->min = aux;
    }

    if (x > this->max) {
        this->max = x;
    }

    int i = floor( ((float) x) / MSQRT );
    int lo = x % MSQRT;

    if (this->clusters.findKey(i).second->pointer == NULL) {
        // Criando um novo cluster caso esteja vazio
        //std::cout << "Cluster da posicao " << i << " esta vazio. Inicializando cluster... " << this->clusters.findKey(i).second->pointer << endl;
        vEBTree* cluster = new vEBTree();
        this->clusters.findKey(i).second->initializePointer(cluster);
        //std::cout << "Cluster da posicao " << i << " agora esta inicializado na posicao " << cluster << " / " << this->clusters.findKey(i).second->pointer << endl << endl;
    }

    // Inserindo no cluster i
    vEBTree* ithCluster = this->clusters.findKey(i).second->pointer;
    ithCluster->insert(lo);

    if (ithCluster->min == ithCluster->max) {
        if (this->resumo == NULL) {
            // Inicializando a árvore resumo caso ela não exista
            this->resumo = new vEBTree();
        }
        this->resumo->insert(i);
    }

    return x;
}

long int vEBTree::findNext(int32_t x) {
    if (x < this->min) {
         return this->min;
    }

    if (x >= this->max) {
        return M;
    }

    int i = floor( ((float) x) / MSQRT );
    int lo = x % MSQRT;

    vEBTree* ithCluster = this->clusters.findKey(i).second->pointer;

    if (lo < ithCluster->max) {
        return (MSQRT * i) + ithCluster->findNext(lo);
    }

    int32_t j = this->resumo->findNext(i);
    return (MSQRT * j) + this->clusters.findKey(j).second->pointer->min;
}

long int vEBTree::findPrevious(int32_t x) {
    if (x > this->max) {
         return this->max;
    }

    if (x <= this->min) {
        return INT32_MIN;
    }

    int i = floor( ((float) x) / MSQRT );
    int lo = x % MSQRT;

    vEBTree* ithCluster = this->clusters.findKey(i).second->pointer;

    if (lo > ithCluster->min) {
        return (MSQRT * i) + ithCluster->findPrevious(lo);
    }

    int32_t j = this->resumo->findPrevious(i);

    if (j == INT32_MIN) {
        if (this->min < x) {
            return this->min;
        } else {
            return INT32_MIN;
        }
    } else {
        return (MSQRT * j) + this->clusters.findKey(j).second->pointer->max;
    }
}

void vEBTree::print() {
    std::cout << this->min << ", ";
    // Continuar o print
}

int main() {

    //HashTable table = HashTable();
    vEBTree tree = vEBTree();

    int32_t k=0;
    unsigned int c;

    while (k != -1 && c != -1) {

        std::cout << "MENU" << endl <<"1 - inserir" << endl << "2 - sucessor" << endl << "3 - predecessor" << endl;
        std::cin >> c;

        switch (c)
        {
        case 1:
            {
                std::cout << "Digite um valor para inserir na arvore: ";
                std::cin >> k;
                std::cout << endl;

                tree.insert(k);
            }
            break;
        case 2:
            {
                std::cout << "Digite um valor para encontrar o sucessor na arvore: ";
                std::cin >> k;
                std::cout << endl;

                int32_t res = tree.findNext(k);
                std::cout << "Sucessor de " << k << " eh: " << res << endl << endl;
            }
            break;
        case 3:
            {
                std::cout << "Digite um valor para encontrar o predecessor na arvore: ";
                std::cin >> k;
                std::cout << endl;

                int32_t res = tree.findPrevious(k);
                std::cout << "Predecessor de " << k << " eh: " << res << endl << endl;
            }
        default:
            break;
        }
    }

    tree.print();

    return 0;
}