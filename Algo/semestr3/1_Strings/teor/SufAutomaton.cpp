#include <iostream>
#include <map>

struct Node {
    Node(int len, Node* link, int number): len(len), link(link), number(number) {}
    Node(const Node&) = default;
    std::map<char, Node*> edges;
    int len;
    Node* link;
    int number;
};

class SufAutomaton {
public:
    SufAutomaton() {
        root = new Node(0, nullptr, ptr++);
        last = root;
    }
    void addString(std::string s) {
        for (auto c : s) {
            addChar(c);
        } 
    }
    void print(Node* node) {
        if (!node) 
            node = root;
        std::cout << "Node " << node->number << std::endl;
        for (auto t: node->edges) {
            std::cout << "---(" << t.first << ")--->" << t.second->number << std::endl;
        }
        std::cout << std::endl;
        for (auto t: node->edges) {
            print(t.second);
        }
    }
private:
    int ptr = 0;
    Node* root;
    Node* last; 
    void addChar(char c) {
        //std::cout << c << std::endl;
        // print(nullptr); 
        Node* Sc = new Node(last->len + 1, nullptr, ptr++);
        Node* p = last;
        last = Sc;
        while (p && p->edges.find(c) == p->edges.end()) {
            p->edges[c] = Sc;
            p = p->link;
        }
        if (!p) {
            Sc->link = root;
            return;
        }
        //std::cout << "Found p -> q" << std::endl;
        Node* q = p->edges[c];
        //std::cout << "q: " << q->number << std::endl;
        if (q->len == p->len + 1) {
            Sc->link = q;
            return; 
        }
        //std::cout << "Clonning q" << std::endl;
        Node* clone = new Node(0, nullptr, ptr++);
        clone->edges = q->edges;
        clone->link = q->link;
        clone->len = p->len + 1; 
        Sc->link = clone;
        q->link = clone; 
        while (p && p->edges[c] == q) {
            p->edges[c] = clone;
            p = p->link;
        }
        //std::cout << "cycle ended" << std::endl; 
        return;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    int n = 0;
    std::cin >> n; 
    for (int i = 0; i < n; i++) {
        std::string s;
        std::cin >> s;
        SufAutomaton sa;
        sa.addString(s);
    }
}
