#include <iostream>

int arr[100];

int main()
{
    int front = 0;
    int rear = 0;
    int size = 100;
    int input = 0;
    while (true)
    {
        size = rear - front;

        rear = (rear + 1) % 100;
        front = (front + 1) % 100;

        if (size == 99 || front - rear == 1)
        {
            std::cout << "fool";
            break;
        }
        if (size == 0)
        {
            std::cout << "empty";
        }

        rear = (rear + 1) % 100;
        front = (front + 1) % 100;

        std::cout << "정수를 입력해주세요(종료:-1)(pop:-2): ";
        std::cin >> input;
        if (input == -1)break;
        if (input == -2)
        {
            std::cout << "pop: " << arr[front] << std::endl;
            front++;
            std::cout << "front인덱스의 값: " << arr[front] << std::endl;
            std::cout << "front 위치: " << front << std::endl;
            std::cout << "rear 위치: " << rear << std::endl;
            std::cout << "남은 배열 사이즈: " << size << std::endl;
        }
        else
        {
            arr[rear] = input;
            rear++;
            std::cout << "front인덱스의 값: " << arr[front] << std::endl;
            std::cout << "front 위치: " << front << std::endl;
            std::cout << "rear 위치: " << rear << std::endl;
            std::cout << "남은 배열 사이즈: " << size << std::endl;
        }
    }
        


    return 0;
}

