#include <crtdbg.h>
#include <iostream>


struct Node
{
    Node() = delete;
    Node(const char* str) : pLinkNode(nullptr)
    {
        strcpy_s(this->str, str);
    }
    char str[32] = "";
    Node* pLinkNode;
};

Node* g_NodePtr = nullptr;

void AddNode(const char* str)
{
    Node* pNewNode = new Node(str);

    pNewNode->pLinkNode = g_NodePtr;
    g_NodePtr = pNewNode;
}

void PrintNode()
{
   PrintNodeReverse(g_NodePtr);
}

void PrintNodeReverse(Node* node)
{
    if (!node) return;
    PrintNodeReverse(node->pLinkNode);
    std::cout << node->str << "도 있고\n";
}
void DeleteNode()
{
    Node* curr = g_NodePtr;
    while (curr)
    {
        Node* next = curr->pLinkNode;
        delete curr;
        curr = next;
    }
    g_NodePtr = nullptr;
}

int main()
{
    std::cout << "시장에 가면 \n";

    AddNode("사과");
    AddNode("배");
    AddNode("귤");

    PrintNode();

    DeleteNode();

    return 0;
}

