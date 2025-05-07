#include <iostream>

int main()
{
    int Stack_A[100] = { 0, };
    int Stack_B[10] = { 0, };
    int input = 0;
    int temp = 0;
    int top = 0;
    int count = 0;
    
    while (true)
    {
        std::cout << "정수를 입력해주세요(종료 : -1)(pop = -2) : ";
        std::cin >> input;
        if (input == -1)break;
        if (input == -2)
        {
            std::cout << "pop : " << Stack_A[count-1] << std::endl;
            if (Stack_A[count - 1] == Stack_B[top])
            {
                top -= 1;
            }
            count -= 1;
            if (count == 0)
            {
                std::cout << "전부 pop 완료" << std::endl;
            }
        }
        else 
        {
            Stack_A[count] = input;
            if (Stack_A[count] > Stack_B[top])
            {
                top++;
                Stack_B[top] = Stack_A[count];
                std::cout << "최대값: " << Stack_B[top] << std::endl;

            }
            else
            {
                std::cout << "최대값: " << Stack_B[top] << std::endl;
            }
            count++;
        }

       

    }

}

