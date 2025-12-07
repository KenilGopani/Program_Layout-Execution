/**
 * Recursive Factorial Program
 * This C program demonstrates recursion with a factorial calculation.
 */

#include <stdio.h>

/**
 * Recursive factorial function
 * 
 * @param n - The number to calculate factorial of
 * @return n! (factorial of n)
 */
int factorial(int n) {
    // Base case: factorial(1) = 1, factorial(0) = 1
    if (n <= 1) {
        return 1;
    }
    
    return n * factorial(n - 1);
}

int main() {
    int n = 5;
    int result;
    result = factorial(n);
    printf("F(%d) = %d\n", n, result);
    
    return 0;
}
