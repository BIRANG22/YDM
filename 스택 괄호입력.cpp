#include <iostream>
char stack[100];

int main() {
    int count = 0;
    char input;
    
    while (true)
    {
        std::cout << "괄호를 입력해주세요: ";

        std::cin >> input;
        if (input == '(' || input == '{' || input == '[')
        {
            stack[count] = input;
            count++;
        }
        else
        {
            if ((stack[count - 1] == '(' &&  input == ')') || (stack[count - 1] == '{' && input == '}') || (stack[count - 1] == '[' && input == ']'))
            {
                count--;
                std::cout << "pop: " << stack[count ] << input << std::endl;
                if (count == 0)break;
            }
            else
            {
                std::cout << "ERROR";
            }
        }
    }
    

    return 0;
}
