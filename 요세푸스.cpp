#include <iostream>

struct Node {
    int data;
    Node* next;
    Node(int v) : data(v), next(nullptr) {}
};

int main() {
    int n, skip;
    std::cin >> n >> skip;

    // 1) 원형 리스트 생성 (1 → 2 → ... → n → back to 1)
    Node* head = nullptr;
    Node* prev = nullptr;
    for (int i = 1; i <= n; ++i) {
        Node* node = new Node(i);
        if (!head) head = node;
        else prev->next = node;
        prev = node;
    }
    prev->next = head;  // 순환 연결

    // 2) skip 만큼 이동 후 삭제 반복
    Node* cur = head;
    Node* before = prev; // cur의 직전 노드
    while (cur->next != cur) {
        // 정확히 skip-1번 이동하면, cur가 삭제 대상이 됨
        for (int i = 1; i < skip; ++i) {
            before = cur;
            cur = cur->next;
        }
        // cur이 삭제 대상
        std::cout << "삭제: " << cur->data << "\n";
        before->next = cur->next;  // 리스트에서 제거
        Node* toDelete = cur;
        cur = cur->next;           // 다음 위치로 이동
        delete toDelete;           // 메모리 해제
    }

    std::cout << "마지막 남음: " << cur->data << "\n";
    delete cur;
    return 0;
}
